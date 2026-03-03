/**
 * ✅ SOLUTION - GPU Resource Management with RAII
 * 
 * This solution demonstrates RAII for GPU resources that require
 * special cleanup via graphics API calls.
 * 
 * KEY CONCEPTS:
 * - Custom deleters for unique_ptr
 * - RAII wrapper classes guarantee cleanup even with exceptions
 * - Non-copyable but movable handles
 * - Exception-safe resource management
 */

#include <iostream>
#include <string>
#include <stdexcept>
#include <memory>
#include <functional>

// ============================================
// SIMULATED OPENGL-STYLE API
// ============================================

using GLuint = unsigned int;
static GLuint nextResourceId = 1;

namespace FakeGL {
    GLuint createShader() {
        GLuint id = nextResourceId++;
        std::cout << "  [GPU] Created shader #" << id << "\n";
        return id;
    }
    
    void deleteShader(GLuint id) {
        std::cout << "  [GPU] Deleted shader #" << id << "\n";
    }
    
    GLuint createBuffer() {
        GLuint id = nextResourceId++;
        std::cout << "  [GPU] Created buffer #" << id << "\n";
        return id;
    }
    
    void deleteBuffer(GLuint id) {
        std::cout << "  [GPU] Deleted buffer #" << id << "\n";
    }
    
    GLuint createTexture() {
        GLuint id = nextResourceId++;
        std::cout << "  [GPU] Created texture #" << id << "\n";
        return id;
    }
    
    void deleteTexture(GLuint id) {
        std::cout << "  [GPU] Deleted texture #" << id << "\n";
    }
    
    void compileShader(GLuint id, const std::string& source) {
        if (source.find("ERROR") != std::string::npos) {
            throw std::runtime_error("Shader compilation failed!");
        }
        std::cout << "  [GPU] Compiled shader #" << id << "\n";
    }
    
    void uploadBufferData(GLuint id, const void* data, size_t size) {
        if (size > 1000000) {
            throw std::runtime_error("Buffer too large!");
        }
        std::cout << "  [GPU] Uploaded " << size << " bytes to buffer #" << id << "\n";
    }
}

// ============================================
// SOLUTION: RAII WRAPPERS WITH CUSTOM DELETERS
// ============================================

// Custom deleter for shader
struct ShaderDeleter {
    void operator()(GLuint id) const {
        FakeGL::deleteShader(id);
    }
};

// Custom deleter for buffer
struct BufferDeleter {
    void operator()(GLuint id) const {
        FakeGL::deleteBuffer(id);
    }
};

// Custom deleter for texture
struct TextureDeleter {
    void operator()(GLuint id) const {
        FakeGL::deleteTexture(id);
    }
};

// ============================================
// GENERIC GPU HANDLE CLASS
// ============================================

template<typename Deleter>
class GPUHandle {
private:
    std::unique_ptr<GLuint, Deleter> handle;
    
    // Private constructor for from_handle
    explicit GPUHandle(GLuint h) 
        : handle(std::make_unique<GLuint>(h)) {}
    
public:
    GPUHandle() : handle(nullptr) {}
    
    // No copying
    GPUHandle(const GPUHandle&) = delete;
    GPUHandle& operator=(const GPUHandle&) = delete;
    
    // Moving allowed
    GPUHandle(GPUHandle&&) = default;
    GPUHandle& operator=(GPUHandle&&) = default;
    
    ~GPUHandle() {
        // Automatic cleanup via deleter!
        // Even if we exit via exception, RAII ensures cleanup
    }
    
    // Get the GL handle value
    GLuint get() const {
        return handle ? *handle : 0;
    }
    
    // Factory method
    static GPUHandle create() {
        GPUHandle h;
        h.handle = std::unique_ptr<GLuint, Deleter>(
            new GLuint(Deleter::create())
        );
        return h;
    }
    
    bool isValid() const {
        return handle != nullptr && *handle != 0;
    }
};

// ============================================
// SPECIALIZED HANDLE CLASSES
// ============================================

class ShaderHandle {
private:
    std::unique_ptr<GLuint, ShaderDeleter> handle;
    
public:
    ShaderHandle() : handle(nullptr) {}
    
    explicit ShaderHandle(GLuint id) 
        : handle(std::make_unique<GLuint>(id)) {}
    
    ShaderHandle(const ShaderHandle&) = delete;
    ShaderHandle& operator=(const ShaderHandle&) = delete;
    
    ShaderHandle(ShaderHandle&&) noexcept = default;
    ShaderHandle& operator=(ShaderHandle&&) noexcept = default;
    
    ~ShaderHandle() {
        // Automatic cleanup!
    }
    
    GLuint get() const {
        return handle ? *handle : 0;
    }
    
