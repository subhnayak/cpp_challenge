/**
 * ✅ SOLUTION - Thread Scheduler: Atomics & Synchronization
 * 
 * Demonstrates:
 * - std::atomic for race condition protection
 * - std::mutex for mutual exclusion
 * - Memory ordering semantics
 * - Lock-free programming
 */

#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include <chrono>

// ============================================
// SOLUTION: ATOMIC COUNTER
// ============================================

class ThreadSafeCounter {
private:
    std::atomic<int> value{0};  // Atomic: thread-safe!
    
public:
    void increment() {
        value.fetch_add(1, std::memory_order_relaxed);
    }
    
    int get() const {
        return value.load(std::memory_order_acquire);
    }
    
    void reset() {
        value.store(0, std::memory_order_release);
    }
};

// ============================================
// SOLUTION: AVOIDING DEADLOCK
// ============================================

class ThreadSafeResource {
private:
    std::mutex mtxA, mtxB;
    int dataA = 0, dataB = 0;
    
public:
    void transferData() {
        // Use scoped_lock to avoid deadlock (automatic ordering)
        std::scoped_lock lock(mtxA, mtxB);
        
        // Safe: locks acquired in consistent order
        dataA += 10;
        dataB -= 10;
    }
    
    int getSum() {
        std::scoped_lock lock(mtxA, mtxB);
        return dataA + dataB;
    }
};

// ============================================
// SOLUTION: MEMORY ORDERING
// ============================================

class MemoryOrderedFlag {
private:
    int data = 0;
    std::atomic<bool> ready{false};
    
public:
    void writer() {
        data = 42;
        // Release: "publish" the data
        ready.store(true, std::memory_order_release);
    }
    
    void reader() {
        // Acquire: wait and synchronize with release
        while (!ready.load(std::memory_order_acquire)) {
            std::this_thread::yield();
        }
        std::cout << "Read data: " << data << "\n";  // Always sees 42!
    }
};

// ============================================
// SOLUTION: READER-WRITER LOCK
// ============================================

class ReadWriteLock {
private:
    std::shared_mutex mtx;
    int sharedData = 0;
    
public:
    int read() {
        // Multiple readers can acquire simultaneously
        std::shared_lock<std::shared_mutex> lock(mtx);
        return sharedData;
    }
    
    void write(int value) {
        // Exclusive access for writer
        std::unique_lock<std::shared_mutex> lock(mtx);
        sharedData = value;
    }
};

// ============================================
// DEMONSTRATIONS
// ============================================

void demonstrateAtomics() {
    std::cout << "\n=== ATOMIC COUNTER ===\n";
    
    ThreadSafeCounter counter;
    
    auto increment_task = [&counter]() {
        for (int i = 0; i < 100000; ++i) {
            counter.increment();
        }
    };
    
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back(increment_task);
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    std::cout << "Final count: " << counter.get() << "\n";
    std::cout << "Expected: 400000\n";
    std::cout << (counter.get() == 400000 ? "✅ Correct!\n" : "❌ Race condition!\n");
}

void demonstrateMemoryOrdering() {
    std::cout << "\n=== MEMORY ORDERING ===\n";
    
    MemoryOrderedFlag flag;
    
    std::thread writer([&flag]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        flag.writer();
    });
    
    std::thread reader([&flag]() {
        flag.reader();
    });
    
    reader.join();
    writer.join();
    
    std::cout << "✅ Correctly synchronized!\n";
}

void demonstrateReadWriteLock() {
    std::cout << "\n=== READ-WRITE LOCK ===\n";
    
    ReadWriteLock rwl;
    
    auto reader_task = [&rwl](int id) {
        for (int i = 0; i < 5; ++i) {
            int value = rwl.read();
            std::cout << "  Reader " << id << " read: " << value << "\n";
        }
    };
    
    auto writer_task = [&rwl](int value) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        rwl.write(value);
        std::cout << "  Writer wrote: " << value << "\n";
    };
    
    std::vector<std::thread> threads;
    threads.emplace_back(reader_task, 1);
    threads.emplace_back(reader_task, 2);
    threads.emplace_back(writer_task, 42);
    
    for (auto& t : threads) {
        t.join();
    }
    
    std::cout << "✅ Multiple readers allowed!\n";
}

int main() {
    demonstrateAtomics();
    demonstrateMemoryOrdering();
    demonstrateReadWriteLock();
    
    std::cout << "\n✅ All concurrency patterns working!\n";
    
    return 0;
}
