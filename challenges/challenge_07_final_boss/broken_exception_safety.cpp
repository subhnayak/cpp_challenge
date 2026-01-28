/**
 * 🔴 BROKEN CODE - Exception Safety
 * 
 * Operations need to be atomic - either complete fully or have no effect.
 * This code leaves things half-done when exceptions occur!
 * 
 * YOUR TASK: Implement strong exception safety!
 */

#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <memory>

// ============================================
// BUG: NO EXCEPTION SAFETY
// ============================================

class BrokenAccount {
private:
    std::string owner;
    int balance;
    std::vector<std::string> transactionLog;
    
public:
    BrokenAccount(const std::string& name, int initial) 
        : owner(name), balance(initial) {}
    
    // BUG: Not exception-safe!
    void transfer(BrokenAccount& to, int amount) {
        if (amount > balance) {
            throw std::runtime_error("Insufficient funds");
        }
        
        balance -= amount;  // Step 1: Deduct from this account
        
        // What if the next line throws?
        transactionLog.push_back("Sent $" + std::to_string(amount));  // Might throw!
        
        to.balance += amount;  // Step 2: Add to other account
        
        // What if THIS throws?
        to.transactionLog.push_back("Received $" + std::to_string(amount));  // Might throw!
        
        // If any step after balance -= amount throws,
        // we've lost money from this account but never added it to 'to'!
    }
    
    int getBalance() const { return balance; }
    const std::string& getOwner() const { return owner; }
    
    void printLog() const {
        std::cout << "  " << owner << "'s log: ";
        for (const auto& entry : transactionLog) {
            std::cout << "[" << entry << "] ";
        }
        std::cout << "\n";
    }
};

void demonstrateNoExceptionSafety() {
    std::cout << "=== NO EXCEPTION SAFETY ===\n\n";
    
    BrokenAccount alice("Alice", 100);
    BrokenAccount bob("Bob", 50);
    
    std::cout << "Before transfer:\n";
    std::cout << "  Alice: $" << alice.getBalance() << "\n";
    std::cout << "  Bob: $" << bob.getBalance() << "\n";
    
    try {
        // Simulate exception during transfer
        alice.transfer(bob, 30);
        std::cout << "\nTransfer succeeded!\n";
    } catch (const std::exception& e) {
        std::cout << "\nTransfer failed: " << e.what() << "\n";
    }
    
    std::cout << "\nAfter transfer:\n";
    std::cout << "  Alice: $" << alice.getBalance() << "\n";
    std::cout << "  Bob: $" << bob.getBalance() << "\n";
    
    // In this case it worked, but if exception occurred midway,
    // money could have been lost!
}

// ============================================
// BUG: LEAKY EXCEPTION HANDLING
// ============================================

class BrokenResourceManager {
private:
    int* resource1;
    int* resource2;
    int* resource3;
    
public:
    BrokenResourceManager() : resource1(nullptr), resource2(nullptr), resource3(nullptr) {}
    
    // BUG: Leaks resources if any allocation throws!
    void initialize(int val1, int val2, int val3) {
        resource1 = new int(val1);  // OK
        
        if (val2 == 0) {
            throw std::runtime_error("val2 cannot be zero!");
            // resource1 is leaked!
        }
        resource2 = new int(val2);  // What if this throws?
        // resource1 is leaked!
        
        if (val3 < 0) {
            throw std::runtime_error("val3 cannot be negative!");
            // resource1 and resource2 are leaked!
        }
        resource3 = new int(val3);
    }
    
    ~BrokenResourceManager() {
        delete resource1;
        delete resource2;
        delete resource3;
    }
};

void demonstrateLeakyExceptionHandling() {
    std::cout << "\n=== LEAKY EXCEPTION HANDLING ===\n\n";
    
    try {
        BrokenResourceManager mgr;
        mgr.initialize(1, 0, 3);  // val2 = 0 triggers exception
    } catch (const std::exception& e) {
        std::cout << "  Exception: " << e.what() << "\n";
        std::cout << "  (resource1 was leaked!)\n";
    }
}

// ============================================
// BUG: HALF-CONSTRUCTED OBJECT
// ============================================

