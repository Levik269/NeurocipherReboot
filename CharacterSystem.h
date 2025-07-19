#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <numeric>
#include <cmath>
#include <functional>

// Origins and backgrounds with descriptions
enum class Origin {
    Corporate, Street, Nomad, Academic
};

enum class Background {
    Military, Criminal, Academic, Corporate, // Исходные значения из конструктора BackgroundSelectionScene
    Hacker, Mercenary, Trader, Technician, StreetDoctor, Detective // Значения из getBackgroundInfo
};

// Origin and Background info structures
struct OriginInfo {
    std::string name;
    std::string description;
    std::string lore;
    std::vector<std::string> bonuses;
};

struct BackgroundInfo {
    std::string name;
    std::string description;
    std::string lore;
    std::vector<std::string> bonuses;
};

// Skill system with more detailed progression
struct SubSkill {
    std::string name;
    std::string description;
    int level = 0;
    int maxLevel = 100;

    int costToIncrease() const {
        if (level >= maxLevel) return -1; // Cannot increase further
        return (level / 10) + 1; // Cost increases every 10 levels
    }

    float getEfficiency() const {
        return std::min(1.0f, level / static_cast<float>(maxLevel));
    }
};

struct Skill {
    std::string name;
    std::string description;
    std::unordered_map<std::string, int> subskills; // Изменено с vector<SubSkill> на map<string, int>

    float getAverageLevel() const {
        if (subskills.empty()) return 0.0f;
        float total = 0;
        for (const auto& [name, level] : subskills) total += level;
        return total / subskills.size();
    }

    int getSkillLevel() const {
        return static_cast<int>(getAverageLevel());
    }

    int getTotalInvestment() const {
        int total = 0;
        for (const auto& [name, level] : subskills) {
            total += level;
        }
        return total;
    }
};

class CharacterStats {
public:
    Origin origin = Origin::Corporate;
    Background background = Background::Hacker;
    std::string characterName = "";

    std::unordered_map<std::string, Skill> skills;

    // Character creation state
    int freeSkillPoints = 50; // Points to distribute during creation
    int totalSkillPoints = 0; // Total points ever earned

    CharacterStats() {
        initializeSkills();
    }

    void CharacterStats::initializeSkills() {
        // Tech skill tree
        skills["Tech"] = Skill{
            "Tech",
            "Technical expertise and digital manipulation",
            {
                {"Hacking", 0},
                {"Repair", 0},
                {"Programming", 0},
                {"Drone Control", 0},
                {"Assembly", 0}
            }
        };

        // Intellect skill tree  
        skills["Intellect"] = Skill{
            "Intellect",
            "Mental capacity and knowledge",
            {
                {"Analytics", 0},
                {"Memory", 0},
                {"Erudition", 0},
                {"Mathematics", 0},
                {"Linguistics", 0},
                {"Chemistry/Physics", 0}
            }
        };

        // Physical skill tree
        skills["Physical"] = Skill{
            "Physical",
            "Bodily capabilities and coordination",
            {
                {"Strength", 0},
                {"Dexterity", 0},
                {"Stamina", 0},
                {"Reaction", 0},
                {"Balance", 0}
            }
        };

        // Biomod skill tree
        skills["Biomod"] = Skill{
            "Biomod",
            "Biological modification and enhancement",
            {
                {"Compatibility", 0},
                {"Adjustment", 0},
                {"Self-Diagnosis", 0},
                {"Implant Communication", 0},
                {"Regeneration", 0}
            }
        };

        // Social skill tree
        skills["Social"] = Skill{
            "Social",
            "Interpersonal skills and influence",
            {
                {"Persuasion", 0},
                {"Deception", 0},
                {"Trading", 0},
                {"Leadership", 0},
                {"Acting", 0}
            }
        };

        // Combat skill tree
        skills["Combat"] = Skill{
            "Combat",
            "Fighting skills and tactical knowledge",
            {
                {"Shooting", 0},
                {"Melee", 0},
                {"Tactics", 0},
                {"Weapon Mastery", 0},
                {"Evasion", 0},
                {"Unarmed", 0}
            }
        };
    }
    

    void applyOriginBonuses() {
        switch (origin) {
        case Origin::Corporate:
            increaseSubskill("Social", "Persuasion", 2);
            increaseSubskill("Tech", "Hacking", 1);
            increaseSubskill("Intellect", "Analytics", 1);
            break;
        case Origin::Street:
            increaseSubskill("Combat", "Evasion", 3);
            increaseSubskill("Social", "Deception", 1);
            break;
        case Origin::Nomad:
            increaseSubskill("Physical", "Stamina", 2);
            increaseSubskill("Combat", "Melee", 1);
            increaseSubskill("Tech", "Repair", 1);
            break;
        case Origin::Academic:
            increaseSubskill("Intellect", "Memory", 2);
            increaseSubskill("Intellect", "Erudition", 2);
            break;
        }
    }

