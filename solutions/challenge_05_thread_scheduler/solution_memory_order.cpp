/**
 * ✅ SOLUTION - Memory Ordering for Synchronization
 */

#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>

class FlagSynchronization {
private:
    int data = 0;
    std::atomic<bool> ready{false};
    
public:
    void writer() {
        data = 42;
        std::cout << "Writer: Set data = 42\n";
        
        // Release: publish the data
        ready.store(true, std::memory_order_release);
        std::cout << "Writer: Set ready = true (with release)\n";
    }
    
    void reader() {
        // Acquire: synchronize with release
        while (!ready.load(std::memory_order_acquire)) {
            std::this_thread::yield();
        }
        std::cout << "Reader: Got ready signal (with acquire)\n";
        std::cout << "Reader: Read data = " << data << "\n";
    }
};

int main() {
    std::cout << "=== MEMORY ORDERING ===\n\n";
    
    FlagSynchronization sync;
    
    std::thread writer([&sync]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        sync.writer();
    });
    
    std::thread reader([&sync]() {
        sync.reader();
    });
    
    reader.join();
    writer.join();
    
    std::cout << "\n✅ Memory ordering correctly synchronized!\n";
    
    return 0;
}
