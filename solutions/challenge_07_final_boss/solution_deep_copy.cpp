/**
 * ✅ SOLUTION - Deep Copy Implementation
 */

#include <iostream>
#include <cstring>

class Player {
private:
    char* inventory;
    int level;
    int* xp;  // Points to shared data
    
public:
    // Constructor
    Player(int lv = 1) : level(lv) {
        inventory = new char[256];
        std::strcpy(inventory, "Empty");
        xp = new int(0);
        std::cout << "  Player created: level " << level << "\n";
    }
    
    // Destructor
    ~Player() {
        delete[] inventory;
        delete xp;
        std::cout << "  Player destroyed\n";
    }
    
    // Deep copy constructor
    Player(const Player& other) : level(other.level) {
        inventory = new char[256];
        std::strcpy(inventory, other.inventory);
        xp = new int(*other.xp);
        std::cout << "  Player deep copied\n";
    }
    
    // Deep copy assignment (copy-and-swap for exception safety)
    Player& operator=(const Player& other) {
        if (this != &other) {
            Player temp(other);  // Deep copy via constructor
            std::swap(inventory, temp.inventory);
            std::swap(level, temp.level);
            std::swap(xp, temp.xp);
        }
        std::cout << "  Player deep assigned\n";
        return *this;
    }
    
    void display() {
        std::cout << "    Level: " << level << ", XP: " << *xp 
                  << ", Inventory: " << inventory << "\n";
    }
};

int main() {
    std::cout << "=== DEEP COPY ===\n\n";
    
    {
        Player p1(10);
        std::cout << "p1: "; p1.display();
        
        Player p2 = p1;  // Deep copy
        std::cout << "p2 (copied): "; p2.display();
        
        // Modifications don't affect p2
        Player p3;
        p3 = p1;  // Deep copy
        std::cout << "p3 (assigned): "; p3.display();
    }
    
    std::cout << "\n✅ All copies are independent!\n";
    
    return 0;
}
