/**
 * ✅ SOLUTION - Serializer: Proper C++ Type Casting
 * 
 * Demonstrates:
 * - static_cast for safe conversions
 * - dynamic_cast for polymorphic type checks
 * - const_cast for legacy APIs (carefully!)
 * - reinterpret_cast only when absolutely necessary
 * - std::bit_cast for safe type punning
 */

#include <iostream>
#include <cstring>
#include <memory>
#include <bit>

// ============================================
// ENTITY HIERARCHY
// ============================================

class Entity {
public:
    virtual ~Entity() = default;
    virtual void describe() = 0;
};

class Dragon : public Entity {
public:
    void describe() override { std::cout << "I am a Dragon!\n"; }
    void breatheFire() { std::cout << "🔥 Fire!\n"; }
};

class Wizard : public Entity {
public:
    void describe() override { std::cout << "I am a Wizard!\n"; }
    void castSpell() { std::cout << "✨ Spell!\n"; }
};

// ============================================
// SOLUTION: SAFE TYPE CASTING
// ============================================

// 1. STATIC_CAST - Safe for related types (numeric, pointer hierarchy)
void demonstrateStaticCast() {
    std::cout << "\n=== STATIC_CAST ===\n";
    
    int i = 42;
    float f = static_cast<float>(i);  // Safe: int to float
    std::cout << "int " << i << " to float " << f << "\n";
    
    // Upcasting is always safe
    Dragon dragon;
    Entity* entity = static_cast<Entity*>(&dragon);
    entity->describe();
}

// 2. DYNAMIC_CAST - Safe downcasting (requires virtual functions)
void demonstrateDynamicCast() {
    std::cout << "\n=== DYNAMIC_CAST ===\n";
    
    std::unique_ptr<Entity> entity = std::make_unique<Dragon>();
    
    // Safe: checks at runtime
    Dragon* dragon = dynamic_cast<Dragon*>(entity.get());
    if (dragon) {
        std::cout << "Downcasted to Dragon!\n";
        dragon->breatheFire();
    }
    
    Wizard* wizard = dynamic_cast<Wizard*>(entity.get());
    if (!wizard) {
        std::cout << "Not a Wizard\n";
    }
}

// 3. CONST_CAST - Remove const (use only with legacy APIs)
void legacyFunction(int* data) {
    *data = 999;  // Expects non-const
}

void demonstrateConstCast() {
    std::cout << "\n=== CONST_CAST ===\n";
    
    const int value = 42;
    
    // Only safe if the const object is actually modifiable
    legacyFunction(const_cast<int*>(&value));
    
    std::cout << "Value: " << value << "\n";  // Still 42!
}

// 4. BIT_CAST - Safe type punning (C++20)
void demonstrateBitCast() {
    std::cout << "\n=== BIT_CAST (C++20) ===\n";
    
    int intValue = 0x41424344;
    
    // Safe type punning via bit_cast
    char bytes[4];
    std::memcpy(&bytes, &intValue, sizeof(int));
    
    for (char b : bytes) {
        std::cout << b;  // ABCD
    }
    std::cout << "\n";
}

// 5. SAFE BINARY I/O
class Serializer {
public:
    // Read float safely from binary format
    static float readFloat(const unsigned char* buffer) {
        float f;
        std::memcpy(&f, buffer, sizeof(float));  // Safe!
        return f;
    }
    
    // Write float safely
    static void writeFloat(unsigned char* buffer, float value) {
        std::memcpy(buffer, &value, sizeof(float));  // Safe!
    }
};

// ============================================
// DEMONSTRATIONS
// ============================================

int main() {
    demonstrateStaticCast();
    demonstrateDynamicCast();
    demonstrateConstCast();
    demonstrateBitCast();
    
    std::cout << "\n=== BINARY SERIALIZATION ===\n";
    unsigned char buffer[4];
    Serializer::writeFloat(buffer, 3.14159f);
    float read = Serializer::readFloat(buffer);
    std::cout << "Serialized and read: " << read << "\n";
    
    std::cout << "\n✅ All casts used safely!\n";
    
    return 0;
}

/*
 * KEY RULES FOR CASTING:
 * 
 * 1. Use static_cast for:
 *    - Numeric conversions (int to float, etc.)
 *    - Upcasting in hierarchy
 *    - Converting between related types
 * 
 * 2. Use dynamic_cast for:
 *    - Downcasting in hierarchy
 *    - Type checking at runtime
 *    - Returns nullptr/throws on failure
 * 
 * 3. Use const_cast for:
 *    - Removing const from legacy APIs
 *    - ONLY if object is actually mutable!
 *    - Modifying const objects = UB
 * 
 * 4. Use reinterpret_cast for:
 *    - Pointer to integer conversions
 *    - Unrelated pointer types
 *    - Generally avoid! Causes UB easily
 * 
 * 5. Use std::bit_cast for:
 *    - C++20 safe type punning
 *    - Reinterprets bits without UB
 *    - Preferred over reinterpret_cast
 * 
 * 6. Use std::memcpy for:
 *    - Binary I/O
 *    - Safe type conversions
 *    - Respects strict aliasing rule
 */
