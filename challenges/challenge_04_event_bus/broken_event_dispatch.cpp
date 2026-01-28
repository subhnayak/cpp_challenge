/**
 * 🔴 BROKEN CODE - Move Semantics for Events
 * 
 * Events contain heavy payloads that shouldn't be copied.
 * The current implementation copies everything!
 * 
 * YOUR TASK: Implement proper move semantics for events!
 */

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <utility>

// ============================================
// HEAVY EVENT PAYLOAD
// ============================================

class EventPayload {
private:
    std::string type;
    std::vector<char> data;  // Could be megabytes!
    bool valid;
    
public:
    EventPayload(const std::string& t, size_t size) 
        : type(t), data(size, 'X'), valid(true) {
        std::cout << "  [Payload CONSTRUCTED: " << type << ", " << size << " bytes]\n";
    }
    
    // Copy constructor (expensive!)
    EventPayload(const EventPayload& other) 
        : type(other.type), data(other.data), valid(other.valid) {
        std::cout << "  [Payload COPIED: " << type << " - EXPENSIVE!]\n";
    }
    
    // Move constructor (cheap!)
    EventPayload(EventPayload&& other) noexcept
        : type(std::move(other.type)), data(std::move(other.data)), valid(other.valid) {
        other.valid = false;
        std::cout << "  [Payload MOVED: " << type << " - cheap]\n";
    }
    
    // Copy assignment
    EventPayload& operator=(const EventPayload& other) {
        if (this != &other) {
            type = other.type;
            data = other.data;
            valid = other.valid;
        }
        std::cout << "  [Payload COPY-ASSIGNED]\n";
        return *this;
    }
    
    // Move assignment
    EventPayload& operator=(EventPayload&& other) noexcept {
        if (this != &other) {
            type = std::move(other.type);
            data = std::move(other.data);
            valid = other.valid;
            other.valid = false;
        }
        std::cout << "  [Payload MOVE-ASSIGNED]\n";
        return *this;
    }
    
    ~EventPayload() {
        std::cout << "  [Payload DESTROYED: " << (valid ? type : "moved-from") << "]\n";
    }
    
    bool isValid() const { return valid; }
    const std::string& getType() const { return type; }
    size_t getSize() const { return data.size(); }
};

// ============================================
// BROKEN EVENT BUS
// ============================================

class BrokenEventBus {
private:
    std::vector<std::function<void(EventPayload)>> handlers;  // BUG: Takes by value!
    
public:
    // BUG: Handler signature takes EventPayload by value - forces copy!
    void on(std::function<void(EventPayload)> handler) {
        handlers.push_back(handler);
    }
    
    // BUG: Takes event by value - forces copy!
    void emit(EventPayload event) {
        std::cout << "\n  [Emitting event...]\n";
        for (auto& handler : handlers) {
            handler(event);  // BUG: Each handler gets a copy!
        }
    }
    
    // BUG: This attempts to forward but still copies!
    void emitBroken(EventPayload&& event) {
        std::cout << "\n  [Emitting rvalue event...]\n";
        for (auto& handler : handlers) {
            handler(event);  // BUG: event is an lvalue here!
        }
    }
};

// ============================================
// DEMONSTRATION OF COPIES
// ============================================

void demonstrateExcessiveCopies() {
    std::cout << "\n=== EXCESSIVE COPIES DEMO ===\n\n";
    
    BrokenEventBus bus;
    
    // Register handlers
    bus.on([](EventPayload e) {
        std::cout << "    Handler 1 received: " << e.getType() << "\n";
    });
    
    bus.on([](EventPayload e) {
        std::cout << "    Handler 2 received: " << e.getType() << "\n";
    });
    
    // Create and emit event
    std::cout << "Creating event...\n";
    EventPayload event("damage", 1000000);  // 1MB payload
    
    std::cout << "\nEmitting (watch the copies!)...\n";
    bus.emit(event);  // Copy to emit(), then copy to each handler!
    
    std::cout << "\nTotal copies made: TOO MANY!\n";
}

// ============================================
// BROKEN ATTEMPT AT MOVE SEMANTICS
// ============================================

