/**
 * 🔴 BROKEN CODE - Diamond Problem
 * 
 * The FlyingWarrior inherits from both Flying and Warrior,
 * which both inherit from Entity. This causes:
 * 1. Ambiguous member access
 * 2. Duplicate base class data
 * 3. Constructor/destructor confusion
 * 
 * YOUR TASK: Fix using virtual inheritance!
 */

#include <iostream>
#include <string>

// ============================================
// BASE CLASS
// ============================================

class Entity {
protected:
    int id;
    int health;
    std::string name;
    
public:
    Entity(int entityId, const std::string& entityName) 
        : id(entityId), health(100), name(entityName) {
        std::cout << "  [Entity] Constructor: " << name << " (id=" << id << ")\n";
    }
    
    virtual ~Entity() {
        std::cout << "  [Entity] Destructor: " << name << "\n";
    }
    
    int getHealth() const { return health; }
    void setHealth(int h) { health = h; }
    
    virtual void describe() {
        std::cout << "  Entity: " << name << ", Health: " << health << "\n";
    }
};

// ============================================
// INTERMEDIATE CLASSES (forming the diamond)
// ============================================

// BUG: No virtual inheritance!
class Flying : public Entity {
protected:
    int altitude;
    
public:
    Flying(int id, const std::string& name) 
        : Entity(id, name), altitude(0) {
        std::cout << "  [Flying] Constructor\n";
    }
    
    virtual ~Flying() {
        std::cout << "  [Flying] Destructor\n";
    }
    
    void fly(int height) {
        altitude = height;
        std::cout << "  Flying at altitude " << altitude << "\n";
    }
    
    void describe() override {
        std::cout << "  Flying Entity: " << name << ", Altitude: " << altitude << "\n";
    }
};

// BUG: No virtual inheritance!  
class Warrior : public Entity {
protected:
    int strength;
    
public:
    Warrior(int id, const std::string& name) 
        : Entity(id, name), strength(50) {
        std::cout << "  [Warrior] Constructor\n";
    }
    
    virtual ~Warrior() {
        std::cout << "  [Warrior] Destructor\n";
    }
    
    void attack() {
        std::cout << "  Attacking with strength " << strength << "\n";
    }
    
    void describe() override {
        std::cout << "  Warrior: " << name << ", Strength: " << strength << "\n";
    }
};

// ============================================
// DIAMOND CLASS (the problem!)
// ============================================

// This inherits Entity TWICE!
class FlyingWarrior : public Flying, public Warrior {
public:
    // BUG: Which Entity should we initialize? Both?
    FlyingWarrior(int id, const std::string& name) 
        : Flying(id, name + "_flying"), 
          Warrior(id, name + "_warrior") {
        std::cout << "  [FlyingWarrior] Constructor\n";
    }
    
    virtual ~FlyingWarrior() {
        std::cout << "  [FlyingWarrior] Destructor\n";
    }
    
    // BUG: Which base class method to call?
    void describe() override {
        // This is ambiguous without specifying which base!
        // Entity::describe();  // Error: ambiguous!
        Flying::describe();     // Have to be specific
        Warrior::describe();
    }
    
    // PUZZLE: What does this return?
    int getTotalHealth() {
        // BUG: There are TWO health variables!
        return Flying::health + Warrior::health;  // This is weird...
    }
};

// ============================================
// DEMONSTRATION OF THE PROBLEM
// ============================================

void demonstrateDiamondProblem() {
    std::cout << "\n=== DIAMOND PROBLEM DEMO ===\n\n";
    
    std::cout << "Creating FlyingWarrior...\n";
    FlyingWarrior hero(1, "Hero");
    
    std::cout << "\n--- Size comparison ---\n";
    std::cout << "sizeof(Entity): " << sizeof(Entity) << "\n";
    std::cout << "sizeof(Flying): " << sizeof(Flying) << "\n";
    std::cout << "sizeof(Warrior): " << sizeof(Warrior) << "\n";
    std::cout << "sizeof(FlyingWarrior): " << sizeof(FlyingWarrior) << "\n";
    // Note: FlyingWarrior is MUCH bigger than expected!
    
    std::cout << "\n--- The ambiguity problem ---\n";
    
    // BUG: Setting health is ambiguous!
    // hero.setHealth(75);  // Error: ambiguous
    hero.Flying::setHealth(75);
    hero.Warrior::setHealth(50);
    
    std::cout << "Flying::health = " << hero.Flying::getHealth() << "\n";
    std::cout << "Warrior::health = " << hero.Warrior::getHealth() << "\n";
    std::cout << "Total health = " << hero.getTotalHealth() << "\n";
    
    std::cout << "\n--- Describe calls ---\n";
    hero.describe();
    
    std::cout << "\n--- Constructor/Destructor count ---\n";
    std::cout << "(Notice Entity is constructed TWICE!)\n";
}

void demonstratePointerCasting() {
    std::cout << "\n=== POINTER CASTING WITH DIAMOND ===\n\n";
    
    FlyingWarrior* fw = new FlyingWarrior(2, "Test");
    
    // BUG: Which Entity does this point to?
    // Entity* e = fw;  // Error: ambiguous conversion!
    
    // Have to be explicit
    Entity* e1 = static_cast<Flying*>(fw);
    Entity* e2 = static_cast<Warrior*>(fw);
    
    std::cout << "FlyingWarrior address: " << fw << "\n";
    std::cout << "As Flying's Entity: " << e1 << "\n";
    std::cout << "As Warrior's Entity: " << e2 << "\n";
    std::cout << "(Notice they're different addresses!)\n";
    
    delete fw;
}

// ============================================
// YOUR TASK
// ============================================

/*
 * TODO: Create solution_diamond.cpp with:
 * 
 * 1. class FixedEntity - Base class
 * 
 * 2. class FixedFlying : virtual public FixedEntity
 *    - Uses virtual inheritance
 * 
 * 3. class FixedWarrior : virtual public FixedEntity
 *    - Uses virtual inheritance
 * 
 * 4. class FixedFlyingWarrior : public FixedFlying, public FixedWarrior
 *    - Must call FixedEntity constructor directly!
 *    - Only one instance of FixedEntity
 * 
 * TRICKY QUESTIONS:
 * 
 * Q1: With virtual inheritance, who is responsible for calling
 *     the virtual base class constructor? Why?
 * 
 * Q2: What happens to the memory layout with virtual inheritance?
 *     Why does sizeof(FixedFlyingWarrior) change?
 * 
 * Q3: Can you cast FixedFlyingWarrior* to FixedEntity* directly
 *     after fixing the diamond? Why does this work now?
 * 
 * Q4: What is the performance overhead of virtual inheritance?
 *     (Hint: look up "virtual base pointer" / vbptr)
 * 
 * Q5: BONUS - What if Flying and Warrior have different ways to
 *     initialize Entity's health? Who wins?
 *     
 *     class Flying : virtual public Entity {
 *         Flying() : Entity(100) {}  // health = 100
 *     };
 *     class Warrior : virtual public Entity {
 *         Warrior() : Entity(200) {}  // health = 200
 *     };
 *     class FlyingWarrior : public Flying, public Warrior {
 *         // What is health initialized to?
 *     };
 */

int main() {
    demonstrateDiamondProblem();
    demonstratePointerCasting();
    
    std::cout << "\n========================================\n";
    std::cout << "Fix the diamond using virtual inheritance!\n";
    std::cout << "========================================\n";
    
    return 0;
}
