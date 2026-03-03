/**
 * ✅ SOLUTION - Safe Binary Format with memcpy
 */

#include <iostream>
#include <cstring>
#include <vector>

class BinarySerializer {
public:
    // Safe: Read float without UB
    static float readFloat(const unsigned char* buffer) {
        float value;
        std::memcpy(&value, buffer, sizeof(float));
        return value;
    }
    
    // Safe: Write float without UB
    static void writeFloat(unsigned char* buffer, float value) {
        std::memcpy(buffer, &value, sizeof(float));
    }
    
    // Safe: Read integer
    static int readInt(const unsigned char* buffer) {
        int value;
        std::memcpy(&value, buffer, sizeof(int));
        return value;
    }
    
    // Safe: Write integer
    static void writeInt(unsigned char* buffer, int value) {
        std::memcpy(buffer, &value, sizeof(int));
    }
};

int main() {
    std::cout << "=== SAFE BINARY SERIALIZATION ===\n\n";
    
    // Buffer for binary data
    unsigned char buffer[8];
    
    // Write data safely
    BinarySerializer::writeFloat(buffer, 3.14159f);
    BinarySerializer::writeInt(buffer + 4, 42);
    
    std::cout << "Written to buffer\n";
    
    // Read data safely
    float f = BinarySerializer::readFloat(buffer);
    int i = BinarySerializer::readInt(buffer + 4);
    
    std::cout << "Read float: " << f << "\n";
    std::cout << "Read int: " << i << "\n";
    
    std::cout << "\n✅ Binary I/O without strict aliasing violations!\n";
    
    return 0;
}