    void applyBackgroundBonuses() {
        switch (background) {
        case Background::Hacker:
            increaseSubskill("Tech", "Hacking", 4);
            increaseSubskill("Tech", "Programming", 2);
            break;
        case Background::Mercenary:
            increaseSubskill("Combat", "Shooting", 3);
            increaseSubskill("Physical", "Strength", 2);
            increaseSubskill("Combat", "Tactics", 1);
            break;
        case Background::Trader:
            increaseSubskill("Social", "Trading", 4);
            increaseSubskill("Social", "Persuasion", 2);
            break;
        case Background::Technician:
            increaseSubskill("Tech", "Repair", 3);
            increaseSubskill("Tech", "Assembly", 2);
            increaseSubskill("Physical", "Dexterity", 1);
            break;
        case Background::StreetDoctor:
            increaseSubskill("Biomod", "Regeneration", 3);
            increaseSubskill("Tech", "Repair", 2);
            increaseSubskill("Intellect", "Chemistry/Physics", 1);
            break;
        case Background::Detective:
            increaseSubskill("Intellect", "Analytics", 3);
            increaseSubskill("Social", "Persuasion", 2);
            increaseSubskill("Intellect", "Memory", 1);
            break;
        }
    }

    // Skill manipulation methods
    bool CharacterStats::increaseSubskill(const std::string& skillName, const std::string& subskillName, int amount = 1) {
        auto skillIt = skills.find(skillName);
        if (skillIt == skills.end()) return false;

        auto subskillIt = skillIt->second.subskills.find(subskillName);
        if (subskillIt != skillIt->second.subskills.end()) {
            subskillIt->second += amount;
            return true;
        }
        return false;
    }

    bool canAffordSubskillIncrease(const std::string& skillName, const std::string& subskillName) const {
        auto skillIt = skills.find(skillName);
        if (skillIt == skills.end()) return false;

        for (const auto& subskill : skillIt->second.subskills) {
            if (subskill.name == subskillName) {
                int cost = subskill.costToIncrease();
                return cost > 0 && freeSkillPoints >= cost;
            }
        }
        return false;
    }

    bool spendSkillPoint(const std::string& skillName, const std::string& subskillName) {
        auto skillIt = skills.find(skillName);
        if (skillIt == skills.end()) return false;

        for (auto& subskill : skillIt->second.subskills) {
            if (subskill.name == subskillName) {
                int cost = subskill.costToIncrease();
                if (cost > 0 && freeSkillPoints >= cost && subskill.level < subskill.maxLevel) {
                    subskill.level++;
                    freeSkillPoints -= cost;
                    return true;
                }
                break;
            }
        }
        return false;
    }

    // Get character power level (sum of all skill levels)
    int getPowerLevel() const {
        int total = 0;
        for (const auto& [name, skill] : skills) {
            total += skill.getTotalInvestment();
        }
        return total;
    }

    // Get character archetype based on highest skills
    std::string getArchetype() const {
        std::string dominantSkill = "";
        int highestLevel = 0;

        for (const auto& [name, skill] : skills) {
            int level = skill.getTotalInvestment();
            if (level > highestLevel) {
                highestLevel = level;
                dominantSkill = name;
            }
        }

        return dominantSkill.empty() ? "Novice" : dominantSkill + " Specialist";
    }

    // Static methods for getting info
    static OriginInfo getOriginInfo(Origin origin) {
        switch (origin) {
        case Origin::Corporate:
            return { "Corporate",
                   "Born into the corporate elite",
                   "You grew up in the gleaming towers of the megacorps, trained from birth in boardroom politics and digital warfare.",
                   {"+2 Persuasion", "+1 Hacking", "+1 Analytics"} };
        case Origin::Street:
            return { "Street",
                   "Forged in the urban wasteland",
                   "The streets raised you hard and fast. You learned to survive where others fall, moving like smoke through the shadows.",
                   {"+3 Evasion", "+1 Deception"} };
        case Origin::Nomad:
            return { "Nomad",
                   "Wanderer of the wasteland",
                   "You come from the tribes that roam the badlands beyond the cities, hardy survivors who value freedom above all.",
                   {"+2 Stamina", "+1 Melee", "+1 Repair"} };
        case Origin::Academic:
            return { "Academic",
                   "Scholar of the old world",
                   "Your mind was shaped in the halls of learning, absorbing knowledge that others have forgotten or abandoned.",
                   {"+2 Memory", "+2 Erudition"} };
        }
        return {};
    }

    static BackgroundInfo getBackgroundInfo(Background background) {
        switch (background) {
        case Background::Hacker:
            return { "Hacker",
                   "Digital ghost in the machine",
                   "You live in the spaces between the code, dancing through firewalls and bending data to your will.",
                   {"+4 Hacking", "+2 Programming"} };
        case Background::Mercenary:
            return { "Mercenary",
                   "Gun for hire",
                   "Violence is your profession, and you're very good at your job. Credits flow to those who can deliver results.",
                   {"+3 Shooting", "+2 Strength", "+1 Tactics"} };
        case Background::Trader:
            return { "Trader",
                   "Master of the deal",
                   "In a world of scarcity, you make things flow. Every conversation is a negotiation, every relationship a transaction.",
                   {"+4 Trading", "+2 Persuasion"} };
        case Background::Technician:
            return { "Technician",
                   "Keeper of the machines",
                   "When the world breaks down, you're the one who fixes it. Your hands know the language of metal and circuits.",
                   {"+3 Repair", "+2 Assembly", "+1 Dexterity"} };
        case Background::StreetDoctor:
            return { "Street Doctor",
                   "Healer in the shadows",
                   "No questions asked, cash up front. You patch up the desperate and the dangerous with skill learned in back alleys.",
                   {"+3 Regeneration", "+2 Repair", "+1 Chemistry/Physics"} };
        case Background::Detective:
            return { "Detective",
                   "Seeker of truth",
                   "In a world of lies and shadows, you find the truth. Every mystery is a puzzle waiting to be solved.",
                   {"+3 Analytics", "+2 Persuasion", "+1 Memory"} };
        }
        return {};
    }
};