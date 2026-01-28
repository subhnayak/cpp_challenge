# 🧙 Challenge 3: The Spell Factory

## The Situation

The spell system needs to support many spell types with type-safe combinations. The previous developer used tons of runtime checks and `dynamic_cast`. It's slow and crashes with wrong spell combinations!

Your task: Use templates, SFINAE, and template specialization to create a type-safe, compile-time validated spell system.

---

## 🎯 Objectives

### Objective 3.1: Template Specialization for Spell Types
**File: `broken_spell_types.cpp`**

Different spells need different damage calculation formulas. Use template specialization instead of if-else chains!

**Your Mission:**
- Create a generic `Spell<T>` template
- Specialize for `FireElement`, `IceElement`, `LightningElement`
- Each specialization has unique damage formula

**The Bad Code:**
```cpp
// DON'T DO THIS
float calculateDamage(Spell* spell) {
    if (auto fire = dynamic_cast<FireSpell*>(spell)) {
        return fire->power * 1.5f;
    } else if (auto ice = dynamic_cast<IceSpell*>(spell)) {
        return ice->power * 0.8f + freezeBonus;
    }
    // ... endless chain
}
```

---

### Objective 3.2: SFINAE Magic Restrictions
**File: `broken_spell_constraints.cpp`**

Some spell combinations are invalid! Use SFINAE to prevent them at compile time.

**Your Mission:**
- Prevent healing spells from targeting undead (compile error!)
- Prevent fire spells from being cast underwater
- Only allow combination spells if elements are compatible

**The Puzzle:**
```cpp
HealSpell heal;
Undead zombie;
heal.cast(zombie);  // This should NOT compile!

FireSpell fire;
WaterZone zone;
fire.castIn(zone);  // Compile error: fire can't be cast in water!
```

---

### Objective 3.3: Variadic Template Spell Chains
**File: `broken_spell_chain.cpp`**

Mages can chain multiple spells together. Use variadic templates!

**Your Mission:**
- `SpellChain<Fire, Ice, Lightning>` casts all three in sequence
- Calculate combined damage/mana cost at compile time
- Support any number of spells in the chain

**The Challenge:**
```cpp
// Should work with any number of spells
auto chain1 = makeSpellChain<Fire>();           // 1 spell
auto chain2 = makeSpellChain<Fire, Ice>();      // 2 spells  
auto chain3 = makeSpellChain<Fire, Ice, Fire, Lightning, Fire>();  // 5 spells

// Compile-time mana calculation
constexpr int totalMana = chain3.getManaCost();  // Computed at compile time!
```

---

### Objective 3.4: Perfect Forwarding Spell Creation
**File: `broken_spell_factory.cpp`**

The spell factory creates spells with various constructor arguments. Use perfect forwarding!

**Your Mission:**
- Create `makeSpell<T>(args...)` that forwards all arguments
- Avoid unnecessary copies of spell parameters
- Support move-only spell components

**The Problem:**
```cpp
// Without perfect forwarding
Spell createSpell(std::string name, Effect effect) {
    return Spell(name, effect);  // Copies name and effect!
}

// With perfect forwarding
template<typename... Args>
Spell createSpell(Args&&... args) {
    return Spell(std::forward<Args>(args)...);  // No copies!
}
```

---

### Objective 3.5: Template Template Parameters
**File: `broken_spell_container.cpp`**

The spell inventory can use different container types. Use template template parameters!

**Your Mission:**
- `SpellBook<SpellType, ContainerTemplate>` works with vector, list, deque
- The container template is itself a template parameter

**The Puzzle:**
```cpp
SpellBook<FireSpell, std::vector> fireBook;    // Uses vector internally
SpellBook<IceSpell, std::list> iceBook;        // Uses list internally
SpellBook<AllSpells, std::deque> mixedBook;    // Uses deque
```

---

## 📝 Files to Modify

1. `broken_spell_types.cpp` → Create `solution_spell_types.cpp`
2. `broken_spell_constraints.cpp` → Create `solution_spell_constraints.cpp`
3. `broken_spell_chain.cpp` → Create `solution_spell_chain.cpp`
4. `broken_spell_factory.cpp` → Create `solution_spell_factory.cpp`
5. `broken_spell_container.cpp` → Create `solution_spell_container.cpp`

---

## ✅ Success Criteria

- Template specializations work correctly for all element types
- Invalid spell combinations fail at compile time, not runtime
- Variadic spell chains work with any number of spells
- Perfect forwarding avoids all unnecessary copies
- Template template parameters work with different STL containers

---

## 💭 Reflection Questions

1. What's the difference between SFINAE and `static_assert` for constraints?
2. Why is `std::forward<T>(t)` needed? What happens without it?
3. How does `std::enable_if` work under the hood?
4. What are the compile-time vs. runtime tradeoffs of heavy template use?
5. When would you prefer concepts (C++20) over SFINAE?

---

## 🔑 Hints

<details>
<summary>Hint 1: Template Specialization Syntax</summary>

```cpp
template<typename Element>
struct Spell { /* generic */ };

template<>
struct Spell<Fire> { /* fire-specific */ };
```
</details>

<details>
<summary>Hint 2: SFINAE with enable_if</summary>

```cpp
template<typename Target,
         typename = std::enable_if_t<!std::is_same_v<Target, Undead>>>
void heal(Target& t) { /* ... */ }
```
</details>

<details>
<summary>Hint 3: Variadic Template Recursion</summary>

```cpp
template<typename First, typename... Rest>
int totalManaCost() {
    return First::ManaCost + totalManaCost<Rest...>();
}

template<>
int totalManaCost<>() { return 0; }  // Base case
```
</details>
