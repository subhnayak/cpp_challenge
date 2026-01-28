/**
 * 🔴 BROKEN CODE - Deadlock Scenarios
 * 
 * Two or more threads are waiting for each other forever.
 * The game freezes randomly!
 * 
 * YOUR TASK: Identify and fix the deadlocks!
 */

#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <string>

// ============================================
// BUG 1: CLASSIC DEADLOCK - LOCK ORDERING
// ============================================

class BankAccount {
private:
    std::mutex mtx;
    double balance;
    std::string name;
    
public:
    BankAccount(const std::string& n, double initial) 
        : name(n), balance(initial) {}
    
    // BUG: This transfer can deadlock!
    void transferTo(BankAccount& other, double amount) {
        // Lock our mutex first
        std::lock_guard<std::mutex> lockThis(mtx);
        
        std::cout << "  " << name << " locked, waiting for " << other.name << "...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Increase deadlock chance
        
        // Lock their mutex second
        std::lock_guard<std::mutex> lockOther(other.mtx);  // POTENTIAL DEADLOCK!
        
        // If another thread is doing other.transferTo(this), we're stuck!
        
        balance -= amount;
        other.balance += amount;
        
        std::cout << "  Transferred $" << amount << " from " << name << " to " << other.name << "\n";
    }
    
    double getBalance() {
        std::lock_guard<std::mutex> lock(mtx);
        return balance;
    }
    
    std::mutex& getMutex() { return mtx; }
    const std::string& getName() const { return name; }
};

void demonstrateBankDeadlock() {
    std::cout << "=== BANK TRANSFER DEADLOCK ===\n\n";
    
    BankAccount alice("Alice", 1000);
    BankAccount bob("Bob", 1000);
    
    std::cout << "Starting simultaneous transfers...\n";
    std::cout << "(This might deadlock! Wait 5 seconds...)\n\n";
    
    std::thread t1([&]() {
        alice.transferTo(bob, 100);  // Alice -> Bob
    });
    
    std::thread t2([&]() {
        bob.transferTo(alice, 50);   // Bob -> Alice (opposite order!)
    });
    
    // Wait with timeout to detect deadlock
    auto start = std::chrono::steady_clock::now();
    
    bool t1Done = false, t2Done = false;
    
    std::thread monitor([&]() {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        if (!t1Done || !t2Done) {
            std::cout << "\n  ⚠️ DEADLOCK DETECTED! Threads stuck!\n";
            std::cout << "  (In real code, this would hang forever)\n";
            // Note: We can't actually break the deadlock safely here
        }
    });
    
    t1.join();
    t1Done = true;
    
    t2.join();
    t2Done = true;
    
    monitor.join();
    
    std::cout << "\nFinal balances: Alice=$" << alice.getBalance() 
              << ", Bob=$" << bob.getBalance() << "\n";
}

// ============================================
// BUG 2: SELF-DEADLOCK
// ============================================

class BrokenRecursive {
private:
    std::mutex mtx;  // BUG: Regular mutex can't be locked twice by same thread!
    int value = 0;
    
public:
    void outer() {
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "  outer() locked\n";
        inner();  // BUG: Calls inner which also tries to lock!
    }
    
    void inner() {
        std::lock_guard<std::mutex> lock(mtx);  // DEADLOCK! Already locked by outer!
        std::cout << "  inner() locked\n";
        value++;
    }
};

void demonstrateSelfDeadlock() {
    std::cout << "\n=== SELF-DEADLOCK (RECURSIVE LOCKING) ===\n\n";
    
    BrokenRecursive obj;
    
    std::cout << "Calling outer() which calls inner()...\n";
    std::cout << "(This will deadlock on non-recursive mutex!)\n\n";
    
    // obj.outer();  // Would hang forever!
    
    std::cout << "  (Skipped to avoid hanging - would deadlock!)\n";
}

// ============================================
// BUG 3: LOCK WHILE HOLDING LOCK (SUBTLE)
// ============================================

class ResourceManager {
private:
    std::mutex resourceMtx;
    std::mutex logMtx;
    int resourceCount = 0;
    
public:
    void acquireResource() {
        std::lock_guard<std::mutex> lock(resourceMtx);
        resourceCount++;
        logAction("acquired");  // BUG: Calls log while holding resourceMtx!
    }
    
