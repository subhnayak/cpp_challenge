/**
 * ✅ SOLUTION - Event Bus with Modern C++
 * 
 * Demonstrates:
 * - Lambda capture semantics (safe captures)
 * - Perfect forwarding for event dispatch
 * - Move semantics for event payloads
 */

#include <iostream>
#include <functional>
#include <vector>
#include <memory>
#include <utility>

// ============================================
// EVENT SYSTEM
// ============================================

struct Event {
    std::string type;
    std::string data;
    
    Event(const std::string& t, const std::string& d) 
        : type(t), data(d) {}
    
    // Move constructor
    Event(Event&& other) noexcept 
        : type(std::move(other.type)), data(std::move(other.data)) {
        std::cout << "  [Event] moved\n";
    }
};

class EventBus {
private:
    std::vector<std::function<void(const Event&)>> handlers;
    
public:
    // Register handler with safe value capture
    void on(const std::string& eventType, std::function<void(const Event&)> handler) {
        // Capture event type by value (safe, lives in closure)
        handlers.push_back([eventType, handler](const Event& e) {
            if (e.type == eventType) {
                handler(e);
            }
        });
    }
    
    // Emit event with move semantics
    void emit(Event&& event) {
        std::cout << "  [Bus] Emitting event: " << event.type << "\n";
        for (auto& handler : handlers) {
            handler(event);
        }
    }
    
    template<typename F>\n    void onTemplate(const std::string& eventType, F&& handler) {\n        // Perfect forwarding\n        handlers.push_back([eventType, h = std::forward<F>(handler)](const Event& e) {\n            if (e.type == eventType) {\n                h(e);\n            }\n        });\n    }\n};\n\nint main() {\n    std::cout << \"=== EVENT BUS DEMO ===\\n\";\n    \n    EventBus bus;\n    \n    // CORRECT: Capture by value\n    bus.on(\"damage\", [](const Event& e) {\n        std::cout << \"  Handler: Damage event - \" << e.data << \"\\n\";\n    });\n    \n    // CORRECT: Using move event\n    Event dmgEvent(\"damage\", \"Player took 10 damage\");\n    bus.emit(std::move(dmgEvent));\n    \n    std::cout << \"\\n✅ Lambda captures safe!\\n\";\n    \n    return 0;\n}\n