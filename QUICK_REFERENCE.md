# 🎮 Mystical Game Engine Challenge - Quick Reference

## Challenge Overview

| Challenge | Theme | Concepts Covered | Difficulty |
|-----------|-------|------------------|------------|
| 1 | Memory Arena | Smart pointers, RAII, Move semantics, RVO | ⭐⭐ |
| 2 | Entity System | Virtual functions, Diamond problem, CRTP | ⭐⭐⭐ |
| 3 | Spell Factory | Templates, Specialization, SFINAE, Variadic | ⭐⭐⭐⭐ |
| 4 | Event Bus | Lambdas, Captures, Perfect forwarding | ⭐⭐⭐ |
| 5 | Thread Scheduler | Atomics, Mutex, Memory ordering | ⭐⭐⭐⭐ |
| 6 | Serializer | Casts, Type punning, bit_cast | ⭐⭐⭐ |
| 7 | Final Boss | Rule of Five, Exception safety, Deep copy | ⭐⭐⭐⭐⭐ |

---

## Concepts Mapped to Challenges

### Memory Management (Challenge 1)
- ✅ new/delete, malloc/free differences
- ✅ Memory leaks, dangling pointers, double-free
- ✅ Smart pointers (unique_ptr, shared_ptr, weak_ptr)
- ✅ RAII (Resource Acquisition Is Initialization)
- ✅ Move semantics (rvalue references, std::move)
- ✅ RVO/NRVO (Return Value Optimization)

### OOP & Inheritance (Challenge 2)
- ✅ Virtual functions & vtable
- ✅ Diamond problem & virtual inheritance
- ✅ CRTP (Curiously Recurring Template Pattern)

### Templates (Challenge 3)
- ✅ Template specialization (partial/full)
- ✅ SFINAE (Substitution Failure Is Not An Error)
- ✅ Variadic templates & fold expressions

### Modern C++ (Challenge 4)
- ✅ Lambda expressions & capture semantics
- ✅ Perfect forwarding (std::forward)
- ✅ std::function and type erasure

### Concurrency (Challenge 5)
- ✅ Threading basics
- ✅ Mutex, locks, deadlocks
- ✅ Atomic operations
- ✅ Memory ordering (acquire/release)

### Type System (Challenge 6)
- ✅ static_cast, dynamic_cast, const_cast, reinterpret_cast
- ✅ Type punning and UB
- ✅ std::bit_cast (C++20)

### Advanced Concepts (Challenge 7)
- ✅ Rule of Three/Five/Zero
- ✅ Exception safety guarantees
- ✅ Deep copy vs shallow copy
- ✅ Copy-and-swap idiom

---

## How to Compile

```bash
# Single file
g++ -std=c++20 -o test broken_file.cpp

# With threading
g++ -std=c++20 -pthread -o test broken_file.cpp

# With warnings (recommended)
g++ -std=c++20 -Wall -Wextra -Wpedantic -o test broken_file.cpp
```

---

## Recommended Order

1. **Challenge 1** - Build foundation with memory concepts
2. **Challenge 2** - Understand OOP pitfalls
3. **Challenge 4** - Learn lambdas (needed for later challenges)
4. **Challenge 3** - Tackle templates (hardest patterns)
5. **Challenge 5** - Concurrency (requires concentration!)
6. **Challenge 6** - Type system mastery
7. **Challenge 7** - Final Boss (combines everything!)

---

## Tips for Success

1. **Read the bug comments** - Each file has `// BUG:` comments
2. **Run the broken code** - See the bugs in action
3. **Create solution files** - Don't modify originals
4. **Run test_runner.cpp** - Verify your solutions
5. **Answer TRICKY QUESTIONS** - Deep understanding check
6. **Use hints sparingly** - Try before peeking!

---

## Key Patterns to Master

### RAII Pattern
```cpp
class Resource {
    Handle h;
public:
    Resource() : h(acquire()) {}
    ~Resource() { release(h); }
};
```

### Copy-and-Swap
```cpp
T& operator=(T other) {  // By value
    swap(*this, other);
    return *this;
}
```

### SFINAE
```cpp
template<typename T>
std::enable_if_t<std::is_integral_v<T>, T> func(T t);
```

### Perfect Forwarding
```cpp
template<typename T>
void forward(T&& arg) {
    func(std::forward<T>(arg));
}
```

---

Good luck, brave developer! 🎮✨
