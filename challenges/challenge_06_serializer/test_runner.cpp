/**
 * Test Runner for Challenge 6: Serializer
 */

#include <iostream>
#include <functional>
#include <vector>
#include <cstring>
#include <cstdint>

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
        std::cout << "║      CHALLENGE 6: SERIALIZER TESTS     ║\n";
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
    }
};

// Test: static_cast for numeric conversions
bool testStaticCastNumeric() {
    std::cout << "  → Testing static_cast for numeric conversions\n";
    
    double d = 3.14159;
    int i = static_cast<int>(d);
    float f = static_cast<float>(d);
    
    std::cout << "    double " << d << " → int " << i << "\n";
    std::cout << "    double " << d << " → float " << f << "\n";
    
    return i == 3 && (f > 3.14f && f < 3.15f);
}

// Test: dynamic_cast for safe downcasting
bool testDynamicCast() {
    std::cout << "  → Testing dynamic_cast for safe downcasting\n";
    
    struct Base { virtual ~Base() = default; };
    struct Derived : Base { int x = 42; };
    struct Other : Base { int y = 99; };
    
    Base* actualDerived = new Derived();
    Base* actualOther = new Other();
    
    Derived* d1 = dynamic_cast<Derived*>(actualDerived);  // Should succeed
    Derived* d2 = dynamic_cast<Derived*>(actualOther);    // Should return nullptr
    
    bool result = (d1 != nullptr) && (d2 == nullptr);
    
    std::cout << "    Cast to correct type: " << (d1 != nullptr ? "success" : "failed") << "\n";
    std::cout << "    Cast to wrong type: " << (d2 == nullptr ? "correctly null" : "incorrectly non-null") << "\n";
    
    delete actualDerived;
    delete actualOther;
    
    return result;
}

// Test: memcpy for safe binary reading
bool testMemcpyBinaryRead() {
    std::cout << "  → Testing memcpy for safe binary reading\n";
    
    float original = 3.14159f;
    char buffer[sizeof(float)];
    
    // Write using memcpy
    std::memcpy(buffer, &original, sizeof(float));
    
    // Read using memcpy
    float read;
    std::memcpy(&read, buffer, sizeof(float));
    
    std::cout << "    Original: " << original << ", Read: " << read << "\n";
    
    return original == read;
}

// Test: Endianness-aware serialization
bool testEndiannessConversion() {
    std::cout << "  → Testing endianness-aware serialization\n";
    
    uint32_t original = 0x12345678;
    
    // Convert to big-endian bytes
    unsigned char bytes[4];
    bytes[0] = (original >> 24) & 0xFF;
    bytes[1] = (original >> 16) & 0xFF;
    bytes[2] = (original >> 8) & 0xFF;
    bytes[3] = original & 0xFF;
    
    // Convert back from big-endian
    uint32_t restored = (static_cast<uint32_t>(bytes[0]) << 24) |
                        (static_cast<uint32_t>(bytes[1]) << 16) |
                        (static_cast<uint32_t>(bytes[2]) << 8) |
                        static_cast<uint32_t>(bytes[3]);
    
    std::cout << "    Original: 0x" << std::hex << original << std::dec << "\n";
    std::cout << "    Restored: 0x" << std::hex << restored << std::dec << "\n";
    
    return original == restored;
}

// Test: const_cast usage
bool testConstCast() {
    std::cout << "  → Testing const_cast (legitimate use case)\n";
    
    // Simulate legacy API that takes non-const but doesn't modify
    auto legacyFunction = [](char* str) {
        // Just reads, doesn't modify
        return strlen(str);
    };
    
    const char* constStr = "Hello";
    size_t len = legacyFunction(const_cast<char*>(constStr));
    
    std::cout << "    Called legacy function with const string, length: " << len << "\n";
    
    return len == 5;
}

int main() {
    TestRunner runner;
    
    runner.addTest("static_cast Numeric", testStaticCastNumeric);
    runner.addTest("dynamic_cast Safe Downcast", testDynamicCast);
    runner.addTest("memcpy Binary Read/Write", testMemcpyBinaryRead);
    runner.addTest("Endianness Conversion", testEndiannessConversion);
    runner.addTest("const_cast Usage", testConstCast);
    
    runner.runAll();
    
    return 0;
}
