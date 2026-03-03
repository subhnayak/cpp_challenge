/**
 * ✅ SOLUTION - Event Dispatch with Move Semantics
 */

#include <iostream>
#include <functional>
#include <vector>
#include <utility>

struct Event {
    std::string type;
    std::string data;
    
    Event(const std::string& t, std::string d) 
        : type(t), data(std::move(d)) {
        std::cout << "  Event created: " << type << "\n";
    }
    
    Event(Event&& other) noexcept 
        : type(std::move(other.type)), data(std::move(other.data)) {
        std::cout << "  Event moved\n";
    }
};

class EventDispatcher {
private:
    std::vector<std::function<void(const Event&)>> handlers;
    
public:
    void subscribe(std::function<void(const Event&)> handler) {
        handlers.push_back(handler);
    }
    
    void emit(Event&& event) {
        std::cout << "Dispatching event: " << event.type << "\n";
        for (auto& handler : handlers) {
            handler(event);
        }
    }
};

int main() {
    std::cout << "=== EVENT DISPATCH ===\n\n";
    
    EventDispatcher dispatcher;
    
    dispatcher.subscribe([](const Event& e) {
        std::cout << "  Handler received: " << e.data << "\n";
    });
    
    dispatcher.emit(Event("Damage", "Player took 10 damage"));
    dispatcher.emit(Event("Heal", "Player healed"));
    
    std::cout << "\n✅ Events dispatched with move semantics!\n";
    
    return 0;
}