    void logAction(const std::string& action) {
        std::lock_guard<std::mutex> lock(logMtx);
        std::cout << "  [LOG] Resource " << action << "\n";
        
        // BUG: If another thread holds logMtx and calls getResourceCount()...
    }
    
    int getResourceCount() {
        std::lock_guard<std::mutex> lock(logMtx);  // Lock log first
        logAction("counting");  // This is fine (same thread, but recursive lock issue)
        
        std::lock_guard<std::mutex> lock2(resourceMtx);  // Lock resource second
        return resourceCount;
        
        // Order: logMtx -> resourceMtx
        // But acquireResource() does: resourceMtx -> logMtx
        // DEADLOCK potential!
    }
};

// ============================================
// BUG 4: CONDITION VARIABLE DEADLOCK
// ============================================

class BrokenQueue {
private:
    std::mutex mtx;
    std::condition_variable cv;
    std::vector<int> data;
    bool stopped = false;
    
public:
    void push(int value) {
        std::lock_guard<std::mutex> lock(mtx);
        data.push_back(value);
        cv.notify_one();
    }
    
    // BUG: This has a subtle issue
    int pop() {
        std::unique_lock<std::mutex> lock(mtx);
        
        // BUG: What if stopped is true but data is empty?
        while (data.empty()) {
            cv.wait(lock);  // What if no one ever pushes and we're stopped?
        }
        
        int value = data.front();
        data.erase(data.begin());
        return value;
    }
    
    void stop() {
        std::lock_guard<std::mutex> lock(mtx);
        stopped = true;
        cv.notify_all();
    }
};

// ============================================
// YOUR TASK
// ============================================

/*
 * TODO: Create solution_deadlock.cpp with:
 * 
 * 1. FixedBankAccount using std::scoped_lock (C++17):
 *    
 *    void transferTo(BankAccount& other, double amount) {
 *        std::scoped_lock lock(mtx, other.mtx);  // Locks both atomically!
 *        // No deadlock possible!
 *        balance -= amount;
 *        other.balance += amount;
 *    }
 *    
 *    Or using std::lock (pre-C++17):
 *    
 *    std::lock(mtx, other.mtx);  // Locks both without deadlock
 *    std::lock_guard<std::mutex> l1(mtx, std::adopt_lock);
 *    std::lock_guard<std::mutex> l2(other.mtx, std::adopt_lock);
 * 
 * 2. FixedRecursive using std::recursive_mutex:
 *    
 *    std::recursive_mutex mtx;  // Can be locked multiple times by same thread
 *    
 *    Or better: refactor to avoid recursive locking!
 * 
 * 3. FixedResourceManager with consistent lock ordering:
 *    Always lock in the same order across all methods!
 * 
 * 4. FixedQueue with proper stop condition:
 *    
 *    while (data.empty() && !stopped) {
 *        cv.wait(lock);
 *    }
 *    if (stopped && data.empty()) {
 *        throw std::runtime_error("Queue stopped");
 *    }
 * 
 * TRICKY QUESTIONS:
 * 
 * Q1: Why is std::recursive_mutex usually a code smell?
 *     What design pattern should you use instead?
 * 
 * Q2: std::scoped_lock uses a deadlock avoidance algorithm.
 *     What algorithm? (Hint: it's not just "lock in order")
 * 
 * Q3: What's the difference between:
 *     cv.wait(lock);
 *     cv.wait(lock, predicate);
 *     Why is the predicate version safer?
 * 
 * Q4: Can you have a deadlock with just ONE mutex?
 *     (Hint: yes! How?)
 * 
 * Q5: BONUS - Implement a "try_lock with timeout":
 *     - Try to acquire both locks
 *     - If can't acquire within 100ms, give up
 *     - Useful for detecting potential deadlocks
 */

int main() {
    // Note: These demos might actually deadlock!
    // Comment out the ones you don't want to test
    
    // demonstrateBankDeadlock();  // Might hang!
    demonstrateSelfDeadlock();
    
    std::cout << "\n========================================\n";
    std::cout << "Fix the deadlocks!\n";
    std::cout << "========================================\n";
    
    return 0;
}
