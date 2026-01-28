# 🎭 Challenge 2: The Entity System

## The Situation

The game's entity system uses inheritance to create different character types. But there's a major problem: the **Diamond Problem** is causing chaos, virtual functions aren't being dispatched correctly, and the vtable seems corrupted!

Your task: Fix the broken inheritance hierarchy and understand how virtual functions really work.

---

## 🎯 Objectives

### Objective 2.1: The Diamond of Death (Virtual Inheritance)
**File: `broken_diamond.cpp`**

The `FlyingWarrior` class inherits from both `Flying` and `Warrior`, which both inherit from `Entity`. This creates the infamous Diamond Problem!

**Your Mission:**
- Figure out why `FlyingWarrior` has TWO copies of `Entity` data
- Use virtual inheritance to fix it
- But watch out - virtual inheritance changes constructor behavior!

**Puzzle Question:** If `Entity` has a member `health`, and you don't use virtual inheritance:
```cpp
FlyingWarrior fw;
fw.Flying::health = 100;
fw.Warrior::health = 50;
// What is fw.health? Is this even valid?
```

---

### Objective 2.2: The Phantom Destructor (Virtual Destructor)
**File: `broken_destructor.cpp`**

Entities are being deleted through base class pointers, but derived class destructors aren't running! Memory leaks everywhere!

**Your Mission:**
- Find where the virtual destructor is missing
- Understand WHY you need virtual destructors
- What's the overhead of making a destructor virtual?

**The Bug:**
```cpp
Entity* entity = new Dragon();
delete entity;  // Dragon's destructor never runs!
```

**Puzzle Question:** If you mark the destructor as `virtual`, but the class has no other virtual functions, does it still need a vtable?

---

### Objective 2.3: The Covariant Return Riddle
**File: `broken_clone.cpp`**

The `clone()` method returns a pointer to the base class, making it annoying to use. Can we make it return the actual derived type?

**Your Mission:**
- Implement clone using covariant return types
- Understand why this works (it shouldn't break Liskov Substitution!)

**The Annoyance:**
```cpp
Dragon* dragon = new Dragon();
Entity* cloned = dragon->clone();  // I wanted a Dragon*, not Entity*!
Dragon* dragonClone = dynamic_cast<Dragon*>(dragon->clone());  // Ugly!
```

---

### Objective 2.4: The CRTP Pattern (Static Polymorphism)
**File: `broken_performance.cpp`**

Virtual function calls are too slow for the hot path! Use CRTP to get polymorphism without vtable overhead.

**Your Mission:**
- Replace virtual dispatch with CRTP for performance-critical code
- Measure the performance difference
- Understand when CRTP is appropriate vs. virtual functions

**The Challenge:**
```cpp
// Virtual version (slow in hot loop)
for (auto& entity : millions_of_entities) {
    entity->update();  // Virtual call every iteration!
}

// CRTP version (no virtual call)
for (auto& entity : entities) {
    entity.update();  // Resolved at compile time!
}
```

---

### Objective 2.5: The Abstract Factory Puzzle (Pure Virtual)
**File: `broken_factory.cpp`**

The entity factory has a confusing mix of pure virtual, virtual, and non-virtual functions. Can you make sense of it?

**Your Mission:**
- Fix the abstract class so it can't be instantiated
- Implement the concrete factories correctly
- Can an abstract class have a constructor? Should it?

**Tricky Code:**
```cpp
class EntityFactory {
public:
    virtual Entity* create() = 0;       // Pure virtual
    virtual void configure() {}          // Virtual with default
    void logCreation() { /* ... */ }     // Non-virtual
    
    // PUZZLE: Should this constructor exist?
    EntityFactory(const std::string& name) : factoryName(name) {}
};
```

---

## 📝 Files to Modify

1. `broken_diamond.cpp` → Create `solution_diamond.cpp`
2. `broken_destructor.cpp` → Create `solution_destructor.cpp`
3. `broken_clone.cpp` → Create `solution_clone.cpp`
4. `broken_performance.cpp` → Create `solution_performance.cpp`
5. `broken_factory.cpp` → Create `solution_factory.cpp`

---

## ✅ Success Criteria

Run `test_runner.cpp` and all tests must pass:
- Diamond problem resolved (only one base class instance)
- All destructors called correctly
- Clone returns correct types
- CRTP version matches virtual version behavior
- Factory pattern works correctly

---

## 💭 Reflection Questions

After completing this challenge, answer these in `my_notes.md`:

1. Draw the memory layout of `FlyingWarrior` with and without virtual inheritance.
2. How does the vtable pointer affect object size? What about multiple inheritance?
3. When is early binding used vs. late binding in C++?
4. Why can't you call virtual functions from constructors safely?
5. What is object slicing and when does it occur?

---

## 🔑 Hints (Use sparingly!)

<details>
<summary>Hint 1: Virtual Inheritance Syntax</summary>

```cpp
class Flying : virtual public Entity { };
class Warrior : virtual public Entity { };
class FlyingWarrior : public Flying, public Warrior { };
```

The most derived class must initialize the virtual base!
</details>

<details>
<summary>Hint 2: Virtual Destructor Rule</summary>

If a class has ANY virtual functions, its destructor should be virtual.
If a class is designed to be inherited from, its destructor should be virtual.
</details>

<details>
<summary>Hint 3: CRTP Pattern</summary>

```cpp
template<typename Derived>
class Base {
public:
    void interface() {
        static_cast<Derived*>(this)->implementation();
    }
};

class Derived : public Base<Derived> {
public:
    void implementation() { /* ... */ }
};
```
</details>
