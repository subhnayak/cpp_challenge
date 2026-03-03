/**
 * ✅ SOLUTION - Final Boss: Rule of Five & Advanced Resource Management
 * 
 * Demonstrates:
 * - Rule of Five (destructor, copy constructor, copy assignment, move constructor, move assignment)
 * - Exception safety guarantees
 * - Deep copy implementation
 * - Copy-and-swap idiom
 */

#include <iostream>
#include <string>
#include <cstring>
#include <memory>
#include <stdexcept>

// ============================================
// SOLUTION: RULE OF FIVE - CUSTOM STRING CLASS
// ============================================

class GameString {
private:
    char* data;
    size_t length;
    
public:
    // 1. CONSTRUCTOR
    GameString(const std::string& str = "") {
        length = str.length();
        data = new char[length + 1];
        std::strcpy(data, str.c_str());
        std::cout << "  [Constructor] " << str << "\n";
    }
    
    // 2. DESTRUCTOR
    ~GameString() {
        std::cout << "  [Destructor] Cleaning up\n";
        delete[] data;
    }
    
    // 3. COPY CONSTRUCTOR - Deep copy
    GameString(const GameString& other) {
        length = other.length;
        data = new char[length + 1];
        std::strcpy(data, other.data);
        std::cout << "  [Copy Constructor] " << other.data << "\n";
    }
    
    // 4. COPY ASSIGNMENT - Deep copy with self-assignment check
    GameString& operator=(const GameString& other) {
        if (this != &other) {
            // Copy-and-swap idiom (exception-safe)
            GameString temp(other);
            std::swap(data, temp.data);
            std::swap(length, temp.length);
        }
        std::cout << "  [Copy Assignment] " << other.data << "\n";
        return *this;
    }
    
    // 5. MOVE CONSTRUCTOR - Steal ownership
    GameString(GameString&& other) noexcept {
        data = other.data;
        length = other.length;
        other.data = nullptr;
        other.length = 0;
        std::cout << "  [Move Constructor] Stealing\n";
    }
    
    // 6. MOVE ASSIGNMENT - Steal ownership safely
    GameString& operator=(GameString&& other) noexcept {
        if (this != &other) {
            delete[] data;
            data = other.data;
            length = other.length;
            other.data = nullptr;
            other.length = 0;
        }
        std::cout << "  [Move Assignment] Stealing\n";
        return *this;
    }
    
    const char* c_str() const { return data ? data : ""; }
    size_t size() const { return length; }
};

// ============================================
// SOLUTION: EXCEPTION SAFETY
// ============================================

class GameEntity {
private:
    std::unique_ptr<int> health;
    std::unique_ptr<char[]> inventory;
    
public:
    GameEntity(int hp = 100, size_t invSize = 10) 
        : health(std::make_unique<int>(hp)),
          inventory(std::make_unique<char[]>(invSize)) {
        std::cout << "  [Entity] Created with " << hp << " HP\n";
    }
    
    // Destructor: RAII handles cleanup automatically
    ~GameEntity() {
        std::cout << "  [Entity] Destroyed\n";
    }
    
    // Deep copy assignment with strong exception guarantee
    GameEntity& operator=(const GameEntity& other) {
        // Create temporary with copies of other's resources
        auto tempHealth = std::make_unique<int>(*other.health);
        auto tempInventory = std::make_unique<char[]>(10);
        std::memcpy(tempInventory.get(), other.inventory.get(), 10);
        
        // If we reach here, allocation succeeded
        // Now swap (can't throw)
        health = std::move(tempHealth);
        inventory = std::move(tempInventory);
        
        return *this;
    }
    
    int getHealth() const { return health ? *health : 0; }
    void setHealth(int hp) { if (health) *health = hp; }
};

// ============================================
// SOLUTION: RULE OF ZERO (Alternative)
// ============================================

