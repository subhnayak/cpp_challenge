/**
 * ✅ SOLUTION - CRTP Performance: Compile-Time vs Runtime Polymorphism
 * 
 * KEY CONCEPTS:
 * - CRTP (Curiously Recurring Template Pattern) enables compile-time polymorphism
 * - No virtual function overhead (no vptr, no vtable lookup)
 * - Trade-off: No heterogeneous collections (can't store different types in same vector)
 * - CRTP enables inlining by compiler (vtable prevents this)
 */

#include <iostream>
#include <vector>
#include <chrono>
#include <memory>
#include <random>
#include <cmath>

// ============================================
// VIRTUAL (RUNTIME) POLYMORPHISM VERSION
// ============================================

class VirtualEntity {
protected:
    float x, y;
    float vx, vy;
    
public:
    VirtualEntity() : x(0), y(0), vx(1), vy(1) {}
    virtual ~VirtualEntity() = default;
    
    // Virtual function - runtime cost (vtable lookup + indirection)
    virtual void update(float dt) {
        x += vx * dt;
        y += vy * dt;
    }
    
    virtual float getSpeed() const {
        return std::sqrt(vx * vx + vy * vy);
    }
    
    float getX() const { return x; }
    float getY() const { return y; }
};

class VirtualPlayer : public VirtualEntity {
private:
    float boost;
    
public:
    VirtualPlayer() : boost(2.0f) {}
    
    void update(float dt) override {
        x += vx * dt * boost;
        y += vy * dt * boost;
    }
    
    float getSpeed() const override {
        return VirtualEntity::getSpeed() * boost;
    }
};

class VirtualEnemy : public VirtualEntity {
private:
    float targetX, targetY;
    
public:
    VirtualEnemy() : targetX(100), targetY(100) {}
    
    void update(float dt) override {
        // Simple chase behavior
        float dx = targetX - x;
        float dy = targetY - y;
        float dist = std::sqrt(dx * dx + dy * dy);
        if (dist > 0.1f) {
            vx = dx / dist;
            vy = dy / dist;
        }
        VirtualEntity::update(dt);
    }
};

// ============================================
// CRTP (COMPILE-TIME) POLYMORPHISM VERSION
// ============================================

/**
 * SOLUTION: Fix CRTP by having base class call derived implementation!
 * 
 * The pattern:
 * 1. Base class stores Derived template parameter
 * 2. Base class calls static_cast to get derived instance
 * 3. Base calls derived().updateImpl() or static_cast<Derived*>(this)->updateImpl()
 * 4. At compile time, compiler knows exact type and can inline everything
 */
template<typename Derived>
class CRTPEntity {
protected:
    float x, y;
    float vx, vy;
    
public:
    CRTPEntity() : x(0), y(0), vx(1), vy(1) {}
    
    // SOLUTION: Base class update() dispatches to derived's updateImpl()
    // This is compile-time dispatch - the compiler generates separate code for each type
    void update(float dt) {
        // The magic: static_cast to get the derived type
        static_cast<Derived*>(this)->updateImpl(dt);
    }
    
    // SOLUTION: Similarly for getSpeed()
    float getSpeed() const {
        return static_cast<const Derived*>(this)->getSpeedImpl();
    }
    
    float getX() const { return x; }
    float getY() const { return y; }
};

/**
 * SOLUTION: CRTPPlayer implements updateImpl() that base will call
 * 
 * Why separate method names (updateImpl vs update)?
 * - update() is the CRTP base interface
 * - updateImpl() is the derived override
 * - Prevents name conflicts and makes the pattern clear
 */
class CRTPPlayer : public CRTPEntity<CRTPPlayer> {
private:
    float boost;
    
public:
    CRTPPlayer() : boost(2.0f) {}
    
    // These are called by the base class through CRTP dispatch
    void updateImpl(float dt) {
        x += vx * dt * boost;
        y += vy * dt * boost;
    }
    
    float getSpeedImpl() const {
        return std::sqrt(vx * vx + vy * vy) * boost;
    }
};

class CRTPEnemy : public CRTPEntity<CRTPEnemy> {
private:
    float targetX = 100;
    float targetY = 100;
    
public:
    // Inherited behavior: CRTPEntity calls this through CRTP
    void updateImpl(float dt) {
        float dx = targetX - x;
        float dy = targetY - y;
        float dist = std::sqrt(dx * dx + dy * dy);
        if (dist > 0.1f) {
            vx = dx / dist;
            vy = dy / dist;
        }
        x += vx * dt;
        y += vy * dt;
    }
    