class BrokenComposite {
private:
    std::vector<std::unique_ptr<int>> items;
    int* rawResource;  // BUG: Raw pointer in RAII class!
    
public:
    BrokenComposite(int count, int value) : rawResource(nullptr) {
        // Add items
        for (int i = 0; i < count; i++) {
            if (i == 3 && count > 3) {
                // Simulate allocation failure
                throw std::bad_alloc();
            }
            items.push_back(std::make_unique<int>(value * i));
        }
        
        // BUG: If this throws, items are cleaned up, but what if
        // we had already modified external state?
        rawResource = new int(value);
    }
    
    ~BrokenComposite() {
        // items will be cleaned up automatically (unique_ptr)
        delete rawResource;  // But this might be null!
    }
};

// ============================================
// DEMONSTRATING COPY-AND-SWAP
// ============================================

class SafeString {
private:
    char* data;
    size_t size;
    
public:
    SafeString(const char* str = "") {
        size = strlen(str);
        data = new char[size + 1];
        strcpy(data, str);
    }
    
    ~SafeString() {
        delete[] data;
    }
    
    SafeString(const SafeString& other) : size(other.size) {
        data = new char[size + 1];
        strcpy(data, other.data);
    }
    
    // BUG: This assignment is NOT exception-safe!
    SafeString& operator=(const SafeString& other) {
        if (this != &other) {
            delete[] data;  // Free old data FIRST
            
            // BUG: If new throws here, we've already deleted data!
            // Object is now in invalid state!
            size = other.size;
            data = new char[size + 1];  // Might throw!
            strcpy(data, other.data);
        }
        return *this;
    }
    
    // Swap function for copy-and-swap
    friend void swap(SafeString& a, SafeString& b) noexcept {
        using std::swap;
        swap(a.data, b.data);
        swap(a.size, b.size);
    }
    
    void print() const {
        std::cout << "  \"" << data << "\"\n";
    }
};

// ============================================
// YOUR TASK
// ============================================

/*
 * TODO: Create solution_transaction.cpp with:
 * 
 * 1. SafeAccount with strong exception guarantee:
 *    
 *    void transfer(SafeAccount& to, int amount) {
 *        // Create copies
 *        SafeAccount fromCopy = *this;
 *        SafeAccount toCopy = to;
 *        
 *        // Do operations on copies (might throw)
 *        fromCopy.withdraw(amount);
 *        toCopy.deposit(amount);
 *        
 *        // Commit with no-throw swap
 *        swap(*this, fromCopy);  // noexcept
 *        swap(to, toCopy);       // noexcept
 *    }
 * 
 * 2. SafeResourceManager using RAII:
 *    
 *    class SafeResourceManager {
 *        std::unique_ptr<int> resource1;
 *        std::unique_ptr<int> resource2;
 *        std::unique_ptr<int> resource3;
 *        
 *        void initialize(int v1, int v2, int v3) {
 *            auto r1 = std::make_unique<int>(v1);
 *            // If next line throws, r1 is cleaned up automatically!
 *            auto r2 = std::make_unique<int>(v2);
 *            // etc.
 *            
 *            // Only commit at the end
 *            resource1 = std::move(r1);
 *            resource2 = std::move(r2);
 *            resource3 = std::move(r3);
 *        }
 *    };
 * 
 * 3. Copy-and-swap assignment:
 *    
 *    SafeString& operator=(SafeString other) {  // By VALUE
 *        swap(*this, other);
 *        return *this;
 *    }  // other's destructor cleans up old data
 * 
 * TRICKY QUESTIONS:
 * 
 * Q1: What's the difference between basic and strong exception guarantee?
 * 
 * Q2: Why is the swap function marked noexcept? What if it throws?
 * 
 * Q3: In copy-and-swap, taking parameter by value enables what optimization?
 *     (Hint: what happens with rvalue arguments?)
 * 
 * Q4: Can destructors throw? What happens if a destructor throws during
 *     stack unwinding from another exception?
 * 
 * Q5: BONUS - Implement a "ScopeGuard" that runs cleanup code on exception:
 *     
 *     {
 *         resource1 = allocate();
 *         auto guard = ScopeGuard([&] { free(resource1); });
 *         
 *         resource2 = allocate();  // If this throws, guard cleans up resource1
 *         
 *         guard.dismiss();  // Success! Don't clean up.
 *     }
 */

int main() {
    demonstrateNoExceptionSafety();
    demonstrateLeakyExceptionHandling();
    
    std::cout << "\n========================================\n";
    std::cout << "Implement strong exception safety!\n";
    std::cout << "========================================\n";
    
    return 0;
}
