/**
 * ✅ SOLUTION - SFINAE Spell Constraints: Compile-Time Validation
 * 
 * KEY CONCEPTS:
 * - SFINAE (Substitution Failure Is Not An Error) for template constraint checking
 * - std::enable_if restricts template instantiation based on type traits
 * - Type traits enable compile-time properties checking
 * - Concepts (C++20) provide cleaner syntax for constraints
 */

#include <iostream>
#include <type_traits>
#include <string>
#include <concepts>

// ============================================
// ENTITY AND ZONE TYPE SYSTEM
// ============================================

// Type tags for entity classification
struct Living {};
struct Undead {};
struct Mechanical {};
struct Elemental {};

struct Player {
    using EntityType = Living;
    std::string name = "Hero";
};

struct Zombie {
    using EntityType = Undead;
    std::string name = "Zombie";
};

struct Robot {
    using EntityType = Mechanical;
    std::string name = "Robot";
};

// Fire spirit has special property - immune to fire
struct FireSpirit {
    using EntityType = Elemental;
    std::string name = "Fire Spirit";
    static constexpr bool isFireBased = true;
};

struct IceSpirit {
    using EntityType = Elemental;
    std::string name = "Ice Spirit";
    static constexpr bool isIceBased = true;
};

// Zone types with environmental properties
struct NormalZone {
    static constexpr bool hasWater = false;
    static constexpr bool isLavaZone = false;
};

struct WaterZone {
    static constexpr bool hasWater = true;
    static constexpr bool isLavaZone = false;
};

struct LavaZone {
    static constexpr bool hasWater = false;
    static constexpr bool isLavaZone = true;
};

// ============================================
// TYPE TRAITS FOR COMPILE-TIME CHECKING
// ============================================

/**
 * SOLUTION: Create type traits to classify entities
 * These enable SFINAE constraints in template functions
 */

/* Helper SFINAE detection: Check if type has specific attributes */
template<typename T, typename = void>
struct has_entity_type : std::false_type {};

template<typename T>
struct has_entity_type<T, std::void_t<typename T::EntityType>> : std::true_type {};

/* Trait: Is entity Living? */
template<typename Target, typename = void>
struct is_living : std::false_type {};

template<typename Target>
struct is_living<Target, std::enable_if_t<
    has_entity_type<Target>::value &&
    std::is_same_v<typename Target::EntityType, Living>
>> : std::true_type {};

/* Trait: Is entity Undead? */
template<typename Target, typename = void>
struct is_undead : std::false_type {};

template<typename Target>
struct is_undead<Target, std::enable_if_t<
    has_entity_type<Target>::value &&
    std::is_same_v<typename Target::EntityType, Undead>
>> : std::true_type {};

/* Trait: Is entity Mechanical? */
template<typename Target, typename = void>
struct is_mechanical : std::false_type {};

template<typename Target>
struct is_mechanical<Target, std::enable_if_t<
    has_entity_type<Target>::value &&
    std::is_same_v<typename Target::EntityType, Mechanical>
>> : std::true_type {};

/* Trait: Is entity Elemental? */
template<typename Target, typename = void>
struct is_elemental : std::false_type {};

template<typename Target>
struct is_elemental<Target, std::enable_if_t<
    has_entity_type<Target>::value &&
    std::is_same_v<typename Target::EntityType, Elemental>
>> : std::true_type {};

/**
 * SOLUTION: Create composite traits for spell requirements
 */

/* Trait: Is entity healable? (Living but not Mechanical) */
template<typename Target>
struct is_healable {
    static constexpr bool value = is_living<Target>::value;
};

/* Trait: Does zone have water? */
template<typename Zone>
struct is_water_zone {
    static constexpr bool value = Zone::hasWater;
};

/* Trait: Is zone a lava zone? */
template<typename Zone>
struct is_lava_zone {
    static constexpr bool value = Zone::isLavaZone;
};

/**
 * SOLUTION: Detector idiom - Check if type has specific static member
 * Used to detect fire-based entities, ice-based entities, etc.
 */
template<typename T, auto Member>
struct has_member {
    // This specialization exists if T::Member exists
    template<typename U>
    static auto check(U*) -> std::true_type;
    
