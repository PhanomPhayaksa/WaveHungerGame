#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <chrono>
#include <thread>
#include <vector>
#include <algorithm>
#include <map>
#include <limits>

using namespace std;

// ========== Utility Functions ==========
/**
 * @brief Clears the console screen based on the operating system.
 */
void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void animateText(const string &text, int delayMs = 50)
{
    for (char c : text)
    {
        cout << c << flush;
        this_thread::sleep_for(chrono::milliseconds(delayMs));
    }
}

int getValidInput(int minChoice, int maxChoice, const string &prompt = "Enter your choice: ")
{
    int choice;
    while (true)
    {
        cout << prompt;
        cin >> choice;

        if (cin.fail() || choice < minChoice || choice > maxChoice)
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! Please enter a number between "
                 << minChoice << " and " << maxChoice << ".\n";
        }
        else
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return choice;
        }
    }
}

// ========== GameSystems ===========
enum class StatusEffect
{
    NONE,
    POISON,      // Takes damage over time
    BLEED,       // Takes damage each turn
    STUN,        // Skips next turn
    STRENGTH_UP, // Increased attack
    WEAKNESS     // Decreased attack
};

map<StatusEffect, string> statusNames = {
    {StatusEffect::NONE, "None"},
    {StatusEffect::POISON, "Poison"},
    {StatusEffect::BLEED, "Bleed"},
    {StatusEffect::STUN, "Stun"},
    {StatusEffect::STRENGTH_UP, "Strength Up"},
    {StatusEffect::WEAKNESS, "Weakness"}};

enum class ItemType
{
    WEAPON,
    ARMOR,
    ACCESSORY,
    POTION
};

class Potion
{
private:
    string name;
    int amount;
    bool isHealthPotion;

public:
    Potion(string n, int a, bool health) : name(n), amount(a), isHealthPotion(health) {}

    string getName() const { return name; }
    int getAmount() const { return amount; }
    bool isHealth() const { return isHealthPotion; }

    void displayInfo() const
    {
        cout << "\033[1;34m" << name << "\033[0m - Restores " << amount
             << (isHealthPotion ? " HP" : " MP") << "\n";
    }
};
// ========= BaseClassItem ========
class Item
{
protected:
    string name;
    string description;
    ItemType type;
    int attackBonus;
    int healthBonus;
    int defenseBonus;
    int manaBonus;

public:
    Item(string n, string desc, ItemType t, int atk = 0, int hp = 0, int def = 0, int mp = 0)
        : name(n), description(desc), type(t), attackBonus(atk), healthBonus(hp), defenseBonus(def), manaBonus(mp) {}

    virtual ~Item() {}

    string getName() const { return name; }
    string getDescription() const { return description; }
    ItemType getType() const { return type; }
    int getAttackBonus() const { return attackBonus; }
    int getHealthBonus() const { return healthBonus; }
    int getDefenseBonus() const { return defenseBonus; }
    int getManaBonus() const { return manaBonus; }

    virtual void applyEffect(class Unit &unit) {}
    virtual void displayInfo() const
    {
        cout << "\033[1;33m" << name << "\033[0m - " << description;
        if (attackBonus > 0)
            cout << " [ATK +" << attackBonus << "]";
        if (healthBonus > 0)
            cout << " [HP +" << healthBonus << "]";
        if (defenseBonus > 0)
            cout << " [DEF +" << defenseBonus << "]";
        if (manaBonus > 0)
            cout << " [MP +" << manaBonus << "]";
        cout << "\n";
    }
};

// ========= DerivedClassItem ===========
// SpecificItemClasses
class FireSword : public Item
{
public:
    FireSword() : Item("Fire Sword", "Burns enemies with fire damage", ItemType::WEAPON, 15) {}
    void applyEffect(class Unit &unit) override;
};

class IceShield : public Item
{
public:
    IceShield() : Item("Ice Shield", "Freezes attackers occasionally", ItemType::ARMOR, 0, 20, 10) {}
    void applyEffect(class Unit &unit) override;
};

class VampireRing : public Item
{
public:
    VampireRing() : Item("Vampire Ring", "Heals user when dealing damage", ItemType::ACCESSORY, 5, 0, 0) {}
    void applyEffect(class Unit &unit) override;
};

class PoisonDagger : public Item
{
public:
    PoisonDagger() : Item("Poison Dagger", "Poisons enemies on hit", ItemType::WEAPON, 8, 0, 0) {}
    void applyEffect(class Unit &unit) override;
};

class DragonScale : public Item
{
public:
    DragonScale() : Item("Dragon Scale", "Grants fire resistance and strength", ItemType::ARMOR, 10, 30, 5) {}
    void applyEffect(class Unit &unit) override;
};

class LightningOrb : public Item
{
public:
    LightningOrb() : Item("Lightning Orb", "Chance to stun enemies", ItemType::ACCESSORY, 12, 0, 0) {}
    void applyEffect(class Unit &unit) override;
};

class HealthPotion : public Item
{
public:
    HealthPotion() : Item("Health Potion", "Restores HP", ItemType::POTION, 0, 30) {}
    void applyEffect(class Unit &unit) override;
};

class ManaPotion : public Item
{
public:
    ManaPotion() : Item("Mana Potion", "Restores MP", ItemType::POTION, 0, 0, 0, 20) {}
    void applyEffect(class Unit &unit) override;
};

