/**
 * 🔴 BROKEN CODE - Rule of Three/Five/Zero
 * 
 * This class manages resources but doesn't follow the rules!
 * Memory leaks, double-frees, and undefined behavior everywhere!
 * 
 * YOUR TASK: Apply the correct rule to fix the class!
 */

#include <iostream>
#include <cstring>
#include <algorithm>
#include <memory>

// ============================================
// BUG: VIOLATES RULE OF THREE
// ============================================

class BrokenString {
private:
    char* data;
    size_t length;
    
public:
    // Constructor
    BrokenString(const char* str = "") {
        length = strlen(str);
        data = new char[length + 1];
        strcpy(data, str);
        std::cout << "  [Constructed: \"" << data << "\"]\n";
    }
    
    // Destructor - WE HAVE THIS
    ~BrokenString() {
        std::cout << "  [Destroying: \"" << (data ? data : "null") << "\"]\n";
        delete[] data;
    }
    
    // BUG: No copy constructor! Compiler generates shallow copy!
    // BrokenString(const BrokenString& other) - missing!
    
    // BUG: No copy assignment! Compiler generates shallow copy!
    // BrokenString& operator=(const BrokenString& other) - missing!
    
    void print() const {
        std::cout << "  String: \"" << (data ? data : "null") << "\"\n";
    }
    
    const char* c_str() const { return data; }
};

void demonstrateRuleOfThreeViolation() {
    std::cout << "=== RULE OF THREE VIOLATION ===\n\n";
    
    std::cout << "Creating s1:\n";
    BrokenString s1("Hello");
    
    std::cout << "\nCopying to s2 (shallow copy!):\n";
    BrokenString s2 = s1;  // Shallow copy - both point to same memory!
    
    std::cout << "\nBoth strings:\n";
    s1.print();
    s2.print();
    
    std::cout << "\nLeaving scope (DOUBLE FREE!):\n";
    // When s2 is destroyed, it frees the memory
    // When s1 is destroyed, it tries to free the SAME memory!
}

// ============================================
// BUG: VIOLATES RULE OF FIVE
// ============================================

class BrokenBuffer {
private:
    unsigned char* data;
    size_t size;
    
public:
    BrokenBuffer(size_t bufferSize) : size(bufferSize) {
        data = new unsigned char[size];
        std::cout << "  [Buffer allocated: " << size << " bytes]\n";
    }
    
    ~BrokenBuffer() {
        std::cout << "  [Buffer freed: " << size << " bytes]\n";
        delete[] data;
    }
    
    // Copy constructor - GOOD!
    BrokenBuffer(const BrokenBuffer& other) : size(other.size) {
        data = new unsigned char[size];
        std::copy(other.data, other.data + size, data);
        std::cout << "  [Buffer copied: " << size << " bytes]\n";
    }
    
    // Copy assignment - GOOD!
    BrokenBuffer& operator=(const BrokenBuffer& other) {
        if (this != &other) {
            delete[] data;
            size = other.size;
            data = new unsigned char[size];
            std::copy(other.data, other.data + size, data);
            std::cout << "  [Buffer copy-assigned: " << size << " bytes]\n";
        }
        return *this;
    }
    
    // BUG: No move constructor! Expensive copies when moving!
    // BUG: No move assignment! Same problem!
    
    size_t getSize() const { return size; }
};

void demonstrateRuleOfFiveViolation() {
    std::cout << "\n=== RULE OF FIVE VIOLATION ===\n\n";
    
    std::cout << "Creating large buffer:\n";
    BrokenBuffer b1(1000000);  // 1MB
    
    std::cout << "\nMoving to b2 (but it copies because no move ctor!):\n";
    BrokenBuffer b2 = std::move(b1);  // This COPIES because no move ctor!
    
    std::cout << "\nBoth buffers still valid (wasteful!):\n";
    std::cout << "  b1 size: " << b1.getSize() << " (should be 0 if moved)\n";
    std::cout << "  b2 size: " << b2.getSize() << "\n";
}

// ============================================
// BETTER: RULE OF ZERO
// ============================================