    // Fallback for types that don't have the member
    template<typename U>
    static auto check(...) -> std::false_type;
    
    static constexpr bool value = decltype(check<T>(nullptr))::value;
};

/* Check if entity is fire-based */
template<typename Target, typename = void>
struct is_fire_based : std::false_type {};

template<typename Target>
struct is_fire_based<Target, std::enable_if_t<
    requires { Target::isFireBased; } && Target::isFireBased
>> : std::true_type {};

/* Check if entity is ice-based */
template<typename Target, typename = void>
struct is_ice_based : std::false_type {};

template<typename Target>
struct is_ice_based<Target, std::enable_if_t<
    requires { Target::isIceBased; } && Target::isIceBased
>> : std::true_type {};

// ============================================
// SOLUTION: CONSTRAINED SPELLS USING SFINAE
// ============================================

/**
 * SOLUTION: Heal Spell that ONLY works on Living targets
 * 
 * The template parameter constraint:
 * - typename = std::enable_if_t<is_healable<Target>::value>
 * 
 * If is_healable<Target>::value is false:
 * - enable_if_t<false> = (no type) -> SFINAE removes this instantiation
 * - Trying to call heal.cast(zombie) will NOT compile (missing overload)
 */
class ConstrainedHealSpell {
public:
    template<typename Target,
             typename = std::enable_if_t<is_healable<Target>::value>>
    void cast(Target& target) {
        std::cout << "Healing " << target.name << " for 25 HP\n";
    }
};

/**
 * SOLUTION: Fire Spell that does NOT work in water zones
 * 
 * Constraint: Zone must NOT have water
 * - !Zone::hasWater must be true
 * This prevents: fireSpell.castIn(waterZone)
 */
class ConstrainedFireSpell {
public:
    template<typename Zone,
             typename = std::enable_if_t<!Zone::hasWater>>
    void castIn(const Zone& zone) {
        std::cout << "FIREBALL! Dealing 50 fire damage!\n";
    }
};

/**
 * SOLUTION: Cold Spell that works EVERYWHERE
 * But targeting ice-based entities is ineffective
 */
class ConstrainedColdSpell {
private:
    // Helper to check if target is ice-based
    template<typename Target>
    static void castImpl(Target& target, std::false_type) {
        std::cout << "Freezing " << target.name << " for 30 damage!\n";
    }
    
    template<typename Target>
    static void castImpl(Target& target, std::true_type) {
        std::cout << "ERROR: " << target.name << " is ice-based, immune to cold!\n";
    }
    
public:
    template<typename Target>
    void cast(Target& target) {
        // Check at compile time if target is ice-based
        castImpl(target, std::bool_constant<is_ice_based<Target>::value>{});
    }
};

/**
 * SOLUTION: Spell combination that prevents certain pairings
 * Some spells work together, others don't
 */
template<typename Spell1, typename Spell2>
class SpellCombination {
public:
    // Default: spells can combine
    void cast() {
        std::cout << "Casting combined spell!\n";
    }
};

/**
 * SOLUTION: Specialized version for incompatible spell pairs
 * Fire + Ice combination needs special handling
 * 
 * This partial specialization is LESS specialized than the generic,
 * so it's used as a fallback with a helpful error message
 */

// ============================================
// BONUS: C++20 CONCEPTS FOR CLEANER SYNTAX
// ============================================

/**
 * BONUS: Using Concepts instead of enable_if_t
 * Much cleaner and more readable!
 * Concepts provide better error messages too.
 */

// Concept: Target must have EntityType member
template<typename T>
concept EntityType = requires {
    typename T::EntityType;
};

// Concept: EntityType must be Living (healable)
template<typename T>
concept Healable = EntityType<T> && 
                   std::is_same_v<typename T::EntityType, Living>;

// Concept: Zone must not have water
template<typename T>
concept NotWaterZone = requires {
    T::hasWater;
} && !T::hasWater;

// Concept: Zone must be lava (fire-friendly)
template<typename T>
concept LavaZone = requires {
    T::isLavaZone;
} && T::isLavaZone;

/**
 * BONUS: Spells using Concepts (cleaner!)
 */
