# 📚 Solutions Index - Mystical Game Engine Challenge

## Quick Reference Table

| Challenge | File | Concepts | Status |
|-----------|------|----------|--------|
| **1: Memory Arena** | | | |
| | `solution_texture_cache.cpp` | shared_ptr, ownership | ✅ |
| | `solution_scene_graph.cpp` | weak_ptr, circular references | ✅ |
| | `solution_gpu_resource.cpp` | RAII, custom deleters, unique_ptr | ✅ |
| | `solution_asset_loader.cpp` | Move semantics, perfect forwarding | ✅ |
| **2: Entity System** | | | |
| | `solution_diamond.cpp` | Virtual inheritance, polymorphism | ✅ |
| | `solution_destructor.cpp` | Virtual destructors | ✅ |
| **3: Spell Factory** | | | |
| | `solution_spell_types.cpp` | Template specialization | ✅ |
| | `solution_spell_chain.cpp` | Variadic templates, fold expressions | ✅ |
| **4: Event Bus** | | | |
| | `solution_event_handler.cpp` | Lambda captures, perfect forwarding | ✅ |
| | `solution_event_dispatch.cpp` | Move semantics for events | ✅ |
| **5: Thread Scheduler** | | | |
| | `solution_counter.cpp` | std::atomic, mutex, std::scoped_lock | ✅ |
| | `solution_deadlock.cpp` | Deadlock prevention | ✅ |
| | `solution_memory_order.cpp` | Memory ordering semantics | ✅ |
| **6: Serializer** | | | |
| | `solution_casts.cpp` | All four C++ casts | ✅ |
| | `solution_binary_format.cpp` | Safe binary I/O with memcpy | ✅ |
| **7: Final Boss** | | | |
| | `solution_resource_class.cpp` | Rule of Five, copy-swap idiom | ✅ |
| | `solution_deep_copy.cpp` | Deep vs shallow copy | ✅ |
| | `solution_exception_safety.cpp` | Exception safety levels | ✅ |

---

## Challenge 1: Memory Arena 🧠

### What You'll Learn
- Smart pointer selection (unique_ptr vs shared_ptr vs weak_ptr)
- Ownership semantics
- RAII pattern for resource management
- Exception-safe cleanup
- Move semantics and perfect forwarding

### Files
- `solution_texture_cache.cpp` - **Problem:** Raw pointers cause leaks | **Solution:** shared_ptr for shared ownership
- `solution_scene_graph.cpp` - **Problem:** Circular references prevent cleanup | **Solution:** weak_ptr breaks cycles
- `solution_gpu_resource.cpp` - **Problem:** GPU resources need special cleanup | **Solution:** RAII with custom deleters
- `solution_asset_loader.cpp` - **Problem:** Unnecessary copies of large objects | **Solution:** Move semantics and emplace

### Key Code Patterns
```cpp
// Shared ownership
auto texture = std::make_shared<Texture>("hero.png");
std::vector<std::shared_ptr<Texture>> sprites;

// Break cycles
class Node {
    std::weak_ptr<Node> parent;  // Non-owning
    std::vector<std::shared_ptr<Node>> children;  // Owning
};

// RAII with custom deleter
std::unique_ptr<GLuint, ShaderDeleter> shader;

// Move semantics
loader.addAsset(std::move(asset));
```

---

## Challenge 2: Entity System 🎭

### What You'll Learn
- Virtual inheritance solving the diamond problem
- Virtual destructors for polymorphic classes
- Polymorphism vs performance (CRTP alternative)
- Proper cleanup order in hierarchies

### Files
- `solution_diamond.cpp` - **Problem:** Diamond inheritance duplicates base | **Solution:** Virtual inheritance
- `solution_destructor.cpp` - **Problem:** Derived destructors not called | **Solution:** Virtual destructors

### Key Code Patterns
```cpp
// Virtual inheritance
class Flying : virtual public Entity { };
class Warrior : virtual public Entity { };
class FlyingWarrior : public Flying, public Warrior { };

// Virtual destructors
class Entity {
public:
    virtual ~Entity() { }  // Always in base classes!
};

// Polymorphic dispatch
Entity* e = creator();
e->attack();  // Virtual call
delete e;     // Proper cleanup
```

---