// ========== BaseClassUnit ==========
class Unit
{
protected:
    string name;
    int maxHealth;
    int health;
    int maxMana;
    int mana;
    int baseAttack;
    int currentAttack;
    int defense;
    map<StatusEffect, int> statusEffects;
    vector<Item *> equipment;
    vector<Potion> potions;
    vector<string> battleLog;

public:
    Unit(string n, int hp, int mp, int atk, int def = 0)
        : name(n), maxHealth(hp), health(hp), maxMana(mp), mana(mp),
          baseAttack(atk), currentAttack(atk), defense(def) {}

    virtual ~Unit()
    {
        for (Item *item : equipment)
            delete item;
    }

    // Getters
    string getName() const { return name; }
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    int getMana() const { return mana; }
    int getMaxMana() const { return maxMana; }
    int getAttack() const { return currentAttack; }
    int getDefense() const { return defense; }
    bool isAlive() const { return health > 0; }
    const vector<Potion> &getPotions() const { return potions; }
    const vector<string> &getBattleLog() const { return battleLog; }
    void clearBattleLog() { battleLog.clear(); }

    // Item management
    void addToBattleLog(const string &message)
    {
        battleLog.push_back(message);
    }
    void addItem(Item *item)
    {
        if (item->getType() == ItemType::POTION)
        {
            if (dynamic_cast<HealthPotion *>(item))
            {
                potions.push_back(Potion(item->getName(), item->getHealthBonus(), true));
            }
            else
            {
                potions.push_back(Potion(item->getName(), item->getManaBonus(), false));
            }
            delete item;
        }
        else
        {
            equipment.push_back(item);
            item->applyEffect(*this);
            battleLog.push_back(name + " equipped " + item->getName() + "!");
        }
    }

    bool showInventory(bool &usedPotion)
    {
        cout << "\n=== INVENTORY ===\n";

        if (equipment.empty() && potions.empty())
        {
            cout << "Inventory is empty.\n";
            cout << "\nPress Enter to continue...";
            cin.ignore();
            cin.get();
            return false;
        }

        if (!equipment.empty())
        {
            cout << "Equipment:\n";
            for (const Item *item : equipment)
            {
                cout << "- ";
                item->displayInfo();
            }
        }

        if (!potions.empty())
        {
            cout << "\nPotions:\n";
            for (size_t i = 0; i < potions.size(); i++)
            {
                cout << i + 1 << ". ";
                potions[i].displayInfo();
            }

            cout << "\nEnter potion number to use (0 to cancel): ";
            int choice = getValidInput(0, potions.size());

            if (choice > 0)
            {
                usePotion(choice - 1);
                usedPotion = true;
                return true;
            }
        }
        else
        {
            cout << "\nPress Enter to continue...";
            cin.ignore();
            cin.get();
        }
        return false;
    }

    bool usePotion(int index)
    {
        if (index < 0 || index >= potions.size())
            return false;

        Potion potion = potions[index];
        if (potion.isHealth())
        {
            heal(potion.getAmount());
        }
        else
        {
            restoreMana(potion.getAmount());
        }

        battleLog.push_back(name + " used " + potion.getName() + "!");
        potions.erase(potions.begin() + index);
        return true;
    }

    // Status effects
    void addStatus(StatusEffect effect, int duration, string source)
    {
        statusEffects[effect] = duration;
        battleLog.push_back(source + " applied " + statusNames.at(effect) + " to " + name + "!");
    }

    bool hasStatus(StatusEffect effect) const
    {
        return statusEffects.count(effect) && statusEffects.at(effect) > 0;
    }

    void clearStatus(StatusEffect effect)
    {
        statusEffects.erase(effect);
    }

    void processStatusEffects()
    {
        vector<StatusEffect> toRemove;

        for (auto &[effect, duration] : statusEffects)
        {
            switch (effect)
            {
            case StatusEffect::POISON:
                takeDamage(5, false, "Poison");
                break;
            case StatusEffect::BLEED:
                takeDamage(3, false, "Bleed");
                break;
            case StatusEffect::STRENGTH_UP:
                currentAttack = baseAttack + 10;
                break;
            case StatusEffect::WEAKNESS:
                currentAttack = max(1, baseAttack - 5);
                break;
            default:
                break;
            }

            if (--duration <= 0)
            {
                toRemove.push_back(effect);
                battleLog.push_back(name + "'s " + statusNames.at(effect) + " wore off!");
            }
        }

        for (auto effect : toRemove)
        {
            statusEffects.erase(effect);
            if (effect == StatusEffect::STRENGTH_UP || effect == StatusEffect::WEAKNESS)
            {
                currentAttack = baseAttack;
            }
        }
    }

    // Combat actions
    void takeDamage(int dmg, bool showBlock = true, string source = "")
    {
        int actualDamage = max(1, dmg - defense);
        int prevHealth = health;
        health = max(0, health - actualDamage);

        string msg = name + " took " + to_string(actualDamage) + " damage";
        if (!source.empty())
            msg += " from " + source;
        msg += "! [" + to_string(prevHealth) + " -> " + to_string(health) + "/" + to_string(maxHealth) + " HP]";
        battleLog.push_back(msg);

        if (showBlock && defense > 0 && actualDamage < dmg)
        {
            battleLog.push_back(name + " blocked " + to_string(dmg - actualDamage) + " damage!");
        }
    }

    void heal(int amount)
    {
        int oldHealth = health;
        health = min(maxHealth, health + amount);
        int healedAmount = health - oldHealth;
        battleLog.push_back(name + " healed " + to_string(healedAmount) + " HP! (" + to_string(health) + "/" + to_string(maxHealth) + ")");
    }

