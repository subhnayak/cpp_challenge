/**
 * 🔴 BROKEN CODE - Memory Ordering Issues
 * 
 * Memory ordering determines how atomic operations synchronize
 * between threads. Getting it wrong causes subtle bugs!
 * 
 * YOUR TASK: Understand and fix the memory ordering issues!
 */

#include <iostream>
#include <thread>
#include <atomic>
#include <cassert>

// ============================================
// BUG 1: RELAXED ORDERING WITH FLAG SYNC
// ============================================

std::atomic<bool> ready{false};
int data = 0;  // Non-atomic!

void producerBroken() {
    data = 42;  // Write data
    // BUG: memory_order_relaxed doesn't guarantee ordering!
    ready.store(true, std::memory_order_relaxed);
}

void consumerBroken() {
    // BUG: Even if we see ready=true, data might not be 42!
    while (!ready.load(std::memory_order_relaxed)) {
        // Spin wait
    }
    
    // On weakly-ordered CPUs (like ARM), data might still be 0!
    // The write to data might not have propagated yet!
    std::cout << "  Consumer read data = " << data << "\n";
}

void demonstrateRelaxedOrdering() {
    std::cout << "=== RELAXED ORDERING BUG ===\n\n";
    
    for (int trial = 0; trial < 5; trial++) {
        data = 0;
        ready.store(false);
        
        std::thread producer(producerBroken);
        std::thread consumer(consumerBroken);
        
        producer.join();
        consumer.join();
        
        // On x86, this usually works (strong memory model)
        // On ARM, this might fail!
        std::cout << "  Trial " << trial << ": data = " << data;
        if (data != 42) std::cout << " ❌ BUG!";
        std::cout << "\n";
    }
    
    std::cout << "\n  (Might always show 42 on x86, but ARM could fail!)\n";
}

// ============================================
// BUG 2: REORDERING IN FLAG PATTERN
// ============================================

class BrokenSpinlock {
private:
    std::atomic<bool> locked{false};
    
public:
    void lock() {
        // BUG: Reordering can cause critical section to execute before lock!
        while (locked.exchange(true, std::memory_order_relaxed)) {
            // Spin
        }
        // Memory operations AFTER this might be reordered BEFORE the exchange!
    }
    
    void unlock() {
        // BUG: Critical section might be reordered AFTER this!
        locked.store(false, std::memory_order_relaxed);
    }
};

int sharedValue = 0;

void demonstrateSpinlockReorder() {
    std::cout << "\n=== SPINLOCK REORDERING BUG ===\n\n";
    
    BrokenSpinlock spinlock;
    const int numThreads = 4;
    const int incrementsPerThread = 10000;
    
    for (int trial = 0; trial < 3; trial++) {
        sharedValue = 0;
        
        std::vector<std::thread> threads;
        for (int i = 0; i < numThreads; i++) {
            threads.emplace_back([&spinlock, incrementsPerThread]() {
                for (int j = 0; j < incrementsPerThread; j++) {
                    spinlock.lock();
                    sharedValue++;  // Might be reordered outside lock!
                    spinlock.unlock();
                }
            });
        }
        
        for (auto& t : threads) t.join();
        
        int expected = numThreads * incrementsPerThread;
        std::cout << "  Trial " << trial << ": value = " << sharedValue
                  << " (expected " << expected << ")";
        if (sharedValue != expected) std::cout << " ❌";
        std::cout << "\n";
    }
}

// ============================================
// BUG 3: SEQ_CST OVERKILL
// ============================================

std::atomic<int> counter1{0};
std::atomic<int> counter2{0};

