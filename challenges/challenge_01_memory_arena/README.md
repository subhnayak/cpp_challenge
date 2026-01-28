# 🧠 Challenge 1: The Memory Arena

## The Situation

The previous developer created a memory management system for our game engine, but it's riddled with memory leaks, dangling pointers, and crashes. Players report that the game consumes 8GB of RAM after just 10 minutes!

Your task: Fix the broken memory arena and implement proper RAII-compliant resource management.

---

## 🎯 Objectives

### Objective 1.1: The Leaky Texture Cache (unique_ptr)
**File: `broken_texture_cache.cpp`**

The texture cache is leaking memory. Every time a texture is loaded, it's never freed!

**Your Mission:**
- The `TextureCache` class uses raw pointers. Convert it to use `std::unique_ptr`
- But wait... there's a twist! Some textures are "shared" across multiple sprites
- Figure out which textures need `unique_ptr` and which need `shared_ptr`
- **HINT:** Look at the `getTexture()` method carefully - what happens when you return a unique_ptr?

**Puzzle Question:** Why can't you simply return a `unique_ptr` from `getTexture()`? What would happen to the cached texture?

---

### Objective 1.2: The Cyclical Reference Trap (weak_ptr)
**File: `broken_scene_graph.cpp`**

The scene graph has parent-child relationships that create reference cycles. Objects never get deleted!

**Your Mission:**
- The `SceneNode` class has a memory leak due to circular references
- Parent nodes hold `shared_ptr` to children, children hold `shared_ptr` to parent
- Use `weak_ptr` appropriately, but WHERE should it go?
- **TRAP:** Putting `weak_ptr` in the wrong place will cause crashes!

**Puzzle Question:** If you use `weak_ptr` for both parent AND children, what problem occurs when you try to access nodes?

---

### Objective 1.3: The Custom Deleter Mystery (RAII)
**File: `broken_gpu_resource.cpp`**

GPU resources (shaders, buffers) require special cleanup via a graphics API. Raw delete won't work!

**Your Mission:**
- Implement custom deleters for OpenGL-style resources
- The resources MUST be freed even if exceptions occur
- Create a RAII wrapper that guarantees cleanup

**Code Smell to Fix:**
```cpp
// This is WRONG - why?
void loadShader() {
    GLuint shader = glCreateShader();
    compileShader(shader);  // What if this throws?
    // ... more code ...
    glDeleteShader(shader); // This might never run!
}
```

---

### Objective 1.4: Move Semantics Puzzle
**File: `broken_asset_loader.cpp`**

The asset loader is making unnecessary copies of large resources!

**Your Mission:**
- Identify where copies are being made instead of moves
- Implement proper move constructors and move assignment operators
- **TRAP:** After a move, what is the state of the source object? The code assumes it's still valid!

**Brain Teaser:** 
```cpp
std::vector<Asset> assets;
Asset bigAsset = loadAsset("huge_model.obj");
assets.push_back(bigAsset);  // Is bigAsset still usable here?
assets.push_back(std::move(bigAsset));  // What about now?
```

---

## 📝 Files to Modify

1. `broken_texture_cache.cpp` → Create `solution_texture_cache.cpp`
2. `broken_scene_graph.cpp` → Create `solution_scene_graph.cpp`  
3. `broken_gpu_resource.cpp` → Create `solution_gpu_resource.cpp`
4. `broken_asset_loader.cpp` → Create `solution_asset_loader.cpp`

---

## ✅ Success Criteria

Run `test_runner.cpp` and all tests must pass:
- No memory leaks (checked with AddressSanitizer)
- No dangling pointer access
- All resources properly freed even with exceptions
- Move operations don't copy

---

## 💭 Reflection Questions

After completing this challenge, answer these in `my_notes.md`:

1. Why is `make_shared` more efficient than `shared_ptr(new T())`?
2. When would you use `weak_ptr::lock()` vs checking `expired()`?
3. What happens if you call `std::move` on an object twice?
4. Explain why RAII is superior to manual resource management for exception safety.

---

## 🔑 Hints (Use sparingly!)

<details>
<summary>Hint 1: Texture Cache</summary>
Think about ownership. Who OWNS the texture? The cache. Who USES it? The sprites. 
Use shared_ptr for shared ownership, or return raw pointers/references for non-owning access.
</details>

<details>
<summary>Hint 2: Circular References</summary>
The "owner" should have shared_ptr. The "non-owner" should have weak_ptr.
In a tree, parents typically own children. Children just reference parents.
</details>

<details>
<summary>Hint 3: Custom Deleters</summary>
unique_ptr<T, Deleter> can take a custom deleter.
Or wrap the resource in a RAII class with destructor that calls the API.
</details>
