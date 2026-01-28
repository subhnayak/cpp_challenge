/**
 * 🔴 BROKEN CODE - Binary Format Type Punning
 * 
 * Reading/writing binary data using pointer casts is undefined behavior!
 * This code "works" on some platforms but fails on others.
 * 
 * YOUR TASK: Fix the type punning using safe methods!
 */

#include <iostream>
#include <cstring>
#include <vector>
#include <cstdint>
#include <bit>  // C++20 for std::bit_cast

// ============================================
// BUG: UNSAFE BINARY READING
// ============================================

class BrokenBinaryReader {
private:
    const char* data;
    size_t position;
    size_t size;
    
public:
    BrokenBinaryReader(const char* buffer, size_t bufferSize) 
        : data(buffer), position(0), size(bufferSize) {}
    
    // BUG: Type punning through pointer cast - UB!
    float readFloat() {
        if (position + sizeof(float) > size) {
            throw std::runtime_error("Buffer overflow");
        }
        
        // WRONG! Violates strict aliasing rule!
        float value = *reinterpret_cast<const float*>(data + position);
        position += sizeof(float);
        return value;
    }
    
    // BUG: Same problem
    int32_t readInt32() {
        if (position + sizeof(int32_t) > size) {
            throw std::runtime_error("Buffer overflow");
        }
        
        // WRONG! Also potential alignment issue!
        int32_t value = *reinterpret_cast<const int32_t*>(data + position);
        position += sizeof(int32_t);
        return value;
    }
    
    // BUG: Might read from unaligned address
    uint64_t readUint64() {
        if (position + sizeof(uint64_t) > size) {
            throw std::runtime_error("Buffer overflow");
        }
        
        // On some architectures, unaligned access crashes!
        uint64_t value = *reinterpret_cast<const uint64_t*>(data + position);
        position += sizeof(uint64_t);
        return value;
    }
};

// ============================================
// BUG: UNSAFE BINARY WRITING
// ============================================

class BrokenBinaryWriter {
private:
    std::vector<char> buffer;
    
public:
    // BUG: Type punning for writing
    void writeFloat(float value) {
        // WRONG! UB due to strict aliasing!
        const char* bytes = reinterpret_cast<const char*>(&value);
        for (size_t i = 0; i < sizeof(float); i++) {
            buffer.push_back(bytes[i]);
        }
    }
    
    // BUG: Same problem
    void writeInt32(int32_t value) {
        const char* bytes = reinterpret_cast<const char*>(&value);
        for (size_t i = 0; i < sizeof(int32_t); i++) {
            buffer.push_back(bytes[i]);
        }
    }
    
    const char* getData() const { return buffer.data(); }
    size_t getSize() const { return buffer.size(); }
};

// ============================================
// BUG: FLOAT-INT TYPE PUNNING
// ============================================

void demonstrateFloatIntPunning() {
    std::cout << "=== FLOAT-INT TYPE PUNNING ===\n\n";
    
    float f = 1.0f;
    
    // BUG: Type punning through pointer - UB!
    int32_t i1 = *reinterpret_cast<int32_t*>(&f);
    std::cout << "  float 1.0 as int (reinterpret): " << i1 << "\n";
    
    // BUG: Type punning through union - also UB in C++!
    union FloatInt {
        float f;
        int32_t i;
    } fi;
    fi.f = 1.0f;
    std::cout << "  float 1.0 as int (union): " << fi.i << "\n";
    std::cout << "  (Both methods are undefined behavior in C++!)\n";
    
    // CORRECT in C++20:
    // int32_t i2 = std::bit_cast<int32_t>(f);
}

// ============================================
// BUG: ENDIANNESS ISSUES
// ============================================

void demonstrateEndiannessIssues() {
    std::cout << "\n=== ENDIANNESS ISSUES ===\n\n";
    
    uint32_t value = 0x12345678;
    
    // Write to buffer
    char buffer[4];
    *reinterpret_cast<uint32_t*>(buffer) = value;  // UB!
    
    std::cout << "  Original value: 0x" << std::hex << value << std::dec << "\n";
    std::cout << "  Bytes in memory: ";
    for (int i = 0; i < 4; i++) {
        std::cout << std::hex << (static_cast<unsigned char>(buffer[i]) & 0xFF) << " ";
    }
    std::cout << std::dec << "\n";
    
    // On little-endian: 78 56 34 12
    // On big-endian: 12 34 56 78
    
    std::cout << "  (Byte order depends on machine architecture!)\n";
    std::cout << "  (Need explicit serialization for portability!)\n";
}

