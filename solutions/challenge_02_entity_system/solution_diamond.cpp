/**
 * ✅ SOLUTION - Entity System: Virtual Inheritance & Diamond Problem
 * 
 * This solution demonstrates:
 * - Virtual inheritance to solve the diamond problem
 * - Proper virtual destructor handling
 * - Virtual function dispatch
 * - CRTP for static polymorphism
 * 
 * DETAILED EXPLANATION:
 * The Diamond Problem occurs when a class inherits from two classes
 * that share a common base class. Without virtual inheritance, you get
 * two copies of the base class, causing bugs and wasted memory.
 */

#include <iostream>
#include <chrono>
#include <vector>
#include <memory>

// ============================================
// SOLUTION: VIRTUAL INHERITANCE
// ============================================

// Base class with virtual destructor
class Entity {
protected:
    int health = 100;
    std::string name;
    
public:
    Entity() { std::cout << "  [Entity] Constructor\n"; }
    
    virtual ~Entity() { std::cout << "  [Entity] Destructor\n"; }
    
    virtual void update() = 0;
    virtual void render() = 0;
    
    int getHealth() const { return health; }
    void setHealth(int h) { health = h; }
};

// Mixin 1: Flying ability
class Flying : virtual public Entity {  // VIRTUAL inheritance!
protected:
    float altitude = 0.0f;
    
public:
    Flying() { std::cout << "  [Flying] Constructor\n"; }
    virtual ~Flying() { std::cout << "  [Flying] Destructor\n"; }
    
    virtual void fly() {
        std::cout << "  [Flying] Flying at altitude: " << altitude << "\n";
    }
};

// Mixin 2: Warrior ability
class Warrior : virtual public Entity {  // VIRTUAL inheritance!
protected:
    int attack = 10;
    
public:
    Warrior() { std::cout << "  [Warrior] Constructor\n"; }
    virtual ~Warrior() { std::cout << "  [Warrior] Destructor\n"; }
    
    virtual void attack_method() {
        std::cout << "  [Warrior] Attacking with power: " << attack << "\n";
    }
};

// Derived: Dragon is both flying and warrior
class FlyingWarrior : public Flying, public Warrior {
public:
    FlyingWarrior(int hp = 100) { 
        std::cout << "  [FlyingWarrior] Constructor\n";
        // Virtual inheritance means Entity is constructed once
        health = hp;
    }
    
    virtual ~FlyingWarrior() { std::cout << "  [FlyingWarrior] Destructor\n"; }
    
    void update() override {
        std::cout << "  [FlyingWarrior] Updating\n";
        fly();
        attack_method();
    }
    
    void render() override {
        std::cout << "  [FlyingWarrior] Rendering\n";
    }
};

// ============================================
// COVARIANT RETURN TYPES
// ============================================

class Cloneable {
public:
    virtual ~Cloneable() = default;
    
    // Covariant return type:
    // Derived class can return derived type instead of base type
    virtual Cloneable* clone() = 0;
};

class Dragon : public Entity, public Cloneable {
public:
    Dragon() : Entity() {}
    virtual ~Dragon() {}
    
    void update() override { std::cout << "Dragon updates\n"; }
    void render() override { std::cout << "Dragon renders\n"; }
    
    // Can return Dragon* instead of Cloneable*!
    Dragon* clone() override {
        return new Dragon(*this);
    }
};

// ============================================
// CRTP: Compile-Time Polymorphism
// ============================================

// CRTP Base Template
template<typename Derived>
class CRTPEntity {
public:
    void update() {
        static_cast<Derived*>(this)->updateImpl();
    }
    
    void render() {
        static_cast<Derived*>(this)->renderImpl();
    }
    
    void attack() {
        static_cast<Derived*>(this)->attackImpl();
    }
};

// CRTP Derived: No vtable overhead!
class FastDragon : public CRTPEntity<FastDragon> {
private:
    int health = 100;
    
public:
    FastDragon() {}
    
    void updateImpl() { /* Fast polymorphism */ }
    void renderImpl() { /* Fast polymorphism */ }
    void attackImpl() { /* Fast polymorphism */ }
};

// ============================================
// VIRTUAL FUNCTION BENCHMARKS
// ============================================