## Challenge 3: Spell Factory 🧙

### What You'll Learn
- Template specialization for type-specific behavior
- SFINAE (Substitution Failure Is Not An Error)
- Variadic templates with fold expressions
- Perfect forwarding
- Template template parameters

### Files
- `solution_spell_types.cpp` - **Problem:** If-else chains for types | **Solution:** Template specialization
- `solution_spell_chain.cpp` - **Problem:** Limited spell combinations | **Solution:** Variadic templates

### Key Code Patterns
```cpp
// Template specialization
template<typename Element>
class Spell { /* generic */ };

template<>
class Spell<FireElement> { /* specialized */ };

// SFINAE constraints
template<typename T,
         typename = std::enable_if_t<is_valid<T>::value>>
void cast(T& target);

// Variadic templates with fold
template<typename... Spells>
constexpr float getTotalDamage() {
    return (SpellDamage<Spells>::value() + ...);
}
```

---

## Challenge 4: Event Bus 📡

### What You'll Learn
- Lambda expressions and capture semantics
- Move semantics for event payloads
- std::function type erasure
- Perfect forwarding
- Return Value Optimization (RVO)

### Files
- `solution_event_handler.cpp` - **Problem:** Dangling reference captures | **Solution:** Value captures
- `solution_event_dispatch.cpp` - **Problem:** Excessive event copying | **Solution:** Move semantics

### Key Code Patterns
```cpp
// Safe lambda captures
bus.on("damage", [data = std::move(localData)](const Event& e) {
    use(data);  // Safe - captured by value
});

// Move events
void emit(Event&& event) {
    for (auto& handler : handlers) {
        handler(event);  // Pass by const&
    }
}

// Perfect forwarding
template<typename F>
void on(const std::string& type, F&& handler) {
    handlers.push_back(std::forward<F>(handler));
}
```

---

## Challenge 5: Thread Scheduler 🔄

### What You'll Learn
- Atomic operations for race condition prevention
- Memory ordering (relax, acquire, release)
- Mutex and lock strategies
- Deadlock prevention
- std::shared_mutex for reader-writer patterns

### Files
- `solution_counter.cpp` - **Problem:** Race conditions on shared counter | **Solution:** std::atomic
- `solution_deadlock.cpp` - **Problem:** Inconsistent lock ordering | **Solution:** std::scoped_lock
- `solution_memory_order.cpp` - **Problem:** Memory reordering breaks synchronization | **Solution:** acquire/release

### Key Code Patterns
```cpp
// Atomic operations
std::atomic<int> counter{0};
counter.fetch_add(1, std::memory_order_relaxed);

// Memory ordering
std::atomic<bool> ready;
data = 42;
ready.store(true, std::memory_order_release);  // Writer

while (!ready.load(std::memory_order_acquire));  // Reader sees 42!

// Deadlock prevention
std::scoped_lock lock(mtxA, mtxB);  // Auto-ordered
```

---

## Challenge 6: Serializer 🔮

### What You'll Learn
- staticcast for safe related-type conversions
- dynamic_cast for runtime polymorphic type checking
- const_cast for legacy API compatibility
- reinterpret_cast dangers and proper usage
- std::memcpy for safe binary I/O

### Files
- `solution_casts.cpp` - **Problem:** C-style casts hide bugs | **Solution:** Proper C++ casts
- `solution_binary_format.cpp` - **Problem:** Type punning causes UB | **Solution:** memcpy

### Key Code Patterns
```cpp
// Static cast for safe conversions
int i = 42;
float f = static_cast<float>(i);

// Dynamic cast for runtime type checking
Dragon* d = dynamic_cast<Dragon*>(entity);
if (d) { d->breatheFire(); }

// Safe binary I/O
float readFloat(const unsigned char* buf) {
    float f;
    std::memcpy(&f, buf, sizeof(float));  // Safe
    return f;
}
```

---

## Challenge 7: Final Boss 🏰

### What You'll Learn
- Rule of Five (all special member functions)
- Rule of Zero (let smart pointers handle it)
- Copy-and-swap idiom for exception safety
- Deep vs shallow copy
- Exception safety levels (basic, strong, no-throw)

