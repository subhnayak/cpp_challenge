# 🏰 Challenge 7: Final Boss - The Legacy Code

## The Situation

You've reached the final boss! An ancient codebase full of resource management nightmares, exception safety holes, and copy semantics disasters. This challenge combines everything you've learned!

Your task: Refactor the legacy code using modern C++ best practices.

---

## 🎯 Objectives

### Objective 7.1: Rule of Three/Five/Zero
**File: `broken_resource_class.cpp`**

A class manages dynamic resources but only implements some special member functions!

**Your Mission:**
- Identify which rule applies
- Implement all necessary special member functions
- Or... apply Rule of Zero and let smart pointers handle it!

**The Rules:**
```
Rule of Three (pre-C++11):
  - Destructor
  - Copy constructor  
  - Copy assignment operator

Rule of Five (C++11+):
  - Destructor
  - Copy constructor
  - Copy assignment operator
  - Move constructor
  - Move assignment operator

Rule of Zero:
  - Let member objects handle their own resources
  - No custom special member functions needed!
```

---

### Objective 7.2: Exception Safety
**File: `broken_transaction.cpp`**

Operations must either complete fully or have no effect. Half-done operations corrupt data!

**Your Mission:**
- Identify exception safety levels
- Implement strong exception guarantee
- Use copy-and-swap idiom

**The Levels:**
```
1. No guarantee: Might leak, corrupt state
2. Basic guarantee: No leaks, valid but unknown state
3. Strong guarantee: Rollback on failure (commit/rollback)
4. No-throw guarantee: Never throws (marked noexcept)
```

---

### Objective 7.3: Deep Copy vs. Shallow Copy
**File: `broken_deep_copy.cpp`**

A class with pointers is being copied, but both copies point to the same data!

**Your Mission:**
- Implement proper deep copy
- Handle self-assignment correctly
- Consider copy-on-write optimization

**The Bug:**
```cpp
class Broken {
    int* data;
    
    // Compiler-generated copy just copies the pointer!
    // Both objects now point to same memory!
    // Double-free when both are destroyed!
};
```

---

### Objective 7.4: Stack Unwinding and Cleanup
**File: `broken_stack_unwind.cpp`**

Resources aren't being cleaned up when exceptions are thrown!

**Your Mission:**
- Understand stack unwinding
- Use RAII for automatic cleanup
- Handle exceptions in destructors

**The Problem:**
```cpp
void process() {
    Resource* r1 = acquire();
    Resource* r2 = acquire();  // What if this throws?
    // r1 is leaked!
    
    doWork();  // What if THIS throws?
    // Both leaked!
    
    release(r2);
    release(r1);
}
```

---

### Objective 7.5: The Complete Game Entity
**File: `broken_game_entity.cpp`**

Combine everything! Create a game entity class that:
- Manages complex resources correctly
- Is copyable and movable
- Is exception-safe
- Uses modern C++ best practices

**Your Mission:**
- Apply all lessons from previous challenges
- Create a production-quality entity class
- Document ownership and invariants

---

## 📝 Files to Modify

1. `broken_resource_class.cpp` → Create `solution_resource_class.cpp`
2. `broken_transaction.cpp` → Create `solution_transaction.cpp`
3. `broken_deep_copy.cpp` → Create `solution_deep_copy.cpp`
4. `broken_stack_unwind.cpp` → Create `solution_stack_unwind.cpp`
5. `broken_game_entity.cpp` → Create `solution_game_entity.cpp`

---

## ✅ Success Criteria

- Rule of Five (or Zero) correctly applied
- Strong exception guarantee where needed
- No memory leaks even with exceptions
- Deep copy works correctly
- Self-assignment handled
- Move semantics don't leave invalid state

---

## 💭 Reflection Questions

1. When should you use Rule of Five vs. Rule of Zero?
2. How does copy-and-swap provide strong exception safety?
3. What happens if a destructor throws during stack unwinding?
4. Why is `noexcept` on move operations important for `std::vector`?
5. How do you implement copy-on-write optimization?

---

## 🔑 Hints

<details>
<summary>Hint 1: Copy-and-Swap Idiom</summary>

```cpp
class Resource {
    Data* data;
    
    friend void swap(Resource& a, Resource& b) noexcept {
        using std::swap;
        swap(a.data, b.data);
    }
    
    // Copy assignment using copy-and-swap
    Resource& operator=(Resource other) {  // By value!
        swap(*this, other);
        return *this;
    }  // 'other' destructor cleans up old data
};
```
</details>

<details>
<summary>Hint 2: Rule of Zero with Smart Pointers</summary>

```cpp
// Instead of:
class Old {
    int* data;
    ~Old() { delete data; }
    // Need copy ctor, copy assign, move ctor, move assign too!
};

// Use:
class Modern {
    std::unique_ptr<int> data;
    // No custom special functions needed!
    // unique_ptr handles everything!
};
```
</details>

<details>
<summary>Hint 3: Strong Exception Safety</summary>

```cpp
void transfer(Account& from, Account& to, int amount) {
    // Strong guarantee: either both succeed or neither changes
    
    // 1. Do operations on copies
    Account fromCopy = from;
    Account toCopy = to;
    
    fromCopy.withdraw(amount);  // Might throw
    toCopy.deposit(amount);     // Might throw
    
    // 2. Only if both succeeded, commit with no-throw swap
    swap(from, fromCopy);  // noexcept
    swap(to, toCopy);      // noexcept
}
```
</details>
