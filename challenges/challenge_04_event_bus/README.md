# 📡 Challenge 4: The Event Bus

## The Situation

The game needs an event system for communication between components. The old system used raw function pointers and manual memory management. It was a nightmare!

Your task: Build a modern event system using lambdas, `std::function`, move semantics, and compile-time features.

---

## 🎯 Objectives

### Objective 4.1: Lambda Capture Semantics
**File: `broken_event_handler.cpp`**

Event handlers use lambdas, but captures are causing lifetime issues!

**Your Mission:**
- Fix dangling reference captures
- Understand value vs. reference captures
- Handle `this` capture correctly

**The Bug:**
```cpp
void registerHandler() {
    std::string localData = "important";
    
    eventBus.on("damage", [&localData](Event& e) {
        std::cout << localData;  // DANGLING! localData is gone!
    });
}  // localData destroyed, but handler still references it!
```

---

### Objective 4.2: Move Semantics for Events
**File: `broken_event_dispatch.cpp`**

Events contain heavy data that shouldn't be copied when dispatched!

**Your Mission:**
- Use move semantics to transfer event ownership
- Implement perfect forwarding in `emit()`
- Handle event data lifecycle correctly

**The Problem:**
```cpp
Event event(hugePayload);  // 10MB of data
eventBus.emit(event);      // Copy 1
eventBus.emit(event);      // Copy 2 - but event was already moved!
```

---

### Objective 4.3: `constexpr` Event Validation
**File: `broken_event_types.cpp`**

Event type strings are checked at runtime. Catch typos at compile time!

**Your Mission:**
- Create compile-time event type validation
- Use `constexpr` and string literals
- Ensure invalid events fail to compile

**The Goal:**
```cpp
eventBus.on("player_damage", handler);   // OK
eventBus.on("playre_damage", handler);   // COMPILE ERROR - typo!
```

---

### Objective 4.4: `std::function` vs. Function Pointers
**File: `broken_callback_system.cpp`**

When should you use `std::function` vs. templates vs. raw pointers?

**Your Mission:**
- Understand the overhead of `std::function`
- Implement a fast path for simple callbacks
- Handle stateful vs. stateless callbacks

**The Trade-off:**
```cpp
std::function<void(Event&)> handler;  // Flexible but has overhead
void (*fastHandler)(Event&);           // Fast but can't capture
template<typename F> void on(F&& f);  // Best of both?
```

---

### Objective 4.5: Copy Elision and RVO
**File: `broken_event_return.cpp`**

Event handlers return results, but copies are everywhere!

**Your Mission:**
- Understand when RVO/NRVO applies
- Force copy elision where possible
- Measure the performance difference

**The Puzzle:**
```cpp
Result handleEvent() {
    Result r;
    r.compute();
    return r;  // Copy or move? Or neither (RVO)?
}
```

---

## 📝 Files to Modify

1. `broken_event_handler.cpp` → Create `solution_event_handler.cpp`
2. `broken_event_dispatch.cpp` → Create `solution_event_dispatch.cpp`
3. `broken_event_types.cpp` → Create `solution_event_types.cpp`
4. `broken_callback_system.cpp` → Create `solution_callback_system.cpp`
5. `broken_event_return.cpp` → Create `solution_event_return.cpp`

---

## ✅ Success Criteria

- No dangling references in lambda captures
- Events are moved, not copied
- Invalid event types fail at compile time
- Callback system is efficient
- RVO is utilized correctly

---

## 💭 Reflection Questions

1. What's the difference between `[=]`, `[&]`, `[this]`, and `[*this]`?
2. When does `std::function` allocate on the heap?
3. What is "guaranteed copy elision" in C++17?
4. How does `std::move` interact with function parameters?
5. When would you use `consteval` instead of `constexpr`?

---

## 🔑 Hints

<details>
<summary>Hint 1: Safe Lambda Captures</summary>

```cpp
// Capture by value for safety
[data = std::move(localData)](Event& e) { /* ... */ }

// Or use shared ownership
auto shared = std::make_shared<Data>(localData);
[shared](Event& e) { /* ... */ }
```
</details>

<details>
<summary>Hint 2: Perfect Forwarding in emit()</summary>

```cpp
template<typename... Args>
void emit(const std::string& type, Args&&... args) {
    auto event = Event(std::forward<Args>(args)...);
    // ...
}
```
</details>

<details>
<summary>Hint 3: Compile-time Strings</summary>

```cpp
template<size_t N>
constexpr bool isValidEventType(const char (&str)[N]) {
    // Check against known event types at compile time
}
```
</details>
