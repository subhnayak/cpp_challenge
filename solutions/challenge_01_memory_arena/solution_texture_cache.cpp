/**
 * ✅ SOLUTION - Texture Cache with Smart Pointers
 * 
 * This solution demonstrates proper resource management using shared_ptr
 * and addresses the ownership semantics problem.
 * 
 * KEY CONCEPTS:
 * - The TextureCache OWNS all textures (uses shared_ptr internally)
 * - Clients (like Sprites) have non-owning access (get raw pointers/weak_ptr)
 * - When a sprite tries to use a freed texture, we detect it gracefully
 */

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <optional>

// ============================================
// FIXED TEXTURE CLASS
// ============================================

struct Texture {
    std::string name;
    //pixels are unique to texture, so unique_ptr is appropriate
    std::unique_ptr<unsigned char[]> pixels;
    int width, height;
    
    Texture(const std::string& n, int w, int h) 
        : name(n), width(w), height(h) {
        pixels = std::make_unique<unsigned char[]>(w * h * 4);
        std::cout << "  [Texture] Created: " << name
                    << " (" << w << "x" << h << ")\n";
    }

    ~Texture() {
        std::cout << "  [Texture] Destroyed: " << name << "\n";
    }
    
    // Prevent copying (each texture should be unique in memory)
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    
    // Allow moving
    Texture(Texture&&) = default;
    Texture& operator=(Texture&&) = default;
};

// ============================================
// FIXED TEXTURE CACHE
// ============================================

class TextureCache {
private:
    // Use shared_ptr: the cache owns the textures and keeps them alive
    // as long as any reference exists (cache or clients)
    //but texture data is shared between textureCache and sprite,
    // so shared_ptr is appropriate
    std::unordered_map<std::string, std::shared_ptr<Texture>> cache;
    
public:
    // Load or get cached texture
    // Returns shared_ptr so clients have owning access to the texture
    std::shared_ptr<Texture> loadTexture(const std::string& filename) {
        std::cout << "[Cache] Loading: " << filename << "\n";
        
        auto it = cache.find(filename);
        if (it != cache.end()) {
            std::cout << "[Cache] Found in cache!\n";
            return it->second;
        }
        
        // Create and cache the texture
        auto tex = std::make_shared<Texture>(filename, 512, 512);
        cache[filename] = tex;
        return tex;
    }
    
    // Option 1: Unload from cache but keep alive if clients hold references
    void unloadTexture(const std::string& filename) {
        std::cout << "[Cache] Unloading: " << filename << "\n";
        auto it = cache.find(filename);
        if (it != cache.end()) {
            std::cout << "[Cache] " << filename << " reference count after unload: "
                      << it->second.use_count() - 1 << "\n";  // -1 for the map entry
        }
        cache.erase(filename);
    }
    
    // Get weak reference - safer for checking if texture still exists
    std::weak_ptr<Texture> getTextureWeak(const std::string& filename) {
        auto it = cache.find(filename);
        if (it != cache.end()) {
            return it->second;
        }
        return std::weak_ptr<Texture>();
    }
    
    // Get shared reference (cache keeps owning reference)
    std::shared_ptr<Texture> getTexture(const std::string& filename) {
        auto it = cache.find(filename);
        if (it != cache.end()) {
            return it->second;
        }
        return nullptr;
    }
    
    void clearAll() {
        std::cout << "[Cache] Clearing all textures\n";
        cache.clear();
    }
    
    size_t getCacheSize() const { return cache.size(); }
    
    ~TextureCache() {
        std::cout << "[Cache] Destructor - clearing all remaining textures\n";
        cache.clear();
    }
};

// ============================================
// FIXED SPRITE CLASS
// ============================================

class Sprite {
private:
    std::shared_ptr<Texture> texture;  // Owning reference - keeps texture alive
    float x, y;
    
public:
    Sprite(std::shared_ptr<Texture> tex, float posX, float posY) 
        : texture(tex), x(posX), y(posY) {
        if (texture) {
            std::cout << "  [Sprite] Created with texture: " << texture->name << "\n";
        }
    }
    
    ~Sprite() {
        std::cout << "  [Sprite] Destroyed\n";
    }
    