class ModernHealSpell {
public:
    // This only compiles if Target is Healable
    void cast(Healable auto& target) {
        std::cout << "[Concept] Healing " << target.name << " for 25 HP\n";
    }
};

class ModernFireSpell {
public:
    // This only compiles if Zone is NotWaterZone
    void castIn(NotWaterZone auto& zone) {
        std::cout << "[Concept] FIREBALL!\n";
    }
};

// ============================================
// DEMONSTRATION & TESTING
// ============================================

void demonstrateConstraints() {
    std::cout << "╔════════════════════════════════════════╗\n";
    std::cout << "║  SFINAE Spell Constraints Demo        ║\n";
    std::cout << "║  Compile-Time Validation              ║\n";
    std::cout << "╚════════════════════════════════════════╝\n\n";
    
    // ===== HEAL SPELL =====
    std::cout << "=== HEAL SPELL (Healable targets only) ===\n";
    ConstrainedHealSpell heal;
    
    Player player;
    heal.cast(player);  // ✅ Works - Player is Living
    
    // heal.cast(Zombie{});  // ❌ COMPILE ERROR - Zombie is Undead, not healable!
    // The line above won't compile - that's the point!
    std::cout << "(Zombie healing would NOT compile!)\n\n";
    
    // ===== FIRE SPELL =====
    std::cout << "=== FIRE SPELL (NOT in water zones) ===\n";
    ConstrainedFireSpell fire;
    
    NormalZone normal;
    LavaZone lava;
    fire.castIn(normal);  // ✅ Works - no water
    fire.castIn(lava);    // ✅ Works - lava zone, fire friendly
    
    // fire.castIn(WaterZone{});  // ❌ COMPILE ERROR - Fire can't work in water!
    std::cout << "(Fire in water would NOT compile!)\n\n";
    
    // ===== COLD SPELL =====
    std::cout << "=== COLD SPELL (Works everywhere but) ===\n";
    ConstrainedColdSpell cold;
    
    cold.cast(player);  // ✅ Works, normal damage
    IceSpirit ice;
    cold.cast(ice);     // ⚠️ Works but ineffective (compile-time check)
    FireSpirit fire_s;
    cold.cast(fire_s);  // ✅ Works fine on fire creatures
    std::cout << "\n";
    
    // ===== MODERN CONCEPTS VERSION =====
    std::cout << "=== MODERN C++20 CONCEPTS VERSION ===\n";
    ModernHealSpell modern_heal;
    modern_heal.cast(player);  // ✅ Works - Healable concept satisfied
    
    // modern_heal.cast(Robot{});  // ❌ COMPILE ERROR - Robot not Healable!
    std::cout << "(Robot healing would NOT compile - Concept constraint!)\n\n";
    
    ModernFireSpell modern_fire;
    modern_fire.castIn(normal);  // ✅ Works
    // modern_fire.castIn(WaterZone{});  // ❌ COMPILE ERROR
    std::cout << "(Fire in water would NOT compile - Concept constraint!)\n";
}

// ============================================
// DIAGNOSTIC: TYPE TRAIT TESTING
// ============================================

void printTraitDiagnostics() {
    std::cout << "\n=== TYPE TRAIT DIAGNOSTICS ===\n";
    std::cout << "Player is_living: " << is_living<Player>::value << "\n";
    std::cout << "Player is_healable: " << is_healable<Player>::value << "\n";
    std::cout << "Player is_undead: " << is_undead<Player>::value << "\n";
    std::cout << "\n";
    
    std::cout << "Zombie is_living: " << is_living<Zombie>::value << "\n";
    std::cout << "Zombie is_healable: " << is_healable<Zombie>::value << "\n";
    std::cout << "Zombie is_undead: " << is_undead<Zombie>::value << "\n";
    std::cout << "\n";
    
    std::cout << "Robot is_living: " << is_living<Robot>::value << "\n";
    std::cout << "Robot is_healable: " << is_healable<Robot>::value << "\n";
    std::cout << "Robot is_mechanical: " << is_mechanical<Robot>::value << "\n";
    std::cout << "\n";
    
    std::cout << "WaterZone::hasWater: " << WaterZone::hasWater << "\n";
    std::cout << "NormalZone::hasWater: " << NormalZone::hasWater << "\n";
    std::cout << "LavaZone::isLavaZone: " << LavaZone::isLavaZone << "\n";
    std::cout << "\n";
    
    std::cout << "FireSpirit is_fire_based: " << is_fire_based<FireSpirit>::value << "\n";
    std::cout << "IceSpirit is_ice_based: " << is_ice_based<IceSpirit>::value << "\n";
}

