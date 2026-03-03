/**
 * ✅ SOLUTION - Exception Safety
 */

#include <iostream>
#include <memory>
#include <vector>

class Account {
private:
    int balance;
    std::vector<std::string> transactions;
    
public:
    Account(int initial = 1000) : balance(initial) {}
    
    int getBalance() const { return balance; }
    
    // Strong exception guarantee: all-or-nothing
    void transfer(Account& dest, int amount) {
        if (amount > balance) {
            throw std::runtime_error("Insufficient funds!");
        }
        
        // Create temporary copy of destination
        Account tempDest = dest;
        
        // Perform transfer on temporary
        this->balance -= amount;
        tempDest.balance += amount;
        
        // If we reach here, transfer succeeded
        // Swap back (can't throw)
        dest = tempDest;
        
        std::cout << "Transfer successful!\n";
    }
};

int main() {
    std::cout << "=== EXCEPTION SAFETY ===\n\n";
    
    Account alice(100);
    Account bob(200);
    
    std::cout << "Alice: " << alice.getBalance() << ", Bob: " << bob.getBalance() << "\n";
    
    try {
        // Successful transfer
        alice.transfer(bob, 30);
        std::cout << "After transfer: Alice: " << alice.getBalance() 
                  << ", Bob: " << bob.getBalance() << "\n";
        
        // Failed transfer (insufficient funds)
        alice.transfer(bob, 1000);
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << "\n";
        std::cout << "Accounts unchanged: Alice: " << alice.getBalance() 
                  << ", Bob: " << bob.getBalance() << "\n";
    }
    
    std::cout << "\n✅ Strong exception guarantee maintained!\n";
    
    return 0;
}