// ============================================
// ALIGNMENT CRASH DEMONSTRATION
// ============================================

void demonstrateAlignmentIssue() {
    std::cout << "\n=== ALIGNMENT ISSUES ===\n\n";
    
    char buffer[16] = {0};
    
    // Try to read uint64_t from unaligned address
    char* unalignedPtr = buffer + 1;  // Intentionally misaligned
    
    std::cout << "  Attempting to read uint64_t from unaligned address...\n";
    std::cout << "  buffer address: " << static_cast<void*>(buffer) << "\n";
    std::cout << "  unaligned address: " << static_cast<void*>(unalignedPtr) << "\n";
    
    // BUG: This might crash on ARM or cause performance penalty on x86!
    // uint64_t value = *reinterpret_cast<uint64_t*>(unalignedPtr);
    
    std::cout << "  (Skipped - would crash on some architectures!)\n";
    std::cout << "  (Use memcpy for safe unaligned access)\n";
}

// ============================================
// YOUR TASK
// ============================================

/*
 * TODO: Create solution_binary_format.cpp with:
 * 
 * 1. SafeBinaryReader using std::memcpy:
 *    
 *    float readFloat() {
 *        float value;
 *        std::memcpy(&value, data + position, sizeof(float));
 *        position += sizeof(float);
 *        return value;
 *    }
 * 
 * 2. SafeBinaryWriter using std::memcpy:
 *    
 *    void writeFloat(float value) {
 *        size_t oldSize = buffer.size();
 *        buffer.resize(oldSize + sizeof(float));
 *        std::memcpy(buffer.data() + oldSize, &value, sizeof(float));
 *    }
 * 
 * 3. Using std::bit_cast (C++20) for type punning:
 *    
 *    int32_t floatBits = std::bit_cast<int32_t>(floatValue);
 *    float backToFloat = std::bit_cast<float>(floatBits);
 * 
 * 4. Portable endianness handling:
 *    
 *    void writeUint32BE(uint32_t value) {
 *        buffer.push_back((value >> 24) & 0xFF);
 *        buffer.push_back((value >> 16) & 0xFF);
 *        buffer.push_back((value >> 8) & 0xFF);
 *        buffer.push_back(value & 0xFF);
 *    }
 * 
 * TRICKY QUESTIONS:
 * 
 * Q1: Why is reinterpret_cast to char* special?
 *     char* can alias any type (for serialization purposes).
 *     But reading through the char* as another type is still UB!
 * 
 * Q2: Is this safe?
 *     float f = 3.14f;
 *     char* bytes = reinterpret_cast<char*>(&f);
 *     for (int i = 0; i < sizeof(float); i++) {
 *         std::cout << bytes[i];  // Reading bytes
 *     }
 *     
 *     What about writing back?
 * 
 * Q3: std::memcpy is optimized. Does it actually copy?
 *     (Hint: compilers often optimize it to a direct load/store)
 * 
 * Q4: std::bit_cast requires same size and trivially copyable types.
 *     What happens if you try:
 *     std::bit_cast<int>(float{}) // int and float both 4 bytes
 *     std::bit_cast<int64_t>(float{}) // different sizes!
 * 
 * Q5: BONUS - Implement a portable binary format that:
 *     - Uses big-endian byte order (network byte order)
 *     - Includes a magic number header for validation
 *     - Can serialize std::string with length prefix
 */

int main() {
    demonstrateFloatIntPunning();
    demonstrateEndiannessIssues();
    demonstrateAlignmentIssue();
    
    // Test the broken reader/writer
    std::cout << "\n=== BROKEN READER/WRITER TEST ===\n\n";
    
    BrokenBinaryWriter writer;
    writer.writeFloat(3.14f);
    writer.writeInt32(42);
    
    BrokenBinaryReader reader(writer.getData(), writer.getSize());
    std::cout << "  Read float: " << reader.readFloat() << "\n";
    std::cout << "  Read int: " << reader.readInt32() << "\n";
    std::cout << "  (Worked by luck - still undefined behavior!)\n";
    
    std::cout << "\n========================================\n";
    std::cout << "Fix the type punning issues!\n";
    std::cout << "========================================\n";
    
    return 0;
}
