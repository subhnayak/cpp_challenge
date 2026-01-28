/**
 * 🔴 BROKEN CODE - Asset Loader with Move Semantics Issues
 * 
 * The asset loader is making unnecessary copies of large data!
 * And there are bugs when using moved-from objects.
 * 
 * YOUR TASK: Fix the move semantics and optimize performance!
 */

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <algorithm>

// ============================================
// HEAVY ASSET CLASS
// ============================================

class HeavyAsset {
private:
    std::string name;
    std::vector<char> data;  // Imagine this is several MB
    bool valid;
    
public:
    // Constructor
    HeavyAsset(const std::string& assetName, size_t size) 
        : name(assetName), data(size, 'X'), valid(true) {
        std::cout << "  [CONSTRUCTOR] Created " << name << " (" << size << " bytes)\n";
    }
    
    // Destructor
    ~HeavyAsset() {
        if (valid) {
            std::cout << "  [DESTRUCTOR] Destroyed " << name << "\n";
        } else {
            std::cout << "  [DESTRUCTOR] Destroyed moved-from asset\n";
        }
    }
    
    // BUG: Missing copy constructor - compiler generates one
    // This causes expensive copies!
    
    // BUG: Missing move constructor - no optimization!
    
    // BUG: Missing copy assignment
    
    // BUG: Missing move assignment
    
    // Accessor
    const std::string& getName() const { return name; }
    size_t getSize() const { return data.size(); }
    bool isValid() const { return valid; }
    
    // Process the asset (needs valid data)
    void process() {
        if (!valid) {
            // BUG: Code assumes this check is enough, but what if
            // someone checks isValid() and THEN moves from the object?
            std::cout << "  [ERROR] Trying to process invalid/moved asset!\n";
            return;
        }
        std::cout << "  [PROCESS] Processing " << name << " (" << data.size() << " bytes)\n";
    }
};

// ============================================
// ASSET LOADER WITH BUGS
// ============================================

class BrokenAssetLoader {
private:
    std::vector<HeavyAsset> loadedAssets;
    
public:
    // BUG: Takes asset by value - forces a copy!
    void addAsset(HeavyAsset asset) {
        std::cout << "  [LOADER] Adding asset...\n";
        loadedAssets.push_back(asset);  // BUG: Another copy here!
    }
    
    // BUG: Returns by value - might copy the entire asset!
    HeavyAsset getAsset(size_t index) {
        return loadedAssets[index];  // Copy!
    }
    
    // BUG: This looks like it should move, but does it?
    HeavyAsset extractAsset(size_t index) {
        HeavyAsset asset = loadedAssets[index];  // BUG: Copy, not move!
        loadedAssets.erase(loadedAssets.begin() + index);
        return asset;  // Is RVO applied here?
    }
    
    void processAll() {
        for (auto& asset : loadedAssets) {
            asset.process();
        }
    }
};

// ============================================
// TRICKY MOVE SEMANTICS PUZZLES
// ============================================

// PUZZLE 1: What does this function return?
HeavyAsset createAsset() {
    HeavyAsset asset("LocalAsset", 1000);
    return asset;  // Copy, move, or neither? (NRVO)
}

// PUZZLE 2: What about this one?
HeavyAsset createAssetConditional(bool flag) {
    HeavyAsset asset1("Asset1", 1000);
    HeavyAsset asset2("Asset2", 2000);
    
    if (flag) {
        return asset1;  // Can NRVO apply here?
    } else {
        return asset2;  // Or here?
    }
}

// PUZZLE 3: Spot the bug!
void transferAsset(std::vector<HeavyAsset>& dest, HeavyAsset& src) {
    dest.push_back(std::move(src));
    
    // BUG: Using src after move!
    std::cout << "Transferred: " << src.getName() << "\n";  // Undefined behavior!
}

// PUZZLE 4: Is this correct?
class AssetManager {
    std::vector<HeavyAsset> assets;
    
public:
    void addAsset(HeavyAsset&& asset) {
        assets.push_back(std::move(asset));
    }
    
    // PUZZLE: This function is broken. Why?
    void addAssets(std::vector<HeavyAsset>&& newAssets) {
        for (auto& asset : newAssets) {
            assets.push_back(asset);  // BUG: Should this be std::move?
        }
    }
};

