# 🔄 Challenge 5: The Thread Scheduler

## The Situation

The game's job system runs tasks across multiple threads, but it's plagued with race conditions, deadlocks, and inconsistent data. Players report random crashes and corrupted saves!

Your task: Fix the concurrency bugs and implement a thread-safe job scheduler.

---

## 🎯 Objectives

### Objective 5.1: The Race Condition Hunt
**File: `broken_counter.cpp`**

Multiple threads increment a shared counter. Sometimes it's wrong!

**Your Mission:**
- Find the race condition
- Fix it using `std::mutex`
- Then optimize with `std::atomic`

**The Bug:**
```cpp
int counter = 0;

void increment() {
    counter++;  // NOT ATOMIC! Read-modify-write is 3 operations!
}
// Multiple threads = lost updates!
```

---

### Objective 5.2: The Deadlock Labyrinth
**File: `broken_deadlock.cpp`**

Two resources need to be locked together. Sometimes threads freeze forever!

**Your Mission:**
- Identify the deadlock scenario
- Fix using `std::lock()` or `std::scoped_lock`
- Understand lock ordering

**The Deadlock:**
```cpp
// Thread 1:             // Thread 2:
lock(mutexA);           lock(mutexB);    // Both locked...
lock(mutexB);  // Wait!  lock(mutexA);   // Wait!
// DEADLOCK - both waiting for each other!
```

---

### Objective 5.3: Memory Ordering Mysteries
**File: `broken_memory_order.cpp`**

Flag-based synchronization isn't working! Understanding memory ordering is key.

**Your Mission:**
- Learn about acquire/release semantics
- Fix the broken flag synchronization
- Understand when `memory_order_relaxed` is (un)safe

**The Subtle Bug:**
```cpp
std::atomic<bool> ready{false};
int data = 0;

// Thread 1:
data = 42;
ready.store(true, std::memory_order_relaxed);

// Thread 2:
while (!ready.load(std::memory_order_relaxed));
use(data);  // Might see data = 0! Why?
```

---

### Objective 5.4: The Reader-Writer Problem
**File: `broken_rwlock.cpp`**

Many threads read, few threads write. Using exclusive locks kills performance!

**Your Mission:**
- Implement using `std::shared_mutex`
- Multiple readers can read simultaneously
- Writers get exclusive access

**The Optimization:**
```cpp
// Bad: All access exclusive
std::mutex mtx;
void read() { lock_guard l(mtx); /* read */ }
void write() { lock_guard l(mtx); /* write */ }

// Good: Readers can share
std::shared_mutex mtx;
void read() { shared_lock l(mtx); /* read */ }
void write() { unique_lock l(mtx); /* write */ }
```

---

### Objective 5.5: Lock-Free Data Structure
**File: `broken_lockfree_queue.cpp`**

The job queue uses locks. Can you make it lock-free?

**Your Mission:**
- Implement a lock-free queue using atomics
- Handle the ABA problem
- Compare performance with locked version

**The Challenge:**
```cpp
// Lock-free push (simplified)
void push(T value) {
    Node* newNode = new Node(value);
    newNode->next = head.load();
    while (!head.compare_exchange_weak(newNode->next, newNode));
}
// But what about memory reclamation? And ABA?
```

---

## 📝 Files to Modify

1. `broken_counter.cpp` → Create `solution_counter.cpp`
2. `broken_deadlock.cpp` → Create `solution_deadlock.cpp`
3. `broken_memory_order.cpp` → Create `solution_memory_order.cpp`
4. `broken_rwlock.cpp` → Create `solution_rwlock.cpp`
5. `broken_lockfree_queue.cpp` → Create `solution_lockfree_queue.cpp`

---

## ✅ Success Criteria

- Counter is always correct after concurrent increments
- No deadlocks possible
- Memory ordering ensures correct synchronization
- Reader-writer lock allows concurrent reads
- (Bonus) Lock-free queue passes stress tests

---

## 💭 Reflection Questions

1. What's the difference between a data race and a race condition?
2. When is `std::atomic<T>` lock-free? How do you check?
3. What are the different memory orderings and when to use each?
4. Why is `std::recursive_mutex` usually a code smell?
5. What is the ABA problem and how do you solve it?

---

## 🔑 Hints

<details>
<summary>Hint 1: Atomic Operations</summary>

```cpp
std::atomic<int> counter{0};
counter.fetch_add(1);  // Atomic increment
counter++;              // Also atomic!
```
</details>

<details>
<summary>Hint 2: Avoiding Deadlock</summary>

```cpp
// Use std::scoped_lock for multiple mutexes
std::scoped_lock lock(mutexA, mutexB);  // Locks both atomically!
```
</details>

<details>
<summary>Hint 3: Acquire-Release</summary>

```cpp
// Writer (release): All writes before this are visible
ready.store(true, std::memory_order_release);

// Reader (acquire): Sees all writes before the release
while (!ready.load(std::memory_order_acquire));
```
</details>