    void restoreMana(int amount)
    {
        int oldMana = mana;
        mana = min(maxMana, mana + amount);
        int restoredAmount = mana - oldMana;
        battleLog.push_back(name + " restored " + to_string(restoredAmount) + " MP! (" + to_string(mana) + "/" + to_string(maxMana) + ")");
    }

    void increaseMaxHealth(int amount)
    {
        maxHealth += amount;
        health += amount;
        battleLog.push_back(name + "'s max HP increased by " + to_string(amount) + "!");
    }

    void increaseMaxMana(int amount)
    {
        maxMana += amount;
        mana += amount;
        battleLog.push_back(name + "'s max MP increased by " + to_string(amount) + "!");
    }

    void increaseAttack(int amount)
    {
        baseAttack += amount;
        currentAttack = baseAttack;
        battleLog.push_back(name + "'s attack increased by " + to_string(amount) + "!");
    }

    void increaseDefense(int amount)
    {
        defense += amount;
        battleLog.push_back(name + "'s defense increased by " + to_string(amount) + "!");
    }

    virtual void attack(Unit &target)
    {
        int targetPrevHealth = target.getHealth();
        battleLog.push_back(name + " attacks " + target.getName() + " for " + to_string(currentAttack) + " damage!");
        target.takeDamage(currentAttack);
        battleLog.push_back(target.getName() + "'s HP: " + to_string(targetPrevHealth) + " -> " +
                            to_string(target.getHealth()) + "/" + to_string(target.getMaxHealth()));
    }

    // Skills
    virtual bool useSkill1(Unit &target) = 0;
    virtual bool useSkill2(Unit &target) = 0;
    virtual bool useSkill3(Unit &target) = 0;
    virtual string getSkill1Name() const = 0;
    virtual string getSkill2Name() const = 0;
    virtual string getSkill3Name() const = 0;
    virtual int getSkill1Cost() const = 0;
    virtual int getSkill2Cost() const = 0;
    virtual int getSkill3Cost() const = 0;

    virtual void displayStatus() const
    {
        cout << name << " - HP: \033[1;31m" << (isAlive() ? to_string(health) : "0") << "/" << maxHealth << "\033[0m"
             << ", MP: \033[1;34m" << mana << "/" << maxMana << "\033[0m"
             << ", ATK: \033[1;33m" << currentAttack << "\033[0m"
             << ", DEF: \033[1;34m" << defense << "\033[0m";

        if (!statusEffects.empty())
        {
            cout << " [Status:";
            for (const auto &[effect, duration] : statusEffects)
            {
                cout << " " << statusNames.at(effect) << "(" << duration << ")";
            }
            cout << "]";
        }
        cout << "\n";
    }

    void displayBattleLog() const
    {
        cout << "\n=== BATTLE LOG ===\n";
        if (battleLog.empty())
        {
            cout << "No actions yet.\n";
        }
        else
        {
            for (const string &log : battleLog)
            {
                cout << "> " << log << "\n";
            }
        }
    }
};

// Item effect implementations
void FireSword::applyEffect(Unit &unit)
{
    unit.increaseAttack(attackBonus);
}

void IceShield::applyEffect(Unit &unit)
{
    unit.increaseMaxHealth(healthBonus);
    unit.increaseDefense(defenseBonus);
}

void VampireRing::applyEffect(Unit &unit)
{
    unit.increaseAttack(attackBonus);
}

void PoisonDagger::applyEffect(Unit &unit)
{
    unit.increaseAttack(attackBonus);
}

void DragonScale::applyEffect(Unit &unit)
{
    unit.increaseAttack(attackBonus);
    unit.increaseMaxHealth(healthBonus);
    unit.increaseDefense(defenseBonus);
}

void LightningOrb::applyEffect(Unit &unit)
{
    unit.increaseAttack(attackBonus);
}

void HealthPotion::applyEffect(Unit &unit)
{
    unit.heal(healthBonus);
}

void ManaPotion::applyEffect(Unit &unit)
{
    unit.restoreMana(manaBonus);
}

// ========= DerivedClassUnit ===========
// Class player
class Warrior : public Unit
{
public:
    Warrior(string n) : Unit(n, 120, 50, 20, 2) {}

    string getSkill1Name() const override { return "Power Strike"; }
    string getSkill2Name() const override { return "Demoralizing Shout"; }
    string getSkill3Name() const override { return "Battle Rage"; }
    int getSkill1Cost() const override { return 15; }
    int getSkill2Cost() const override { return 10; }
    int getSkill3Cost() const override { return 20; }

    bool useSkill1(Unit &target) override
    {
        if (mana < getSkill1Cost())
        {
            battleLog.push_back("Not enough MP for Power Strike!");
            return false;
        }

        mana -= getSkill1Cost();
        int dmg = currentAttack + 25;
        int targetPrevHealth = target.getHealth();
        battleLog.push_back(name + " uses Power Strike on " + target.getName() + " for " + to_string(dmg) + " damage!");
        target.takeDamage(dmg);
        battleLog.push_back(target.getName() + "'s HP: " + to_string(targetPrevHealth) + " -> " +
                            to_string(target.getHealth()) + "/" + to_string(target.getMaxHealth()));
        return true;
    }

    bool useSkill2(Unit &target) override
    {
        if (mana < getSkill2Cost())
        {
            battleLog.push_back("Not enough MP for Demoralizing Shout!");
            return false;
        }

        mana -= getSkill2Cost();
        int targetPrevAttack = target.getAttack();
        battleLog.push_back(name + " uses Demoralizing Shout on " + target.getName() + "!");
        target.addStatus(StatusEffect::WEAKNESS, 3, name);
        battleLog.push_back(target.getName() + "'s ATK: " + to_string(targetPrevAttack) + " -> " +
                            to_string(target.getAttack()));
        return true;
    }