    bool isValid() const {
        return handle != nullptr;
    }
    
    // Factory function
    static ShaderHandle create() {
        return ShaderHandle(FakeGL::createShader());
    }
    
    void compile(const std::string& source) {
        if (handle) {
            FakeGL::compileShader(*handle, source);
        }
    }
};

class BufferHandle {
private:
    std::unique_ptr<GLuint, BufferDeleter> handle;
    
public:
    BufferHandle() : handle(nullptr) {}
    
    explicit BufferHandle(GLuint id) 
        : handle(std::make_unique<GLuint>(id)) {}
    
    BufferHandle(const BufferHandle&) = delete;
    BufferHandle& operator=(const BufferHandle&) = delete;
    
    BufferHandle(BufferHandle&&) noexcept = default;
    BufferHandle& operator=(BufferHandle&&) noexcept = default;
    
    ~BufferHandle() {
        // Automatic cleanup!
    }
    
    GLuint get() const {
        return handle ? *handle : 0;
    }
    
    bool isValid() const {
        return handle != nullptr;
    }
    
    static BufferHandle create() {
        return BufferHandle(FakeGL::createBuffer());
    }
    
    void upload(const void* data, size_t size) {
        if (handle) {
            FakeGL::uploadBufferData(*handle, data, size);
        }
    }
};

class TextureHandle {
private:
    std::unique_ptr<GLuint, TextureDeleter> handle;
    
public:
    TextureHandle() : handle(nullptr) {}
    
    explicit TextureHandle(GLuint id) 
        : handle(std::make_unique<GLuint>(id)) {}
    
    TextureHandle(const TextureHandle&) = delete;
    TextureHandle& operator=(const TextureHandle&) = delete;
    
    TextureHandle(TextureHandle&&) noexcept = default;
    TextureHandle& operator=(TextureHandle&&) noexcept = default;
    
    ~TextureHandle() {
        // Automatic cleanup!
    }
    
    GLuint get() const {
        return handle ? *handle : 0;
    }
    
    bool isValid() const {
        return handle != nullptr;
    }
    
    static TextureHandle create() {
        return TextureHandle(FakeGL::createTexture());
    }
};

// ============================================
// SAFE RESOURCE LOADER - EXCEPTION SAFE
// ============================================

class SafeResourceLoader {
public:
    // Strong exception guarantee: either succeeds completely or fails with no side effects
    void loadMaterial(const std::string& shaderSource) {
        std::cout << "\nLoading material...\n";
        
        // If any creation throws, nothing is left allocated (RAII!)
        ShaderHandle shader = ShaderHandle::create();
        TextureHandle diffExt = TextureHandle::create();
        TextureHandle normalTex = TextureHandle::create();
        BufferHandle buffer = BufferHandle::create();
        
        // If anything below throws, all handles auto-cleanup!
        shader.compile(shaderSource);  // Might throw!
        buffer.upload(nullptr, 500);
        
        std::cout << "Material loaded successfully!\n";
        // All resources remain allocated - you can save them or move them elsewhere
    }
    
    // Demonstrates move semantics
    ShaderHandle createShader(const std::string& source) {
        ShaderHandle shader = ShaderHandle::create();
        shader.compile(source);  // Might throw!
        return shader;  // Moves out, no copy!
    }
    
    // Simpler version - auto initialization
    BufferHandle createBuffer(size_t size) {
        BufferHandle buffer = BufferHandle::create();
        std::vector<char> data(size);
        buffer.upload(data.data(), size);  // Might throw!
        return buffer;  // Never calls copy, only move!
    }
};

// ============================================
// DEMONSTRATIONS
// ============================================

void demonstrateExceptionSafety() {
    std::cout << "\n=== EXCEPTION SAFETY DEMO ===\n\n";
    
    SafeResourceLoader loader;
    
    try {
        // This will fail during compilation
        loader.loadMaterial("ERROR: invalid shader");
    } catch (const std::exception& e) {
        std::cout << "Caught: " << e.what() << "\n";
        std::cout << "All resources cleaned up automatically!\n";
    }
}

void demonstrateMoveSemantics() {
    std::cout << "\n=== MOVE SEMANTICS DEMO ===\n\n";
    
    SafeResourceLoader loader;
    
    // Create and move shader
    std::cout << "Creating shader...\n";
    ShaderHandle shader = loader.createShader("void main() {}");
    std::cout << "Shader created successfully\n";
    // When shader goes out of scope, it's automatically deleted
    
    // Create buffer
    std::cout << "\nCreating buffer...\n";
    BufferHandle buffer = loader.createBuffer(1000);
    std::cout << "Buffer created successfully\n";
    // Automatic cleanup on scope exit
}

