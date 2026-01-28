/**
 * 🔴 BROKEN CODE - C-Style Casts and Wrong Cast Types
 * 
 * C-style casts are dangerous because they can perform different
 * operations depending on context. C++ casts make intent explicit!
 * 
 * YOUR TASK: Replace all C-style casts with appropriate C++ casts!
 */

#include <iostream>
#include <cstdint>
#include <string>

// ============================================
// BAD: C-STYLE CASTS EVERYWHERE
// ============================================

void demonstrateCStyleCastAmbiguity() {
    std::cout << "=== C-STYLE CAST AMBIGUITY ===\n\n";
    
    // Example 1: Numeric conversion (safe, should be static_cast)
    int intVal = 42;
    float floatVal = (float)intVal;  // What kind of cast is this?
    std::cout << "  (float)int: " << floatVal << "\n";
    
    // Example 2: Pointer to integer (dangerous!)
    int* ptr = &intVal;
    uintptr_t ptrAsInt = (uintptr_t)ptr;  // reinterpret_cast needed
    std::cout << "  (uintptr_t)ptr: " << ptrAsInt << "\n";
    
    // Example 3: Removing const (dangerous!)
    const int constVal = 100;
    int* mutablePtr = (int*)&constVal;  // const_cast needed - but is it safe?
    // *mutablePtr = 200;  // UB if constVal is truly const!
    std::cout << "  (int*)&constVal: compiles but dangerous!\n";
    
    // Example 4: Unrelated types (extremely dangerous!)
    float f = 3.14f;
    int* intFromFloat = (int*)&f;  // reinterpret_cast - UB!
    std::cout << "  (int*)&float: " << *intFromFloat << " (UB!)\n";
}

// ============================================
// BUG: WRONG CAST TYPE CHOSEN
// ============================================

class Base {
public:
    virtual ~Base() = default;
    virtual void identify() { std::cout << "  I am Base\n"; }
};

class Derived : public Base {
public:
    int extraData = 42;
    void identify() override { std::cout << "  I am Derived\n"; }
    void derivedOnly() { std::cout << "  Derived-only function, extraData=" << extraData << "\n"; }
};

class Unrelated {
public:
    int data = 123;
};

void demonstrateWrongCast() {
    std::cout << "\n=== WRONG CAST TYPE ===\n\n";
    
    Base* base = new Derived();
    
    // BUG: Using static_cast for downcast - UNSAFE!
    Derived* d1 = static_cast<Derived*>(base);  // Works here by luck
    d1->derivedOnly();
    
    // BUG: What if base wasn't actually a Derived?
    Base* actuallyBase = new Base();
    Derived* d2 = static_cast<Derived*>(actuallyBase);  // Compiles but WRONG!
    // d2->derivedOnly();  // Would access invalid memory!
    
    std::cout << "  static_cast succeeded but might be wrong!\n";
    std::cout << "  (Use dynamic_cast for safety)\n";
    
    delete base;
    delete actuallyBase;
}

// ============================================
// BUG: DANGEROUS REINTERPRET_CAST USAGE
// ============================================

void demonstrateDangerousReinterpret() {
    std::cout << "\n=== DANGEROUS REINTERPRET_CAST ===\n\n";
    
    // BUG: Type punning through reinterpret_cast
    float f = 1.0f;
    int* ip = reinterpret_cast<int*>(&f);
    std::cout << "  Float " << f << " as int bits: " << *ip << "\n";
    
    // This violates strict aliasing!
    // The compiler may assume ip and f don't alias,
    // leading to undefined behavior.
    
    *ip = 0x40000000;  // Writing through int pointer
    std::cout << "  After int write, float = " << f << " (might be wrong!)\n";
    
    // BUG: Casting between unrelated class types
    Derived d;
    Unrelated* u = reinterpret_cast<Unrelated*>(&d);  // Complete nonsense!
    std::cout << "  Unrelated->data: " << u->data << " (garbage!)\n";
}

// ============================================
// BUG: CONST_CAST ABUSE
// ============================================

void legacyFunction(char* str) {
    // This function promises not to modify str, but signature is wrong
    std::cout << "  Legacy function received: " << str << "\n";
}