    bool useSkill3(Unit &target) override
    {
        if (mana < getSkill3Cost())
        {
            battleLog.push_back("Not enough MP for Battle Rage!");
            return false;
        }

        mana -= getSkill3Cost();
        int prevAttack = currentAttack;
        battleLog.push_back(name + " enters Battle Rage!");
        addStatus(StatusEffect::STRENGTH_UP, 3, name);
        battleLog.push_back(name + "'s ATK: " + to_string(prevAttack) + " -> " +
                            to_string(currentAttack));
        return true;
    }
};

class Archer : public Unit
{
public:
    Archer(string n) : Unit(n, 80, 35, 30, 1) {}

    string getSkill1Name() const override { return "Poison Arrow"; }
    string getSkill2Name() const override { return "Piercing Shot"; }
    string getSkill3Name() const override { return "Double Shot"; }
    int getSkill1Cost() const override { return 10; }
    int getSkill2Cost() const override { return 15; }
    int getSkill3Cost() const override { return 20; }

    bool useSkill1(Unit &target) override
    {
        if (mana < getSkill1Cost())
        {
            battleLog.push_back("Not enough MP for Poison Arrow!");
            return false;
        }

        mana -= getSkill1Cost();
        int dmg = currentAttack;
        int targetPrevHealth = target.getHealth();
        battleLog.push_back(name + " shoots Poison Arrow at " + target.getName() + " for " + to_string(dmg) + " damage!");
        target.takeDamage(dmg);
        target.addStatus(StatusEffect::POISON, 3, name);
        battleLog.push_back(target.getName() + "'s HP: " + to_string(targetPrevHealth) + " -> " +
                            to_string(target.getHealth()) + "/" + to_string(target.getMaxHealth()));
        return true;
    }

    bool useSkill2(Unit &target) override
    {
        if (mana < getSkill2Cost())
        {
            battleLog.push_back("Not enough MP for Piercing Shot!");
            return false;
        }

        mana -= getSkill2Cost();
        int dmg = currentAttack + 10;
        int targetPrevHealth = target.getHealth();
        battleLog.push_back(name + " uses Piercing Shot on " + target.getName() + " for " + to_string(dmg) + " damage!");
        target.takeDamage(dmg);
        target.addStatus(StatusEffect::BLEED, 2, name);
        battleLog.push_back(target.getName() + "'s HP: " + to_string(targetPrevHealth) + " -> " +
                            to_string(target.getHealth()) + "/" + to_string(target.getMaxHealth()));
        return true;
    }

    bool useSkill3(Unit &target) override
    {
        if (mana < getSkill3Cost())
        {
            battleLog.push_back("Not enough MP for Double Shot!");
            return false;
        }

        mana -= getSkill3Cost();
        int targetPrevHealth = target.getHealth();
        battleLog.push_back(name + " uses Double Shot on " + target.getName() + "!");
        target.takeDamage(currentAttack);
        target.takeDamage(currentAttack);
        battleLog.push_back(target.getName() + "'s HP: " + to_string(targetPrevHealth) + " -> " +
                            to_string(target.getHealth()) + "/" + to_string(target.getMaxHealth()));
        return true;
    }
};

class Mage : public Unit
{
public:
    Mage(string n) : Unit(n, 70, 80, 25, 0) {}

    string getSkill1Name() const override { return "Fireball"; }
    string getSkill2Name() const override { return "Ice Nova"; }
    string getSkill3Name() const override { return "Life Drain"; }
    int getSkill1Cost() const override { return 20; }
    int getSkill2Cost() const override { return 15; }
    int getSkill3Cost() const override { return 25; }

    bool useSkill1(Unit &target) override
    {
        if (mana < getSkill1Cost())
        {
            battleLog.push_back("Not enough MP for Fireball!");
            return false;
        }

        mana -= getSkill1Cost();
        int dmg = currentAttack + 20;
        int targetPrevHealth = target.getHealth();
        battleLog.push_back(name + " casts Fireball on " + target.getName() + " for " + to_string(dmg) + " damage!");
        target.takeDamage(dmg);
        battleLog.push_back(target.getName() + "'s HP: " + to_string(targetPrevHealth) + " -> " +
                            to_string(target.getHealth()) + "/" + to_string(target.getMaxHealth()));
        return true;
    }

    bool useSkill2(Unit &target) override
    {
        if (mana < getSkill2Cost())
        {
            battleLog.push_back("Not enough MP for Ice Nova!");
            return false;
        }

        mana -= getSkill2Cost();
        battleLog.push_back(name + " casts Ice Nova on " + target.getName() + "!");
        target.addStatus(StatusEffect::STUN, 1, name);
        battleLog.push_back(target.getName() + " is stunned for 1 turn!");
        return true;
    }

