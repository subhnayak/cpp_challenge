/**
 * 🔴 BROKEN CODE - Virtual Destructor Problem
 * 
 * Deleting derived objects through base class pointers
 * doesn't call the derived destructor. MEMORY LEAK!
 * 
 * YOUR TASK: Fix the destructor chain!
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>

// ============================================
// BASE ENTITY CLASS
// ============================================

class Entity {
protected:
    int id;
    std::string name;
    
public:
    Entity(int entityId, const std::string& entityName) 
        : id(entityId), name(entityName) {
        std::cout << "  [Entity] Created: " << name << "\n";
    }
    
    // BUG: Non-virtual destructor!
    ~Entity() {
        std::cout << "  [Entity] Destroyed: " << name << "\n";
    }
    
    virtual void update() {
        std::cout << "  [Entity] Updating " << name << "\n";
    }
    
    const std::string& getName() const { return name; }
};

// ============================================
// DERIVED CLASSES WITH RESOURCES
// ============================================

class Dragon : public Entity {
private:
    int* fireBreathData;  // Simulated large resource
    size_t dataSize;
    
public:
    Dragon(int id, const std::string& name) 
        : Entity(id, name), dataSize(1000) {
        fireBreathData = new int[dataSize];
        std::cout << "  [Dragon] Allocated fire breath data for " << name << "\n";
    }
    
    // BUG: This destructor is NEVER called when deleted via Entity*!
    ~Dragon() {
        std::cout << "  [Dragon] Freeing fire breath data for " << name << "\n";
        delete[] fireBreathData;
    }
    
    void update() override {
        std::cout << "  [Dragon] " << name << " breathes fire!\n";
    }
};

class Goblin : public Entity {
private:
    std::vector<std::string>* inventory;
    
public:
    Goblin(int id, const std::string& name) 
        : Entity(id, name) {
        inventory = new std::vector<std::string>{"gold", "dagger", "potion"};
        std::cout << "  [Goblin] Created inventory for " << name << "\n";
    }
    
    // BUG: This destructor is NEVER called when deleted via Entity*!
    ~Goblin() {
        std::cout << "  [Goblin] Dropping inventory for " << name << "\n";
        delete inventory;
    }
    
    void update() override {
        std::cout << "  [Goblin] " << name << " lurks in shadows...\n";
    }
};

// ============================================
// A MORE COMPLEX CASE: MULTIPLE LEVELS
// ============================================

class Monster : public Entity {
protected:
    int level;
    
public:
    Monster(int id, const std::string& name, int lvl) 
        : Entity(id, name), level(lvl) {
        std::cout << "  [Monster] Level " << level << "\n";
    }
    
    // BUG: No virtual destructor either!
    ~Monster() {
        std::cout << "  [Monster] Destroying level " << level << " monster\n";
    }
    
    void update() override {
        std::cout << "  [Monster] " << name << " growls!\n";
    }
};

class Boss : public Monster {
private:
    std::string* specialAbility;
    
public:
    Boss(int id, const std::string& name, int level) 
        : Monster(id, name, level) {
        specialAbility = new std::string("Ultimate Attack");
        std::cout << "  [Boss] Gained ability: " << *specialAbility << "\n";
    }
    
    // BUG: Never called when deleting via Entity* OR Monster*!
    ~Boss() {
        std::cout << "  [Boss] Losing ability: " << *specialAbility << "\n";
        delete specialAbility;
    }
    
    void update() override {
        std::cout << "  [Boss] " << name << " uses " << *specialAbility << "!\n";
    }
};

// ============================================
// DEMONSTRATION OF THE BUG
// ============================================

void demonstrateDestructorLeak() {
    std::cout << "\n=== DESTRUCTOR LEAK DEMO ===\n\n";
    
    std::cout << "--- Creating entities via base pointer ---\n";
    Entity* dragon = new Dragon(1, "Smaug");
    Entity* goblin = new Goblin(2, "Sneaky");
    
    std::cout << "\n--- Updating (polymorphism works!) ---\n";
    dragon->update();
    goblin->update();
    
    std::cout << "\n--- Deleting via base pointer (LEAK!) ---\n";
    delete dragon;  // Only ~Entity() runs! Dragon data leaked!
    delete goblin;  // Only ~Entity() runs! Goblin inventory leaked!
    
    std::cout << "\n(Notice: Dragon and Goblin destructors never ran!)\n";
}

void demonstrateThreeLevelHierarchy() {
    std::cout << "\n=== THREE-LEVEL HIERARCHY LEAK ===\n\n";
    
    std::cout << "--- Creating Boss via Entity* ---\n";
    Entity* boss = new Boss(3, "Lich King", 99);
    
    std::cout << "\n--- Deleting via Entity* ---\n";
    delete boss;  // Only ~Entity() runs! Monster and Boss destructors skipped!
    
    std::cout << "\n--- Creating Boss via Monster* ---\n";
    Monster* boss2 = new Boss(4, "Dragon Lord", 100);
    
    std::cout << "\n--- Deleting via Monster* ---\n";
    delete boss2;  // Only ~Monster() and ~Entity() run! Boss destructor skipped!
}

void demonstrateWithContainer() {
    std::cout << "\n=== CONTAINER OF BASE POINTERS ===\n\n";
    
    std::vector<Entity*> entities;
    entities.push_back(new Dragon(5, "Dragon1"));
    entities.push_back(new Goblin(6, "Goblin1"));
    entities.push_back(new Boss(7, "FinalBoss", 50));
    
    std::cout << "\n--- Clearing the container ---\n";
    for (Entity* e : entities) {
        delete e;  // All leak!
    }
    entities.clear();
}

// ============================================
// YOUR TASK
// ============================================

/*
 * TODO: Create solution_destructor.cpp with:
 * 
 * 1. Fix all classes to have proper virtual destructors
 * 
 * 2. Verify that deleting via base pointer calls ALL destructors
 * 
 * 3. Use unique_ptr<Entity> for automatic cleanup
 * 
 * TRICKY QUESTIONS:
 * 
 * Q1: If Entity only has a virtual destructor (no other virtual functions),
 *     does it still need a vtable? What's the memory overhead?
 * 
 * Q2: Can you make the destructor "pure virtual"? Why would you?
 *     What's the syntax?
 * 
 * Q3: The C++ Core Guidelines say "A base class destructor should be
 *     either public and virtual, or protected and non-virtual."
 *     Why protected and non-virtual? What does that achieve?
 * 
 * Q4: What if you have a class that should NEVER be deleted through
 *     a base pointer? How would you enforce that?
 * 
 * Q5: BONUS - If you delete a derived object through a base pointer
 *     WITHOUT virtual destructor, is it:
 *     a) Undefined behavior
 *     b) Well-defined but wrong behavior
 *     c) Implementation-defined
 *     (Hint: check the C++ standard!)
 * 
 * Q6: EXTRA TRICKY - What happens here?
 *     
 *     struct Base {
 *         virtual ~Base() = default;
 *     };
 *     struct Derived : Base {
 *         ~Derived() override;  // Note: not virtual explicitly
 *     };
 *     
 *     Is Derived's destructor virtual? Why?
 */

int main() {
    demonstrateDestructorLeak();
    demonstrateThreeLevelHierarchy();
    demonstrateWithContainer();
    
    std::cout << "\n========================================\n";
    std::cout << "Memory leaked! Add virtual destructors!\n";
    std::cout << "Compile with -fsanitize=address to confirm\n";
    std::cout << "========================================\n";
    
    return 0;
}