void demonstrateConstCastAbuse() {
    std::cout << "\n=== CONST_CAST ABUSE ===\n\n";
    
    // Legitimate use: calling legacy function that doesn't modify
    const char* message = "Hello";
    legacyFunction(const_cast<char*>(message));  // OK if legacyFunction doesn't modify
    
    // DANGEROUS: Modifying truly const data
    const int truly_const = 42;
    int* ptr = const_cast<int*>(&truly_const);
    
    std::cout << "  Before modification: " << truly_const << "\n";
    // *ptr = 100;  // UNDEFINED BEHAVIOR!
    // std::cout << "  After modification: " << truly_const << "\n";
    // Might still print 42 due to compiler optimization!
    
    std::cout << "  (Modifying const data is UB even with const_cast!)\n";
}

// ============================================
// CAST IDENTIFICATION PUZZLE
// ============================================

void castIdentificationPuzzle() {
    std::cout << "\n=== CAST IDENTIFICATION PUZZLE ===\n\n";
    
    std::cout << "For each C-style cast, identify the equivalent C++ cast:\n\n";
    
    int i = 10;
    double d = 3.14;
    const int ci = 20;
    Base* b = new Derived();
    void* vp = &i;
    
    // Puzzle 1
    double d1 = (double)i;
    std::cout << "  1. (double)i → ??? cast\n";
    
    // Puzzle 2
    int i2 = (int)d;
    std::cout << "  2. (int)d → ??? cast\n";
    
    // Puzzle 3
    int* ip = (int*)&ci;
    std::cout << "  3. (int*)&ci → ??? cast\n";
    
    // Puzzle 4
    Derived* dp = (Derived*)b;
    std::cout << "  4. (Derived*)b → ??? cast (but ??? is safer!)\n";
    
    // Puzzle 5
    int* ip2 = (int*)vp;
    std::cout << "  5. (int*)vp → ??? cast\n";
    
    // Puzzle 6
    unsigned int ui = (unsigned int)i;
    std::cout << "  6. (unsigned int)i → ??? cast\n";
    
    // Puzzle 7
    char* cp = (char*)&i;
    std::cout << "  7. (char*)&i → ??? cast (special case!)\n";
    
    delete b;
}

// ============================================
// YOUR TASK
// ============================================

/*
 * TODO: Create solution_casts.cpp with:
 * 
 * 1. Replace ALL C-style casts with appropriate C++ casts
 * 
 * 2. Cast selection rules:
 *    
 *    static_cast:
 *    - Numeric conversions (int→float, etc.)
 *    - Upcast (Derived*→Base*)
 *    - void* to typed pointer (if you know the type)
 *    - Explicit conversions (user-defined conversion operators)
 *    
 *    dynamic_cast:
 *    - Downcast (Base*→Derived*) with runtime check
 *    - Cross-cast in multiple inheritance
 *    - Requires polymorphic type (at least one virtual function)
 *    
 *    const_cast:
 *    - Add or remove const (and volatile)
 *    - ONLY use when calling legacy APIs that don't modify
 *    
 *    reinterpret_cast:
 *    - Pointer to integer and back
 *    - Between unrelated pointer types (dangerous!)
 *    - char* access is special (always allowed for serialization)
 * 
 * TRICKY QUESTIONS:
 * 
 * Q1: What's the difference between:
 *     static_cast<Base*>(derived)   // Upcast
 *     static_cast<Derived*>(base)   // Downcast
 *     Which is always safe? Which needs dynamic_cast?
 * 
 * Q2: Why does dynamic_cast need virtual functions?
 *     (Hint: RTTI storage)
 * 
 * Q3: This compiles but is it safe?
 *     const std::string str = "hello";
 *     char* ptr = const_cast<char*>(str.c_str());
 *     ptr[0] = 'H';  // ???
 * 
 * Q4: reinterpret_cast to char* is special. Why?
 *     (Hint: aliasing rules have an exception)
 * 
 * Q5: BONUS - What does this do?
 *     struct A { int x; };
 *     struct B { int y; };
 *     
 *     A a{10};
 *     B* bp = reinterpret_cast<B*>(&a);
 *     int value = bp->y;  // What is value? Is this UB?
 */

int main() {
    demonstrateCStyleCastAmbiguity();
    demonstrateWrongCast();
    demonstrateDangerousReinterpret();
    demonstrateConstCastAbuse();
    castIdentificationPuzzle();
    
    std::cout << "\n========================================\n";
    std::cout << "Replace all C-style casts with C++ casts!\n";
    std::cout << "========================================\n";
    
    return 0;
}