    bool useSkill3(Unit &target) override
    {
        if (mana < getSkill3Cost())
        {
            battleLog.push_back("Not enough MP for Life Drain!");
            return false;
        }

        mana -= getSkill3Cost();
        int dmg = currentAttack + 5;
        int targetPrevHealth = target.getHealth();
        int prevHealth = health;
        battleLog.push_back(name + " drains life from " + target.getName() + " for " + to_string(dmg) + " damage!");
        target.takeDamage(dmg);
        heal(dmg / 2);
        battleLog.push_back(target.getName() + "'s HP: " + to_string(targetPrevHealth) + " -> " +
                            to_string(target.getHealth()) + "/" + to_string(target.getMaxHealth()));
        battleLog.push_back(name + "'s HP: " + to_string(prevHealth) + " -> " +
                            to_string(health) + "/" + to_string(maxHealth));
        return true;
    }
};
// ======= DerivedClassUnit =======
// ======= BossClass =======
class BossUnit : public Unit
{
private:
    string skillName1, skillName2, skillName3;
    int skillCost1, skillCost2, skillCost3;

public:
    BossUnit(string n, string sk1, string sk2, string sk3, int atk, int hp, int def = 0)
        : Unit(n, hp, 60, atk, def), skillName1(sk1), skillName2(sk2), skillName3(sk3),
          skillCost1(15), skillCost2(20), skillCost3(25) {}

    string getSkill1Name() const override { return skillName1; }
    string getSkill2Name() const override { return skillName2; }
    string getSkill3Name() const override { return skillName3; }
    int getSkill1Cost() const override { return skillCost1; }
    int getSkill2Cost() const override { return skillCost2; }
    int getSkill3Cost() const override { return skillCost3; }

    bool useSkill1(Unit &target) override
    {
        if (mana < skillCost1)
        {
            battleLog.push_back(name + " doesn't have enough MP for " + skillName1 + "!");
            return false;
        }

        mana -= skillCost1;
        int dmg = currentAttack + 20;
        int targetPrevHealth = target.getHealth();
        battleLog.push_back(name + " uses " + skillName1 + " on " + target.getName() + " for " + to_string(dmg) + " damage!");
        target.takeDamage(dmg);
        battleLog.push_back(target.getName() + "'s HP: " + to_string(targetPrevHealth) + " -> " +
                            to_string(target.getHealth()) + "/" + to_string(target.getMaxHealth()));
        return true;
    }

    bool useSkill2(Unit &target) override
    {
        if (mana < skillCost2)
        {
            battleLog.push_back(name + " doesn't have enough MP for " + skillName2 + "!");
            return false;
        }

        mana -= skillCost2;
        int dmg = currentAttack + 15; // Added damage component
        int targetPrevHealth = target.getHealth();
        battleLog.push_back(name + " uses " + skillName2 + " on " + target.getName() + " for " + to_string(dmg) + " damage!");
        target.takeDamage(dmg);
        target.addStatus(StatusEffect::POISON, 3, name);
        battleLog.push_back(target.getName() + "'s HP: " + to_string(targetPrevHealth) + " -> " +
                            to_string(target.getHealth()) + "/" + to_string(target.getMaxHealth()));
        battleLog.push_back(target.getName() + " is poisoned for 3 turns!");
        return true;
    }

    bool useSkill3(Unit &target) override
    {
        if (mana < skillCost3)
        {
            battleLog.push_back(name + " doesn't have enough MP for " + skillName3 + "!");
            return false;
        }

        mana -= skillCost3;
        int dmg = currentAttack + 10; // Added damage component
        int targetPrevHealth = target.getHealth();
        int prevHealth = health;
        battleLog.push_back(name + " uses " + skillName3 + " on " + target.getName() + " for " + to_string(dmg) + " damage!");
        target.takeDamage(dmg);
        target.addStatus(StatusEffect::STUN, 1, name);
        heal(20);
        battleLog.push_back(target.getName() + "'s HP: " + to_string(targetPrevHealth) + " -> " +
                            to_string(target.getHealth()) + "/" + to_string(target.getMaxHealth()));
        battleLog.push_back(target.getName() + " is stunned for 1 turn!");
        battleLog.push_back(name + "'s HP: " + to_string(prevHealth) + " -> " +
                            to_string(health) + "/" + to_string(maxHealth));
        return true;
    }

    void attack(Unit &target) override
    {
        int targetPrevHealth = target.getHealth();
        battleLog.push_back(name + " attacks " + target.getName() + " for " + to_string(currentAttack) + " damage!");
        target.takeDamage(currentAttack);
        battleLog.push_back(target.getName() + "'s HP: " + to_string(targetPrevHealth) + " -> " +
                            to_string(target.getHealth()) + "/" + to_string(target.getMaxHealth()));
    }

    void displayStatus() const override
    {
        cout << "\033[1;31m"; // Red color for boss
        Unit::displayStatus();
        cout << "\033[0m";
    }
};

// ========== Game Functions ==========
bool coinFlip()
{
    cout << "\n=== COIN FLIP TO DETERMINE TURN ORDER ===\n";
    cout << "Choose: 1. Heads  2. Tails\n";

    int choice = getValidInput(1, 2, "Your choice: ");
    bool coinResult = (rand() % 2 == 0);
    string coinName = coinResult ? "Heads" : "Tails";

    cout << "\nFlipping coin...\n";
    this_thread::sleep_for(chrono::seconds(1));
    cout << "Result: " << coinName << "!\n";

    bool playerWon = (choice == 1 && coinResult) || (choice == 2 && !coinResult);

    if (playerWon)
    {
        cout << "\033[1;32mYou won the coin flip! You go first!\033[0m\n";
    }
    else
    {
        cout << "\033[1;31mYou lost the coin flip! Enemy goes first!\033[0m\n";
    }

    this_thread::sleep_for(chrono::seconds(2));
    return playerWon;
}