void demonstrateBrokenMove() {
    std::cout << "\n=== BROKEN MOVE ATTEMPT ===\n\n";
    
    BrokenEventBus bus;
    
    bus.on([](EventPayload e) {
        std::cout << "    Handler received: " << e.getType() << "\n";
    });
    
    EventPayload event("attack", 500000);
    
    std::cout << "\nAttempting to emit with rvalue...\n";
    bus.emitBroken(std::move(event));
    
    // BUG: What's the state of event now?
    std::cout << "\nOriginal event valid? " << (event.isValid() ? "yes" : "no") << "\n";
    std::cout << "(It should be 'no' if move worked, but...)\n";
}

// ============================================
// THE std::move DOESN'T MOVE PUZZLE
// ============================================

void demonstrateMoveDoesntMove() {
    std::cout << "\n=== std::move DOESN'T ACTUALLY MOVE ===\n\n";
    
    EventPayload original("test", 1000);
    
    std::cout << "\nCalling std::move (does nothing by itself!)...\n";
    EventPayload&& rref = std::move(original);  // Just a cast, no move!
    
    std::cout << "Original still valid? " << (original.isValid() ? "yes" : "no") << "\n";
    std::cout << "(std::move just casts to rvalue reference!)\n";
    
    std::cout << "\nNOW actually moving into new object...\n";
    EventPayload moved = std::move(original);  // Move constructor called
    
    std::cout << "Original valid after real move? " << (original.isValid() ? "yes" : "no") << "\n";
}

// ============================================
// YOUR TASK
// ============================================

/*
 * TODO: Create solution_event_dispatch.cpp with:
 * 
 * 1. OptimizedEventBus with proper move semantics:
 *    
 *    class OptimizedEventBus {
 *        // Handlers take const reference - no copy!
 *        std::vector<std::function<void(const EventPayload&)>> handlers;
 *        
 *        void emit(const EventPayload& event);      // For lvalues
 *        void emit(EventPayload&& event);           // For rvalues
 *        
 *        // Or: perfect forwarding
 *        template<typename E>
 *        void emit(E&& event) {
 *            for (auto& handler : handlers) {
 *                handler(std::forward<E>(event));
 *            }
 *        }
 *    };
 * 
 * 2. Event factory with perfect forwarding:
 *    
 *    template<typename... Args>
 *    EventPayload createEvent(Args&&... args) {
 *        return EventPayload(std::forward<Args>(args)...);
 *    }
 * 
 * 3. Move-only event type (can't be copied):
 *    
 *    class MoveOnlyEvent {
 *        MoveOnlyEvent(const MoveOnlyEvent&) = delete;
 *        MoveOnlyEvent& operator=(const MoveOnlyEvent&) = delete;
 *        MoveOnlyEvent(MoveOnlyEvent&&) = default;
 *        MoveOnlyEvent& operator=(MoveOnlyEvent&&) = default;
 *    };
 * 
 * TRICKY QUESTIONS:
 * 
 * Q1: What's the difference between these?
 *     void foo(T t);        // By value
 *     void foo(T& t);       // Lvalue reference
 *     void foo(T&& t);      // Rvalue reference
 *     void foo(const T& t); // Const lvalue reference
 *     
 *     Which can bind to: lvalue, rvalue, const lvalue, const rvalue?
 * 
 * Q2: Inside a function taking T&&, the parameter 't' is itself an lvalue!
 *     Why? And how do you "forward" the rvalue-ness?
 * 
 * Q3: Why does this NOT work?
 *     
 *     void emit(EventPayload&& event) {
 *         handler1(event);  // event is lvalue!
 *         handler2(std::move(event));  // Moved!
 *         handler3(std::move(event));  // UB - moved from!
 *     }
 * 
 * Q4: When should handlers take by value vs. const reference?
 *     - void handler(EventPayload event);       // Value
 *     - void handler(const EventPayload& event); // Const ref
 *     
 *     What are the tradeoffs?
 * 
 * Q5: BONUS - Implement "event stealing":
 *     Handler can optionally "steal" the event data, preventing
 *     subsequent handlers from seeing it. How?
 */

int main() {
    demonstrateExcessiveCopies();
    demonstrateBrokenMove();
    demonstrateMoveDoesntMove();
    
    std::cout << "\n========================================\n";
    std::cout << "Implement proper move semantics!\n";
    std::cout << "========================================\n";
    
    return 0;
}
