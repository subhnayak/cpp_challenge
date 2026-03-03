/**
 * ✅ SOLUTION - Asset Loader with Proper Move Semantics
 * 
 * This solution demonstrates:
 * - Proper move constructors and move assignment
 * - Avoiding unnecessary copies
 * - Using perfect forwarding with emplace_back
 * - Moved-from state handling
 * 
 * KEY CONCEPTS:
 * - Delete copy operations (assets are expensive to copy)
 * - Implement move operations (cheap to move)
 * - Use rvalue references to accept temporary objects
 * - RVO/NRVO optimization for return values
 */

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <algorithm>
#include <memory>
#include <utility>

// ============================================
// OPTIMIZED ASSET CLASS
// ============================================

class OptimizedAsset {
private:
    std::string name;
    std::vector<char> data;
    bool valid;
    
public:
    // Constructor
    OptimizedAsset(const std::string& assetName, size_t size) 
        : name(assetName), data(size, 'X'), valid(true) {
        std::cout << "  [CONSTRUCTOR] Created " << name << " (" << size << " bytes)\n";
    }
    
    // Destructor
    ~OptimizedAsset() {
        if (valid) {
            std::cout << "  [DESTRUCTOR] Destroyed " << name << "\n";
        } else {
            std::cout << "  [DESTRUCTOR] Destroyed moved-from asset\n";
        }
    }
    
    // DELETE copy operations - assets are too expensive to copy!
    OptimizedAsset(const OptimizedAsset&) = delete;
    OptimizedAsset& operator=(const OptimizedAsset&) = delete;
    
    // IMPLEMENT move constructor - transfer ownership efficiently
    OptimizedAsset(OptimizedAsset&& other) noexcept 
        : name(std::move(other.name)), 
          data(std::move(other.data)), 
          valid(other.valid) {
        other.valid = false;  // Mark source as moved-from
        std::cout << "  [MOVE CONSTRUCTOR] Moved " << name << "\n";
    }
    
    // IMPLEMENT move assignment - transfer ownership efficiently
    OptimizedAsset& operator=(OptimizedAsset&& other) noexcept {
        if (this != &other) {
            name = std::move(other.name);
            data = std::move(other.data);
            valid = other.valid;
            other.valid = false;  // Mark source as moved-from
            std::cout << "  [MOVE ASSIGNMENT] Moved to " << name << "\n";
        }
        return *this;
    }
    
    // Accessor - const version
    const std::string& getName() const { 
        return name; 
    }
    
    size_t getSize() const { 
        return data.size(); 
    }
    
    bool isValid() const { 
        return valid; 
    }
    
    // Process the asset (needs valid data)
    void process() {
        if (!valid) {
            std::cout << "  [ERROR] Trying to process invalid/moved asset!\n";
            return;
        }
        std::cout << "  [PROCESS] Processing " << name << " (" << data.size() << " bytes)\n";
    }
};

// ============================================
// OPTIMIZED ASSET LOADER
// ============================================

class OptimizedAssetLoader {
private:
    std::vector<OptimizedAsset> loadedAssets;
    
public:
    // Add asset by rvalue reference (move semantics)
    void addAsset(OptimizedAsset&& asset) {
        std::cout << "  [LOADER] Adding asset...\n";
        loadedAssets.push_back(std::move(asset));  // Move, no copy!
    }
    
    // Emplace version - perfect forwarding (best performance!)
    template<typename... Args>
    void emplaceAsset(Args&&... args) {
        std::cout << "  [LOADER] Emplacing asset...\n";
        loadedAssets.emplace_back(std::forward<Args>(args)...);  // No move needed!
    }
    
    // Get reference (no copy!)
    const OptimizedAsset* getAsset(size_t index) {
        if (index < loadedAssets.size()) {
            return &loadedAssets[index];
        }
        return nullptr;
    }
    
    // Get mutable reference
    OptimizedAsset* getAssetMutable(size_t index) {
        if (index < loadedAssets.size()) {
            return &loadedAssets[index];
        }
        return nullptr;
    }
    
    // Extract and remove asset (move out of vector)
    OptimizedAsset extractAsset(size_t index) {
        if (index >= loadedAssets.size()) {
            throw std::out_of_range("Invalid index");
        }
        
        // Use std::move to guarantee move semantics
        OptimizedAsset asset = std::move(loadedAssets[index]);
        loadedAssets.erase(loadedAssets.begin() + index);
        return asset;
    }
    
    void processAll() {
        for (auto& asset : loadedAssets) {
            asset.process();
        }
    }
    
    size_t getAssetCount() const {
        return loadedAssets.size();
    }
};

// ============================================
// OPTIMAL ASSET CREATION FUNCTIONS
// ============================================

// Simple version with NRVO guarantee
OptimizedAsset createSimpleAsset() {
    OptimizedAsset asset("LocalAsset", 1000);
    return asset;  // NRVO: typically Zero copies
}