void showSkillsMenu(Unit *player, Unit *target)
{
    cout << "\n=== SKILLS MENU ===\n";
    cout << "1. " << player->getSkill1Name() << " (Cost: " << player->getSkill1Cost() << " MP)\n";
    cout << "2. " << player->getSkill2Name() << " (Cost: " << player->getSkill2Cost() << " MP)\n";
    cout << "3. " << player->getSkill3Name() << " (Cost: " << player->getSkill3Cost() << " MP)\n";

    int skillChoice = getValidInput(1, 3, "Choose skill (1-3): ");

    bool skillUsed = false;
    switch (skillChoice)
    {
    case 1:
        skillUsed = player->useSkill1(*target);
        break;
    case 2:
        skillUsed = player->useSkill2(*target);
        break;
    case 3:
        skillUsed = player->useSkill3(*target);
        break;
    }

    if (!skillUsed)
    {
        cout << "Using basic attack instead.\n";
        player->attack(*target);
    }
}

vector<Item *> generateRandomItems()
{
    vector<Item *> allItems = {
        new FireSword(),
        new IceShield(),
        new VampireRing(),
        new PoisonDagger(),
        new DragonScale(),
        new LightningOrb()};

    vector<Item *> selectedItems;
    vector<int> usedIndices;

    // Select 3 random items
    for (int i = 0; i < 3; i++)
    {
        int randomIndex;
        do
        {
            randomIndex = rand() % allItems.size();
        } while (find(usedIndices.begin(), usedIndices.end(), randomIndex) != usedIndices.end());

        usedIndices.push_back(randomIndex);
        selectedItems.push_back(allItems[randomIndex]);
    }

    // Clean up unused items
    for (int i = 0; i < allItems.size(); i++)
    {
        if (find(usedIndices.begin(), usedIndices.end(), i) == usedIndices.end())
        {
            delete allItems[i];
        }
    }

    return selectedItems;
}

vector<Item *> generateRandomPotions()
{
    vector<Item *> potions;
    int numPotions = rand() % 3 + 1; // 1-3 potions

    for (int i = 0; i < numPotions; i++)
    {
        if (rand() % 2 == 0)
        {
            potions.push_back(new HealthPotion());
        }
        else
        {
            potions.push_back(new ManaPotion());
        }
    }

    return potions;
}

Unit *createPlayer(const string &playerName)
{
    cout << "\nSelect a class for " << playerName << ":\n"
         << "1. Warrior (High HP, Medium MP, Physical skills)\n"
         << "2. Archer (Medium HP, Poison/Bleed skills)\n"
         << "3. Mage (Low HP, High MP, Magic skills)\n";

    int choice = getValidInput(1, 3, "Choose (1-3): ");

    switch (choice)
    {
    case 1:
        return new Warrior(playerName);
    case 2:
        return new Archer(playerName);
    case 3:
        return new Mage(playerName);
    default:
        return new Warrior(playerName);
    }
}

void displayBattleHeader(int stage, Unit *player, Unit *boss)
{
    cout << "========================================\n";
    cout << "| STAGE " << stage << " BATTLE";
    cout << string(30 - to_string(stage).length(), ' ') << "|\n";
    cout << "========================================\n";

    cout << "\n=== YOUR STATUS ===\n";
    player->displayStatus();

    cout << "\n=== ENEMY STATUS ===\n";
    boss->displayStatus();
    cout << "\n";
}

