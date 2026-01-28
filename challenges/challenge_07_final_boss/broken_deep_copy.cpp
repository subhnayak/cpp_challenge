/**
 * 🔴 BROKEN CODE - Deep Copy vs Shallow Copy
 * 
 * Classes with pointers need deep copies, but the compiler
 * generates shallow copies by default!
 * 
 * YOUR TASK: Implement proper deep copy semantics!
 */

#include <iostream>
#include <cstring>
#include <algorithm>

// ============================================
// BUG: SHALLOW COPY DISASTER
// ============================================

class ShallowProblem {
private:
    int* array;
    size_t size;
    
public:
    ShallowProblem(size_t n) : size(n) {
        array = new int[size];
        for (size_t i = 0; i < size; i++) {
            array[i] = static_cast<int>(i);
        }
        std::cout << "  [Constructed array at " << array << "]\n";
    }
    
    ~ShallowProblem() {
        std::cout << "  [Destroying array at " << array << "]\n";
        delete[] array;
    }
    
    // BUG: No copy constructor defined!
    // Compiler generates: ShallowProblem(const ShallowProblem& other)
    //   : array(other.array), size(other.size) {}
    // This just copies the POINTER, not the data!
    
    // BUG: No copy assignment defined!
    // Same problem - just copies the pointer
    
    void set(size_t index, int value) {
        if (index < size) {
            array[index] = value;
        }
    }
    
    int get(size_t index) const {
        return (index < size) ? array[index] : -1;
    }
    
    void print(const std::string& name) const {
        std::cout << "  " << name << " (at " << array << "): [";
        for (size_t i = 0; i < size; i++) {
            std::cout << array[i];
            if (i < size - 1) std::cout << ", ";
        }
        std::cout << "]\n";
    }
};

void demonstrateShallowCopyProblem() {
    std::cout << "=== SHALLOW COPY PROBLEM ===\n\n";
    
    std::cout << "Creating original:\n";
    ShallowProblem original(5);
    original.print("original");
    
    std::cout << "\nCopying to 'copy':\n";
    ShallowProblem copy = original;  // Shallow copy!
    copy.print("copy");
    
    std::cout << "\nNotice: Both point to SAME address!\n";
    
    std::cout << "\nModifying 'copy' element 0 to 999:\n";
    copy.set(0, 999);
    
    std::cout << "\nBoth objects affected (shared data!):\n";
    original.print("original");
    copy.print("copy");
    
    std::cout << "\nLeaving scope (DOUBLE FREE!):\n";
    // Both destructors will try to delete the same memory!
}

// ============================================
// BUG: COMPLEX OBJECT WITH NESTED POINTERS
// ============================================

struct Node {
    int value;
    Node* next;
    
    Node(int v) : value(v), next(nullptr) {}
};

class BrokenLinkedList {
private:
    Node* head;
    size_t count;
    
public:
    BrokenLinkedList() : head(nullptr), count(0) {}
    
    void append(int value) {
        Node* newNode = new Node(value);
        if (!head) {
            head = newNode;
        } else {
            Node* current = head;
            while (current->next) {
                current = current->next;
            }
            current->next = newNode;
        }
        count++;
    }
    
    ~BrokenLinkedList() {
        Node* current = head;
        while (current) {
            Node* next = current->next;
            delete current;
            current = next;
        }
    }
    
    // BUG: Shallow copy - only copies head pointer!
    // All nodes are shared between original and copy!
    
    void print(const std::string& name) const {
        std::cout << "  " << name << ": ";
        Node* current = head;
        while (current) {
            std::cout << current->value;
            if (current->next) std::cout << " -> ";
            current = current->next;
        }
        std::cout << " (head=" << head << ")\n";
    }
    
    void setFirst(int value) {
        if (head) head->value = value;
    }
};

void demonstrateNestedPointerProblem() {
    std::cout << "\n=== NESTED POINTER PROBLEM ===\n\n";
    
    BrokenLinkedList list1;
    list1.append(1);
    list1.append(2);
    list1.append(3);
    
    std::cout << "Original list:\n";
    list1.print("list1");
    
    std::cout << "\nCopying (shallow!):\n";
    BrokenLinkedList list2 = list1;
    list2.print("list2");
    
    std::cout << "\nModifying list2's first element to 999:\n";
    list2.setFirst(999);
    
    std::cout << "\nBoth lists affected:\n";
    list1.print("list1");
    list2.print("list2");
    
    std::cout << "\n(Destructor will cause double-free!)\n";
}

