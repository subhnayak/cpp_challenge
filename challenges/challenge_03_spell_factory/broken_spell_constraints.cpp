/**
 * 🔴 BROKEN CODE - SFINAE Spell Constraints
 * 
 * Some spell combinations are invalid and should be prevented
 * at COMPILE TIME, not runtime!
 * 
 * YOUR TASK: Use SFINAE to create compile-time constraints!
 */

#include <iostream>
#include <type_traits>
#include <string>

// ============================================
// ENTITY TYPE TRAITS
// ============================================

// Type tags
struct Living {};
struct Undead {};
struct Mechanical {};
struct Elemental {};

// Targets
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

struct FireSpirit {
    using EntityType = Elemental;
    std::string name = "Fire Spirit";
    static constexpr bool isFireBased = true;
};

// Zone types
struct NormalZone {
    static constexpr bool hasWater = false;
    static constexpr bool hasFire = false;
};

struct WaterZone {
    static constexpr bool hasWater = true;
    static constexpr bool hasFire = false;
};

struct LavaZone {
    static constexpr bool hasWater = false;
    static constexpr bool hasFire = true;
};

// ============================================
// BAD: RUNTIME CHECKING VERSION
// ============================================

class BadHealSpell {
public:
    template<typename Target>
    void cast(Target& target) {
        // BAD: Runtime check that could be compile-time!
        if constexpr (std::is_same_v<typename Target::EntityType, Undead>) {
            // This still compiles, just doesn't do anything at runtime
            std::cout << "ERROR: Cannot heal undead!\n";
            return;
        }
        
        std::cout << "Healing " << target.name << "\n";
    }
};

class BadFireSpell {
public:
    template<typename Zone>
    void castIn(const Zone& zone) {
        // BAD: Runtime check
        if (zone.hasWater) {
            std::cout << "ERROR: Fire doesn't work in water!\n";
            return;
        }
        
        std::cout << "Casting fireball!\n";
    }
};

// ============================================
// BROKEN SFINAE VERSION
// ============================================

// BUG: This doesn't actually prevent compilation!
class BrokenHealSpell {
public:
    // TODO: This should not compile if Target is Undead
    template<typename Target>
    void cast(Target& target) {
        // BUG: No SFINAE constraint!
        std::cout << "Healing " << target.name << "\n";
    }
};

// BUG: Fire spell that works everywhere (even underwater!)
class BrokenFireSpell {
public:
    // TODO: Should not compile if Zone::hasWater is true
    template<typename Zone>
    void castIn(const Zone&) {
        std::cout << "Casting fireball!\n";
    }
};

// BUG: This should prevent combining incompatible elements
template<typename Spell1, typename Spell2>
class BrokenSpellCombination {
public:
    // TODO: Should not compile if spells are incompatible
    // (e.g., Fire + Ice should require special handling)
    void cast() {
        std::cout << "Casting combined spell!\n";
    }
};

// ============================================
// TYPE TRAITS FOR SPELLS (BROKEN)
// ============================================

// BUG: These type traits are incomplete
template<typename T>
struct is_healable {
    // BUG: Always returns true!
    static constexpr bool value = true;
};

template<typename T>
struct is_fire_immune {
    static constexpr bool value = false;
};

// Specializations needed...
// template<> struct is_healable<Undead> { static constexpr bool value = false; };

// ============================================
// DEMONSTRATION
// ============================================

void demonstrateBadRuntime() {
    std::cout << "=== BAD RUNTIME CHECKING ===\n\n";
    
    BadHealSpell heal;
    Player player;
    Zombie zombie;
    
    heal.cast(player);  // Works
    heal.cast(zombie);  // Runtime error (but still compiles!)
    
    BadFireSpell fire;
    WaterZone water;
    NormalZone normal;
    
    fire.castIn(normal);  // Works
    fire.castIn(water);   // Runtime error (but still compiles!)
}

void demonstrateBrokenSFINAE() {
    std::cout << "\n=== BROKEN SFINAE VERSION ===\n\n";
    
    BrokenHealSpell heal;
    Zombie zombie;
    
    // BUG: This compiles but shouldn't!
    heal.cast(zombie);
    std::cout << "(This shouldn't have compiled!)\n";
    
    BrokenFireSpell fire;
    WaterZone water;
    
    // BUG: This compiles but shouldn't!
    fire.castIn(water);
    std::cout << "(Fire in water? This shouldn't compile!)\n";
}

// ============================================
// YOUR TASK
// ============================================

/*
 * TODO: Create solution_spell_constraints.cpp with:
 * 
 * 1. Type traits for entity classification:
 *    - is_living<T>
 *    - is_undead<T>  
 *    - is_mechanical<T>
 *    - is_healable<T> (living but not mechanical)
 * 
 * 2. ConstrainedHealSpell that ONLY compiles for healable targets:
 *    
 *    template<typename Target,
 *             typename = std::enable_if_t<is_healable<Target>::value>>
 *    void cast(Target& target);
 * 
 * 3. ConstrainedFireSpell that won't compile in water zones:
 *    
 *    template<typename Zone,
 *             typename = std::enable_if_t<!Zone::hasWater>>
 *    void castIn(const Zone& zone);
 * 
 * 4. BONUS: Use concepts (C++20) instead of SFINAE:
 *    
 *    template<typename T>
 *    concept Healable = requires { typename T::EntityType; } 
 *                       && !std::is_same_v<typename T::EntityType, Undead>;
 *    
 *    void cast(Healable auto& target);
 * 
 * TRICKY QUESTIONS:
 * 
 * Q1: What's the difference between:
 *     std::enable_if_t<condition, void>
 *     std::enable_if_t<condition>
 *     Are they the same?
 * 
 * Q2: SFINAE only works in "immediate context". What does that mean?
 *     Why does this NOT work?
 *     
 *     template<typename T>
 *     void foo(T t) {
 *         static_assert(is_healable<T>::value);  // Not SFINAE!
 *     }
 * 
 * Q3: You have two overloads:
 *     template<typename T> void spell(T& t) requires Healable<T>;
 *     template<typename T> void spell(T& t) requires Undead<T>;
 *     
 *     What happens if T satisfies both? Neither?
 * 
 * Q4: SFINAE error messages are notoriously bad. How can you make them better?
 *     (Hint: static_assert with a message in the generic overload)
 * 
 * Q5: CHALLENGE - Implement this constraint:
 *     "FireSpell can target anything EXCEPT entities with isFireBased = true"
 *     
 *     How do you check if a type has a specific static member?
 *     (Hint: detector idiom or std::void_t)
 */

int main() {
    demonstrateBadRuntime();
    demonstrateBrokenSFINAE();
    
    std::cout << "\n========================================\n";
    std::cout << "Add SFINAE constraints to get compile errors!\n";
    std::cout << "========================================\n";
    
    // GOAL: These lines should NOT compile after you fix it:
    // 
    // ConstrainedHealSpell heal;
    // Zombie zombie;
    // heal.cast(zombie);  // COMPILE ERROR!
    //
    // ConstrainedFireSpell fire;
    // WaterZone water;
    // fire.castIn(water);  // COMPILE ERROR!
    
    return 0;
}