// Conditional return - NRVO may or may not apply
OptimizedAsset createAssetConditional(bool flag) {
    OptimizedAsset asset1("Asset1", 1000);
    OptimizedAsset asset2("Asset2", 2000);
    
    if (flag) {
        return asset1;  // Might not NRVO with multiple returns
    } else {
        return asset2;
    }
}

// Using explicit std::move (not always needed with NRVO)
OptimizedAsset createAssetWithMove() {
    OptimizedAsset asset("MovedAsset", 500);
    // In C++17+, RVO/NRVO usually applies even without explicit move
    return asset;
}

// ============================================
// DEMONSTRATIONS
// ============================================

void demonstrateOptimizedCopy() {
    std::cout << "\n=== OPTIMIZED ASSET LOADING ===\n\n";
    
    OptimizedAssetLoader loader;
    
    std::cout << "Creating asset...\n";
    OptimizedAsset asset("BigModel", 1000000);
    
    std::cout << "\nAdding to loader (move semantics!)...\n";
    loader.addAsset(std::move(asset));
    
    std::cout << "\nAsset after move:\n";
    std::cout << "  valid: " << (asset.isValid() ? "yes" : "no") << "\n";
    std::cout << "  Accessing moved asset creates no-op:\n";
    asset.process();
    
    std::cout << "\nLoader contains:\n";
    if (auto* loaded = loader.getAsset(0)) {
        std::cout << "  Asset: " << loaded->getName() << "\n";
    }
}

void demonstrateEmplaceOptimization() {
    std::cout << "\n=== EMPLACE OPTIMIZATION ===\n\n";
    
    OptimizedAssetLoader loader;
    
    std::cout << "Using emplace_back (no move needed!)...\n";
    loader.emplaceAsset("DirectAsset", 500000);
    
    std::cout << "\nUsing addAsset with temporary (one move)...\n";
    loader.addAsset(OptimizedAsset("TemporaryAsset", 300000));
    
    std::cout << "\nUsing addAsset with variable (explicit move)...\n";
    OptimizedAsset myAsset("ExplicitMove", 200000);
    loader.addAsset(std::move(myAsset));
    
    std::cout << "\nAssets in loader: " << loader.getAssetCount() << "\n";
    std::cout << "Process all:\n";
    loader.processAll();
}

void demonstrateRVOOptimization() {
    std::cout << "\n=== RVO/NRVO OPTIMIZATION ===\n\n";
    
    std::cout << "Creating and returning asset (zero-copy with RVO)...\n";
    OptimizedAsset a = createSimpleAsset();
    
    std::cout << "\nConditional return (RVO might not apply)...\n";
    OptimizedAsset b = createAssetConditional(true);
    
    std::cout << "\nAdding directly to loader (RVO + move)...\n";
    OptimizedAssetLoader loader;
    // This is even more optimized:
    loader.emplaceAsset("DirectConstruction", 100000);
}