    float getSpeedImpl() const {
        return std::sqrt(vx * vx + vy * vy);
    }
};

// ============================================
// ADVANCED CRTP: SFINAE-BASED METHOD DETECTION
// ============================================

/**
 * BONUS CHALLENGE: Call method only if derived has it!
 * Uses SFINAE (Substitution Failure Is Not An Error)
 */

// Detect if type T has updateImpl method
template<typename T>
struct has_updateImpl {
private:
    // If T has updateImpl, this overload exists
    template<typename U>
    static auto check(U*) -> decltype(std::declval<U>().updateImpl(0.0f), std::true_type{});
    
    // Fallback
    template<typename U>
    static std::false_type check(...);
    
public:
    static constexpr bool value = std::is_same_v<decltype(check<T>(nullptr)), std::true_type>;
};

// Advanced CRTP with optional derived methods
template<typename Derived>
class SmartCRTPEntity {
protected:
    float x = 0, y = 0;
    float vx = 1, vy = 1;
    
public:
    // Default update (for derived classes that don't implement updateImpl)
    void update(float dt) {
        x += vx * dt;
        y += vy * dt;
    }
    
    // If derived has updateImpl(), call it; else use default
    // (In practice, you'd use std::enable_if or if constexpr)
    float getSpeed() const {
        return std::sqrt(vx * vx + vy * vy);
    }
};

// ============================================
// BENCHMARKING
// ============================================

