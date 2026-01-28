/**
 * 🔴 BROKEN CODE - GPU Resource Management
 * 
 * GPU resources (shaders, buffers, textures) require special API calls
 * to create and destroy. Regular new/delete won't work!
 * 
 * The problem: Resources aren't freed properly, especially when exceptions occur.
 * 
 * YOUR TASK: Implement RAII wrappers with custom deleters for GPU resources!
 */

#include <iostream>
#include <string>
#include <stdexcept>
#include <memory>
#include <functional>

// ============================================
// SIMULATED OPENGL-STYLE API
// (Pretend these are real GPU driver calls)
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
        // Simulate compilation that might fail
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
// BROKEN SHADER CLASS
// ============================================

class BrokenShader {
private:
    GLuint handle;
    bool compiled;
    
public:
    BrokenShader() : compiled(false) {
        handle = FakeGL::createShader();
    }
    
    // BUG: No destructor! GPU resource leaked!
    
    // BUG: Copy is allowed - but what does copying a GPU resource mean?
    // Two objects will have the same handle, and both will try to delete it!
    
    void compile(const std::string& source) {
        FakeGL::compileShader(handle, source);  // BUG: If this throws, handle is leaked!
        compiled = true;
    }
    
    GLuint getHandle() const { return handle; }
};

// ============================================
// BROKEN BUFFER CLASS  
// ============================================

class BrokenBuffer {
private:
    GLuint handle;
    
public:
    BrokenBuffer() {
        handle = FakeGL::createBuffer();
    }
    
    ~BrokenBuffer() {
        // At least there's a destructor...
        FakeGL::deleteBuffer(handle);
    }
    
    // BUG: Default copy constructor/assignment!
    // If you copy this buffer, both copies have the same handle.
    // When one is destroyed, the other has a dangling handle!
    
    void upload(const void* data, size_t size) {
        FakeGL::uploadBufferData(handle, data, size);
    }
    
    GLuint getHandle() const { return handle; }
};

// ============================================
// BROKEN RESOURCE LOADER
// ============================================

class BrokenResourceLoader {
public:
    // BUG: Multiple resources created, only some cleaned up on error
    void loadMaterial(const std::string& shaderSource) {
        std::cout << "\nLoading material...\n";
        
        GLuint shader = FakeGL::createShader();
        GLuint diffuseTexture = FakeGL::createTexture();
        GLuint normalTexture = FakeGL::createTexture();
        GLuint buffer = FakeGL::createBuffer();
        
        // BUG: If any of these fail, previously created resources are leaked!
        FakeGL::compileShader(shader, shaderSource);  // Might throw!
        FakeGL::uploadBufferData(buffer, nullptr, 500);
        
        // Manual cleanup (but we never reach here if exception thrown!)
        FakeGL::deleteShader(shader);
        FakeGL::deleteTexture(diffuseTexture);
        FakeGL::deleteTexture(normalTexture);
        FakeGL::deleteBuffer(buffer);
        
        std::cout << "Material loaded successfully!\n";
    }
    
    // ATTEMPTED FIX (but still wrong!)
    void loadMaterialWithTryCatch(const std::string& shaderSource) {
        std::cout << "\nLoading material with try-catch...\n";
        
        GLuint shader = 0;
        GLuint texture = 0;
        
        try {
            shader = FakeGL::createShader();
            texture = FakeGL::createTexture();
            
            FakeGL::compileShader(shader, shaderSource);
            
            // Cleanup
            FakeGL::deleteShader(shader);
            FakeGL::deleteTexture(texture);
            
        } catch (...) {
            // BUG: Trying to delete, but what if createTexture() was the one that threw?
            // We'd be deleting an uninitialized handle!
            if (shader != 0) FakeGL::deleteShader(shader);
            if (texture != 0) FakeGL::deleteTexture(texture);
            throw;
        }
    }
};

// ============================================
// DEMONSTRATION OF BUGS
// ============================================