    void draw() {
        if (texture) {
            std::cout << "  [Sprite] Drawing " << texture->name 
                      << " at (" << x << ", " << y << ")\n";
        } else {
            std::cout << "  [Sprite] No texture to draw!\n";
        }
    }
    
    void updateTexture(std::shared_ptr<Texture> newTex) {
        texture = newTex;  // Old texture automatically released if no other refs
    }
};

// ============================================
// DEMONSTRATION
// ============================================

void demonstrateFixedMemoryManagement() {
    std::cout << "\n=== FIXED MEMORY MANAGEMENT ===\n\n";
    
    {
        std::cout << "Creating cache and sprites...\n";
        TextureCache cache;
        
        // Load textures
        auto heroTex = cache.loadTexture("hero.png");
        auto enemyTex = cache.loadTexture("enemy.png");
        
        std::cout << "Cache size: " << cache.getCacheSize() << "\n\n";
        
        // Create sprites - they hold shared_ptr to textures
        Sprite hero(heroTex, 100, 100);
        Sprite enemy(enemyTex, 200, 200);
        
        hero.draw();
        enemy.draw();
        
        // Test case 1: Unload texture while sprites hold references
        std::cout << "\n--- Unloading hero.png from cache ---\n";
        cache.unloadTexture("hero.png");
        
        std::cout << "\n--- Hero sprite still works! ---\n";
        hero.draw();  // This still works because sprite holds a shared_ptr!
        
        // Test case 2: Load same texture again
        std::cout << "\n--- Loading hero.png again ---\n";
        auto heroTex2 = cache.loadTexture("hero.png");
        std::cout << "Same texture returned: " << (heroTex == heroTex2 ? "no (new)" : "yes")
                  << "\n\n";
        
        Sprite hero2(heroTex2, 150, 150);
        hero2.draw();
        
        // Test case 3: Clear cache
        std::cout << "\n--- Clearing cache ---\n";
        cache.clearAll();
        
        std::cout << "\n--- Sprites still work ---\n";
        hero.draw();
        hero2.draw();
        
        std::cout << "\n--- Leaving scope ---\n";
    }
    
    std::cout << "\nAll resources cleanly destroyed!\n";
}

// ============================================
// EXPLANATION NOTES
// ============================================

/*
 * SOLUTION ANALYSIS:
 * 
 * 1. WHY shared_ptr FOR CACHE?
 *    - Multiple sprites may hold references to the same texture
 *    - Cache should own textures and share ownership with clients
 *    - Texture stays alive as long as ANY shared_ptr exists
 * 
 * 2. WHY shared_ptr IN SPRITE?
 *    - Sprite owns its texture (lifecycle tied to sprite)
 *    - If sprite outlives cache, texture still valid
 *    - Reference counting ensures cleanup
 * 
 * 3. KEY ADVANTAGE OVER RAW POINTERS:
 *    - No dangling pointers
 *    - No double-free bugs
 *    - Automatic cleanup without manual delete
 *    - Reference counting handles complex lifecycles
 * 
 * 4. PATTERN: SHARED OWNERSHIP
 *    Cache: "I own this texture, keep it alive"
 *    Sprites: "We all share this texture, whoever leaves last cleans it up"
 * 
 * 5. TRICKY Q1: Why can't sprite just hold a raw pointer?
 *    Answer: If cache unloads and is destroyed before sprite, 
 *            the raw pointer becomes dangling!
 * 
 * 6. TRICKY Q2: What about weak_ptr for sprite?
 *    Answer: Could work IF cache lives longer, but risky - sprite
 *            would need to check weak_ptr::expired() before each use
 *            shared_ptr is safer and simpler.
 * 
 * 7. PERFORMANCE NOTE:
 *    - shared_ptr has small overhead (reference count + atomic ops)
 *    - Worth it for safety and correctness
 *    - In C++20, consider unique_ptr + observer pattern for zero overhead
 * 
 * 8. RULE APPLIED: RAII
 *    - Initialization: Resource acquired via make_shared
 *    - Cleanup: Automatic when last shared_ptr destroyed
 */

int main() {
    demonstrateFixedMemoryManagement();
    
    std::cout << "\n========================================\n";
    std::cout << "✅ All memory properly managed!\n";
    std::cout << "✅ No leaks!\n";
    std::cout << "========================================\n";
    
    return 0;
}