void benchmarkVirtual() {
    std::cout << "\n=== VIRTUAL POLYMORPHISM ===\n";
    
    std::vector<std::unique_ptr<Entity>> entities;
    for (int i = 0; i < 1000; ++i) {
        entities.push_back(std::make_unique<FlyingWarrior>(100 + i));
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int iter = 0; iter < 10000; ++iter) {
        for (auto& entity : entities) {
            entity->update();     // Virtual call
            entity->render();     // Virtual call
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Time: " << duration.count() << " μs\n";
}

void benchmarkCRTP() {
    std::cout << "\n=== CRTP (Static) POLYMORPHISM ===\n";
    
    std::vector<FastDragon> entities;
    for (int i = 0; i < 1000; ++i) {
        entities.push_back(FastDragon());
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int iter = 0; iter < 10000; ++iter) {
        for (auto& entity : entities) {
            entity.update();      // Inlined call
            entity.render();      // Inlined call
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Time: " << duration.count() << " μs\n";
}

// ============================================
// DEMONSTRATIONS
// ============================================

void demonstrateDiamondSolution() {
    std::cout << "\n=== DIAMOND PROBLEM SOLVED ===\n\n";
    
    {
        FlyingWarrior dragon(200);
        
        // With virtual inheritance: only ONE copy of health!
        std::cout << "\nDragon health: " << dragon.getHealth() << "\n";
        
        dragon.update();
        dragon.render();
    }
    
    std::cout << "\n✅ All destructors called in correct order!\n";
}

void demonstrateCovariantReturns() {
    std::cout << "\n=== COVARIANT RETURN TYPES ===\n\n";
    
    Dragon d;
    
    // Returns Dragon*, not Cloneable*
    Dragon* clone = d.clone();
    std::cout << "Clone created: " << (clone ? "success" : "failed") << "\n";
    delete clone;
}

// ============================================
// DETAILED EXPLANATION NOTES
// ============================================

/*
 * KEY CONCEPTS:
 * 
 * 1. THE DIAMOND PROBLEM:
 * 
 *    Without Virtual Inheritance:
 *    
 *    FlyingWarrior
 *    ├── Flying
 *    │   └── Entity
 *    │       ├── health = 100 (from Flying's Entity)
 *    │       └── name = "..." (from Flying's Entity)
 *    └── Warrior  
 *        └── Entity
 *            ├── health = 100 (from Warrior's Entity) ← DIFFERENT!
 *            └── name = "..." (from Warrior's Entity)
 *    
 *    Problem: Two copies of Entity! Waste of memory!
 *             FlyingWarrior::health ambiguous - which Entity's health?
 * 
 * 2. THE FIX: VIRTUAL INHERITANCE:
 * 
 *    class Flying : virtual public Entity { };
 *    class Warrior : virtual public Entity { };
 * 
 *    With Virtual Inheritance:
 *    
 *    FlyingWarrior
 *    ├── Flying
 *    │   └── shared Entity
 *    │       ├── health = 100
 *    │       └── name = "..."
 *    └── Warrior
 *        └── shared Entity (same as above!)
 * 
 *    Result: Only ONE copy of Entity!
 *            FlyingWarrior::health is unambiguous
 * 
 * 3. HOW VIRTUAL INHERITANCE WORKS:
 * 
 *    - Base class moved to "most derived" in object layout
 *    - Derived classes use virtual pointers to base
 *    - Slight memory/time overhead (vprt + offset calculation)
 *    - But correctness benefit usually worth it
 * 
 * 4. VIRTUAL DESTRUCTORS:
 * 
 *    Must use virtual destructors when using inheritance!
 *    
 *    If you don't:
 *    Entity* e = new FlyingWarrior();
 *    delete e;  // Calls Entity::~Entity(), not FlyingWarrior::~FlyingWarrior()!
 *    // Derived resources leaked!
 *    
 *    With virtual:
 *    delete e;  // Calls FlyingWarrior::~FlyingWarrior() ✓
 *               // Then Flying::~Flying()  ✓
 *               // Then Warrior::~Warrior()  ✓
 *               // Finally Entity::~Entity()  ✓
 * 
 * 5. VIRTUAL FUNCTION DISPATCH:
 * 
 *    Virtual functions found via vtable:
 *    
 *    Entity* e = new FlyingWarrior();
 *    e->update();  // Runtime: looks up FlyingWarrior::update in vtable
 * 
 *    Cost: One extra pointer dereference
 *    Worth it: Type safety, polymorphism
 * 
 * 6. COVARIANT RETURN TYPES:
 * 
 *    virtual Cloneable* clone() = 0;  // Base
 *    virtual Dragon* clone() override;  // Derived
 * 
 *    Dragon* d = dynamic_cast<Dragon*>(thing->clone());
 *    // Works! Covariant return type.
 *    
 *    Violates LSP? No! Dragon* IS-A Cloneable*
 *    It's actually MORE specific, which is safe.
 * 
 * 7. CRTP ALTERNATIVE:
 * 
 *    Avoids vtable overhead through templates!
 *    
 *    template<typename Derived>
 *    class Base {
 *        void dispatch() {
 *            static_cast<Derived*>(this)->impl();
 *        }
 *    };
 *    
 *    class Derived : public Base<Derived> {
 *        void impl() { /* ... */ }
 *    };
 *    
 *    Benefits: No vtable, compiler can inline
 *    Downside: No runtime polymorphism, template code bloat
 * 
 * 8. VIRTUAL INHERITANCE COSTS:
 * 
 *    Size: +4-8 bytes per instance (virtual pointer to base)
 *    Time: One extra pointer dereference per method call
 *    Compile: More complex object layout
 *    
 *    Usually worth it for correctness!
 * 
 * 9. WHEN TO USE WHAT:
 * 
 *    Simple inheritance: No virtual inheritance needed
 *    Multiple inheritance, shared base: USE VIRTUAL!
 *    Hot loop, no runtime polymorphism: Consider CRTP
 *    Default: Virtual inheritance + virtual functions
 * 
 * 10. CONSTRUCTION ORDER WITH VIRTUAL INHERITANCE:
 * 
 *     Order: Most-derived → Left base → Right base → Virtual base
 * 
 *     FlyingWarrior() {
 *         Entity() calls virtual base constructor first
 *         Flying() calls
 *         Warrior() calls
 *         FlyingWarrior() body
 *     }
 * 
 *     Important for initialization!
 */

int main() {
    demonstrateDiamondSolution();
    demonstrateCovariantReturns();
    
    // Benchmark comparison
    benchmarkVirtual();
    benchmarkCRTP();
    
    std::cout << "\n========================================\n";
    std::cout << "✅ Diamond problem solved!\n";
    std::cout << "✅ Virtual inheritance working!\n";
    std::cout << "✅ All destructors called properly!\n";
    std::cout << "========================================\n";
    
    return 0;
}