// ============================================
// DEMONSTRATION
// ============================================

void demonstrateCopyOverhead() {
    std::cout << "\n=== COPY OVERHEAD DEMO ===\n\n";
    
    BrokenAssetLoader loader;
    
    std::cout << "Creating asset...\n";
    HeavyAsset asset("BigModel", 1000000);  // 1MB
    
    std::cout << "\nAdding to loader (watch for copies!)...\n";
    loader.addAsset(asset);  // How many copies?
    
    std::cout << "\nOriginal asset still valid: " << asset.isValid() << "\n";
    
    std::cout << "\nGetting asset from loader...\n";
    auto retrieved = loader.getAsset(0);  // Another copy!
    
    std::cout << "\nDone!\n";
}

void demonstrateMoveSemanticsBugs() {
    std::cout << "\n=== MOVE SEMANTICS BUGS DEMO ===\n\n";
    
    std::vector<HeavyAsset> assets;
    HeavyAsset asset("Movable", 100);
    
    std::cout << "Before move - valid: " << asset.isValid() << "\n";
    
    // Move the asset
    assets.push_back(std::move(asset));
    
    // BUG: Using moved-from object!
    std::cout << "After move - valid: " << asset.isValid() << "\n";  // What's the value?
    std::cout << "After move - name: " << asset.getName() << "\n";   // Defined? Undefined?
}

void demonstrateReturnValueOptimization() {
    std::cout << "\n=== RVO/NRVO DEMO ===\n\n";
    
    std::cout << "Calling createAsset()...\n";
    HeavyAsset a = createAsset();  // How many constructions?
    
    std::cout << "\nCalling createAssetConditional(true)...\n";
    HeavyAsset b = createAssetConditional(true);  // NRVO possible?
    
    std::cout << "\nCalling createAssetConditional(false)...\n";
    HeavyAsset c = createAssetConditional(false);  // What about now?
    
    std::cout << "\nDone!\n";
}

// ============================================
// YOUR TASK
// ============================================

/*
 * TODO: Create the following in solution_asset_loader.cpp:
 * 
 * 1. class OptimizedAsset
 *    - Proper move constructor
 *    - Proper move assignment operator
 *    - Deleted copy constructor/assignment (too expensive!)
 *    - Clear "moved-from" state that can be checked
 * 
 * 2. class OptimizedAssetLoader
 *    - addAsset() that accepts rvalue reference
 *    - addAsset() overload for lvalue (should it exist?)
 *    - getAsset() that returns reference, not copy
 *    - extractAsset() that actually moves
 * 
 * TRICKY QUESTIONS:
 * 
 * Q1: If copy is deleted, how do you add an lvalue asset to the loader?
 *     Options:
 *     a) User must always std::move()
 *     b) Provide a clone() method
 *     c) Allow copy but make it explicit somehow
 *     Which is best for game engine performance?
 * 
 * Q2: After std::move(asset), what MUST be true about asset?
 *     a) It's destroyed
 *     b) It's in a valid but unspecified state
 *     c) It's unchanged
 *     d) It's null/empty
 *     Which is correct according to the C++ standard?
 * 
 * Q3: Write a function that returns a HeavyAsset and GUARANTEES
 *     no copy occurs (only move at most). How do you ensure NRVO?
 * 
 * Q4: std::move() doesn't actually move anything. What does it do?
 *     Why is it named "move" then?
 * 
 * Q5: BONUS - Implement "perfect forwarding" version of addAsset:
 *     
 *     template<typename... Args>
 *     void emplaceAsset(Args&&... args);
 *     
 *     How does this avoid even the move?
 * 
 * BENCHMARK CHALLENGE:
 * Measure the time to add 1000 large assets with:
 * - BrokenAssetLoader (copies)
 * - OptimizedAssetLoader (moves)
 * - emplace_back version (perfect forwarding)
 * 
 * The difference should be dramatic!
 */

int main() {
    demonstrateCopyOverhead();
    demonstrateMoveSemanticsBugs();
    demonstrateReturnValueOptimization();
    
    std::cout << "\n========================================\n";
    std::cout << "Count the constructor/destructor calls!\n";
    std::cout << "Optimize to reduce unnecessary copies!\n";
    std::cout << "========================================\n";
    
    return 0;
}