void gameLoop(Unit *player)
{
    const vector<string> bossNames = {
        "Goblin King", "Shadow Knight", "Crimson Wraith",
        "Lich Queen", "Doom Reaper"};

    const vector<string> bossSkill1 = {
        "Goblin Smash", "Shadow Blade", "Crimson Slash",
        "Necroflame", "Void Strike"};

    const vector<string> bossSkill2 = {
        "Poison Cloud", "Dark Mist", "Blood Curse",
        "Soul Drain", "Void Corruption"};

    const vector<string> bossSkill3 = {
        "Stunning Roar", "Shadow Bind", "Crimson Howl",
        "Necrotic Heal", "Void Stasis"};

    for (int stage = 1; stage <= 5; ++stage)
    {
        // Create boss
        int bossIndex = stage - 1;
        BossUnit *boss = new BossUnit(
            bossNames[bossIndex],
            bossSkill1[bossIndex],
            bossSkill2[bossIndex],
            bossSkill3[bossIndex],
            10 + stage * 5,
            70 + stage * 20,
            stage - 1);

        cout << "\n--- ENEMY APPEARED ---\n";
        boss->displayStatus();

        // Coin flip for turn order
        bool playerFirst = coinFlip();

        // Combat loop
        while (player->isAlive() && boss->isAlive())
        {
            clearScreen();
            displayBattleHeader(stage, player, boss);

            // Display combined battle log
            player->displayBattleLog();
            player->clearBattleLog();
            boss->clearBattleLog();

            if (playerFirst)
            {
                // Player turn
                if (player->hasStatus(StatusEffect::STUN))
                {
                    player->addToBattleLog("\033[1;35mYou are stunned and skip your turn!\033[0m");
                    player->clearStatus(StatusEffect::STUN);
                }
                else
                {
                    player->addToBattleLog("\n=== YOUR TURN ===");
                    cout << "\n=== YOUR TURN ===\n";
                    cout << "1. Attack\n2. Use Skills\n3. Inventory\n4. Pass\n";

                    int action = getValidInput(1, 4, "Choose action: ");
                    bool usedPotion = false;

                    switch (action)
                    {
                    case 1:
                        player->attack(*boss);
                        break;
                    case 2:
                        showSkillsMenu(player, boss);
                        break;
                    case 3:
                        if (player->showInventory(usedPotion))
                        {
                            player->processStatusEffects();
                            this_thread::sleep_for(chrono::seconds(1));
                            continue;
                        }
                        break;
                    case 4:
                        player->addToBattleLog("You pass your turn.");
                        cout << "You pass your turn.\n";
                        break;
                    }
                }

                player->processStatusEffects();
                if (!boss->isAlive())
                    break;

                // Boss turn
                player->addToBattleLog("\n=== " + boss->getName() + "'s TURN ===");
                cout << "\n=== " << boss->getName() << "'s TURN ===\n";
                this_thread::sleep_for(chrono::milliseconds(800));

                if (boss->hasStatus(StatusEffect::STUN))
                {
                    player->addToBattleLog("\033[1;35m" + boss->getName() + " is stunned and skips turn!\033[0m");
                    cout << "\033[1;35m" << boss->getName() << " is stunned and skips turn!\033[0m\n";
                    boss->clearStatus(StatusEffect::STUN);
                }
                else
                {
                    int bossAction = rand() % 4;
                    string actionDesc = "";

                    if (bossAction == 3)
                    {
                        actionDesc = boss->getName() + " chooses to attack!";
                        player->addToBattleLog(actionDesc);
                        cout << actionDesc << endl;
                        boss->attack(*player);
                    }
                    else
                    {
                        switch (bossAction)
                        {
                        case 0:
                            actionDesc = boss->getName() + " uses " + boss->getSkill1Name() + "!";
                            player->addToBattleLog(actionDesc);
                            cout << actionDesc << endl;
                            boss->useSkill1(*player);
                            break;
                        case 1:
                            actionDesc = boss->getName() + " uses " + boss->getSkill2Name() + "!";
                            player->addToBattleLog(actionDesc);
                            cout << actionDesc << endl;
                            boss->useSkill2(*player);
                            break;
                        case 2:
                            actionDesc = boss->getName() + " uses " + boss->getSkill3Name() + "!";
                            player->addToBattleLog(actionDesc);
                            cout << actionDesc << endl;
                            boss->useSkill3(*player);
                            break;
                        }
                    }
                }

                boss->processStatusEffects();
            }
            else // Boss goes first
            {
                // Boss turn
                player->addToBattleLog("\n=== " + boss->getName() + "'s TURN ===");
                cout << "\n=== " << boss->getName() << "'s TURN ===\n";
                this_thread::sleep_for(chrono::milliseconds(800));

                if (boss->hasStatus(StatusEffect::STUN))
                {
                    player->addToBattleLog("\033[1;35m" + boss->getName() + " is stunned and skips turn!\033[0m");
                    cout << "\033[1;35m" << boss->getName() << " is stunned and skips turn!\033[0m\n";
                    boss->clearStatus(StatusEffect::STUN);
                }
                else
                {
                    int bossAction = rand() % 4;
                    string actionDesc = "";

                    if (bossAction == 3)
                    {
                        actionDesc = boss->getName() + " chooses to attack!";
                        player->addToBattleLog(actionDesc);
                        cout << actionDesc << endl;
                        boss->attack(*player);
                    }
                    else
                    {
                        switch (bossAction)
                        {
                        case 0:
                            actionDesc = boss->getName() + " uses " + boss->getSkill1Name() + "!";
                            player->addToBattleLog(actionDesc);
                            cout << actionDesc << endl;
                            boss->useSkill1(*player);
                            break;
                        case 1:
                            actionDesc = boss->getName() + " uses " + boss->getSkill2Name() + "!";
                            player->addToBattleLog(actionDesc);
                            cout << actionDesc << endl;
                            boss->useSkill2(*player);
                            break;
                        case 2:
                            actionDesc = boss->getName() + " uses " + boss->getSkill3Name() + "!";
                            player->addToBattleLog(actionDesc);
                            cout << actionDesc << endl;
                            boss->useSkill3(*player);
                            break;
                        }
                    }
                }

                boss->processStatusEffects();

                // Immediately update display after boss action
                clearScreen();
                displayBattleHeader(stage, player, boss);
                player->displayBattleLog();
                player->clearBattleLog();
                boss->clearBattleLog();

                if (!player->isAlive())
                {
                    clearScreen();
                    displayBattleHeader(stage, player, boss);
                    cout << "\n\033[1;31mYou were defeated in stage " << stage << "!\033[0m\n";
                    delete boss;
                    return;
                }

                // Player turn
                if (player->hasStatus(StatusEffect::STUN))
                {
                    player->addToBattleLog("\033[1;35mYou are stunned and skip your turn!\033[0m");
                    cout << "\033[1;35mYou are stunned and skip your turn!\033[0m\n";
                    player->clearStatus(StatusEffect::STUN);
                }
                else
                {
                    player->addToBattleLog("\n=== YOUR TURN ===");
                    cout << "\n=== YOUR TURN ===\n";
                    cout << "1. Attack\n2. Use Skills\n3. Inventory\n4. Pass\n";

                    int action = getValidInput(1, 4, "Choose action: ");
                    bool usedPotion = false;

                    switch (action)
                    {
                    case 1:
                        player->attack(*boss);
                        break;
                    case 2:
                        showSkillsMenu(player, boss);
                        break;
                    case 3:
                        if (player->showInventory(usedPotion))
                        {
                            player->processStatusEffects();
                            this_thread::sleep_for(chrono::seconds(1));
                            continue;
                        }
                        break;
                    case 4:
                        player->addToBattleLog("You pass your turn.");
                        cout << "You pass your turn.\n";
                        break;
                    }
                }

                player->processStatusEffects();
            }

            this_thread::sleep_for(chrono::seconds(1));
        }

        if (!player->isAlive())
        {
            clearScreen();
            displayBattleHeader(stage, player, boss);
            cout << "\n\033[1;31mYou were defeated in stage " << stage << "!\033[0m\n";
            delete boss;
            break;
        }

        // Boss defeated
        clearScreen();
        displayBattleHeader(stage, player, boss);
        cout << "\n\033[1;32mYou defeated " << boss->getName() << "!\033[0m\n";

        // Drop random potions
        vector<Item *> droppedPotions = generateRandomPotions();
        for (Item *potion : droppedPotions)
        {
            player->addItem(potion);
        }

        delete boss;

        if (stage < 5)
        {
            // Stage upgrade system
            cout << "\n=== STAGE COMPLETE! CHOOSE UPGRADE ===\n";
            cout << "1. Heal (+30 HP)\n";
            cout << "2. Restore Mana (+20 MP)\n";
            cout << "3. Increase Attack (+5 ATK)\n";
            cout << "4. Increase Defense (+3 DEF)\n";

            int upgrade = getValidInput(1, 4, "Choose (1-4): ");

            switch (upgrade)
            {
            case 1:
                player->heal(30);
                break;
            case 2:
                player->restoreMana(20);
                break;
            case 3:
                player->increaseAttack(5);
                break;
            case 4:
                player->increaseDefense(3);
                break;
            }

            // Item selection system
            cout << "\n=== ITEM SELECTION ===\n";
            cout << "Choose 1 item from the following 3 options:\n";

            vector<Item *> itemChoices = generateRandomItems();

            for (int i = 0; i < itemChoices.size(); i++)
            {
                cout << (i + 1) << ". ";
                itemChoices[i]->displayInfo();
            }

            int itemChoice = getValidInput(1, 3, "Choose item (1-3): ");

            player->addItem(itemChoices[itemChoice - 1]);

            // Clean up other items
            for (int i = 0; i < itemChoices.size(); i++)
            {
                if (i != itemChoice - 1)
                    delete itemChoices[i];
            }

            this_thread::sleep_for(chrono::seconds(2));
        }
    }

    if (player->isAlive())
    {
        cout << "\n\033[1;32m=== CONGRATULATIONS! ===\033[0m\n";
        cout << "\033[1;33mYou defeated all bosses and conquered the dungeon!\033[0m\n";
        cout << "\n=== FINAL STATUS ===\n";
        player->displayStatus();
    }

    delete player;
}