class ModernString {
private:
    std::unique_ptr<char[]> data;
    size_t length;
    
public:
    ModernString(const char* str = "") : length(strlen(str)) {
        data = std::make_unique<char[]>(length + 1);
        strcpy(data.get(), str);
        std::cout << "  [Modern constructed: \"" << data.get() << "\"]\n";
    }
    
    // No destructor needed! unique_ptr handles it!
    // No copy constructor needed... wait, this doesn't compile for copy!
    
    // Actually, unique_ptr is move-only, so this class is move-only too.
    // If you want copies, you need to implement them manually
    // OR use shared_ptr (but that has overhead).
    
    void print() const {
        std::cout << "  Modern String: \"" << (data ? data.get() : "null") << "\"\n";
    }
};

// ============================================
// BUG: WRONG SELF-ASSIGNMENT HANDLING
// ============================================

class BrokenSelfAssign {
private:
    int* data;
    
public:
    BrokenSelfAssign(int value) : data(new int(value)) {}
    
    ~BrokenSelfAssign() { delete data; }
    
    // BUG: What if this == &other?
    BrokenSelfAssign& operator=(const BrokenSelfAssign& other) {
        delete data;  // Delete our data first
        // BUG: If this == &other, we just deleted the data we're about to copy!
        data = new int(*other.data);  // Copy... but other.data was just deleted!
        return *this;
    }
    
    int get() const { return *data; }
};

void demonstrateSelfAssignBug() {
    std::cout << "\n=== SELF-ASSIGNMENT BUG ===\n\n";
    
    BrokenSelfAssign obj(42);
    std::cout << "  Before self-assign: " << obj.get() << "\n";
    
    // obj = obj;  // This would crash or read garbage!
    std::cout << "  (Self-assignment commented out - would crash!)\n";
}

// ============================================
// YOUR TASK
// ============================================

/*
 * TODO: Create solution_resource_class.cpp with:
 * 
 * 1. FixedString following Rule of Five:
 *    
 *    class FixedString {
 *        char* data;
 *        size_t length;
 *        
 *    public:
 *        FixedString(const char* str);
 *        ~FixedString();
 *        
 *        // Copy operations (deep copy)
 *        FixedString(const FixedString& other);
 *        FixedString& operator=(const FixedString& other);
 *        
 *        // Move operations
 *        FixedString(FixedString&& other) noexcept;
 *        FixedString& operator=(FixedString&& other) noexcept;
 *    };
 * 
 * 2. Rule of Zero alternative:
 *    
 *    class ZeroString {
 *        std::string data;  // std::string handles everything!
 *        // No special member functions needed!
 *    };
 * 
 * 3. Copy-and-swap idiom for exception safety:
 *    
 *    FixedString& operator=(FixedString other) {  // By value = copy/move
 *        swap(*this, other);
 *        return *this;
 *    }
 * 
 * TRICKY QUESTIONS:
 * 
 * Q1: Why should move operations be noexcept?
 *     (Hint: std::vector reallocation)
 * 
 * Q2: In copy-and-swap, why take the parameter by value?
 *     What happens for lvalues vs rvalues?
 * 
 * Q3: Rule of Zero says "use smart pointers". But unique_ptr
 *     is move-only. How do you make a copyable class with 
 *     unique_ptr members?
 * 
 * Q4: Is this correct?
 *     
 *     ~MyClass() noexcept = default;
 *     
 *     What about:
 *     
 *     ~MyClass() = default;
 * 
 * Q5: BONUS - Implement a "copy-on-write" string:
 *     - Multiple instances can share the same data
 *     - Only copy when modification is needed
 *     - (Hint: use shared_ptr with a write counter)
 * 
 * Q6: What's the difference between:
 *     FixedString(const FixedString&) = default;
 *     FixedString(const FixedString&) = delete;
 */

int main() {
    demonstrateRuleOfThreeViolation();
    demonstrateRuleOfFiveViolation();
    demonstrateSelfAssignBug();
    
    std::cout << "\n========================================\n";
    std::cout << "Apply the correct rule (3, 5, or 0)!\n";
    std::cout << "========================================\n";
    
    return 0;
}