// ============================================
// BUG: SELF-ASSIGNMENT PROBLEM
// ============================================

class BadSelfAssign {
private:
    int* data;
    
public:
    BadSelfAssign(int value) : data(new int(value)) {}
    
    ~BadSelfAssign() { delete data; }
    
    // Trying to do deep copy, but...
    BadSelfAssign& operator=(const BadSelfAssign& other) {
        // BUG: No self-assignment check!
        delete data;  // If this == &other, we just deleted our own data!
        data = new int(*other.data);  // Now other.data points to freed memory!
        return *this;
    }
    
    int get() const { return *data; }
};

void demonstrateSelfAssignment() {
    std::cout << "\n=== SELF-ASSIGNMENT PROBLEM ===\n\n";
    
    BadSelfAssign obj(42);
    std::cout << "  Before: " << obj.get() << "\n";
    
    // Self-assignment
    // obj = obj;  // CRASH or UB!
    std::cout << "  (Self-assignment commented out to avoid crash)\n";
}

// ============================================
// YOUR TASK
// ============================================

/*
 * TODO: Create solution_deep_copy.cpp with:
 * 
 * 1. DeepCopyArray with proper deep copy:
 *    
 *    class DeepCopyArray {
 *        int* array;
 *        size_t size;
 *        
 *    public:
 *        // Deep copy constructor
 *        DeepCopyArray(const DeepCopyArray& other) : size(other.size) {
 *            array = new int[size];
 *            std::copy(other.array, other.array + size, array);
 *        }
 *        
 *        // Deep copy assignment (with self-assignment check)
 *        DeepCopyArray& operator=(const DeepCopyArray& other) {
 *            if (this != &other) {
 *                int* newArray = new int[other.size];  // Allocate first!
 *                std::copy(other.array, other.array + other.size, newArray);
 *                delete[] array;  // Only then delete old
 *                array = newArray;
 *                size = other.size;
 *            }
 *            return *this;
 *        }
 *    };
 * 
 * 2. DeepLinkedList with recursive deep copy:
 *    
 *    Node* deepCopyNodes(Node* source) {
 *        if (!source) return nullptr;
 *        Node* newNode = new Node(source->value);
 *        newNode->next = deepCopyNodes(source->next);
 *        return newNode;
 *    }
 * 
 * 3. Using copy-and-swap for exception safety:
 *    
 *    DeepCopyArray& operator=(DeepCopyArray other) {  // By value
 *        swap(*this, other);
 *        return *this;
 *    }
 *    // No self-assignment check needed!
 *    // Handles exception safely too!
 * 
 * TRICKY QUESTIONS:
 * 
 * Q1: Why allocate new memory BEFORE deleting old in the naive approach?
 *     What happens if allocation fails and you've already deleted?
 * 
 * Q2: Copy-and-swap takes parameter by value. Isn't that inefficient
 *     (two copies: one to create 'other', one in copy constructor)?
 *     (Hint: copy elision and move semantics)
 * 
 * Q3: For linked list deep copy, what if the list is circular?
 *     How do you detect and handle cycles?
 * 
 * Q4: When is shallow copy actually desirable?
 *     (Hint: shared ownership, immutable data)
 * 
 * Q5: BONUS - Implement copy-on-write (COW):
 *     - Multiple objects share the same data initially
 *     - When one modifies, it makes a private copy first
 *     - Optimization for read-heavy workloads
 */

int main() {
    // Note: This will crash with double-free!
    // Comment out parts to test individual sections.
    
    // demonstrateShallowCopyProblem();  // Double free!
    // demonstrateNestedPointerProblem();  // Double free!
    demonstrateSelfAssignment();
    
    std::cout << "\n========================================\n";
    std::cout << "Implement proper deep copy!\n";
    std::cout << "========================================\n";
    
    return 0;
}
