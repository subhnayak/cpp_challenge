/**
 * Test Runner for Challenge 4: Event Bus
 */

#include <iostream>
#include <functional>
#include <vector>
#include <memory>
#include <string>

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
        std::cout << "║      CHALLENGE 4: EVENT BUS TESTS      ║\n";
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

// Test: Lambda captures work correctly
bool testLambdaValueCapture() {
    std::cout << "  → Testing value capture survives scope\n";
    
    std::function<int()> captured;
    
    {
        int value = 42;
        captured = [value]() { return value; };
    }
    // value is gone, but captured copy should work
    
    return captured() == 42;
}

// Test: Lambda mutable capture
bool testMutableCapture() {
    std::cout << "  → Testing mutable lambda state\n";
    
    auto counter = [count = 0]() mutable { return ++count; };
    
    int r1 = counter();
    int r2 = counter();
    int r3 = counter();
    
    std::cout << "    Counts: " << r1 << ", " << r2 << ", " << r3 << "\n";
    
    return r1 == 1 && r2 == 2 && r3 == 3;
}

// Test: Move capture works
bool testMoveCapture() {
    std::cout << "  → Testing move capture\n";
    
    auto ptr = std::make_unique<int>(100);
    
    auto lambda = [p = std::move(ptr)]() {
        return *p;
    };
    
    // ptr should be null now
    bool ptrMoved = (ptr == nullptr);
    bool valuePreserved = (lambda() == 100);
    
    std::cout << "    Pointer moved: " << ptrMoved << ", Value preserved: " << valuePreserved << "\n";
    
    return ptrMoved && valuePreserved;
}

// Test: Move semantics avoid copies
bool testMoveSemantics() {
    std::cout << "  → Testing move avoids copies\n";
    
    int copyCount = 0;
    int moveCount = 0;
    
    struct Counter {
        int* copies;
        int* moves;
        
        Counter(int* c, int* m) : copies(c), moves(m) {}
        Counter(const Counter& o) : copies(o.copies), moves(o.moves) { (*copies)++; }
        Counter(Counter&& o) noexcept : copies(o.copies), moves(o.moves) { (*moves)++; }
    };
    
    Counter c(&copyCount, &moveCount);
    Counter c2 = std::move(c);  // Should move
    
    std::cout << "    Copies: " << copyCount << ", Moves: " << moveCount << "\n";
    
    return moveCount >= 1 && copyCount == 0;
}

// Test: Perfect forwarding
bool testPerfectForwarding() {
    std::cout << "  → Testing perfect forwarding\n";
    
    bool lvalueReceived = false;
    bool rvalueReceived = false;
    
    auto forwarder = [&]<typename T>(T&& arg) {
        if constexpr (std::is_lvalue_reference_v<T>) {
            lvalueReceived = true;
        } else {
            rvalueReceived = true;
        }
    };
    
    int x = 5;
    forwarder(x);      // lvalue
    forwarder(5);      // rvalue
    
    std::cout << "    Lvalue: " << lvalueReceived << ", Rvalue: " << rvalueReceived << "\n";
    
    return lvalueReceived && rvalueReceived;
}

// Test: weak_ptr for safe callbacks
bool testWeakPtrCallback() {
    std::cout << "  → Testing weak_ptr for safe callbacks\n";
    
    std::function<bool()> callback;
    
    {
        auto shared = std::make_shared<int>(42);
        std::weak_ptr<int> weak = shared;
        
        callback = [weak]() {
            if (auto locked = weak.lock()) {
                return true;  // Object still alive
            }
            return false;  // Object gone
        };
        
        // Object still alive
        if (!callback()) return false;
    }
    
    // Object destroyed, callback should return false
    return !callback();
}

int main() {
    TestRunner runner;
    
    runner.addTest("Lambda Value Capture", testLambdaValueCapture);
    runner.addTest("Mutable Capture State", testMutableCapture);
    runner.addTest("Move Capture", testMoveCapture);
    runner.addTest("Move Semantics", testMoveSemantics);
    runner.addTest("Perfect Forwarding", testPerfectForwarding);
    runner.addTest("weak_ptr Callbacks", testWeakPtrCallback);
    
    runner.runAll();
    
    return 0;
}
