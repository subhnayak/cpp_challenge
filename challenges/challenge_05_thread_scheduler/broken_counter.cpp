/**
 * 🔴 BROKEN CODE - Race Condition in Counter
 * 
 * Multiple threads increment a shared counter.
 * The final count is often WRONG!
 * 
 * YOUR TASK: Fix the race condition!
 */

#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>
#include <chrono>

// ============================================
// BUG: UNSYNCHRONIZED COUNTER
// ============================================

class BrokenCounter {
private:
    int count = 0;  // BUG: Not thread-safe!
    
public:
    void increment() {
        // BUG: This is NOT atomic!
        // It's actually: temp = count; temp++; count = temp;
        count++;  // Read-modify-write = 3 operations!
    }
    
    void add(int value) {
        // BUG: Same problem!
        count += value;
    }
    
    int get() const {
        return count;  // BUG: Might read partial value on some platforms!
    }
    
    void reset() {
        count = 0;
    }
};

// ============================================
// DEMONSTRATION OF THE RACE CONDITION
// ============================================

void demonstrateRaceCondition() {
    std::cout << "=== RACE CONDITION DEMO ===\n\n";
    
    BrokenCounter counter;
    const int numThreads = 10;
    const int incrementsPerThread = 100000;
    const int expectedTotal = numThreads * incrementsPerThread;
    
    std::cout << "Starting " << numThreads << " threads, each incrementing " 
              << incrementsPerThread << " times\n";
    std::cout << "Expected total: " << expectedTotal << "\n\n";
    
    // Run multiple trials
    for (int trial = 0; trial < 5; trial++) {
        counter.reset();
        
        std::vector<std::thread> threads;
        
        for (int i = 0; i < numThreads; i++) {
            threads.emplace_back([&counter, incrementsPerThread]() {
                for (int j = 0; j < incrementsPerThread; j++) {
                    counter.increment();
                }
            });
        }
        
        for (auto& t : threads) {
            t.join();
        }
        
        int actual = counter.get();
        std::cout << "Trial " << (trial + 1) << ": Got " << actual;
        
        if (actual != expectedTotal) {
            std::cout << " ❌ (lost " << (expectedTotal - actual) << " increments!)";
        } else {
            std::cout << " ✓";
        }
        std::cout << "\n";
    }
}

// ============================================
// BROKEN MUTEX ATTEMPT
// ============================================

class BrokenMutexCounter {
private:
    int count = 0;
    std::mutex mtx;
    
public:
    void increment() {
        // BUG: What if an exception is thrown before unlock?
        mtx.lock();
        count++;
        // If something throws here, mutex is never unlocked = DEADLOCK!
        mtx.unlock();
    }
    
    // BUG: Forgot to lock!
    int get() const {
        return count;  // Data race when reading while another thread writes!
    }
    
    // BUG: Double-lock attempt
    void addTwice() {
        mtx.lock();
        count++;
        // BUG: Trying to lock again = DEADLOCK!
        // mtx.lock();  // Would hang forever
        count++;
        mtx.unlock();
    }
    
    void reset() {
        count = 0;
    }
};

// ============================================
// WRONG ATOMIC USAGE
// ============================================

class BrokenAtomicCounter {
private:
    std::atomic<int> count{0};
    
public:
    // This is correct!
    void increment() {
        count++;  // Atomic operation
    }
    
    // BUG: Check-then-act is NOT atomic!
    void incrementIfBelow(int limit) {
        if (count < limit) {  // Check
            count++;           // Act - but another thread might have incremented!
        }
        // Another thread could have incremented count between check and act!
    }
    
    // BUG: Multiple operations on atomic aren't atomic together
    void addTwoIfBothBelowLimit(std::atomic<int>& other, int limit) {
        if (count < limit && other < limit) {
            count++;   // Atomic individually...
            other++;   // ...but not together!
        }
        // Another thread could observe count incremented but not other!
    }
    
    int get() const {
        return count.load();
    }
    
    void reset() {
        count.store(0);
    }
};

void demonstrateBrokenAtomic() {
    std::cout << "\n=== BROKEN ATOMIC CHECK-THEN-ACT ===\n\n";
    
    BrokenAtomicCounter counter;
    const int limit = 1000;
    
    std::cout << "Attempting to increment to " << limit << " (not past!)\n";
    std::cout << "Using 10 threads with check-then-act...\n\n";
    
    for (int trial = 0; trial < 5; trial++) {
        counter.reset();
        
        std::vector<std::thread> threads;
        
        for (int i = 0; i < 10; i++) {
            threads.emplace_back([&counter, limit]() {
                for (int j = 0; j < 200; j++) {
                    counter.incrementIfBelow(limit);
                }
            });
        }
        
        for (auto& t : threads) {
            t.join();
        }
        
        int actual = counter.get();
        std::cout << "Trial " << (trial + 1) << ": Got " << actual;
        
        if (actual > limit) {
            std::cout << " ❌ (exceeded limit by " << (actual - limit) << "!)";
        } else if (actual == limit) {
            std::cout << " ✓ (exactly at limit)";
        } else {
            std::cout << " (below limit)";
        }
        std::cout << "\n";
    }
}

// ============================================
// YOUR TASK
// ============================================

/*
 * TODO: Create solution_counter.cpp with:
 * 
 * 1. FixedMutexCounter using std::lock_guard:
 *    
 *    void increment() {
 *        std::lock_guard<std::mutex> lock(mtx);
 *        count++;
 *    }  // Automatically unlocked here, even on exception!
 * 
 * 2. FixedAtomicCounter using proper atomic operations:
 *    
 *    // For simple increment:
 *    count.fetch_add(1);
 *    
 *    // For check-then-act, use compare_exchange:
 *    void incrementIfBelow(int limit) {
 *        int current = count.load();
 *        while (current < limit) {
 *            if (count.compare_exchange_weak(current, current + 1)) {
 *                return;  // Success!
 *            }
 *            // current is updated to new value, try again
 *        }
 *    }
 * 
 * 3. Benchmark both solutions!
 * 
 * TRICKY QUESTIONS:
 * 
 * Q1: What's the difference between:
 *     count++
 *     count.fetch_add(1)
 *     count.fetch_add(1, std::memory_order_relaxed)
 * 
 * Q2: compare_exchange_weak vs compare_exchange_strong:
 *     - weak can fail spuriously (even if value matches)
 *     - strong never fails spuriously
 *     When would you use weak? (Hint: loops)
 * 
 * Q3: Is std::atomic<T> always lock-free?
 *     How do you check? (Hint: is_lock_free())
 *     What types are usually lock-free?
 * 
 * Q4: std::mutex vs std::atomic - which is faster for:
 *     - Simple counter increment?
 *     - Complex data structure update?
 *     - High contention vs low contention?
 * 
 * Q5: BONUS - Implement a "batch counter" that:
 *     - Threads batch their increments locally
 *     - Periodically flush to shared atomic
 *     - Provides "eventual" count accuracy
 *     
 *     This is faster but less precise. When is this acceptable?
 */

int main() {
    demonstrateRaceCondition();
    demonstrateBrokenAtomic();
    
    std::cout << "\n========================================\n";
    std::cout << "Fix the race conditions!\n";
    std::cout << "========================================\n";
    
    return 0;
}
