/**
 * 🔴 BROKEN CODE - Scene Graph with Circular References
 * 
 * A scene graph is a tree structure where each node can have:
 * - Multiple children
 * - One parent (except root)
 * 
 * The problem: Circular shared_ptr references cause memory leaks!
 * 
 * YOUR TASK: Fix the circular reference issue using weak_ptr correctly!
 */

#include <iostream>
#include <memory>
#include <string>
#include <vector>

class SceneNode : public std::enable_shared_from_this<SceneNode> {
private:
    std::string name;
    
    // BUG: Circular reference! Parent holds shared_ptr to child,
    // child holds shared_ptr to parent. Neither can ever be freed!
    std::shared_ptr<SceneNode> parent;
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
        // BUG: This creates a cycle!
        child->parent = shared_from_this();  // Parent -> Child AND Child -> Parent
        children.push_back(child);
        std::cout << "  [>] " << name << " added child: " << child->name << "\n";
    }
    
    void removeChild(const std::string& childName) {
        // PUZZLE: Even after removing, will the child be destroyed?
        // What if the child has its own children?
        children.erase(
            std::remove_if(children.begin(), children.end(),
                [&](const std::shared_ptr<SceneNode>& node) {
                    return node->name == childName;
                }),
            children.end()
        );
        std::cout << "  [x] " << name << " removed child: " << childName << "\n";
    }
    
    // BUG: What if parent was destroyed? This returns a shared_ptr to garbage!
    std::shared_ptr<SceneNode> getParent() {
        return parent;
    }
    
    // Calculate world position by traversing up the tree
    std::pair<float, float> getWorldPosition() {
        float worldX = localX;
        float worldY = localY;
        
        // PUZZLE: If parent is a weak_ptr, how do you safely traverse?
        if (parent) {
            auto [parentX, parentY] = parent->getWorldPosition();
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
        
        // PUZZLE: How do you print parent's name if parent is weak_ptr?
        if (parent) {
            std::cout << " (parent: " << parent->name << ")";
        }
        std::cout << "\n";
        
        for (auto& child : children) {
            child->printTree(depth + 1);
        }
    }
    
    // Utility to count how many nodes exist
    static int& getInstanceCount() {
        static int count = 0;
        return count;
    }
};

// ============================================
// DEMONSTRATION OF THE MEMORY LEAK
// ============================================

void demonstrateCircularReferenceLeak() {
    std::cout << "\n=== CIRCULAR REFERENCE LEAK DEMO ===\n\n";
    
    {
        std::cout << "Creating scene graph...\n";
        
        // Create a simple scene
        auto root = std::make_shared<SceneNode>("Root");
        auto player = std::make_shared<SceneNode>("Player");
        auto weapon = std::make_shared<SceneNode>("Weapon");
        auto enemy = std::make_shared<SceneNode>("Enemy");
        
        // Build the tree
        root->addChild(player);
        player->addChild(weapon);
        root->addChild(enemy);
        
        // Set positions
        root->setLocalPosition(0, 0);
        player->setLocalPosition(100, 100);
        weapon->setLocalPosition(10, 0);  // Relative to player
        
        std::cout << "\nScene tree:\n";
        root->printTree();
        
        // Get world position of weapon
        auto [wx, wy] = weapon->getWorldPosition();
        std::cout << "\nWeapon world position: (" << wx << ", " << wy << ")\n";
        
        std::cout << "\nLeaving scope - nodes should be destroyed...\n";
    }
    
    std::cout << "\n--- Scope exited ---\n";
    std::cout << "If you don't see [-] destroyed messages for ALL nodes,\n";
    std::cout << "there's a memory leak!\n";
}

// ============================================
// TRICKY SCENARIO: ORPHANED SUBTREES
// ============================================

void demonstrateOrphanedSubtree() {
    std::cout << "\n=== ORPHANED SUBTREE SCENARIO ===\n\n";
    
    std::shared_ptr<SceneNode> savedChild;
    
    {
        auto root = std::make_shared<SceneNode>("Root");
        auto child = std::make_shared<SceneNode>("Child");
        auto grandchild = std::make_shared<SceneNode>("Grandchild");
        
        root->addChild(child);
        child->addChild(grandchild);
        
        // Save a reference to the grandchild
        savedChild = grandchild;
        
        // PUZZLE: What happens when root goes out of scope?
        // The grandchild is kept alive by savedChild, but what about
        // its parent pointer? Is it still valid?
        
        std::cout << "\nRoot going out of scope...\n";
    }
    
    std::cout << "\nTrying to access saved grandchild's parent...\n";
    
    // BUG: If parent is still a shared_ptr, this keeps the entire
    // chain alive (leak). If it's weak_ptr but we don't check,
    // we might access freed memory!
    auto parent = savedChild->getParent();
    if (parent) {
        std::cout << "Parent name: " << "???" << "\n";  // Can we safely access this?
    }
}

// ============================================
// YOUR TASK: Create a fixed version!
// ============================================

/*
 * TODO: Create the following in solution_scene_graph.cpp:
 * 
 * 1. class FixedSceneNode
 *    - Use weak_ptr for parent reference (non-owning)
 *    - Keep shared_ptr for children (owning)
 *    - Safe getParent() that returns optional or checks validity
 *    - Safe getWorldPosition() that handles orphaned nodes
 * 
 * TRICKY QUESTIONS:
 * 
 * Q1: After you fix the parent to be weak_ptr, how do you safely
 *     implement getWorldPosition()? What if parent expired mid-calculation?
 * 
 * Q2: Should getParent() return shared_ptr, weak_ptr, or raw pointer?
 *     What are the implications of each?
 * 
 * Q3: If a node is removed from the tree but someone still holds a
 *     shared_ptr to it, what should getWorldPosition() return?
 *     Options:
 *     a) Throw an exception
 *     b) Return local position only
 *     c) Return (0,0)
 *     d) Return std::optional<pair<float,float>>
 *     Which is best? Why?
 * 
 * Q4: BONUS - Can you detect if a node is "orphaned" (detached from root)?
 *     How would you implement isAttachedToRoot()?
 * 
 * EXTRA CHALLENGE:
 * Implement a "reparent" function that moves a node from one parent to another.
 * What edge cases must you handle?
 */

int main() {
    demonstrateCircularReferenceLeak();
    demonstrateOrphanedSubtree();
    
    std::cout << "\n\n========================================\n";
    std::cout << "Check destructor messages above!\n";
    std::cout << "Missing destructors = Memory leak!\n";
    std::cout << "========================================\n";
    
    return 0;
}
