/**
 * ✅ SOLUTION - Spell Factory with Templates & SFINAE
 * 
 * Demonstrates:
 * - Template specialization for different spell types
 * - SFINAE for compile-time constraints
 * - Variadic templates for spell chains
 * - Perfect forwarding for spell creation
 */

#include <iostream>
#include <vector>
#include <utility>
#include <type_traits>

// ============================================
// ELEMENT TYPES
// ============================================

struct FireElement {};
struct IceElement {};
struct LightningElement {};
struct HealElement {};
struct Undead {};

// ============================================
// GENERIC SPELL TEMPLATE
// ============================================

template<typename ElementType>
class Spell {
public:
    float calculateDamage(float power = 1.0f) {
        return power;  // Default implementation
    }
    
    void cast() {
        std::cout << "Casting generic spell\n";
    }
};

// ============================================
// TEMPLATE SPECIALIZATION
// ============================================

template<>
class Spell<FireElement> {
public:
    float calculateDamage(float power = 1.0f) {
        return power * 1.5f;  // Fire does 1.5x damage
    }
    
    void cast() {
        std::cout << "🔥 Casting Fire Spell! (1.5x damage)\n";
    }
};

template<>
class Spell<IceElement> {
public:
    float calculateDamage(float power = 1.0f) {
        return power * 0.8f + 20.0f;  // Ice does 0.8x + freeze bonus
    }
    
    void cast() {
        std::cout << "❄️  Casting Ice Spell! (0.8x + 20 freeze damage)\n";
    }
};

template<>
class Spell<LightningElement> {
public:
    float calculateDamage(float power = 1.0f) {
        return power * 2.0f;  // Lightning is strongest
    }
    
    void cast() {
        std::cout << "⚡ Casting Lightning Spell! (2x damage)\n";
    }
};

// ============================================
// SFINAE CONSTRAINTS
// ============================================

// Type trait: is the target healable?
template<typename T>
struct is_healable : std::false_type {};

// Heal works on living things, not undead
template<>
struct is_healable<Undead> : std::false_type {};  // Undead can't be healed

// SFINAE: Only allow healing if target is healable
template<typename TargetType>
class HealSpell {
public:
    template<typename T = TargetType,
             typename = std::enable_if_t<is_healable<T>::value>>
    void cast(T& target) {
        std::cout << "✨ Healing target for 50 HP\n";
    }
};

// ============================================
// VARIADIC TEMPLATES - SPELL CHAINS
// ============================================

template<typename... Spells>
class SpellChain {
public:
    static constexpr float getTotalDamage() {
        return (Spell<Spells>().calculateDamage(10.0f) + ...);
    }
    
    static constexpr int getChainLength() {
        return sizeof...(Spells);
    }
    
    void cast() {
        std::cout << "🔗 Casting spell chain (" << getChainLength() << " spells):\n";
        (castOne<Spells>(), ...);
    }
    
private:
    template<typename SpellType>
    static void castOne() {
        Spell<SpellType> spell;
        spell.cast();
    }
};

// ============================================
// PERFECT FORWARDING FACTORY
// ============================================

template<typename SpellType, typename... Args>
auto makeSpell(Args&&... args) {
    // Perfect forwarding: preserves lvalue/rvalue nature
    return Spell<SpellType>();
}

// ============================================
// TEMPLATE TEMPLATE PARAMETERS
// ============================================

template<typename SpellType, template<typename> typename ContainerTemplate>
class Spellbook {
private:
    ContainerTemplate<Spell<SpellType>> spells;
    
public:
    void addSpell() {
        spells.emplace_back();
    }
    
    size_t getSpellCount() const {
        return spells.size();
    }
    
    void castAll() {
        for (auto& spell : spells) {
            spell.cast();
        }
    }
};

// ============================================
// DEMONSTRATIONS
// ============================================

void demonstrateSpecialization() {
    std::cout << "\n=== TEMPLATE SPECIALIZATION ===\n";
    
    Spell<FireElement> fireball;
    Spell<IceElement> iceshot;
    Spell<LightningElement> lightning;
    
    std::cout << "Fire damage: " << fireball.calculateDamage(100) << "\n";
    std::cout << "Ice damage: " << iceshot.calculateDamage(100) << "\n";
    std::cout << "Lightning damage: " << lightning.calculateDamage(100) << "\n";
    
    fireball.cast();
    iceshot.cast();
    lightning.cast();
}

void demonstrateVariadicChain() {
    std::cout << "\n=== VARIADIC SPELL CHAINS ===\n";
    
    using ChainType = SpellChain<FireElement, IceElement, LightningElement>;
    
    std::cout << "Chain length: " << ChainType::getChainLength() << "\n";
    std::cout << "Total damage: " << ChainType::getTotalDamage() << "\n";
    
    ChainType chain;
    chain.cast();
}

void demonstrateTemplateTemplateParams() {
    std::cout << "\n=== TEMPLATE TEMPLATE PARAMETERS ===\n";
    
    Spellbook<FireElement, std::vector> fireBook;
    fireBook.addSpell();
    fireBook.addSpell();
    
    std::cout << "Fire spells: " << fireBook.getSpellCount() << "\n";
    fireBook.castAll();
}

int main() {
    demonstrateSpecialization();
    demonstrateVariadicChain();
    demonstrateTemplateTemplateParams();
    
    std::cout << "\n✅ All template features working!\n";
    
    return 0;
}
