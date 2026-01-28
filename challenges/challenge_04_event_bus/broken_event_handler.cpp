/**
 * 🔴 BROKEN CODE - Lambda Capture Issues
 * 
 * Event handlers use lambdas with captures, but there are
 * dangerous lifetime issues causing crashes!
 * 
 * YOUR TASK: Fix the capture semantics!
 */

#include <iostream>
#include <functional>
#include <vector>
#include <string>
#include <memory>
#include <map>

// ============================================
// SIMPLE EVENT SYSTEM
// ============================================

struct Event {
    std::string type;
    std::map<std::string, std::string> data;
};

class EventBus {
private:
    std::map<std::string, std::vector<std::function<void(const Event&)>>> handlers;
    
public:
    void on(const std::string& eventType, std::function<void(const Event&)> handler) {
        handlers[eventType].push_back(handler);
    }
    
    void emit(const std::string& eventType, const Event& event) {
        if (handlers.count(eventType)) {
            for (auto& handler : handlers[eventType]) {
                handler(event);
            }
        }
    }
    
    void clear() {
        handlers.clear();
    }
};

// ============================================
// BUG 1: DANGLING REFERENCE CAPTURE
// ============================================

void bugDanglingReference(EventBus& bus) {
    std::cout << "--- BUG 1: Dangling Reference ---\n";
    
    std::string localName = "Player1";
    int localScore = 100;
    
    // BUG: Capturing local variables by reference!
    bus.on("game_over", [&localName, &localScore](const Event& e) {
        // When this runs, localName and localScore are DESTROYED!
        std::cout << "  Player: " << localName << ", Score: " << localScore << "\n";
    });
    
    std::cout << "  (Handler registered, leaving scope...)\n";
}
// localName and localScore destroyed here! Handler now has dangling references!

// ============================================
// BUG 2: CAPTURING 'this' IN A TEMPORARY
// ============================================

class BuggyPlayer {
private:
    std::string name;
    int health;
    
public:
    BuggyPlayer(const std::string& n) : name(n), health(100) {}
    
    void registerDamageHandler(EventBus& bus) {
        // BUG: Capturing 'this' but Player might be destroyed!
        bus.on("damage", [this](const Event& e) {
            std::cout << "  " << name << " takes damage! Health: " << health << "\n";
        });
    }
    
    ~BuggyPlayer() {
        std::cout << "  [Player " << name << " destroyed]\n";
    }
};

void bugThisCapture(EventBus& bus) {
    std::cout << "\n--- BUG 2: Dangling 'this' Capture ---\n";
    
    {
        BuggyPlayer player("Hero");
        player.registerDamageHandler(bus);
        std::cout << "  (Player leaving scope...)\n";
    }
    // Player is destroyed! Handler has dangling 'this'!
    
    std::cout << "  (Player destroyed, handler still registered!)\n";
}

// ============================================
// BUG 3: UNINTENDED COPY IN CAPTURE
// ============================================

class HeavyData {
public:
    std::vector<int> data;
    
    HeavyData() : data(1000000, 42) {
        std::cout << "  [HeavyData CONSTRUCTED]\n";
    }
    
    HeavyData(const HeavyData& other) : data(other.data) {
        std::cout << "  [HeavyData COPIED - expensive!]\n";
    }
    
    HeavyData(HeavyData&& other) noexcept : data(std::move(other.data)) {
        std::cout << "  [HeavyData MOVED - cheap!]\n";
    }
    
    ~HeavyData() {
        std::cout << "  [HeavyData destroyed]\n";
    }
};

void bugExpensiveCopy(EventBus& bus) {
    std::cout << "\n--- BUG 3: Unintended Expensive Copy ---\n";
    
    HeavyData heavyData;
    
    // BUG: Capturing by value copies the entire 1MB vector!
    bus.on("process", [heavyData](const Event& e) {
        std::cout << "  Processing " << heavyData.data.size() << " items\n";
    });
    
    // Another handler - another copy!
    bus.on("analyze", [heavyData](const Event& e) {
        std::cout << "  Analyzing " << heavyData.data.size() << " items\n";
    });
    
    std::cout << "  (Watch how many copies were made!)\n";
}

// ============================================
// BUG 4: MUTABLE LAMBDA CONFUSION
// ============================================

