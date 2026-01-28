/**
 * Test Runner for Challenge 1: Memory Arena
 * 
 * Compile with:
 *   g++ -std=c++20 -fsanitize=address -o test test_runner.cpp
 * 
 * Run with:
 *   ./test
 * 
 * The AddressSanitizer will detect:
 * - Memory leaks
 * - Use-after-free
 * - Double-free
 */

#include <iostream>
#include <memory>
#include <cassert>
#include <functional>
#include <vector>

// ============================================
// TEST FRAMEWORK
// ============================================

class TestRunner {
private:
    struct TestCase {
        std::string name;
        std::function<bool()> test;
    };
    std::vector<TestCase> tests;
    int passed = 0;
    int failed = 0;
    
public:
    void addTest(const std::string& name, std::function<bool()> test) {
        tests.push_back({name, test});
    }
    
    void runAll() {
        std::cout << "\n╔════════════════════════════════════════╗\n";
        std::cout << "║     CHALLENGE 1: MEMORY ARENA TESTS     ║\n";
        std::cout << "╚════════════════════════════════════════╝\n\n";
        
        for (const auto& tc : tests) {
            std::cout << "► Testing: " << tc.name << "\n";
            try {
                bool result = tc.test();
                if (result) {
                    std::cout << "  ✓ PASSED\n\n";
                    passed++;
                } else {
                    std::cout << "  ✗ FAILED\n\n";
                    failed++;
                }
            } catch (const std::exception& e) {
                std::cout << "  ✗ EXCEPTION: " << e.what() << "\n\n";
                failed++;
            }
        }
        
        std::cout << "════════════════════════════════════════\n";
        std::cout << "Results: " << passed << " passed, " << failed << " failed\n";
        std::cout << "════════════════════════════════════════\n";
        
        if (failed == 0) {
            std::cout << "\n🎉 All tests passed! Challenge 1 complete!\n";
        } else {
            std::cout << "\n😅 Some tests failed. Keep trying!\n";
        }
    }
};

// ============================================
// PLACEHOLDER TESTS 
// (Replace with tests against your solution!)
// ============================================

// Test 1: Texture cache doesn't leak
bool testTextureCacheNoLeak() {
    std::cout << "  → Include your solution_texture_cache.cpp and implement test\n";
    // TODO: Create textures, destroy cache, verify no leaks (ASAN will catch)
    return true;  // Placeholder
}

// Test 2: Scene graph circular references fixed
bool testSceneGraphNoCircularLeak() {
    std::cout << "  → Include your solution_scene_graph.cpp and implement test\n";
    // TODO: Create parent-child nodes, verify all destroyed when root released
    return true;  // Placeholder
}

// Test 3: GPU resources cleaned up on exception
bool testGPUResourceExceptionSafety() {
    std::cout << "  → Include your solution_gpu_resource.cpp and implement test\n";
    // TODO: Throw during resource creation, verify cleanup
    return true;  // Placeholder
}

// Test 4: Asset loader uses move semantics
bool testAssetLoaderMoveSemantics() {
    std::cout << "  → Include your solution_asset_loader.cpp and implement test\n";
    // TODO: Add asset, verify no unnecessary copies (count constructor calls)
    return true;  // Placeholder
}

// Test 5: weak_ptr correctly handles expired parent
bool testWeakPtrExpiration() {
    std::cout << "  → Testing weak_ptr expiration handling\n";
    
    std::weak_ptr<int> weak;
    
    {
        auto shared = std::make_shared<int>(42);
        weak = shared;
        
        // Inside scope - should work
        if (auto locked = weak.lock()) {
            std::cout << "    Value while alive: " << *locked << "\n";
        }
    }
    
    // Outside scope - shared is gone
    if (weak.expired()) {
        std::cout << "    Correctly detected expiration\n";
        return true;
    }
    
    return false;
}

// Test 6: Custom deleter works correctly
bool testCustomDeleter() {
    std::cout << "  → Testing unique_ptr with custom deleter\n";
    
    bool deleted = false;
    
    {
        auto deleter = [&deleted](int* p) {
            std::cout << "    Custom deleter called!\n";
            deleted = true;
            delete p;
        };
        
        std::unique_ptr<int, decltype(deleter)> ptr(new int(42), deleter);
    }
    
    return deleted;
}

// Test 7: Move constructor leaves source in valid state
bool testMoveConstructorValidState() {
    std::cout << "  → Testing move semantics with vector\n";
    
    std::vector<int> original = {1, 2, 3, 4, 5};
    std::vector<int> moved = std::move(original);
    
    // original is in valid but unspecified state
    // We can still call methods on it
    std::cout << "    Moved vector size: " << moved.size() << "\n";
    std::cout << "    Original vector size after move: " << original.size() << "\n";
    
    // This should be safe (valid state)
    original.clear();
    original.push_back(10);
    
    return moved.size() == 5 && original.size() == 1;
}

// Test 8: shared_ptr reference counting
bool testSharedPtrRefCount() {
    std::cout << "  → Testing shared_ptr reference counting\n";
    
    auto ptr1 = std::make_shared<int>(100);
    std::cout << "    After creation, use_count: " << ptr1.use_count() << "\n";
    
    auto ptr2 = ptr1;
    std::cout << "    After copy, use_count: " << ptr1.use_count() << "\n";
    
    {
        auto ptr3 = ptr1;
        std::cout << "    Inside scope, use_count: " << ptr1.use_count() << "\n";
    }
    
    std::cout << "    After scope, use_count: " << ptr1.use_count() << "\n";
    
    return ptr1.use_count() == 2;
}

int main() {
    TestRunner runner;
    
    // Add all tests
    runner.addTest("Texture Cache No Leak", testTextureCacheNoLeak);
    runner.addTest("Scene Graph No Circular Leak", testSceneGraphNoCircularLeak);
    runner.addTest("GPU Resource Exception Safety", testGPUResourceExceptionSafety);
    runner.addTest("Asset Loader Move Semantics", testAssetLoaderMoveSemantics);
    runner.addTest("weak_ptr Expiration Handling", testWeakPtrExpiration);
    runner.addTest("Custom Deleter", testCustomDeleter);
    runner.addTest("Move Constructor Valid State", testMoveConstructorValidState);
    runner.addTest("shared_ptr Reference Counting", testSharedPtrRefCount);
    
    runner.runAll();
    
    std::cout << "\n════════════════════════════════════════\n";
    std::cout << "Don't forget to compile with -fsanitize=address\n";
    std::cout << "to catch memory leaks automatically!\n";
    std::cout << "════════════════════════════════════════\n";
    
    return 0;
}