void benchmarkVirtual(int iterations, int seed = 42) {
    std::vector<std::unique_ptr<VirtualEntity>> entities;
    
    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> dist(0.5f, 2.0f);
    
    // Create mixed entities
    for (int i = 0; i < 1000; i++) {
        if (i % 2 == 0) {
            entities.push_back(std::make_unique<VirtualPlayer>());
        } else {
            entities.push_back(std::make_unique<VirtualEnemy>());
        }
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; i++) {
        for (auto& entity : entities) {
            entity->update(0.016f);  // Virtual call incurs runtime cost
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Virtual polymorphism: " << duration.count() << " ms\n";
}

void benchmarkCRTP(int iterations, int seed = 42) {
    std::vector<CRTPPlayer> players(500);
    std::vector<CRTPEnemy> enemies(500);
    
    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> dist(0.5f, 2.0f);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; i++) {
        // Each type is updated separately - no virtual calls
        // Compiler knows exact type and can inline everything
        for (auto& player : players) {
            player.update(0.016f);  // Compile-time dispatch, fully inlinable
        }
        for (auto& enemy : enemies) {
            enemy.update(0.016f);
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "CRTP (compile-time): " << duration.count() << " ms\n";
}

// ============================================
// ANALYSIS & KEY INSIGHTS
// ============================================

void printAnalysis() {
    std::cout << "\n========== PERFORMANCE ANALYSIS ==========\n\n";
    
    std::cout << "VIRTUAL DISPATCH (Runtime Polymorphism):\n";
    std::cout << "- Each object has 8-byte vptr (64-bit systems)\n";
    std::cout << "- Each call triggers vtable lookup (cache miss risk)\n";
    std::cout << "- Compiler CANNOT inline virtual calls\n";
    std::cout << "- Branch prediction struggles with vtable indirection\n";
    std::cout << "- Memory size: 1000 objects × 8 bytes = 8 KB overhead\n";
    std::cout << "- For 1M entities: 8 MB overhead\n\n";
    
    std::cout << "CRTP (Compile-Time Polymorphism):\n";
    std::cout << "- No vptr - zero memory overhead\n";
    std::cout << "- Call is known at compile time\n";
    std::cout << "- Compiler fully inlines the call\n";
    std::cout << "- Branch predictor knows path in advance\n";
    std::cout << "- Perfect for hot loops with millions of iterations\n";
    std::cout << "- Trade-off: Can't store mixed types in single vector\n\n";
    
    std::cout << "CRTP LIMITATIONS:\n";
    std::cout << "- No heterogeneous collections (need separate vectors)\n";
    std::cout << "- Type must be known at compile time\n";
    std::cout << "- Can cause code bloat (template instantiation)\n";
    std::cout << "- Harder to use from external code\n\n";
    
    std::cout << "WHEN TO USE EACH:\n";
    std::cout << "- Virtual: Small number of calls, many types, dynamic dispatch needed\n";
    std::cout << "- CRTP: Hot path with millions of calls, known types, need speed\n";
    std::cout << "- Mixed: Use virtual for external API, CRTP for internal hot loops\n";
}

// ============================================
// BONUS: CRTP MIXIN PATTERN
// ============================================

/**
 * BONUS: CRTP enables powerful mixin patterns!
 * Each mixin provides a feature to the derived class.
 */
template<typename Derived>
class Movable {
public:
    void move(float dx, float dy) {
        std::cout << "Moving by (" << dx << ", " << dy << ")\n";
    }
};

template<typename Derived>
class Drawable {
public:
    void draw() const {
        std::cout << "Drawing sprite\n";
    }
};

template<typename Derived>
class Damageable {
protected:
    int health = 100;
public:
    void takeDamage(int d) {
        health -= d;
        std::cout << "Health: " << health << "\n";
    }
};

// Player inherits from multiple CRTP mixins
class GameEntity : public Movable<GameEntity>,
                   public Drawable<GameEntity>,
                   public Damageable<GameEntity> {
public:
    void tick() {
        draw();
        move(1.0f, 2.0f);
        takeDamage(5);
    }
};

// ============================================
// MAIN - DEMONSTRATION & BENCHMARK
// ============================================

int main() {
    std::cout << "╔════════════════════════════════════════╗\n";
    std::cout << "║  CRTP: Compile-Time Polymorphism      ║\n";
    std::cout << "║  Performance Comparison: Virtual vs CRTP║\n";
    std::cout << "╚════════════════════════════════════════╝\n\n";
    
    const int ITERATIONS = 10000;
    const int SEED = 42;
    
    std::cout << "=== BENCHMARK ===\n";
    std::cout << "Iterations: " << ITERATIONS << "\n";
    std::cout << "Entities: 1000 (500 players + 500 enemies)\n";
    std::cout << "per iteration\n\n";
    
    benchmarkVirtual(ITERATIONS, SEED);
    benchmarkCRTP(ITERATIONS, SEED);
    
    printAnalysis();
    
    // Demonstrate CRTP mixin pattern
    std::cout << "\n=== CRTP MIXIN PATTERN ===\n";
    GameEntity entity;
    entity.tick();
    
    std::cout << "\n✅ CRTP KEY INSIGHT:\n";
    std::cout << "Static polymorphism (CRTP) shifts dispatch cost from runtime\n";
    std::cout << "to compile time. Enabling aggressive inlining and zero ptr overhead.\n\n";
    
    std::cout << "=== THREAD SAFETY NOTE ===\n";
    std::cout << "CRTP entities don't have vtptr, making them more cache-friendly.\n";
    std::cout << "With 1M entities: 8 MB memory saved (no vptr array).\n";
    std::cout << "Better cache locality = faster updates in tight loops.\n";
    
    return 0;
}

/**
 * TRICKY ANSWER KEY:
 * 
 * Q1: Why can't you store CRTPPlayer and CRTPEnemy in the same vector?
 *     A: They're different types! CRTPPlayer is CRTPEntity<CRTPPlayer>,
 *        CRTPEnemy is CRTPEntity<CRTPEnemy>. No common base (without virtual).
 *        Virtual polymorphism allows heterogeneous collections (same base class).
 * 
 * Q2: Can static_cast in CRTP ever be unsafe?
 *     A: YES! If derived inherits wrong:
 *        class Evil : public CRTPEntity<CRTPPlayer> { };
 *        Evil e;
 *        e.update(0.016f);  // static_cast lies! e is not CRTPPlayer!
 *        The pattern REQUIRES: class X : public CRTPEntity<X>
 *        This is compile-time checked only by naming convention.
 * 
 * Q3: Can you combine virtual + CRTP?
 *     A: YES! Use CRTP internally in implementation, virtual for API.
 *        Or use virtual base with CRTP derived classes for mixins.
 * 
 * Q4: Memory layout difference?
 *     A: Virtual has 8-byte vptr per object.
 *        1M entities × 8 bytes = 8 MB overhead.
 *        Cache misses add up in tight loops. CRTP eliminates this.
 * 
 * Q5: BONUS - Mixin resolution:
 *     Multiple inheritance + CRTP:
 *     class X : Movable<X>, Drawable<X>, Damageable<X> {}
 *     Each mixin is a SEPARATE specialization - no diamond issue!
 *     Each provides different functionality through CRTP.
 */