void demonstrateShaderLeak() {
    std::cout << "\n=== SHADER LEAK DEMO ===\n";
    
    BrokenShader shader;
    
    try {
        shader.compile("ERROR: invalid syntax");  // This will throw!
    } catch (const std::exception& e) {
        std::cout << "Caught: " << e.what() << "\n";
    }
    
    std::cout << "Shader leaked! No deleteShader called!\n";
}

void demonstrateDoubleFree() {
    std::cout << "\n=== DOUBLE FREE DEMO ===\n";
    
    BrokenBuffer buffer1;
    BrokenBuffer buffer2 = buffer1;  // Copy! Both have same handle!
    
    std::cout << "buffer1 handle: " << buffer1.getHandle() << "\n";
    std::cout << "buffer2 handle: " << buffer2.getHandle() << "\n";
    
    std::cout << "Leaving scope - watch what happens...\n";
    // Both destructors will try to delete the same handle!
}

void demonstrateMaterialLoadLeak() {
    std::cout << "\n=== MATERIAL LOAD LEAK DEMO ===\n";
    
    BrokenResourceLoader loader;
    
    try {
        loader.loadMaterial("ERROR: bad shader");
    } catch (...) {
        std::cout << "Exception caught, but resources leaked!\n";
    }
}

// ============================================
// YOUR TASK: Create RAII wrappers!
// ============================================

/*
 * TODO: Create the following in solution_gpu_resource.cpp:
 * 
 * 1. class ShaderHandle - RAII wrapper for GLuint shader
 *    - Creates shader in constructor
 *    - Deletes shader in destructor
 *    - Non-copyable (delete copy operations)
 *    - Movable (transfer ownership)
 * 
 * 2. class BufferHandle - Same pattern for buffers
 * 
 * 3. class TextureHandle - Same pattern for textures
 * 
 * 4. Alternative: Using unique_ptr with custom deleter
 *    - Can you use unique_ptr<GLuint, CustomDeleter> instead?
 *    - What are the tradeoffs?
 * 
 * 5. class SafeResourceLoader
 *    - Uses RAII handles so resources are always cleaned up
 *    - Exception-safe material loading
 * 
 * TRICKY QUESTIONS:
 * 
 * Q1: Why can't you use unique_ptr<GLuint> directly? 
 *     (Hint: what would delete do to a GLuint?)
 * 
 * Q2: If you want to use unique_ptr with a custom deleter, the deleter
 *     is part of the type. How do you write a generic "GPU resource pointer"
 *     that works for shaders, buffers, and textures?
 * 
 * Q3: What should happen if you try to use a ShaderHandle after it's been moved from?
 *     Options:
 *     a) Undefined behavior
 *     b) No-op (silently do nothing)
 *     c) Throw an exception
 *     d) Assert/abort
 *     Which is best for a game engine?
 * 
 * Q4: BONUS - Implement a "GPU resource pool" that reuses handles instead
 *     of constantly creating/destroying them. This is a real optimization
 *     used in game engines!
 * 
 * PUZZLE: Write loadMaterial() using RAII so that this code is exception-safe:
 * 
 *     void loadMaterial(const std::string& source) {
 *         ShaderHandle shader;
 *         TextureHandle tex1, tex2;
 *         BufferHandle buffer;
 *         
 *         shader.compile(source);  // Might throw!
 *         buffer.upload(data, size);  // Might throw!
 *         
 *         // If we reach here, all resources valid
 *         // When function returns (normally or via exception),
 *         // all resources automatically cleaned up!
 *     }
 */

int main() {
    std::cout << "=== GPU RESOURCE BUGS DEMO ===\n";
    
    demonstrateShaderLeak();
    demonstrateDoubleFree();
    demonstrateMaterialLoadLeak();
    
    std::cout << "\n========================================\n";
    std::cout << "Check output above - many GPU resources leaked!\n";
    std::cout << "========================================\n";
    
    return 0;
}