void demonstrateMoveSemanticsBenefit() {
    std::cout << "\n=== PERFORMANCE COMPARISON ===\n\n";
    
    const int NUM_ASSETS = 1000;
    const size_t ASSET_SIZE = 100000;
    
    // Optimized version (moves)
    std::cout << "Optimized version (moves only):\n";
    {
        auto start = std::chrono::high_resolution_clock::now();
        
        OptimizedAssetLoader loader;
        for (int i = 0; i < NUM_ASSETS; ++i) {
            loader.emplaceAsset("Asset" + std::to_string(i), ASSET_SIZE);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "  Loaded " << loader.getAssetCount() << " assets\n";
        std::cout << "  Time: " << duration.count() << "ms\n\n";
    }
}

void demonstrateExtractAsset() {
    std::cout << "\n=== EXTRACTING ASSETS ===\n\n";
    
    OptimizedAssetLoader loader;
    loader.emplaceAsset("Asset1", 1000);
    loader.emplaceAsset("Asset2", 2000);
    loader.emplaceAsset("Asset3", 3000);
    
    std::cout << "Assets in loader: " << loader.getAssetCount() << "\n\n";
    
    std::cout << "Extracting middle asset...\n";
    OptimizedAsset extracted = loader.extractAsset(1);
    
    std::cout << "\nExtracted asset: " << extracted.getName() << "\n";
    std::cout << "Assets remaining: " << loader.getAssetCount() << "\n";
    
    // extracted will be destroyed here, triggering destructor
}

// ============================================
// EXPLANATION NOTES
// ============================================

/*
 * SOLUTION ANALYSIS:
 * 
 * 1. WHY DELETE COPY OPERATIONS?
 *    - Assets contain large vector<char> (MB of data)
 *    - Copying would be very expensive
 *    - They're unique resources (like GPU handles)
 *    - Only want to copy if explicitly requested
 * 
 * 2. MOVE SEMANTICS:
 *    struct Asset {
 *        Asset(Asset&& other) : data(std::move(other.data)) {
 *            other.mark_moved();
 *        }
 *    };
 * 
 *    - std::move(other.data) transfers ownership of the vector
 *    - No allocation, no copying, just pointer swap!
 *    - O(1) instead of O(n)
 * 
 * 3. MOVED-FROM STATE:
 *    - After move, source object is in valid but unspecified state
 *    - std::vector after move is typically empty
 *    - We set valid=false to make it explicit
 *    - All operations on moved-from object are safe (no-op)
 * 
 * 4. ADDASSET WITH RVALUE REFERENCE:
 *    void addAsset(OptimizedAsset&& asset)
 *    
 *    - Only accepts temporaries or std::move'd objects
 *    - User must explicitly indicate they want to move
 *    - Loader then moves into vector
 *    - Result: ONE move (very fast!)
 * 
 * 5. EMPLACE OPTIMIZATION:
 *    loader.emplaceAsset("Name", size_t);
 *    
 *    - Constructs asset directly in the vector
 *    - Zero moves, zero copies
 *    - Arguments forwarded to constructor
 *    - This is the FASTEST approach!
 * 
 * 6. PERFECT FORWARDING:
 *    template<typename... Args>
 *    void emplaceAsset(Args&&... args) {
 *        loadedAssets.emplace_back(std::forward<Args>(args)...);
 *    }
 *    
 *    - Preserves reference type of each argument
 *    - Lvalues stay lvalues, rvalues stay rvalues
 *    - No extra copies needed
 * 
 * 7. RVO/NRVO (Return Value Optimization):
 *    OptimizedAsset createAsset() {
 *        OptimizedAsset asset("Name", 1000);
 *        return asset;  // NRVO: asset constructed directly in return
 *    }
 *    
 *    // In C++17+, guaranteed by standard
 *    // Even without explicit move!
 * 
 * 8. KEY RULES:
 *    Rule of Three: destructor, copy constructor, copy assignment
 *    Rule of Five: + move constructor, move assignment
 *    Rule of Zero: use smart pointers, let them handle it
 * 
 *    For OptimizedAsset, we follow Rule of Five:
 *    - Destructor: cleanup (not needed, vectors do it)
 *    - Copy ops: deleted (intentional)
 *    - Move ops: implemented (transfer ownership)
 * 
 * 9. COMPARISON WITH BROKEN VERSION:
 *    Broken:
 *      void addAsset(HeavyAsset asset)  // Takes by value!
 *      First copy (pass by value), second copy (push_back)
 *      Total: 2 copies per asset
 *    
 *    Fixed:
 *      void addAsset(OptimizedAsset&& asset)
 *      Move into vector: 1 move per asset
 *      ~100x faster for large objects!
 * 
 * 10. PRACTICAL PATTERNS:
 *     
 *     Pattern 1: Emplace (best)
 *     loader.emplaceAsset("name", size);
 *     
 *     Pattern 2: Move temporary
 *     loader.addAsset(OptimizedAsset("name", size));
 *     
 *     Pattern 3: Move variable
 *     OptimizedAsset a("name", size);
 *     loader.addAsset(std::move(a));
 *     
 *     Pattern 4: Copy reference? (not needed, just use pointer)
 *     auto* asset = loader.getAsset(0);  // No copy!
 * 
 * 11. EXTRACT PATTERN:
 *     OptimizedAsset asset = loader.extractAsset(0);
 *     
 *     - Removes from loader's vector
 *     - Returns ownership to caller
 *     - Zero-copy operation (just moves)
 * 
 * 12. C++11 MOVE SEMANTICS RULES:
 *     
 *     T&& parameter: accepts rvalues
 *     std::move(lvalue): converts lvalue to rvalue
 *     std::forward<T>(arg): preserves value category
 *     
 *     Rvalue refs bind to temporaries and moved objects
 *     Lvalue refs bind to variables and such
 *     
 * 13. SUBTLE ISSUE IN BROKEN CODE:
 *     After std::move(asset), the original asset is "moved-from"
 *     Using it afterward is technically UB if we don't define state
 *     
 *     We define state: valid=false
 *     Now operations on moved-from asset are well-defined (no-op)
 */

int main() {
    demonstrateOptimizedCopy();
    demonstrateEmplaceOptimization();
    demonstrateRVOOptimization();
    demonstrateMoveSemanticsBenefit();
    demonstrateExtractAsset();
    
    std::cout << "\n========================================\n";
    std::cout << "✅ All assets efficiently moved!\n";
    std::cout << "✅ No unnecessary copies!\n";
    std::cout << "✅ Performance optimized!\n";
    std::cout << "========================================\n";
    
    return 0;
}