void bugMutableCapture(EventBus& bus) {
    std::cout << "\n--- BUG 4: Mutable Lambda Confusion ---\n";
    
    int callCount = 0;
    
    // BUG: Capturing by value means each call has its own copy
    bus.on("tick", [callCount](const Event& e) mutable {
        callCount++;
        std::cout << "  Tick! Count: " << callCount << "\n";
    });
    
    // Emit multiple times
    Event tick{"tick", {}};
    std::cout << "  Emitting 3 ticks...\n";
    bus.emit("tick", tick);
    bus.emit("tick", tick);
    bus.emit("tick", tick);
    
    // What's the original callCount?
    std::cout << "  Original callCount: " << callCount << " (expected: 3, actual: 0!)\n";
}

// ============================================
// BUG 5: CAPTURE INIT MISUSE
// ============================================

void bugCaptureInit() {
    std::cout << "\n--- BUG 5: Capture Init Lifetime ---\n";
    
    std::function<void()> storedLambda;
    
    {
        std::string temp = "temporary data";
        
        // This LOOKS safe because we're using capture init...
        // BUG: But we're still capturing a reference!
        storedLambda = [&data = temp]() {
            std::cout << "  Data: " << data << "\n";
        };
        
        std::cout << "  (temp going out of scope...)\n";
    }
    
    std::cout << "  Calling stored lambda (DANGER!)...\n";
    // storedLambda();  // CRASH! data is a reference to destroyed temp
}

// ============================================
// DEMONSTRATION
// ============================================

void demonstrateAllBugs() {
    std::cout << "\n=== LAMBDA CAPTURE BUGS ===\n\n";
    
    EventBus bus;
    
    bugDanglingReference(bus);
    
    // Trigger the handler (will access dangling memory!)
    std::cout << "\n  Triggering game_over event (DANGER!)...\n";
    // bus.emit("game_over", {"game_over", {}});  // Would crash!
    
    bus.clear();
    
    bugThisCapture(bus);
    
    // Trigger the handler (will access dangling 'this'!)
    std::cout << "  Triggering damage event (DANGER!)...\n";
    // bus.emit("damage", {"damage", {}});  // Would crash!
    
    bus.clear();
    
    bugExpensiveCopy(bus);
    
    bus.clear();
    
    bugMutableCapture(bus);
    
    bus.clear();
    
    bugCaptureInit();
}

// ============================================
// YOUR TASK
// ============================================

/*
 * TODO: Create solution_event_handler.cpp with:
 * 
 * 1. SafeEventBus that handles lifetime correctly
 * 
 * 2. Fixed handlers using:
 *    - Value capture with move: [data = std::move(localData)]
 *    - Shared ownership: [shared = std::make_shared<Data>(localData)]
 *    - [*this] instead of [this] for object copies
 *    - Reference capture only when CERTAIN of lifetime
 * 
 * 3. WeakReference pattern for object handlers:
 *    
 *    class SafePlayer : public std::enable_shared_from_this<SafePlayer> {
 *        void registerHandler(EventBus& bus) {
 *            std::weak_ptr<SafePlayer> weakSelf = weak_from_this();
 *            bus.on("damage", [weakSelf](const Event& e) {
 *                if (auto self = weakSelf.lock()) {
 *                    self->takeDamage();
 *                }
 *            });
 *        }
 *    };
 * 
 * TRICKY QUESTIONS:
 * 
 * Q1: What's the difference between these captures?
 *     [x]      - copy x
 *     [&x]     - reference to x
 *     [x = x]  - copy x (same as [x])
 *     [x = std::move(x)]  - move x into capture
 *     [&x = x] - reference to x (same as [&x])
 *     
 * Q2: [=, &x] vs [&, x] - what does each capture?
 * 
 * Q3: [this] vs [*this] (C++17) - when do you need [*this]?
 *     
 *     class Widget {
 *         void foo() {
 *             auto f1 = [this]() { bar(); };   // Captures this pointer
 *             auto f2 = [*this]() { bar(); };  // Captures copy of *this
 *         }
 *     };
 * 
 * Q4: Can you capture a unique_ptr in a lambda? How?
 *     
 *     auto ptr = std::make_unique<int>(42);
 *     auto f = [???]() { use(*ptr); };
 * 
 * Q5: BONUS - Implement a "scoped event handler" that automatically
 *     unregisters when destroyed:
 *     
 *     class ScopedHandler {
 *         ~ScopedHandler() { eventBus.unregister(myHandlerId); }
 *     };
 */

int main() {
    demonstrateAllBugs();
    
    std::cout << "\n========================================\n";
    std::cout << "Fix the lambda capture issues!\n";
    std::cout << "========================================\n";
    
    return 0;
}
