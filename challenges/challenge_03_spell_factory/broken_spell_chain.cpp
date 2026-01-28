/**
 * 🔴 BROKEN CODE - Variadic Template Spell Chains
 * 
 * Mages can cast multiple spells in a chain. We need variadic templates
 * to support any number of spells in the chain!
 * 
 * YOUR TASK: Implement proper variadic template spell chains!
 */

#include <iostream>
#include <string>
#include <tuple>
#include <utility>

// ============================================
// SPELL DEFINITIONS
// ============================================

struct FireSpell {
    static constexpr int ManaCost = 20;
    static constexpr int BaseDamage = 50;
    static constexpr const char* Name = "Fireball";
    
    static void cast() {
        std::cout << "  🔥 Casting " << Name << "! (Damage: " << BaseDamage << ")\n";
    }
};

struct IceSpell {
    static constexpr int ManaCost = 15;
    static constexpr int BaseDamage = 30;
    static constexpr const char* Name = "Ice Lance";
    
    static void cast() {
        std::cout << "  ❄️  Casting " << Name << "! (Damage: " << BaseDamage << ")\n";
    }
};

struct LightningSpell {
    static constexpr int ManaCost = 35;
    static constexpr int BaseDamage = 80;
    static constexpr const char* Name = "Lightning Bolt";
    
    static void cast() {
        std::cout << "  ⚡ Casting " << Name << "! (Damage: " << BaseDamage << ")\n";
    }
};

struct HealSpell {
    static constexpr int ManaCost = 25;
    static constexpr int BaseHealing = 60;
    static constexpr const char* Name = "Heal";
    
    static void cast() {
        std::cout << "  💚 Casting " << Name << "! (Healing: " << BaseHealing << ")\n";
    }
};

// ============================================
// BAD: HARD-CODED SPELL CHAINS
// ============================================

class BadSpellChain2 {
public:
    template<typename S1, typename S2>
    static void cast() {
        std::cout << "Casting 2-spell chain:\n";
        S1::cast();
        S2::cast();
        std::cout << "Total mana: " << (S1::ManaCost + S2::ManaCost) << "\n";
    }
};

class BadSpellChain3 {
public:
    template<typename S1, typename S2, typename S3>
    static void cast() {
        std::cout << "Casting 3-spell chain:\n";
        S1::cast();
        S2::cast();
        S3::cast();
        std::cout << "Total mana: " << (S1::ManaCost + S2::ManaCost + S3::ManaCost) << "\n";
    }
};

// BAD: Need to create a new class for every chain length!

// ============================================
// BROKEN VARIADIC TEMPLATE VERSION
// ============================================

// BUG: This doesn't work correctly
template<typename... Spells>
class BrokenSpellChain {
public:
    // BUG: How do you iterate over a parameter pack?
    static void cast() {
        std::cout << "Casting " << sizeof...(Spells) << "-spell chain:\n";
        
        // BUG: This doesn't compile! Can't iterate like this!
        // for (auto spell : Spells...) {
        //     spell::cast();
        // }
        
        std::cout << "(Spells not actually cast - broken!)\n";
    }
    
    // BUG: Can't calculate total mana cost
    static constexpr int getTotalManaCost() {
        // How do you sum ManaCost for each spell in the pack?
        return 0;  // Wrong!
    }
    
    // BUG: Can't calculate total damage
    static constexpr int getTotalDamage() {
        return 0;  // Wrong!
    }
};

// ============================================
// BROKEN RECURSIVE APPROACH
// ============================================

// Base case: no spells
template<typename... Spells>
struct BrokenManaCalculator {
    // BUG: This should be the base case, but it matches everything!
    static constexpr int value = 0;
};

// BUG: This recursive case never gets used!
template<typename First, typename... Rest>
struct BrokenManaCalculatorRecursive {
    static constexpr int value = First::ManaCost + BrokenManaCalculator<Rest...>::value;
};

// ============================================
// BROKEN FOLD EXPRESSION ATTEMPT (C++17)
// ============================================

template<typename... Spells>
void brokenCastAll() {
    // BUG: How do you use fold expressions to call static methods?
    // (Spells::cast(), ...);  // Does this work?
    
    std::cout << "(Fold expression not implemented)\n";
}

