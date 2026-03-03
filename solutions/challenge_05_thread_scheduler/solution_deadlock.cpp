/**
 * ✅ SOLUTION - Deadlock Prevention
 * 
 * Demonstrates proper lock ordering and scoped_lock to avoid deadlocks
 */

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>

class SafeTransfer {
private:
    std::mutex mtxA, mtxB;
    int accountA = 100, accountB = 100;
    
public:
    // WRONG: No ordering guarantees
    // void unsafeTransfer() {\n    //     lock(mtxA);  // Thread 1 gets A\n    //     lock(mtxB);  // Blocked waiting for B\n    //     // Thread 2 has B, waiting for A = DEADLOCK!\n    // }\n    
    // RIGHT: scoped_lock ensures consistent ordering
    void safeTransfer() {
        std::scoped_lock lock(mtxA, mtxB);  // Auto-orders locks
        accountA -= 10;
        accountB += 10;
    }
    
    int getSum() {
        std::scoped_lock lock(mtxA, mtxB);
        return accountA + accountB;
    }
    
    void print() {
        std::scoped_lock lock(mtxA, mtxB);
        std::cout << "A: " << accountA << ", B: " << accountB << ", Sum: " << (accountA + accountB) << "\n";
    }
};

int main() {
    std::cout << "=== DEADLOCK PREVENTION ===\n\n";
    
    SafeTransfer safe;
    
    auto task = [&safe]() {
        for (int i = 0; i < 5; ++i) {
            safe.safeTransfer();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    };
    
    std::thread t1(task);
    std::thread t2(task);
    
    t1.join();
    t2.join();
    
    std::cout << "Final state: ";
    safe.print();
    
    std::cout << "\n✅ No deadlock! scoped_lock handles ordering!\n";
    
    return 0;
}
