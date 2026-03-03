/**
 * ✅ SOLUTION - Scene Graph with Circular Reference Fix
 * 
 * This solution fixes the circular reference memory leak by using weak_ptr
 * for parent references.
 * 
 * KEY CONCEPTS:
 * - Parent nodes OWN children (use shared_ptr)
 * - Children only REFERENCE parents (use weak_ptr) - breaking the cycle
 * - When parent is deleted, children become orphaned but don't crash
 */

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <optional>

class SceneNode : public std::enable_shared_from_this<SceneNode> {
private:
    std::string name;
    
    // FIXED: Use weak_ptr for parent (break circular reference)
    std::weak_ptr<SceneNode> parent;
    
    // Keep shared_ptr for children (parents own children)
    std::vector<std::shared_ptr<SceneNode>> children;
    
    float localX, localY;
    
public:
    SceneNode(const std::string& nodeName) 
        : name(nodeName), localX(0), localY(0) {
        std::cout << "  [+] SceneNode created: " << name << "\n";
    }
    
    ~SceneNode() {
        std::cout << "  [-] SceneNode destroyed: " << name << "\n";
    }
    
    void addChild(std::shared_ptr<SceneNode> child) {
        // Parent-child relationship: parent owns child
        child->parent = shared_from_this();  // weak_ptr - no cycle!
        children.push_back(child);
        std::cout << "  [>] " << name << " added child: " << child->name << "\n";
    }
    
    void removeChild(const std::string& childName) {
        children.erase(
            std::remove_if(children.begin(), children.end(),
                [&](const std::shared_ptr<SceneNode>& node) {
                    return node->name == childName;
                }),
            children.end()
        );
        std::cout << "  [x] " << name << " removed child: " << childName << "\n";
    }
    
    // Safe parent access - returns Optional (empty if parent was destroyed)
    std::optional<std::shared_ptr<SceneNode>> getParent() {
        if (auto p = parent.lock()) {  // lock() creates shared_ptr from weak_ptr
            return p;
        }
        return std::nullopt;  // Parent was destroyed
    }
    
    // Calculate world position with safe parent traversal
    std::pair<float, float> getWorldPosition() {
        float worldX = localX;
        float worldY = localY;
        
        // Safely traverse up the tree
        if (auto p = parent.lock()) {
            auto [parentX, parentY] = p->getWorldPosition();
            worldX += parentX;
            worldY += parentY;
        }
        
        return {worldX, worldY};
    }
    
    void setLocalPosition(float x, float y) {
        localX = x;
        localY = y;
    }
    
    void printTree(int depth = 0) {
        std::string indent(depth * 2, ' ');
        std::cout << indent << "- " << name;
        
        // Safely print parent's name
        if (auto p = parent.lock()) {
            std::cout << " (parent: " << p->name << ")";
        } else if (depth > 0) {
            std::cout << " (parent: ORPHANED)";
        }
        std::cout << "\n";
        
        for (auto& child : children) {
            child->printTree(depth + 1);
        }
    }
    
    // Utility: Check if this node is still attached to a root
    bool isAttachedToRoot() {
        if (auto p = parent.lock()) {
            return p->isAttachedToRoot();
        }
        return true;  // This is root or orphaned - consider it "root-like"
    }
    
    // Reparent a node from one parent to another
    void reparentChild(std::shared_ptr<SceneNode> child, std::shared_ptr<SceneNode> newParent) {
        // Remove from current parent
        auto currentParent = child->parent.lock();
        if (currentParent) {
            currentParent->removeChild(child->name);
        }
        
        // Add to new parent
        newParent->addChild(child);
    }
    
    std::string getName() const { return name; }
};

// ============================================
// DEMONSTRATION OF THE FIX
// ============================================

void demonstrateCircularReferenceFix() {
    std::cout << "\n=== CIRCULAR REFERENCE FIX DEMO ===\n\n";
    
    {
        std::cout << "Creating scene graph...\n";
        
        // Create nodes
        auto root = std::make_shared<SceneNode>("Root");
        auto player = std::make_shared<SceneNode>("Player");
        auto weapon = std::make_shared<SceneNode>("Weapon");
        auto enemy = std::make_shared<SceneNode>("Enemy");
        
        // Build tree
        root->addChild(player);
        player->addChild(weapon);
        root->addChild(enemy);
        
        // Set positions
        root->setLocalPosition(0, 0);
        player->setLocalPosition(100, 100);
        weapon->setLocalPosition(10, 0);
        
        std::cout << "\nScene tree:\n";
        root->printTree();
        
        // Get weapon's world position
        auto [wx, wy] = weapon->getWorldPosition();
        std::cout << "\nWeapon world position: (" << wx << ", " << wy << ")\n";
        
        // Check parent access
        if (auto p = weapon->getParent()) {
            std::cout << "Weapon's parent: " << p.value()->getName() << "\n";
        }
        
        std::cout << "\nLeaving scope - all nodes should destroy...\n";
    }
    
    std::cout << "\n✅ All nodes properly destroyed! No circular reference leak!\n";
}

