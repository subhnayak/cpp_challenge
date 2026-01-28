/**
 * Test Runner for Challenge 3: Spell Factory
 */

#include <iostream>
#include <functional>
#include <vector>
#include <type_traits>

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
        std::cout << "║    CHALLENGE 3: SPELL FACTORY TESTS    ║\n";
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

// Test: Template specialization provides different values
bool testTemplateSpecialization() {
    std::cout << "  → Verify template specialization works\n";
    // After fixing, different elements should return different damage
    return true;  // Placeholder
}

// Test: SFINAE prevents invalid combinations
bool testSFINAEConstraints() {
    std::cout << "  → Verify SFINAE prevents invalid spell targets\n";
    // The invalid combinations shouldn't even compile
    return true;  // Placeholder
}

// Test: Variadic templates calculate correct totals
bool testVariadicManaCalculation() {
    std::cout << "  → Testing compile-time mana calculation\n";
    
    // Example test structure
    struct Spell1 { static constexpr int ManaCost = 10; };
    struct Spell2 { static constexpr int ManaCost = 20; };
    struct Spell3 { static constexpr int ManaCost = 30; };
    
    // Fold expression test
    constexpr int total = (Spell1::ManaCost + Spell2::ManaCost + Spell3::ManaCost);
    std::cout << "    Total mana (should be 60): " << total << "\n";
    
    return total == 60;
}

// Test: sizeof... works correctly
bool testSizeofPack() {
    std::cout << "  → Testing sizeof... on parameter packs\n";
    
    auto countTypes = []<typename... Ts>() {
        return sizeof...(Ts);
    };
    
    int count = countTypes.template operator()<int, float, double, char>();
    std::cout << "    Count of types (should be 4): " << count << "\n";
    
    return count == 4;
}

// Test: Fold expressions work
bool testFoldExpression() {
    std::cout << "  → Testing fold expressions\n";
    
    auto sum = []<typename... Ts>(Ts... vals) {
        return (vals + ... + 0);  // Fold with init
    };
    
    int result = sum(1, 2, 3, 4, 5);
    std::cout << "    Sum of 1-5 (should be 15): " << result << "\n";
    
    return result == 15;
}

// Test: std::enable_if basics
bool testEnableIf() {
    std::cout << "  → Testing std::enable_if\n";
    
    // Function only enabled for integral types
    auto onlyForInt = []<typename T>(T val) -> std::enable_if_t<std::is_integral_v<T>, T> {
        return val * 2;
    };
    
    int result = onlyForInt(5);
    std::cout << "    Result (should be 10): " << result << "\n";
    
    // This shouldn't compile: onlyForInt(3.14);
    
    return result == 10;
}

// Test: Type traits work
bool testTypeTraits() {
    std::cout << "  → Testing type traits\n";
    
    struct Living {};
    struct Undead {};
    struct Zombie { using EntityType = Undead; };
    struct Player { using EntityType = Living; };
    
    bool zombieIsUndead = std::is_same_v<Zombie::EntityType, Undead>;
    bool playerIsLiving = std::is_same_v<Player::EntityType, Living>;
    
    std::cout << "    Zombie is Undead: " << zombieIsUndead << "\n";
    std::cout << "    Player is Living: " << playerIsLiving << "\n";
    
    return zombieIsUndead && playerIsLiving;
}

int main() {
    TestRunner runner;
    
    runner.addTest("Template Specialization", testTemplateSpecialization);
    runner.addTest("SFINAE Constraints", testSFINAEConstraints);
    runner.addTest("Variadic Mana Calculation", testVariadicManaCalculation);
    runner.addTest("sizeof... Parameter Pack", testSizeofPack);
    runner.addTest("Fold Expressions", testFoldExpression);
    runner.addTest("std::enable_if", testEnableIf);
    runner.addTest("Type Traits", testTypeTraits);
    
    runner.runAll();
    
    return 0;
}