// ========== UI Functions ==========
void printLogo()
{
    cout << "\033[1;35m";
    cout << R"(
__        __  _    _  _____
\ \      / //| |  | |/ ____|
 \ \ /\ / /  | |__| || |  __
  \ V  V /   |  __  || | |_ |
   \_/\_/    |_|  |_| \_____|
             |_|  |_| \_____|
)" << '\n';
    cout << "\033[0m";
}

void showIntro()
{
    clearScreen();
    animateText("\nInitializing Wave HG Universe...\n", 30);
    this_thread::sleep_for(chrono::milliseconds(800));
    clearScreen();
    for (int i = 0; i < 3; i++)
    {
        cout << "\n\n\n\n";
        this_thread::sleep_for(chrono::milliseconds(200));
        clearScreen();
    }
    printLogo();
    animateText("\nText base game\n\n", 40);
    this_thread::sleep_for(chrono::seconds(2));
}

void showMainMenu()
{
    clearScreen();
    printLogo();
    animateText("\nWelcome to Wave Hunger Game Turn Based RPG!\n\n");
    cout << "1. Start New Game\n2. How to Play\n3. Exit\n";
}

void showHowToPlay()
{
    clearScreen();
    printLogo();
    animateText("WELCOME TO Wave Hunger Game Turn Based RPG!\n\n");

    animateText("HOW TO PLAY: \n", 10);
    animateText("- Choose your class: Warrior, Archer, or Mage\n", 10);
    animateText("- Battle against bosses in turn-based combat\n", 10);
    animateText("- Use normal attacks or powerful skills (consume MP)\n", 10);
    animateText("- Use inventory items to recover HP or MP\n", 10);
    animateText("- Win by defeating all 5 bosses!\n\n", 10);

    animateText("KEY SYSTEMS:\n"), 10;
    animateText("- Coin flip determines turn order each battle\n", 10);
    animateText("- Upgrade system: Choose buffs after each win (Heal, MP, ATK, DEF)\n", 10);
    animateText("- Loot system: Pick 1 of 3 random powerful items\n", 10);
    animateText("- Status effects like Poison, Bleed, Stun add strategy\n\n", 10);

    animateText("GOOD LUCK, HERO!\n");

    animateText("Press Enter to return to main menu...");
    cin.ignore();
    cin.get();
}

int main()
{
    srand(time(0));
    showIntro();

    while (true)
    {
        showMainMenu();
        int choice = getValidInput(1, 3, "Choose (1-3): ");

        switch (choice)
        {
        case 1:
        {
            clearScreen();
            printLogo();
            string playerName;
            animateText("\nEnter your hero's name: ");
            getline(cin, playerName);

            Unit *player = createPlayer(playerName);
            clearScreen();
            printLogo();
            animateText("\nPreparing for battle...\n");
            this_thread::sleep_for(chrono::seconds(1));

            gameLoop(player);

            animateText("\nPress Enter to return to main menu...");
            cin.ignore();
            cin.get();
            break;
        }
        case 2:
            showHowToPlay();
            break;
        case 3:
            clearScreen();
            printLogo();
            animateText("\nThanks for playing Wave Hunger Game Turn Based RPG!\n");
            animateText("May your adventures continue...\n");
            return 0;
        }
    }
}