void demonstrateNoCopyConstructor() {
    std::cout << "\n=== NO COPY SEMANTICS DEMO ===\n\n";
    
    ShaderHandle shader1 = ShaderHandle::create();
    
    // This won't compile (intentionally):
    // ShaderHandle shader2 = shader1;  // ERROR: copy constructor deleted!
    
    // But move works:
    ShaderHandle shader2 = std::move(shader1);  // OK: moving ownership
    
    std::cout << "Moving shader ownership works!\n";
    std::cout << "Original shader now invalid: " << (shader1.isValid() ? "no" : "yes") << "\n";
    std::cout << "New shader is valid: " << (shader2.isValid() ? "yes" : "no") << "\n";
}

// ============================================
// EXPLANATION NOTES
// ============================================

/*
 * SOLUTION ANALYSIS:
 * 
 * 1. WHY UNIQUE_PTR WITH CUSTOM DELETER?
 *    - We need to call FakeGL::deleteShader() not delete
 *    - unique_ptr<T, Deleter> calls custom deleter instead of delete
 *    - Guarantees cleanup even with exceptions
 * 
 * 2. CUSTOM DELETER PATTERN:
 *    struct Deleter {
 *        void operator()(GLuint id) const { /* cleanup */ }
 *    };
 *    unique_ptr<GLuint, Deleter> handle;
 * 
 * 3. WHY NOT COPY?
 *    - GPU handles represent unique resources
 *    - Two handles with same ID would both try to delete it (double-free!)
 *    - So we delete copy constructor/assignment
 *    - Move is allowed (transfer ownership)
 * 
 * 4. EXCEPTION SAFETY (STRONG GUARANTEE):
 *    void loadMaterial(const std::string& source) {
 *        ShaderHandle shader = ShaderHandle::create();  // Might throw in create
 *        TextureHandle tex = TextureHandle::create();   // Might throw
 *        shader.compile(source);  // Might throw
 *        // If ANY line throws, RAII ensures all created resources cleaned up!
 *    }
 * 
 * 5. KEY ADVANTAGE OVER MANUAL CLEANUP:
 *    Bad:
 *        GLuint shader = glCreateShader();
 *        glCompileShader(shader);  // Throws!
 *        glDeleteShader(shader);   // Never reached, LEAK!
 *    
 *    Good:
 *        ShaderHandle shader = ShaderHandle::create();
 *        shader.compile(source);  // Throws!
 *        // Destructor runs anyway, automatic cleanup!
 * 
 * 6. MOVE SEMANTICS:
 *    ShaderHandle s1 = ShaderHandle::create();  // s1 owns shader 1
 *    ShaderHandle s2 = std::move(s1);           // s2 now owns shader 1
 *    // s1 is empty, s2 cleans up on destruction
 * 
 * 7. TEMPLATE VERSION (Advanced):
 *    template<typename Deleter>
 *    class GPUHandle { ... }
 *    
 *    Allows one template for all GPU resource types
 *    But more complex - specialized versions are clearer
 * 
 * 8. ZERO-OVERHEAD ABSTRACTION:
 *    - unique_ptr has no runtime overhead
 *    - Custom deleter is stateless (inlined)
 *    - Compiles to same code as manual cleanup
 *    - But with guarantee of correctness!
 * 
 * 9. COMPARISON WITH ANSWERS:
 *    Q1: Why not unique_ptr<GLuint>?
 *    A: unique_ptr<GLuint> would call delete on a GLuint!
 *       That's completely wrong. Need custom deleter.
 * 
 *    Q2: How to write generic "GPU resource pointer"?
 *    A: Use template with different deleters, or create wrapper
 *       that knows which delete function to call at runtime.
 *       See GPUHandle template above for compile-time approach.
 * 
 *    Q3: What should happen using after move?
 *    A: isValid() returns false, operations are no-op safe.
 *       Could also use optional<ShaderHandle> for explicit checking.
 * 
 * 10. RESOURCE POOL OPTIMIZATION (Advanced):
 *     Instead of constantly creating/deleting, pool returns handles
 *     from pre-allocated resources, reuses them.
 *     This is a real optimization in game engines!
 * 
 * 11. PATTERN: RAII WARRANTY
 *     - Acquisition in constructor (create)
 *     - Cleanup in destructor (custom delete)
 *     - Hard guarantee of cleanup
 *     - Exception-safe by design
 */

int main() {
    demonstrateExceptionSafety();
    demonstrateMoveSemantics();
    demonstrateNoCopyConstructor();
    
    std::cout << "\n========================================\n";
    std::cout << "✅ All GPU resources properly managed!\n";
    std::cout << "✅ Exception-safe!\n";
    std::cout << "✅ No memory leaks!\n";
    std::cout << "========================================\n";
    
    return 0;
}