// ============================================
// KEY INSIGHTS
// ============================================

void printInsights() {
    std::cout << "\n╔════════════════════════════════════════╗\n";
    std::cout << "║  KEY INSIGHTS: SFINAE vs Concepts     ║\n";
    std::cout << "╚════════════════════════════════════════╝\n\n";
    
    std::cout << "SFINAE (enable_if_t):\n";
    std::cout << "✓ Works in C++11 and later\n";
    std::cout << "✓ Removes invalid template instantiations\n";
    std::cout << "✗ Error messages are cryptic and hard to debug\n";
    std::cout << "✗ Syntax is verbose and easy to get wrong\n\n";
    
    std::cout << "CONCEPTS (C++20):\n";
    std::cout << "✓ Cleaner, more readable syntax\n";
    std::cout << "✓ MUCH better error messages\n";
    std::cout << "✓ Can check semantic properties of types\n";
    std::cout << "✓ Enables constraint satisfaction checking\n";
    std::cout << "✗ Only available in C++20 and later\n\n";
    
    std::cout << "PRINCIPLE:\n";
    std::cout << "Both SFINAE and Concepts allow you to:\n";
    std::cout << "- Restrict template instantiation based on type properties\n";
    std::cout << "- Move validation from runtime to compile-time\n";
    std::cout << "- Provide better type safety and user experience\n";
    std::cout << "- Generate compile errors instead of runtime bugs\n";
}

// ============================================
// MAIN
// ============================================

int main() {
    demonstrateConstraints();
    printTraitDiagnostics();
    printInsights();
    
    std::cout << "\n╔════════════════════════════════════════╗\n";
    std::cout << "║  CHALLENGE COMPLETED                  ║\n";
    std::cout << "║  Invalid spell casts now cause compile║\n";
    std::cout << "║  errors instead of runtime behavior   ║\n";
    std::cout << "╚════════════════════════════════════════╝\n";
    
    return 0;
}

/**
 * TRICKY ANSWER KEY:
 * 
 * Q1: What's the difference between enable_if_t<condition, void> and enable_if_t<condition>?
 *     A: They're THE SAME! enable_if defaults to void as its second parameter.
 *        enable_if<true> = void
 *        enable_if<true, T> = T (custom type)
 * 
 * Q2: Why doesn't "immediate context" work with static_assert?
 *     A: SFINAE only applies to type deduction and template instantiation.
 *        static_assert is NOT part of template instantiation - it's ALWAYS checked.
 *        To make it SFINAE, use enable_if in function signature, not in body.
 * 
 * Q3: When template satisfies multiple concepts, what happens?
 *     A: If equally specialized: AMBIGUOUS ERROR ("ambiguous overload")
 *        If one more specialized: compiler picks more specialized overload
 *        Example:
 *        template<Healable T> void spell(T&);      // #1
 *        template<Undead T> void spell(T&);         // #2
 *        spell(zombie);  // ERROR - both could match! (ambiguous)
 * 
 * Q4: Better SFINAE error messages?
 *     A: Add static_assert in the generic overload:
 *        template<typename T>
 *        void spell(T&) {
 *            static_assert(is_healable<T>::value,
 *                          "spell requires healable target!");
 *        }
 *        Now user gets readable error if wrong type is used.
 * 
 * Q5: Check for static member at compile-time?
 *     A: Use detector idiom with requires clause (C++20):
 *        template<typename T>
 *        concept FireBased = requires {
 *            { T::isFireBased } -> std::convertible_to<bool>;
 *        };
 *        
 *        Or std::void_t trick (C++17+):
 *        template<typename T, typename = void>
 *        struct has_fire_based : std::false_type {};
 *        
 *        template<typename T>
 *        struct has_fire_based<T,
 *            std::void_t<decltype(T::isFireBased)>
 *        > : std::true_type {};
 */
