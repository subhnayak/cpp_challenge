/**
 * 🔴 BROKEN CODE - CRTP Performance Challenge
 * 
 * Virtual function calls have overhead. In hot paths with millions
 * of calls, this adds up! CRTP provides compile-time polymorphism.
 * 
 * YOUR TASK: Implement CRTP version and benchmark the difference!
 */

#include <iostream>
#include <vector>
#include <chrono>
#include <memory>
#include <random>

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
    
    // Virtual function - late binding, runtime cost
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

// BUG: This CRTP implementation is incomplete/incorrect!
template<typename Derived>
class CRTPEntity {
protected:
    float x, y;
    float vx, vy;
    
public:
    CRTPEntity() : x(0), y(0), vx(1), vy(1) {}
    
    // BUG: This doesn't actually call the derived implementation!
    void update(float dt) {
        x += vx * dt;
        y += vy * dt;
    }
    
    // TODO: How do you call derived class's implementation?
    float getSpeed() const {
        return std::sqrt(vx * vx + vy * vy);
    }
    
    float getX() const { return x; }
    float getY() const { return y; }
    
    // PUZZLE: How do you access the derived class?
    Derived& derived() {
        return static_cast<Derived&>(*this);
    }
    
    const Derived& derived() const {
        return static_cast<const Derived&>(*this);
    }
};

// BUG: Inheriting wrong!
class CRTPPlayer : public CRTPEntity<CRTPPlayer> {
private:
    float boost;
    
public:
    CRTPPlayer() : boost(2.0f) {}
    
    // TODO: This needs to be called by the base class somehow
    void updateImpl(float dt) {
        x += vx * dt * boost;
        y += vy * dt * boost;
    }
    
    float getSpeedImpl() const {
        return CRTPEntity::getSpeed() * boost;
    }
};

class CRTPEnemy : public CRTPEntity<CRTPEnemy> {
private:
    float targetX, targetY;
    
public:
    CRTPEnemy() : targetX(100), targetY(100) {}
    
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
};

// ============================================
// BENCHMARKING
// ============================================

void benchmarkVirtual(int iterations) {
    std::vector<std::unique_ptr<VirtualEntity>> entities;
    
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
            entity->update(0.016f);  // Virtual call
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Virtual version: " << duration.count() << " ms\n";
}

// BUG: This benchmark is broken because CRTP isn't set up correctly
void benchmarkCRTP(int iterations) {
    std::vector<CRTPPlayer> players(500);
    std::vector<CRTPEnemy> enemies(500);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; i++) {
        for (auto& player : players) {
            player.update(0.016f);  // Should be compile-time dispatch
        }
        for (auto& enemy : enemies) {
            enemy.update(0.016f);
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "CRTP version: " << duration.count() << " ms\n";
    
    // BUG: Results are wrong because base class update() is called, not derived!
}

// ============================================
// YOUR TASK
// ============================================

/*
 * TODO: Create solution_performance.cpp with:
 * 
 * 1. Fix the CRTP implementation so that:
 *    - CRTPEntity::update() calls derived().updateImpl()
 *    - CRTPEntity::getSpeed() calls derived().getSpeedImpl()
 *    - The base class provides defaults if derived doesn't implement
 * 
 * 2. Implement detection of whether derived has the method:
 *    - If CRTPEnemy has updateImpl(), call it
 *    - If not, use default base implementation
 *    (Hint: use SFINAE or if constexpr!)
 * 
 * 3. Run benchmarks and measure the difference
 * 
 * TRICKY QUESTIONS:
 * 
 * Q1: Why can't you store CRTPPlayer and CRTPEnemy in the same vector?
 *     What's the fundamental difference from virtual polymorphism?
 * 
 * Q2: CRTP relies on static_cast. Can this ever be unsafe?
 *     What if someone inherits wrong?
 *     
 *     class Oops : public CRTPEntity<CRTPPlayer> { };  // BUG!
 *     
 * Q3: Can you combine virtual functions with CRTP? When would you?
 * 
 * Q4: What's the memory layout difference?
 *     - VirtualEntity has a vptr (8 bytes on 64-bit)
 *     - CRTPEntity<T> has no vptr
 *     Does this matter for a million entities?
 * 
 * Q5: BONUS - Implement a "mixins" pattern using CRTP:
 * 
 *     template<typename Derived>
 *     class Movable { void move(); };
 *     
 *     template<typename Derived>
 *     class Drawable { void draw(); };
 *     
 *     class Player : public Movable<Player>, public Drawable<Player> { };
 *     
 *     How do you resolve the diamond problem here?
 * 
 * BENCHMARK CHALLENGE:
 * 
 * Test with 10,000 iterations and 1000 entities.
 * How much faster is CRTP?
 * 
 * Try with compiler optimizations:
 * - Without: g++ -O0
 * - With: g++ -O3
 * 
 * Does the difference change with optimization? Why?
 */

int main() {
    std::cout << "=== POLYMORPHISM BENCHMARK ===\n\n";
    
    const int ITERATIONS = 10000;
    
    std::cout << "Running " << ITERATIONS << " iterations with 1000 entities each...\n\n";
    
    benchmarkVirtual(ITERATIONS);
    benchmarkCRTP(ITERATIONS);
    
    std::cout << "\n(Note: CRTP version is broken - fix it!)\n";
    
    std::cout << "\n========================================\n";
    std::cout << "Fix CRTP implementation and re-benchmark!\n";
    std::cout << "========================================\n";
    
    return 0;
}
