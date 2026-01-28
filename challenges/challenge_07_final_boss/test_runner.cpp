/**
 * Test Runner for Challenge 7: Final Boss
 */

#include <iostream>
#include <functional>
#include <vector>
#include <memory>
#include <cstring>

class TestRunner {
private:
    struct TestCase {
        std::string name;
        std::function<bool()> test;
    };
    std::vector<TestCase> tests;
    int passed = 0;
    int failed = 0;
    
public:
    void addTest(const std::string& name, std::function<bool()> test) {
        tests.push_back({name, test});
    }
    
    void runAll() {
        std::cout << "\n╔════════════════════════════════════════╗\n";
        std::cout << "║     CHALLENGE 7: FINAL BOSS TESTS      ║\n";
        std::cout << "╚════════════════════════════════════════╝\n\n";
        
        for (const auto& tc : tests) {
            std::cout << "► Testing: " << tc.name << "\n";
            try {
                bool result = tc.test();
                if (result) {
                    std::cout << "  ✓ PASSED\n\n";
                    passed++;
                } else {
                    std::cout << "  ✗ FAILED\n\n";
                    failed++;
                }
            } catch (const std::exception& e) {
                std::cout << "  ✗ EXCEPTION: " << e.what() << "\n\n";
                failed++;
            }
        }
        
        std::cout << "════════════════════════════════════════\n";
        std::cout << "Results: " << passed << " passed, " << failed << " failed\n";
        std::cout << "════════════════════════════════════════\n";
        
        if (failed == 0) {
            std::cout << "\n🎉 CONGRATULATIONS! You've defeated the Final Boss!\n";
            std::cout << "You are now a C++ Master!\n";
        }
    }
};

// Test: Deep copy creates independent copy
bool testDeepCopy() {
    std::cout << "  → Testing deep copy creates independent copy\n";
    
    class DeepArray {
        int* data;
        size_t size;
    public:
        DeepArray(size_t n) : size(n), data(new int[n]) {
            for (size_t i = 0; i < n; i++) data[i] = static_cast<int>(i);
        }
        DeepArray(const DeepArray& other) : size(other.size), data(new int[other.size]) {
            std::copy(other.data, other.data + size, data);
        }
        ~DeepArray() { delete[] data; }
        
        void set(size_t i, int v) { if (i < size) data[i] = v; }
        int get(size_t i) const { return (i < size) ? data[i] : -1; }
    };
    
    DeepArray original(5);
    DeepArray copy = original;
    
    copy.set(0, 999);
    
    // Original should be unchanged
    return original.get(0) == 0 && copy.get(0) == 999;
}

// Test: Move semantics work
bool testMoveSemantics() {
    std::cout << "  → Testing move semantics\n";
    
    class Movable {
        std::unique_ptr<int> data;
    public:
        Movable(int v) : data(std::make_unique<int>(v)) {}
        Movable(Movable&& other) noexcept = default;
        Movable& operator=(Movable&& other) noexcept = default;
        
        bool hasData() const { return data != nullptr; }
        int get() const { return data ? *data : -1; }
    };
    
    Movable m1(42);
    Movable m2 = std::move(m1);
    
    return !m1.hasData() && m2.get() == 42;
}

// Test: Self-assignment is handled
bool testSelfAssignment() {
    std::cout << "  → Testing self-assignment handling\n";
    
    class SafeClass {
        int* data;
    public:
        SafeClass(int v) : data(new int(v)) {}
        SafeClass(const SafeClass& other) : data(new int(*other.data)) {}
        SafeClass& operator=(const SafeClass& other) {
            if (this != &other) {
                int* newData = new int(*other.data);
                delete data;
                data = newData;
            }
            return *this;
        }
        ~SafeClass() { delete data; }
        int get() const { return *data; }
    };
    
    SafeClass obj(42);
    obj = obj;  // Self-assignment
    
    return obj.get() == 42;
}

// Test: Copy-and-swap works
bool testCopyAndSwap() {
    std::cout << "  → Testing copy-and-swap idiom\n";
    
    class SwapClass {
        int* data;
        size_t size;
        
        friend void swap(SwapClass& a, SwapClass& b) noexcept {
            using std::swap;
            swap(a.data, b.data);
            swap(a.size, b.size);
        }
    public:
        SwapClass(int v) : size(1), data(new int[1]{v}) {}
        SwapClass(const SwapClass& other) : size(other.size), data(new int[other.size]) {
            std::copy(other.data, other.data + size, data);
        }
        SwapClass& operator=(SwapClass other) {  // By value!
            swap(*this, other);
            return *this;
        }
        ~SwapClass() { delete[] data; }
        int get() const { return data[0]; }
    };
    
    SwapClass a(10), b(20);
    a = b;
    
    return a.get() == 20;
}

// Test: Rule of Zero with unique_ptr
bool testRuleOfZero() {
    std::cout << "  → Testing Rule of Zero with unique_ptr\n";
    
    class ZeroClass {
        std::unique_ptr<int> data;
    public:
        ZeroClass(int v) : data(std::make_unique<int>(v)) {}
        // No destructor, copy ops, or move ops defined!
        // But it's move-only (unique_ptr is not copyable)
        
        int get() const { return *data; }
    };
    
    ZeroClass a(42);
    ZeroClass b = std::move(a);  // Move works
    
    return b.get() == 42;
}

// Test: noexcept move operations
bool testNoexceptMove() {
    std::cout << "  → Testing noexcept move operations\n";
    
    class Noexcept {
        int* data;
    public:
        Noexcept() : data(new int(0)) {}
        Noexcept(Noexcept&& other) noexcept : data(other.data) {
            other.data = nullptr;
        }
        ~Noexcept() { delete data; }
        
        static constexpr bool is_nothrow_move = 
            std::is_nothrow_move_constructible_v<Noexcept>;
    };
    
    std::cout << "    Move constructor is noexcept: " << Noexcept::is_nothrow_move << "\n";
    
    return Noexcept::is_nothrow_move;
}

int main() {
    TestRunner runner;
    
    runner.addTest("Deep Copy Independence", testDeepCopy);
    runner.addTest("Move Semantics", testMoveSemantics);
    runner.addTest("Self-Assignment Safety", testSelfAssignment);
    runner.addTest("Copy-and-Swap Idiom", testCopyAndSwap);
    runner.addTest("Rule of Zero", testRuleOfZero);
    runner.addTest("noexcept Move Operations", testNoexceptMove);
    
    runner.runAll();
    
    return 0;
}