void demonstrateOrphanedNodes() {
    std::cout << "\n=== ORPHANED NODES DEMO ===\n\n";
    
    std::shared_ptr<SceneNode> savedChild;
    
    {
        auto root = std::make_shared<SceneNode>("Root");
        auto child = std::make_shared<SceneNode>("Child");
        auto grandchild = std::make_shared<SceneNode>("Grandchild");
        
        root->addChild(child);
        child->addChild(grandchild);
        
        // Save reference to grandchild (it stays alive even if tree is destroyed)
        savedChild = grandchild;
        
        std::cout << "\nTree structure:\n";
        root->printTree();
        
        std::cout << "\nRoot going out of scope...\n";
    }
    
    std::cout << "\nGrandchild still alive (we hold a shared_ptr to it)\n";
    std::cout << "Its parent: ";
    
    if (auto p = savedChild->getParent()) {
        std::cout << p.value()->getName() << "\n";
        
        // The parent is also still alive!
        if (auto gp = p.value()->getParent()) {
            std::cout << "Its parent's parent: " << gp.value()->getName() << "\n";
        } else {
            std::cout << "Its parent's parent: DESTROYED (orphaned)\n";
        }
    } else {
        std::cout << "DESTROYED (parent was weak_ptr)\n";
    }
    
    std::cout << "\nSaved grandchild world position: ";
    auto [x, y] = savedChild->getWorldPosition();
    std::cout << "(" << x << ", " << y << ")\n";
    
    std::cout << "\n✅ Orphaned nodes work correctly!\n";
}

void demonstrateReparenting() {
    std::cout << "\n=== REPARENTING DEMO ===\n\n";
    
    {
        auto root = std::make_shared<SceneNode>("Root");
        auto parent1 = std::make_shared<SceneNode>("Parent1");
        auto parent2 = std::make_shared<SceneNode>("Parent2");
        auto child = std::make_shared<SceneNode>("Child");
        
        root->addChild(parent1);
        root->addChild(parent2);
        parent1->addChild(child);
        
        std::cout << "Initial structure:\n";
        root->printTree();
        
        std::cout << "\nReparenting child from Parent1 to Parent2...\n";
        root->reparentChild(child, parent2);
        
        std::cout << "\nNew structure:\n";
        root->printTree();
        
        std::cout << "\nChild's parent is now: ";
        if (auto p = child->getParent()) {
            std::cout << p.value()->getName() << "\n";
        }
    }
    
    std::cout << "\n✅ Reparenting works correctly!\n";
}

// ============================================
// EXPLANATION NOTES
// ============================================

/*
 * SOLUTION ANALYSIS:
 * 
 * 1. THE CIRCULAR REFERENCE PROBLEM:
 *    Parent: shared_ptr<Child> → keeps Child alive
 *    Child: shared_ptr<Parent> → keeps Parent alive
 *    Result: Neither can be deleted = MEMORY LEAK!
 * 
 * 2. HOW WEAK_PTR SOLVES IT:
 *    Parent: shared_ptr<Child> → keeps Child alive (1)
 *    Child: weak_ptr<Parent> → doesn't keep Parent alive (0)
 *    Result: When no other refs to Parent exist, it's deleted
 *            Child's weak_ptr becomes invalid (can detect this)
 * 
 * 3. OWNERSHIP SEMANTICS:
 *    - Tree nodes OWN their children
 *    - Children don't own parents (just reference them)
 *    - This is the natural hierarchy
 * 
 * 4. WEAK_PTR USAGE:
 *    std::weak_ptr<T> wp = shared_ptr;
 *    std::shared_ptr<T> sp = wp.lock();  // Returns shared_ptr or nullptr
 *    if (auto sp = wp.lock()) { ... }     // Safe access pattern
 *    bool expired = wp.expired();         // Check if target was deleted
 * 
 * 5. SAFE PARENT ACCESS:
 *    Used optional<shared_ptr> return to indicate if parent exists
 *    This makes the API explicit about parent destruction
 * 
 * 6. ORPHANED NODES:
 *    When parent is destroyed, child's weak_ptr becomes invalid
 *    But child can still exist if held by shared_ptr
 *    Child gracefully handles this (local position only in getWorldPosition)
 * 
 * 7. REPARENTING:
 *    Safely move nodes between parents
 *    Old parent releases ownership, new parent acquires it
 * 
 * 8. TRICKY QUESTION ANSWERED:
 *    "Why can't you use weak_ptr for both parent and children?"
 *    Answer: Then NOBODY owns the nodes! They'd all be destroyed immediately.
 *            The tree structure wouldn't exist at all.
 * 
 * 9. MEMORY LAYOUT:
 *    Before fix:
 *      Parent (ref_count=2) → Child (ref_count=2)
 *                             ↑_______|
 *      Result: Both stuck with ref_count=1, never deleted!
 *    
 *    After fix:
 *      Parent (ref_count=1) → Child (ref_count=1)
 *                             weak_ptr (not counted)
 *      Result: When Parent shared_ptr releases, count→0, deleted!
 *              Then Child shared_ptr releases, count→0, deleted!
 */

int main() {
    demonstrateCircularReferenceFix();
    demonstrateOrphanedNodes();
    demonstrateReparenting();
    
    std::cout << "\n========================================\n";
    std::cout << "✅ All circular references fixed!\n";
    std::cout << "✅ Tree structure works correctly!\n";
    std::cout << "========================================\n";
    
    return 0;
}
