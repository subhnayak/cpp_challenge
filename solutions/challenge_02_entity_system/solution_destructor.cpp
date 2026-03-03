/**
 * ✅ SOLUTION - Proper Destructor & Virtual Functions
 * 
 * Demonstrates:
 * - Virtual destructors for polymorphic classes
 * - Proper cleanup through inheritance hierarchy
 * - Why raw pointers need virtual destructors
 */

#include <iostream>
#include <memory>
#include <vector>

class Entity {
protected:
    std::string name;
    
public:
    Entity(const std::string& n = "Entity") : name(n) {
        std::cout << "Entity ctor: " << name << "\n";
    }
    
    // CRITICAL: Virtual destructor for polymorphic classes!
    virtual ~Entity() {
        std::cout << "Entity dtor: " << name << "\n";
    }
    
    virtual void attack() = 0;
};

class Dragon : public Entity {
private:
    int firepower = 50;
    
public:
    Dragon() : Entity("Dragon") {
        std::cout << "Dragon ctor\n";
    }
    
    virtual ~Dragon() {
        std::cout << "Dragon dtor\n";
        // Cleanup dragon-specific resources
    }
    
    void attack() override {
        std::cout << "Dragon attacks with " << firepower << " firepower!\n";
    }
};

class Wizard : public Entity {
private:
    int mana = 100;
    
public:
    Wizard() : Entity("Wizard") {
        std::cout << "Wizard ctor\n";
    }
    
    virtual ~Wizard() {
        std::cout << "Wizard dtor\n";
        // Cleanup wizard-specific resources
    }
    
    void attack() override {
        std::cout << "Wizard casts spell with " << mana << " mana!\n";
    }
};

int main() {
    std::cout << "=== VIRTUAL DESTRUCTOR DEMO ===\n\n";
    
    {
        // Using polymorphic pointer
        Entity* entity = new Dragon();
        entity->attack();
        delete entity;  // Now calls Dragon::~Dragon() then Entity::~Entity()
        
        std::cout << "\n";
        
        entity = new Wizard();
        entity->attack();
        delete entity;  // Calls Wizard::~Wizard() then Entity::~Entity()
    }
    
    std::cout << "\n✅ All destructors called correctly!\n";
    
    return 0;
}
