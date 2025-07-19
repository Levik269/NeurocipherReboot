#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <numeric>
#include <cmath>

// Origins and backgrounds
enum class Origin {
    Corporate, Street, Nomad, Academic
};

enum class Background {
    Hacker, Mercenary, Trader, Technician, StreetDoctor, Detective
};

// Skill system
struct SubSkill {
    std::string name;
    int level = 0;
    int costToIncrease() const { return level + 1; }
};

struct Skill {
    std::string name;
    std::vector<SubSkill> subskills;

    float getAverageLevel() const {
        if (subskills.empty()) return 0.0f;
        float total = 0;
        for (const auto& ss : subskills) total += ss.level;
        return total / subskills.size();
    }

    int getSkillLevel() const {
        return static_cast<int>(getAverageLevel() / 10);
    }

    int getXPForNextLevel() const {
        int currentLevel = getSkillLevel();
        return static_cast<int>(10 * std::pow(1.5, currentLevel));
    }
};

struct CharacterStats {
    Origin origin;
    Background background;

    std::unordered_map<std::string, Skill> skills;

    CharacterStats() {
        // Define skills and subskills
        skills["Tech"] = Skill{ "Tech", {
            {"Hacking"}, {"Repair"}, {"Programming"}, {"Drone Control"}, {"Assembly"}
        } };

        skills["Intellect"] = Skill{ "Intellect", {
            {"Analytics"}, {"Memory"}, {"Erudition"}, {"Mathematics"}, {"Linguistics"}, {"Chemistry/Physics"}
        } };

        skills["Physical"] = Skill{ "Physical", {
            {"Strength"}, {"Dexterity"}, {"Stamina"}, {"Reaction"}, {"Balance"}
        } };

        skills["Biomod"] = Skill{ "Biomod", {
            {"Compatibility"}, {"Adjustment"}, {"Self-Diagnosis"}, {"Implant Communication"}, {"Regeneration"}
        } };

        skills["Social"] = Skill{ "Social", {
            {"Persuasion"}, {"Deception"}, {"Trading"}, {"Leadership"}, {"Acting"}
        } };

        skills["Combat"] = Skill{ "Combat", {
            {"Shooting"}, {"Melee"}, {"Tactics"}, {"Weapon Mastery"}, {"Evasion"}, {"Unarmed"}
        } };
    }

    void applyOriginBonuses() {
        // Example modifiers, should be designed carefully
        switch (origin) {
        case Origin::Corporate:
            skills["Social"].subskills[0].level += 1; // Persuasion
            skills["Tech"].subskills[0].level += 1;   // Hacking
            break;
        case Origin::Street:
            skills["Combat"].subskills[4].level += 2; // Evasion
            break;
        case Origin::Nomad:
            skills["Physical"].subskills[2].level += 1; // Stamina
            skills["Combat"].subskills[1].level += 1;   // Melee
            break;
        case Origin::Academic:
            skills["Intellect"].subskills[1].level += 2; // Memory
            break;
        }
    }

    void applyBackgroundBonuses() {
        switch (background) {
        case Background::Hacker:
            skills["Tech"].subskills[0].level += 3; // Hacking
            break;
        case Background::Mercenary:
            skills["Combat"].subskills[0].level += 2; // Shooting
            skills["Physical"].subskills[0].level += 1; // Strength
            break;
        case Background::Trader:
            skills["Social"].subskills[2].level += 3; // Trading
            break;
        case Background::Technician:
            skills["Tech"].subskills[1].level += 2; // Repair
            break;
        case Background::StreetDoctor:
            skills["Biomod"].subskills[4].level += 2; // Regeneration
            skills["Tech"].subskills[1].level += 1;   // Repair
            break;
        case Background::Detective:
            skills["Intellect"].subskills[0].level += 2; // Analytics
            skills["Social"].subskills[0].level += 1;     // Persuasion
            break;
        }
    }
};
