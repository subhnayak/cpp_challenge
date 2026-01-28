# 🎮 Mystical Game Engine Challenge

Welcome to the **Mystical Game Engine Challenge** - a hands-on C++ learning project where you'll build components of a mini game engine while mastering advanced C++ concepts!

## 🎯 The Story

You've joined "Arcane Studios" as a C++ developer. The lead developer mysteriously disappeared, leaving behind broken code, cryptic TODOs, and half-implemented features. Your mission: Fix, complete, and enhance the game engine while learning crucial C++ concepts along the way.

---

## 📁 Project Structure

```
ref_folder/
├── challenges/
│   ├── challenge_01_memory_arena/      # Memory Management & Smart Pointers
│   ├── challenge_02_entity_system/     # OOP & Virtual Functions
│   ├── challenge_03_spell_factory/     # Templates & Generic Programming
│   ├── challenge_04_event_bus/         # Modern C++ Features
│   ├── challenge_05_thread_scheduler/  # Concurrency & Multithreading
│   ├── challenge_06_serializer/        # Type System & Casting
│   └── challenge_07_final_boss/        # Advanced Concepts (Rule of 5, etc.)
├── engine_core/                        # Shared engine utilities
├── tests/                              # Test files to verify your solutions
└── solutions/                          # Hidden solutions (don't peek!)
```

---

## 🏆 Challenges Overview

### Challenge 1: The Memory Arena (Memory Management)
**Difficulty: ⭐⭐⭐**
The game's memory allocator is leaking everywhere! Implement a custom memory arena with smart pointers.
- Fix the `unique_ptr` vs `shared_ptr` confusion
- Implement RAII-compliant resource handles
- Debug mysterious dangling pointer bugs using `weak_ptr`

### Challenge 2: The Entity System (OOP & Inheritance)
**Difficulty: ⭐⭐⭐⭐**
The entity hierarchy is a mess with the diamond problem causing chaos!
- Fix the broken `Character` class hierarchy
- Understand why the vtable is corrupted
- Implement proper virtual inheritance

### Challenge 3: The Spell Factory (Templates)
**Difficulty: ⭐⭐⭐⭐⭐**
Create a type-safe spell casting system using template magic.
- Implement template specializations for different spell types
- Use SFINAE to restrict spell combinations
- Master variadic templates for spell chains

### Challenge 4: The Event Bus (Modern C++)
**Difficulty: ⭐⭐⭐**
Build a modern event system using lambdas and move semantics.
- Capture semantics puzzles
- Perfect forwarding challenges
- `constexpr` compile-time event validation

### Challenge 5: The Thread Scheduler (Concurrency)
**Difficulty: ⭐⭐⭐⭐⭐**
The game's job system has race conditions everywhere!
- Find and fix data races
- Implement lock-free structures with atomics
- Master memory ordering

### Challenge 6: The Serializer (Type System)
**Difficulty: ⭐⭐⭐⭐**
Save/load game state with proper type casting.
- Choose the right cast for each situation
- Handle type punning safely
- Debug undefined behavior

### Challenge 7: Final Boss - The Legacy Code (Advanced)
**Difficulty: ⭐⭐⭐⭐⭐**
Refactor ancient code using modern C++ best practices.
- Apply Rule of Five correctly
- Handle exception safety
- Master copy/move semantics

---

## 🚀 Getting Started

1. Each challenge folder contains:
   - `README.md` - Challenge description and hints
   - `broken_code.cpp` - The buggy implementation to fix
   - `objectives.hpp` - Interface you must implement
   - `test_runner.cpp` - Tests to verify your solution

2. Compile with: `g++ -std=c++20 -o test test_runner.cpp your_solution.cpp`

3. Run tests: `./test`

---

## 💡 Rules

1. **No peeking at solutions** until you've genuinely tried
2. **Each challenge builds on previous ones** - complete in order
3. **Comments marked `// PUZZLE:` contain hints**
4. **Comments marked `// BUG:` indicate broken code to fix**
5. **Comments marked `// TODO:` are features you must implement**

---

## 🎓 Learning Outcomes

By completing all challenges, you'll master:
- Smart pointers and RAII
- Virtual functions and vtables
- Template metaprogramming and SFINAE
- Move semantics and perfect forwarding
- Concurrent programming and atomics
- Type-safe casting
- Exception safety and resource management

**Good luck, brave developer! The fate of Arcane Studios rests in your hands!** 🧙‍♂️