void demonstrateSeqCstOverkill() {
    std::cout << "\n=== SEQ_CST PERFORMANCE IMPACT ===\n\n";
    
    // Sequential consistency is safe but slow
    auto seqCstIncrement = [](std::atomic<int>& c, int n) {
        for (int i = 0; i < n; i++) {
            c.fetch_add(1, std::memory_order_seq_cst);
        }
    };
    
    // Relaxed is faster but unsafe for synchronization
    auto relaxedIncrement = [](std::atomic<int>& c, int n) {
        for (int i = 0; i < n; i++) {
            c.fetch_add(1, std::memory_order_relaxed);
        }
    };
    
    const int N = 1000000;
    
    auto start1 = std::chrono::high_resolution_clock::now();
    seqCstIncrement(counter1, N);
    auto end1 = std::chrono::high_resolution_clock::now();
    
    auto start2 = std::chrono::high_resolution_clock::now();
    relaxedIncrement(counter2, N);
    auto end2 = std::chrono::high_resolution_clock::now();
    
    std::cout << "  seq_cst time: " 
              << std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1).count() 
              << " µs\n";
    std::cout << "  relaxed time: " 
              << std::chrono::duration_cast<std::chrono::microseconds>(end2 - start2).count() 
              << " µs\n";
    std::cout << "\n  (Relaxed is faster but not always safe!)\n";
}

// ============================================
// BUG 4: ACQUIRE-RELEASE MISMATCH
// ============================================

std::atomic<int> syncVar{0};
int payload = 0;

void writerMismatch() {
    payload = 123;
    // Writer uses release...
    syncVar.store(1, std::memory_order_release);
}

void readerMismatch() {
    // BUG: Reader uses relaxed instead of acquire!
    while (syncVar.load(std::memory_order_relaxed) != 1) {}
    
    // payload might not be 123 because relaxed doesn't synchronize!
    std::cout << "  Reader got payload = " << payload << "\n";
}

// ============================================
// YOUR TASK
// ============================================

/*
 * TODO: Create solution_memory_order.cpp with:
 * 
 * 1. Fixed producer-consumer using acquire-release:
 *    
 *    // Producer
 *    data = 42;
 *    ready.store(true, std::memory_order_release);  // Release
 *    
 *    // Consumer
 *    while (!ready.load(std::memory_order_acquire)) {}  // Acquire
 *    use(data);  // Guaranteed to see data=42!
 * 
 * 2. Fixed spinlock:
 *    
 *    void lock() {
 *        while (locked.exchange(true, std::memory_order_acquire)) {}
 *    }
 *    void unlock() {
 *        locked.store(false, std::memory_order_release);
 *    }
 * 
 * 3. Understand when each memory order is appropriate:
 *    - relaxed: Counter increment (no synchronization needed)
 *    - acquire: Reading a flag/lock
 *    - release: Writing a flag/lock
 *    - acq_rel: Read-modify-write (like exchange in lock)
 *    - seq_cst: When you need total ordering (rare)
 * 
 * TRICKY QUESTIONS:
 * 
 * Q1: What does "happens-before" mean in C++ memory model?
 *     
 * Q2: On x86, most memory operations have acquire-release semantics
 *     by default. Why does the code still need to specify them?
 *     (Hint: compiler reordering)
 * 
 * Q3: What's the difference between:
 *     - std::atomic_thread_fence(memory_order_acquire)
 *     - load(memory_order_acquire)
 *     When would you use a fence?
 * 
 * Q4: Can you implement a mutex using just atomics and memory ordering?
 *     What primitive operations do you need?
 * 
 * Q5: BONUS - Implement a "SPSC queue" (single producer, single consumer)
 *     using only acquire-release semantics. This is a common lock-free
 *     pattern!
 * 
 * MEMORY ORDER CHEAT SHEET:
 * 
 * relaxed:  No synchronization, only atomicity
 * consume:  (Deprecated, avoid)
 * acquire:  No reads/writes can be reordered BEFORE this
 * release:  No reads/writes can be reordered AFTER this  
 * acq_rel:  Both acquire and release
 * seq_cst:  Total ordering (strictest, slowest)
 */

int main() {
    demonstrateRelaxedOrdering();
    demonstrateSpinlockReorder();
    demonstrateSeqCstOverkill();
    
    std::cout << "\n========================================\n";
    std::cout << "Fix the memory ordering issues!\n";
    std::cout << "========================================\n";
    
    return 0;
}
