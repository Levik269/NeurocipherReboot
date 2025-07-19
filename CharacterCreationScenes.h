// CharacterCreationScenes.h
#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <string>
#include "Scene.h"
#include "Config.h"
#include "GlitchRenderer.h"
#include "CharacterSystem.h"

// Base class for character creation stages
class CharacterCreationScene : public Scene {
protected:
    GameConfig& config;
    sf::Font font;
    GlitchRenderer glitchRenderer;
    CharacterStats& character;

    // UI elements
    std::vector<std::unique_ptr<sf::Text>> uiTexts;
    std::vector<sf::RectangleShape> buttons;
    std::vector<std::string> buttonTexts;

    // Navigation
    int selectedIndex = 0;
    bool finished = false;
    std::unique_ptr<Scene> nextScene;

    // Colors
    sf::Color normalColor = sf::Color(139, 0, 0);      // Dark red
    sf::Color highlightColor = sf::Color(255, 100, 100); // Bright red
    sf::Color backgroundButtonColor = sf::Color(20, 20, 20, 180);
    sf::Color highlightButtonColor = sf::Color(40, 0, 0, 200);

    virtual void setupUI(sf::RenderWindow& window) = 0;
    virtual void handleSelection(int index) = 0;

    void updatePositions(sf::RenderWindow& window);
    void drawButton(sf::RenderWindow& window, const sf::FloatRect& bounds,
        const std::string& text, bool isSelected, bool isHovered);

public:
    CharacterCreationScene(GameConfig& config, CharacterStats& character);

    void update(float deltaTime, sf::RenderWindow& window) override;
    void render(sf::RenderWindow& window) override;
    void handleEvent(const sf::Event& event, sf::RenderWindow& window) override;
    bool isFinished() const override { return finished; }
    std::unique_ptr<Scene> extractNextScene() { return std::move(nextScene); }
};

// Origin selection scene
class OriginSelectionScene : public CharacterCreationScene {
private:
    std::vector<Origin> origins;
    std::unique_ptr<sf::Text> titleText;
    std::unique_ptr<sf::Text> descriptionText;
    std::unique_ptr<sf::Text> loreText;
    std::unique_ptr<sf::Text> bonusesText;
    std::unique_ptr<sf::Text> instructionText;

    void setupUI(sf::RenderWindow& window) override;
    void handleSelection(int index) override;
    void updateDescription(int index);

public:
    OriginSelectionScene(GameConfig& config, CharacterStats& character);
};

// Background selection scene
class BackgroundSelectionScene : public CharacterCreationScene {
private:
    std::vector<Background> backgrounds;
    std::unique_ptr<sf::Text> titleText;
    std::unique_ptr<sf::Text> descriptionText;
    std::unique_ptr<sf::Text> loreText;
    std::unique_ptr<sf::Text> bonusesText;
    std::unique_ptr<sf::Text> instructionText;

    void setupUI(sf::RenderWindow& window) override;
    void handleSelection(int index) override;
    void updateDescription(int index);

public:
    BackgroundSelectionScene(GameConfig& config, CharacterStats& character);
};

// Skill distribution scene
class SkillDistributionScene : public CharacterCreationScene {
private:
    struct SkillButton {
        std::string skillName;
        std::string subskillName;
        sf::FloatRect bounds;
        int currentLevel;
        int cost;
        bool canAfford;
    };

    std::vector<SkillButton> skillButtons;
    std::unique_ptr<sf::Text> titleText;
    std::unique_ptr<sf::Text> pointsText;
    std::unique_ptr<sf::Text> instructionText;
    std::unique_ptr<sf::Text> skillInfoText;
    std::unique_ptr<sf::Text> finishButtonText;

    sf::RectangleShape finishButton;
    bool showFinishButton = false;
    int hoveredSkillIndex = -1;

    void setupUI(sf::RenderWindow& window) override;
    void handleSelection(int index) override;
    void updateSkillButtons();
    void updateSkillInfo(int index);
    void drawSkillTree(sf::RenderWindow& window, const std::string& skillName,
        const Skill& skill, sf::Vector2f position);

public:
    SkillDistributionScene(GameConfig& config, CharacterStats& character);
    void render(sf::RenderWindow& window) override;
    void handleEvent(const sf::Event& event, sf::RenderWindow& window) override;
};

// Character name input scene
class CharacterNameScene : public CharacterCreationScene {
private:
    std::string inputText = "";
    bool isInputActive = true;
    float cursorBlinkTime = 0.0f;
    bool showCursor = true;

    std::unique_ptr<sf::Text> titleText;
    std::unique_ptr<sf::Text> inputDisplayText;
    std::unique_ptr<sf::Text> instructionText;

    sf::RectangleShape inputBox;

    void setupUI(sf::RenderWindow& window) override;
    void handleSelection(int index) override;
    void updateInputDisplay();

public:
    CharacterNameScene(GameConfig& config, CharacterStats& character);
    void update(float deltaTime, sf::RenderWindow& window) override;
    void handleEvent(const sf::Event& event, sf::RenderWindow& window) override;
};