template<typename... Spells>
constexpr int brokenSumMana() {
    // BUG: Is this correct? What if Spells is empty?
    return (Spells::ManaCost + ...);  // Might not compile with empty pack!
}

// ============================================
// DEMONSTRATION
// ============================================

void demonstrateBadWay() {
    std::cout << "=== BAD HARD-CODED CHAINS ===\n\n";
    
    BadSpellChain2::cast<FireSpell, IceSpell>();
    std::cout << "\n";
    BadSpellChain3::cast<FireSpell, IceSpell, LightningSpell>();
    
    std::cout << "\n(Need a new class for every chain length!)\n";
}

void demonstrateBrokenVariadic() {
    std::cout << "\n=== BROKEN VARIADIC VERSION ===\n\n";
    
    BrokenSpellChain<FireSpell, IceSpell>::cast();
    std::cout << "Mana cost: " << BrokenSpellChain<FireSpell, IceSpell>::getTotalManaCost() << "\n";
    
    BrokenSpellChain<FireSpell, IceSpell, LightningSpell, HealSpell>::cast();
    
    std::cout << "\n(Nothing actually works!)\n";
}

// ============================================
// YOUR TASK
// ============================================

/*
 * TODO: Create solution_spell_chain.cpp with:
 * 
 * 1. Compile-time mana calculator using recursive template:
 *    
 *    template<typename... Spells>
 *    struct ManaCost;
 *    
 *    template<>
 *    struct ManaCost<> { static constexpr int value = 0; };  // Base case
 *    
 *    template<typename First, typename... Rest>
 *    struct ManaCost<First, Rest...> {
 *        static constexpr int value = First::ManaCost + ManaCost<Rest...>::value;
 *    };
 * 
 * 2. Cast all spells using fold expression (C++17):
 *    
 *    template<typename... Spells>
 *    void castAll() {
 *        (Spells::cast(), ...);  // Fold expression!
 *    }
 * 
 * 3. Complete SpellChain class:
 *    
 *    template<typename... Spells>
 *    class SpellChain {
 *        static void cast();
 *        static constexpr int getTotalManaCost();
 *        static constexpr int getTotalDamage();
 *        static constexpr int getSpellCount() { return sizeof...(Spells); }
 *    };
 * 
 * 4. BONUS: Compile-time spell validation
 *    - Prevent chains with HealSpell followed by damage spell
 *    - Require minimum mana before chain creation
 * 
 * TRICKY QUESTIONS:
 * 
 * Q1: What's the difference between these fold expressions?
 *     (Spells::cast(), ...);   // Right fold
 *     (..., Spells::cast());   // Left fold
 *     (0 + ... + Spells::ManaCost);  // With init value
 * 
 * Q2: Empty parameter pack handling:
 *     - What does sizeof...(Spells) return for empty pack?
 *     - What does (Spells::ManaCost + ...) return for empty pack?
 *       (Hint: it's a compile error without init value!)
 * 
 * Q3: Can you access the Nth spell in the pack?
 *     SpellChain<Fire, Ice, Lightning>::getSpell<1>()  // Ice
 *     (Hint: std::tuple and std::get!)
 * 
 * Q4: How do you iterate with an index?
 *     template<typename... Spells>
 *     void castWithIndex() {
 *         // Print "Spell 1: Fireball", "Spell 2: Ice Lance", etc.
 *     }
 *     (Hint: std::index_sequence!)
 * 
 * Q5: CHALLENGE - Implement "spell combo bonus":
 *     - If Fire is followed by Ice, add steam damage
 *     - If same spell twice in a row, add echo bonus
 *     (Hint: this requires comparing adjacent types!)
 */

int main() {
    demonstrateBadWay();
    demonstrateBrokenVariadic();
    
    std::cout << "\n========================================\n";
    std::cout << "Implement variadic templates correctly!\n";
    std::cout << "========================================\n";
    
    // GOAL: This should work after you fix it:
    //
    // using MyChain = SpellChain<FireSpell, IceSpell, LightningSpell>;
    // 
    // constexpr int cost = MyChain::getTotalManaCost();  // Compile-time!
    // constexpr int count = MyChain::getSpellCount();     // 3
    // 
    // MyChain::cast();  // Casts all three spells
    //
    // static_assert(cost == 20 + 15 + 35, "Mana calculation wrong!");
    
    return 0;
}
