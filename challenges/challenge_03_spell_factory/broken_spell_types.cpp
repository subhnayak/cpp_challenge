/**
 * 🔴 BROKEN CODE - Spell Types with Template Specialization
 * 
 * The current implementation uses runtime polymorphism and dynamic_cast
 * to handle different spell types. It's slow and error-prone!
 * 
 * YOUR TASK: Use template specialization for compile-time dispatch!
 */

#include <iostream>
#include <string>
#include <cmath>

// ============================================
// BAD: RUNTIME POLYMORPHISM VERSION
// ============================================

// Element types (just tags for now)
struct FireElement { static constexpr const char* name = "Fire"; };
struct IceElement { static constexpr const char* name = "Ice"; };
struct LightningElement { static constexpr const char* name = "Lightning"; };
struct HolyElement { static constexpr const char* name = "Holy"; };

// The BAD way - runtime polymorphism
class BadSpell {
public:
    virtual ~BadSpell() = default;
    virtual float calculateDamage(float power) const = 0;
    virtual std::string getElement() const = 0;
    virtual float getManaCost() const = 0;
};

class BadFireSpell : public BadSpell {
public:
    float calculateDamage(float power) const override {
        return power * 1.5f;  // Fire does 50% more damage
    }
    std::string getElement() const override { return "Fire"; }
    float getManaCost() const override { return 20.0f; }
};

class BadIceSpell : public BadSpell {
public:
    float calculateDamage(float power) const override {
        return power * 0.8f + 10.0f;  // Ice does less direct damage but has flat bonus
    }
    std::string getElement() const override { return "Ice"; }
    float getManaCost() const override { return 15.0f; }
};

class BadLightningSpell : public BadSpell {
public:
    float calculateDamage(float power) const override {
        return power * power * 0.1f;  // Lightning scales quadratically!
    }
    std::string getElement() const override { return "Lightning"; }
    float getManaCost() const override { return 30.0f; }
};

// BAD: This function uses dynamic_cast to determine spell type
float badCalculateTotalDamage(BadSpell* spell1, BadSpell* spell2, float power) {
    float damage = 0;
    
    // Terrible! Runtime type checking!
    if (dynamic_cast<BadFireSpell*>(spell1) && dynamic_cast<BadIceSpell*>(spell2)) {
        damage = spell1->calculateDamage(power) + spell2->calculateDamage(power);
        // Fire + Ice = steam bonus!
        damage *= 1.2f;
    } else if (dynamic_cast<BadFireSpell*>(spell1) && dynamic_cast<BadFireSpell*>(spell2)) {
        // Double fire = explosion!
        damage = spell1->calculateDamage(power) * 2.5f;
    } else {
        damage = spell1->calculateDamage(power) + spell2->calculateDamage(power);
    }
    
    return damage;
}

// ============================================
// BROKEN TEMPLATE VERSION (YOUR STARTING POINT)
// ============================================

// BUG: Generic template with no specialization
template<typename Element>
class Spell {
public:
    // BUG: Same calculation for ALL elements - wrong!
    static float calculateDamage(float power) {
        return power;  // No element-specific behavior!
    }
    
    // BUG: Generic mana cost - should vary by element
    static constexpr float ManaCost = 10.0f;
    
    static constexpr const char* getElement() {
        return "Unknown";  // BUG: Should return element name!
    }
};

// BUG: Partial specialization attempt that doesn't compile
// template<>
// class Spell<FireElement> {
//     // ... forgot to implement!
// };

// ============================================
// COMBINATION SPELL - BROKEN
// ============================================

// BUG: This should calculate combo bonuses based on element types
template<typename Element1, typename Element2>
class CombinedSpell {
public:
    // BUG: No special handling for element combinations!
    static float calculateComboDamage(float power) {
        return Spell<Element1>::calculateDamage(power) + 
               Spell<Element2>::calculateDamage(power);
    }
    
