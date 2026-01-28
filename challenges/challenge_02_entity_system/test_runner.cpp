/**
 * Test Runner for Challenge 2: Entity System
 */

#include <iostream>
#include <functional>
#include <vector>
#include <memory>

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
        std::cout << "║    CHALLENGE 2: ENTITY SYSTEM TESTS    ║\n";
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
    }
};

// Test virtual inheritance resolves diamond
bool testDiamondResolved() {
    std::cout << "  → Verify only one base class instance exists\n";
    // Include your solution and test that FlyingWarrior has only one Entity
    return true;  // Placeholder
}

// Test virtual destructor chain
bool testVirtualDestructor() {
    std::cout << "  → Verify all destructors are called\n";
    // Track destructor calls with a counter
    return true;  // Placeholder
}

// Test covariant return types
bool testCovariantReturn() {
    std::cout << "  → Verify clone returns correct type\n";
    return true;  // Placeholder
}

// Test CRTP performance
bool testCRTPCorrectness() {
    std::cout << "  → Verify CRTP calls derived implementation\n";
    return true;  // Placeholder
}

// Test abstract class properties
bool testAbstractClass() {
    std::cout << "  → Verify abstract class cannot be instantiated\n";
    // EntityFactory should not compile if you try to instantiate it
    return true;  // Placeholder
}

// Test vtable existence
bool testVTableBasics() {
    std::cout << "  → Basic vtable verification\n";
    
    struct NoVirtual { int x; };
    struct WithVirtual { virtual void f() {} int x; };
    
    std::cout << "    Size without virtual: " << sizeof(NoVirtual) << "\n";
    std::cout << "    Size with virtual: " << sizeof(WithVirtual) << "\n";
    
    // The virtual version should be larger due to vptr
    return sizeof(WithVirtual) > sizeof(NoVirtual);
}

// Test dynamic_cast behavior
bool testDynamicCast() {
    std::cout << "  → Testing dynamic_cast\n";
    
    struct Base { virtual ~Base() = default; };
    struct Derived : Base { int extra; };
    struct Other : Base { float other; };
    
    Base* d = new Derived();
    Base* o = new Other();
    
    // dynamic_cast to correct type should succeed
    Derived* dd = dynamic_cast<Derived*>(d);
    // dynamic_cast to wrong type should return nullptr
    Derived* od = dynamic_cast<Derived*>(o);
    
    bool result = (dd != nullptr) && (od == nullptr);
    
    delete d;
    delete o;
    
    return result;
}

int main() {
    TestRunner runner;
    
    runner.addTest("Diamond Problem Resolved", testDiamondResolved);
    runner.addTest("Virtual Destructor Chain", testVirtualDestructor);
    runner.addTest("Covariant Return Types", testCovariantReturn);
    runner.addTest("CRTP Correctness", testCRTPCorrectness);
    runner.addTest("Abstract Class", testAbstractClass);
    runner.addTest("VTable Basics", testVTableBasics);
    runner.addTest("Dynamic Cast", testDynamicCast);
    
    runner.runAll();
    
    return 0;
}
