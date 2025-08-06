#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <functional>
#include <array>
#include <string>
#include "Scene.h"
#include "Config.h"
#include "GlitchRenderer.h"

enum class SkillType {
    TECH = 0,
    INTELLECT,
    BIOMOD,
    SOCIAL,
    PHYSICAL,
    COMBAT,
    COUNT
};

struct SkillButton {
    sf::RectangleShape shape;
    std::unique_ptr<sf::Text> text;
    std::function<void()> onClick;
    bool isHovered = false;

    SkillButton(const sf::Font& font, const std::string& buttonText, sf::Vector2f position, sf::Vector2f size) {
        shape.setSize(size);
        shape.setPosition(position);
        shape.setFillColor(sf::Color(50, 50, 50, 180));
        shape.setOutlineColor(sf::Color(139, 0, 0));
        shape.setOutlineThickness(2.0f);

        text = std::make_unique<sf::Text>(font, buttonText);
        text->setFillColor(sf::Color(139, 0, 0));
        text->setCharacterSize(20);

        // Центрируем текст в кнопке
        sf::FloatRect textBounds = text->getLocalBounds();
        text->setPosition(sf::Vector2f(
            position.x + (size.x - textBounds.size.x) / 2.0f - textBounds.position.x,
            position.y + (size.y - textBounds.size.y) / 2.0f - textBounds.position.y
        ));
    }

    bool contains(sf::Vector2f point) const {
        return shape.getGlobalBounds().contains(point);
    }

    void setHovered(bool hovered) {
        isHovered = hovered;
        if (hovered) {
            shape.setFillColor(sf::Color(80, 80, 80, 200));
            shape.setOutlineColor(sf::Color(200, 0, 0));
        }
        else {
            shape.setFillColor(sf::Color(50, 50, 50, 180));
            shape.setOutlineColor(sf::Color(139, 0, 0));
        }
    }

    void render(sf::RenderWindow& window) {
        window.draw(shape);
        if (text) window.draw(*text);
    }
};

struct SkillLine {
    std::unique_ptr<sf::Text> nameText;
    std::unique_ptr<sf::Text> valueText;
    std::unique_ptr<SkillButton> plusButton;
    std::unique_ptr<SkillButton> minusButton;
    int currentValue = 0;
    SkillType type;

    SkillLine(const sf::Font& font, const std::string& skillName, SkillType skillType,
        sf::Vector2f position, float scale = 1.0f) : type(skillType) {

        nameText = std::make_unique<sf::Text>(font, skillName);
        nameText->setFillColor(sf::Color(139, 0, 0));
        nameText->setCharacterSize(static_cast<unsigned int>(24 * scale));
        nameText->setPosition(position);

        valueText = std::make_unique<sf::Text>(font, "0");
        valueText->setFillColor(sf::Color(139, 0, 0));
        valueText->setCharacterSize(static_cast<unsigned int>(24 * scale));
        valueText->setPosition(sf::Vector2f(position.x + 300 * scale, position.y));

        sf::Vector2f buttonSize(30 * scale, 30 * scale);

        minusButton = std::make_unique<SkillButton>(
            font, "-",
            sf::Vector2f(position.x + 350 * scale, position.y - 3 * scale),
            buttonSize
        );

        plusButton = std::make_unique<SkillButton>(
            font, "+",
            sf::Vector2f(position.x + 390 * scale, position.y - 3 * scale),
            buttonSize
        );
    }

    void updateValue() {
        if (valueText) valueText->setString(std::to_string(currentValue));
    }

    void render(sf::RenderWindow& window) {
        if (nameText) window.draw(*nameText);
        if (valueText) window.draw(*valueText);
        if (minusButton) minusButton->render(window);
        if (plusButton) plusButton->render(window);
    }
};

class FreePoints : public Scene {
private:
    sf::Texture backgroundTexture;
    std::optional<sf::Sprite> backgroundSprite;
    sf::Font font;
    GameConfig& config;

    // UI Elements
    std::unique_ptr<sf::Text> titleText;
    std::unique_ptr<sf::Text> remainingPointsText;
    std::array<std::unique_ptr<SkillLine>, static_cast<size_t>(SkillType::COUNT)> skillLines;

    // Game State
    int remainingPoints = 20;
    std::array<int, static_cast<size_t>(SkillType::COUNT)> skillValues = { 0 };

    // Glitch and UI
    GlitchRenderer glitchRenderer;
    std::unique_ptr<Scene> nextScene;
    bool finished = false;

    // Helper methods
    void initializeUI();
    void setupSkillLine(SkillType type, const std::string& name, float yOffset);
    void updateButtonHover(sf::Vector2f mousePos);
    void handleButtonClick(sf::Vector2f mousePos);
    void updateRemainingPointsDisplay();
    void updatePositions(sf::RenderWindow& window);
    bool canAddPoint(SkillType type) const;
    bool canRemovePoint(SkillType type) const;
    void addPoint(SkillType type);
    void removePoint(SkillType type);

public:
    FreePoints(GameConfig& config);
    void update(float deltaTime, sf::RenderWindow& window) override;
    void render(sf::RenderWindow& window) override;
    void handleEvent(const sf::Event& event, sf::RenderWindow& window) override;
    bool isFinished() const override;
    std::unique_ptr<Scene> extractNextScene();
};