    // TODO: Fire + Ice should give steam bonus
    // TODO: Fire + Fire should give explosion bonus
    // TODO: Lightning + Water should give massive bonus
    // TODO: Holy + Holy should double healing
};

// ============================================
// DEMONSTRATION
// ============================================

void demonstrateBadWay() {
    std::cout << "=== BAD RUNTIME POLYMORPHISM ===\n\n";
    
    BadFireSpell fire;
    BadIceSpell ice;
    BadLightningSpell lightning;
    
    float power = 100.0f;
    
    std::cout << "Fire damage: " << fire.calculateDamage(power) << "\n";
    std::cout << "Ice damage: " << ice.calculateDamage(power) << "\n";
    std::cout << "Lightning damage: " << lightning.calculateDamage(power) << "\n";
    
    std::cout << "\nCombo (Fire+Ice): " << badCalculateTotalDamage(&fire, &ice, power) << "\n";
    std::cout << "Combo (Fire+Fire): " << badCalculateTotalDamage(&fire, &fire, power) << "\n";
    
    std::cout << "\n(Notice all the virtual calls and dynamic_casts!)\n";
}

void demonstrateBrokenTemplates() {
    std::cout << "\n=== BROKEN TEMPLATE VERSION ===\n\n";
    
    float power = 100.0f;
    
    // BUG: All these return the same damage because no specialization!
    std::cout << "Fire damage: " << Spell<FireElement>::calculateDamage(power) << "\n";
    std::cout << "Ice damage: " << Spell<IceElement>::calculateDamage(power) << "\n";
    std::cout << "Lightning damage: " << Spell<LightningElement>::calculateDamage(power) << "\n";
    
    // BUG: Combo doesn't calculate special bonuses!
    std::cout << "\nCombo (Fire+Ice): " << CombinedSpell<FireElement, IceElement>::calculateComboDamage(power) << "\n";
    
    std::cout << "\n(All damage values are the same - specialization needed!)\n";
}

// ============================================
// YOUR TASK
// ============================================

/*
 * TODO: Create solution_spell_types.cpp with:
 * 
 * 1. Full template specializations for Spell<FireElement>, Spell<IceElement>, etc.
 *    Each with unique:
 *    - calculateDamage() formula
 *    - ManaCost value
 *    - Element name
 * 
 * 2. Partial specialization for CombinedSpell to handle specific combos:
 *    - CombinedSpell<FireElement, IceElement> - steam bonus (1.2x)
 *    - CombinedSpell<FireElement, FireElement> - explosion (2.5x)
 *    - CombinedSpell<LightningElement, IceElement> - shatter (2.0x)
 * 
 * 3. BONUS: constexpr damage calculation
 *    - Make damage calculable at compile time!
 *    
 *    constexpr float damage = Spell<FireElement>::calculateDamage(100);
 * 
 * TRICKY QUESTIONS:
 * 
 * Q1: What's the difference between:
 *     template<> class Spell<Fire> { };     // Full specialization
 *     template<typename T> class Spell<T*> { };  // Partial specialization
 *     When would you use each?
 * 
 * Q2: Can you specialize just ONE member function instead of the whole class?
 *     
 *     template<typename T> class Spell {
 *         void cast();
 *     };
 *     
 *     template<>
 *     void Spell<Fire>::cast() { }  // Is this valid?
 * 
 * Q3: What happens if you try to use Spell<WaterElement> but there's
 *     no specialization for WaterElement? 
 *     a) Compile error
 *     b) Uses generic template
 *     c) Runtime error
 * 
 * Q4: How do you FORCE a compile error if someone uses an unsupported element?
 *     (Hint: delete the generic template, or use static_assert)
 * 
 * Q5: BONUS - Can you use constexpr if (C++17) instead of specialization
 *     for some cases? When is this better/worse than specialization?
 */

int main() {
    demonstrateBadWay();
    demonstrateBrokenTemplates();
    
    std::cout << "\n========================================\n";
    std::cout << "Add template specializations to fix!\n";
    std::cout << "========================================\n";
    
    return 0;
}
