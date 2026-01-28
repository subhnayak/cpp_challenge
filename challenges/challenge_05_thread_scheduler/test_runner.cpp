/**
 * Test Runner for Challenge 5: Thread Scheduler
 */

#include <iostream>
#include <functional>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>

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
        std::cout << "║  CHALLENGE 5: THREAD SCHEDULER TESTS   ║\n";
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

// Test: Atomic increment is correct
bool testAtomicIncrement() {
    std::cout << "  → Testing atomic increment\n";
    
    std::atomic<int> counter{0};
    const int numThreads = 10;
    const int incrementsPerThread = 10000;
    
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; i++) {
        threads.emplace_back([&counter, incrementsPerThread]() {
            for (int j = 0; j < incrementsPerThread; j++) {
                counter++;
            }
        });
    }
    
    for (auto& t : threads) t.join();
    
    int expected = numThreads * incrementsPerThread;
    int actual = counter.load();
    
    std::cout << "    Expected: " << expected << ", Got: " << actual << "\n";
    
    return actual == expected;
}

// Test: Mutex protects shared data
bool testMutexProtection() {
    std::cout << "  → Testing mutex protection\n";
    
    int counter = 0;
    std::mutex mtx;
    const int numThreads = 10;
    const int incrementsPerThread = 10000;
    
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; i++) {
        threads.emplace_back([&counter, &mtx, incrementsPerThread]() {
            for (int j = 0; j < incrementsPerThread; j++) {
                std::lock_guard<std::mutex> lock(mtx);
                counter++;
            }
        });
    }
    
    for (auto& t : threads) t.join();
    
    int expected = numThreads * incrementsPerThread;
    
    std::cout << "    Expected: " << expected << ", Got: " << counter << "\n";
    
    return counter == expected;
}

// Test: scoped_lock prevents deadlock
bool testScopedLock() {
    std::cout << "  → Testing scoped_lock for deadlock prevention\n";
    
    std::mutex mtx1, mtx2;
    std::atomic<bool> success{true};
    
    auto threadFunc1 = [&]() {
        for (int i = 0; i < 1000; i++) {
            std::scoped_lock lock(mtx1, mtx2);
            // Do nothing, just test locking
        }
    };
    
    auto threadFunc2 = [&]() {
        for (int i = 0; i < 1000; i++) {
            std::scoped_lock lock(mtx2, mtx1);  // Opposite order - no deadlock!
        }
    };
    
    std::thread t1(threadFunc1);
    std::thread t2(threadFunc2);
    
    t1.join();
    t2.join();
    
    std::cout << "    Completed without deadlock\n";
    
    return success;
}

// Test: is_lock_free check
bool testAtomicLockFree() {
    std::cout << "  → Testing atomic lock-free status\n";
    
    std::atomic<int> atomicInt;
    std::atomic<bool> atomicBool;
    std::atomic<long long> atomicLongLong;
    
    std::cout << "    atomic<int> lock-free: " << atomicInt.is_lock_free() << "\n";
    std::cout << "    atomic<bool> lock-free: " << atomicBool.is_lock_free() << "\n";
    std::cout << "    atomic<long long> lock-free: " << atomicLongLong.is_lock_free() << "\n";
    
    // Usually true on modern systems
    return atomicInt.is_lock_free();
}

// Test: Compare exchange works correctly
bool testCompareExchange() {
    std::cout << "  → Testing compare_exchange\n";
    
    std::atomic<int> value{10};
    
    // Should succeed - value is 10, expecting 10, setting to 20
    int expected = 10;
    bool success1 = value.compare_exchange_strong(expected, 20);
    
    // Should fail - value is now 20, but expecting 10
    expected = 10;
    bool success2 = value.compare_exchange_strong(expected, 30);
    
    std::cout << "    First CAS (10->20): " << success1 << "\n";
    std::cout << "    Second CAS (10->30): " << success2 << " (expected to fail)\n";
    std::cout << "    'expected' updated to: " << expected << "\n";
    std::cout << "    Final value: " << value.load() << "\n";
    
    return success1 && !success2 && expected == 20 && value.load() == 20;
}

int main() {
    TestRunner runner;
    
    runner.addTest("Atomic Increment", testAtomicIncrement);
    runner.addTest("Mutex Protection", testMutexProtection);
    runner.addTest("scoped_lock Deadlock Prevention", testScopedLock);
    runner.addTest("Atomic Lock-Free Status", testAtomicLockFree);
    runner.addTest("Compare Exchange", testCompareExchange);
    
    runner.runAll();
    
    return 0;
}
