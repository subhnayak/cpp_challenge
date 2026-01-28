/**
 * 🔴 BROKEN CODE - Texture Cache
 * 
 * This texture cache has multiple issues:
 * 1. Memory leaks everywhere
 * 2. Dangling pointer risk
 * 3. Incorrect ownership semantics
 * 
 * YOUR TASK: Fix this code using smart pointers correctly!
 */

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

// Simulated texture data (imagine this is several MB of image data)
struct Texture {
    std::string name;
    unsigned char* pixels;  // BUG: Raw pointer to heap data!
    int width, height;
    
    Texture(const std::string& n, int w, int h) 
        : name(n), width(w), height(h) {
        pixels = new unsigned char[w * h * 4];  // RGBA
        std::cout << "  [Texture] Created: " << name << " (" << w << "x" << h << ")\n";
    }
    
    // BUG: No destructor! Memory leak!
    // TODO: Should there be a destructor here? What about copy/move?
};

class TextureCache {
private:
    // BUG: Using raw pointers - who owns these textures?
    std::unordered_map<std::string, Texture*> cache;
    
public:
    // BUG: This creates a new texture every time, even if cached!
    Texture* loadTexture(const std::string& filename) {
        std::cout << "[Cache] Loading: " << filename << "\n";
        
        // PUZZLE: This check exists but something is wrong with the logic...
        if (cache.find(filename) != cache.end()) {
            std::cout << "[Cache] Found in cache!\n";
            return cache[filename];
        }
        
        // Simulate loading from disk
        Texture* tex = new Texture(filename, 512, 512);
        cache[filename] = tex;
        return tex;
    }
    
    // BUG: What happens to the Texture* that was stored?
    void unloadTexture(const std::string& filename) {
        std::cout << "[Cache] Unloading: " << filename << "\n";
        cache.erase(filename);  // Memory leak! The Texture is never deleted!
    }
    
    // BUG: Dangerous! What if the cache clears this texture?
    Texture* getTexture(const std::string& filename) {
        auto it = cache.find(filename);
        if (it != cache.end()) {
            return it->second;  // Returning raw pointer - dangerous!
        }
        return nullptr;
    }
    
    void clearAll() {
        std::cout << "[Cache] Clearing all textures\n";
        cache.clear();  // BUG: All Texture objects leaked!
    }
    
    // BUG: No destructor - all cached textures leaked when cache is destroyed!
};

// Sprite that uses textures
class Sprite {
private:
    Texture* texture;  // BUG: Raw pointer - what if texture is deleted?
    float x, y;
    
public:
    Sprite(Texture* tex, float posX, float posY) 
        : texture(tex), x(posX), y(posY) {}
    
    void draw() {
        if (texture) {  // BUG: This check isn't enough! Texture might be deleted but pointer non-null
            std::cout << "  [Sprite] Drawing " << texture->name << " at (" << x << ", " << y << ")\n";
        }
    }
    
    // PUZZLE: What should happen if someone calls cache.unloadTexture() 
    // while this sprite still references it?
};

// ============================================
// DEMONSTRATION OF BUGS
// ============================================

void demonstrateBugs() {
    std::cout << "\n=== BUG DEMONSTRATION ===\n\n";
    
    TextureCache cache;
    
    // Load some textures
    Texture* heroTex = cache.loadTexture("hero.png");
    Texture* enemyTex = cache.loadTexture("enemy.png");
    
    // Create sprites using the textures
    Sprite hero(heroTex, 100, 100);
    Sprite enemy(enemyTex, 200, 200);
    
    hero.draw();
    enemy.draw();
    
    // BUG SCENARIO 1: Unload texture while sprite still uses it
    std::cout << "\n--- Unloading hero texture ---\n";
    cache.unloadTexture("hero.png");
    
    // BUG: hero.draw() will access freed memory (or just leak it)!
    std::cout << "\n--- Attempting to draw hero (DANGER!) ---\n";
    hero.draw();  // Undefined behavior or crash!
    
    // BUG SCENARIO 2: Memory leak on clear
    std::cout << "\n--- Clearing cache ---\n";
    cache.clearAll();
    
    // BUG SCENARIO 3: Loading same texture multiple times
    std::cout << "\n--- Loading hero.png again ---\n";
    cache.loadTexture("hero.png");
    cache.loadTexture("hero.png");  // Should return cached, but does it?
    
    std::cout << "\n--- Destructor runs, more leaks! ---\n";
}

// ============================================
// YOUR TASK: Create a fixed version!
// ============================================

/*
 * TODO: Create the following in solution_texture_cache.cpp:
 * 
 * 1. class FixedTexture
 *    - Proper destructor that frees pixels
 *    - Deleted copy constructor/assignment (textures shouldn't be copied!)
 *    - Move constructor/assignment if needed
 * 
 * 2. class FixedTextureCache  
 *    - Use appropriate smart pointers (which ones? Think carefully!)
 *    - Proper cleanup on destruction
 *    - Thread-safe? (bonus challenge)
 * 
 * 3. class FixedSprite
 *    - Safe texture reference that knows if texture was unloaded
 *    - Consider: should sprite prevent texture from being unloaded?
 *      Or should it gracefully handle the texture disappearing?
 * 
 * QUESTIONS TO ANSWER:
 * 
 * Q1: If multiple sprites share the same texture, which smart pointer
 *     should the cache use? Which should the sprite use?
 * 
 * Q2: If you want sprites to "keep texture alive" even if cache unloads it,
 *     what's the implementation?
 * 
 * Q3: If you want sprites to detect "texture was unloaded" and show a
 *     placeholder instead, what's the implementation?
 * 
 * Q4: Which approach (Q2 or Q3) is better for a game engine? Why?
 */

int main() {
    demonstrateBugs();
    
    std::cout << "\n\n========================================\n";
    std::cout << "Memory leaks occurred! Check with valgrind/ASAN\n";
    std::cout << "========================================\n";
    
    return 0;
}