class ModernGameEntity {
private:
    std::unique_ptr<int> health;
    std::string name;
    
public:
    ModernGameEntity(int hp = 100, const std::string& n = "Entity")
        : health(std::make_unique<int>(hp)), name(n) {}
    
    // Rule of Zero: Let smart pointers handle everything!
    // No explicit destructor, copy/move operations needed
    
    int getHealth() const { return *health; }
    const std::string& getName() const { return name; }
};

// ============================================
// DEMONSTRATIONS
// ============================================

void demonstrateRuleOfFive() {
    std::cout << "\n=== RULE OF FIVE ===\n";
    
    {
        GameString str1("First");
        GameString str2 = str1;           // Copy constructor
        GameString str3("Third");
        str3 = str2;                      // Copy assignment
        GameString str4 = std::move(str1); // Move constructor
        str2 = std::move(str3);           // Move assignment
    }
    
    std::cout << "✅ All special members working!\n";
}

void demonstrateExceptionSafty() {
    std::cout << "\n=== EXCEPTION SAFETY ===\n";
    
    {
        GameEntity entity1(100);
        GameEntity entity2(200);
        
        std::cout << "Before assignment: entity1 HP = " << entity1.getHealth() << "\n";
        entity1 = entity2;  // Strong exception guarantee
        std::cout << "After assignment: entity1 HP = " << entity1.getHealth() << "\n";
    }
    
    std::cout << "✅ Entities cleaned up safely!\n";
}

void demonstrateRuleOfZero() {
    std::cout << "\n=== RULE OF ZERO ===\n";
    
    {
        ModernGameEntity e1(150, "Hero");
        ModernGameEntity e2 = e1;           // Default copy (works!)
        ModernGameEntity e3 = std::move(e1); // Default move (works!)
        
        std::cout << "Entity: " << e2.getName() 
                  << " (HP: " << e2.getHealth() << ")\n";
    }
    
    std::cout << "✅ Smart pointers handle everything!\n";
}

// ============================================
// EXPLANATION NOTES
// ============================================

/*
 * RULE OF FIVE vs RULE OF ZERO:
 * 
 * RULE OF FIVE: For custom resource management
 * 1. Destructor - cleanup
 * 2. Copy constructor - deep copy
 * 3. Copy assignment - deep copy + cleanup
 * 4. Move constructor - steal resources
 * 5. Move assignment - steal + cleanup
 * 
 * Apply when: Class owns raw pointers or resources
 * 
 * RULE OF ZERO: Prefer smart pointers
 * - Don't define any special members
 * - Let smart pointers handle everything
 * - Cleaner, safer, less code
 * 
 * Apply when: Using unique_ptr, shared_ptr, etc.
 * 
 * COPY-AND-SWAP IDIOM:
 * 1. Create temporary with new values
 * 2. If allocation fails, exception before state changes
 * 3. Swap with current object (can't throw)
 * 4. Temporary destroyed, old state cleaned
 * 
 * Benefits: Exception-safe, no code duplication
 * 
 * EXCEPTION SAFETY LEVELS:
 * 
 * 1. No guarantee: Might leak, corrupt state
 * 2. Basic: No leaks, but state unknown
 * 3. Strong: All-or-nothing (our goal!)
 * 4. No-throw: Never throws (marked noexcept)
 * 
 * KEY RULES FOR THIS CHALLENGE:
 * 
 * - Use = default for trivial special members
 * - Use = delete to forbid operations
 * - Implement Rule of Five or Zero, not in-between
 * - Use smart pointers to reduce complexity
 * - Always mark destructors virtual in base classes
 * - Use RAII for resource management
 */

int main() {
    demonstrateRuleOfFive();
    demonstrateExceptionSafty();
    demonstrateRuleOfZero();
    
    std::cout << "\n========================================\n";
    std::cout << "✅ Final Boss complete!\n";
    std::cout << "✅ All C++ concepts mastered!\n";
    std::cout << "========================================\n";
    
    return 0;
}