### Files
- `solution_resource_class.cpp` - **Problem:** Incomplete special member functions | **Solution:** Rule of Five
- `solution_deep_copy.cpp` - **Problem:** Shallow copy shares data | **Solution:** Deep copy with proper semantics
- `solution_exception_safety.cpp` - **Problem:** State corruption on exception | **Solution:** Strong guarantee

### Key Code Patterns
```cpp
// Rule of Five
class Resource {
    ~Resource();  // Destructor
    Resource(const Resource&);  // Copy constructor
    Resource& operator=(const Resource&);  // Copy assignment
    Resource(Resource&&) noexcept;  // Move constructor
    Resource& operator=(Resource&&) noexcept;  // Move assignment
};

// Copy-and-swap
Resource& operator=(const Resource& other) {
    Resource temp(other);  // Deep copy
    std::swap(*this, temp);  // Can't throw
    return *this;
}

// Deep copy
Resource(const Resource& other) {
    data = new int(*other.data);
}
```

---

## Compilation Commands

### Individual Files
```bash
# Compile and run
cd solutions/challenge_XX_name/
g++ -std=c++20 -Wall -Wextra -Wpedantic -o solution solution_*.cpp
./solution
```

### With Threading (Challenge 5)
```bash
g++ -std=c++20 -pthread -Wall -Wextra solution_counter.cpp -o solution
```

### With Sanitizers (Catch Memory Issues)
```bash
g++ -std=c++20 -fsanitize=address -fsanitize=undefined solution.cpp -o solution
```

---

## Recommended Study Order

1. **Challenge 1** - Foundation: Memory management essentials
2. **Challenge 2** - OOP: Inheritance and polymorphism
3. **Challenge 4** - Modern C++: Lambdas and move semantics
4. **Challenge 3** - Templates: Advanced generic programming
5. **Challenge 5** - Concurrency: Thread synchronization
6. **Challenge 6** - Type System: Safe casting
7. **Challenge 7** - Advanced: Everything combined

---

## Common Patterns

### Smart Pointer Selection
| Situation | Best Choice |
|-----------|------------|
| Single owner | `unique_ptr` |
| Multiple owners | `shared_ptr` |
| Non-owning reference | `weak_ptr` |
| Stack-allocated | No smart pointer |

### Memory Ordering
| Scenario | Choice |
|----------|--------|
| Relaxed (no sync) | `memory_order_relaxed` |
| Acquire side | `memory_order_acquire` |
| Release side | `memory_order_release` |
| Both sides | `memory_order_acq_rel` |
| Full barrier | `memory_order_seq_cst` |

### Casting Rules
| Type | Use Case |
|------|----------|
| `static_cast` | Same hierarchy, numeric conversions |
| `dynamic_cast` | Downcasting with safety check |
| `const_cast` | Remove const (careful!) |
| `reinterpret_cast` | Unrelated types (dangerous!) |
| `std::bit_cast` | Safe type punning (C++20) |

---

## Verification Checklist

- [ ] No compilation warnings
- [ ] No memory leaks (run with AddressSanitizer)
- [ ] Proper virtual destructors in base classes
- [ ] Smart pointers used appropriately
- [ ] Move semantics implemented where needed
- [ ] Thread-safe with atomics/mutexes
- [ ] Exception-safe operations
- [ ] No C-style casts
- [ ] Const-correctness maintained
- [ ] RAII pattern applied

---

## Additional Resources

### Online References
- **cppreference.com** - Standard library documentation
- **cpptips.com** - Practical C++ techniques
- **isocpp.org** - Official C++ standards

### Books
- "Effective C++" by Scott Meyers
- "Modern C++ Design" by Andrei Alexandrescu
- "C++ Concurrency in Action" by Anthony Williams

### Videos
- CppCon talks on specific topics
- Jason Turner's C++ YouTube channel
- Herb Sutter's "Guru of the Week"

---

## Next Steps

After mastering these challenges:

✅ **Real-world Projects**
- Build a game engine subsystem
- Implement a thread pool
- Create a plugin system

✅ **Advanced Topics**
- Concepts (C++20)
- Ranges library
- Modules (C++20)
- Coroutines

✅ **Performance Optimization**
- Profiling and benchmarking
- Lock-free data structures
- Memory layout optimization

---

**Last Updated:** 2026
**C++ Standard:** C++20  
**Status:** Complete ✅

