/**
 * ✅ SOLUTION - Spell Chain with Variadic Templates
 * 
 * Demonstrates variadic template magic for compile-time spell chain combinations
 */

#include <iostream>
#include <tuple>

// Element types
struct Fire {};
struct Ice {};
struct Lightning {};

// Generic spell
template<typename ElementType>
struct SpellDamage {
    static constexpr float value() { return 10.0f; }
};

template<>
struct SpellDamage<Fire> {
    static constexpr float value() { return 15.0f; }
};

template<>
struct SpellDamage<Ice> {
    static constexpr float value() { return 8.0f; }
};

template<>
struct SpellDamage<Lightning> {
    static constexpr float value() { return 20.0f; }
};

// Fold expression: sum all spell damages
template<typename... Elements>
class SpellChain {
public:
    static constexpr float getTotalDamage() {
        return (SpellDamage<Elements>::value() + ...);
    }
    
    static constexpr int getSpellCount() {
        return sizeof...(Elements);
    }
    
    void cast() {
        std::cout << "🔗 Chain of " << getSpellCount() << " spells\n";
        std::cout << "Total damage: " << getTotalDamage() << "\n";
        castImpl(std::index_sequence_for<Elements...>{});
    }
    
private:
    template<typename... E, size_t... I>
    void castImpl(std::index_sequence<I...>) {
        (..., castSpell<E>());
    }
    
    template<typename E>
    static void castSpell() {
        std::cout << "  Cast spell (damage: " << SpellDamage<E>::value() << ")\n";
    }
};

int main() {
    using Chain = SpellChain<Fire, Ice, Lightning, Fire>;
    
    std::cout << "Spell Chain Stats:\n";
    std::cout << "  Length: " << Chain::getSpellCount() << "\n";
    std::cout << "  Total Damage: " << Chain::getTotalDamage() << " (compile-time!)\n";
    std::cout << "\nCasting:\n";
    
    Chain chain;
    chain.cast();
    
    std::cout << "\n✅ Variadic templates working!\n";
    return 0;
}
