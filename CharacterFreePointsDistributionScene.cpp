#include "MainMenuScene.h"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <memory>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <Scene.h>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics.hpp>
#include <CharacterCreationScenes.h>
#include <CharacterSpecializationScene.h>
#include <CharacterFreePointsDistributionScene.h>
#include <CharacterAppearance.h>
#include "GlitchRenderer.h"

namespace {
    // UI Constants
    constexpr float BASE_WIDTH = 1280.0f;
    constexpr float BASE_HEIGHT = 720.0f;
    constexpr unsigned int TITLE_SIZE = 48;
    constexpr unsigned int TEXT_SIZE = 24;
    const sf::Color MENU_COLOR(139, 0, 0);
    const sf::Color TEXT_COLOR(139, 0, 0);

    // Font paths to try
    const std::vector<std::string> FONT_PATHS = {
        "assets/fonts/digital-7 (italic).ttf",
        "arial.ttf",
        "../assets/fonts/arial.ttf",
        "C:\\Windows\\Fonts\\arial.ttf",
        "C:\\Windows\\Fonts\\calibri.ttf"
    };
}

FreePoints::FreePoints(GameConfig& config)
    : config(config) {
    std::srand(std::time(nullptr));

    // Load font first
    bool fontLoaded = false;
    for (const auto& path : FONT_PATHS) {
        if (font.openFromFile(path)) {
            fontLoaded = true;
            std::cout << "Font loaded successfully from: " << path << std::endl;
            break;
        }
    }

    if (!fontLoaded) {
        std::cerr << "Warning: Could not load any font file. Text may not display correctly." << std::endl;
    }

    // Initialize text objects after font is loaded (font first in SFML 3.1)
    titleText = std::make_unique<sf::Text>(font, "");
    remainingPointsText = std::make_unique<sf::Text>(font, "");

    // Load background
    if (!backgroundTexture.loadFromFile("assets/textures/menu.png")) {
        std::cerr << "Failed to load background image.\n";
    }
    else {
        backgroundSprite.emplace(backgroundTexture);
    }

    initializeUI();
}

void FreePoints::initializeUI() {
    // Title
    titleText->setString("DISTRIBUTE SKILL POINTS");
    titleText->setFillColor(MENU_COLOR);
    titleText->setCharacterSize(TITLE_SIZE);

    // Remaining points display
    remainingPointsText->setFillColor(TEXT_COLOR);
    remainingPointsText->setCharacterSize(TEXT_SIZE);
    updateRemainingPointsDisplay();

    // Setup skill lines
    setupSkillLine(SkillType::TECH, "Technology", 0);
    setupSkillLine(SkillType::INTELLECT, "Intellect", 1);
    setupSkillLine(SkillType::BIOMOD, "Bio-modifications", 2);
    setupSkillLine(SkillType::SOCIAL, "Social", 3);
    setupSkillLine(SkillType::PHYSICAL, "Physical", 4);
    setupSkillLine(SkillType::COMBAT, "Combat", 5);
}

void FreePoints::setupSkillLine(SkillType type, const std::string& name, float yOffset) {
    size_t index = static_cast<size_t>(type);
    sf::Vector2f basePosition(100.0f, 200.0f + yOffset * 60.0f);

    skillLines[index] = std::make_unique<SkillLine>(font, name, type, basePosition);

    // Setup button callbacks
    skillLines[index]->plusButton->onClick = [this, type]() {
        addPoint(type);
        };

    skillLines[index]->minusButton->onClick = [this, type]() {
        removePoint(type);
        };
}

void FreePoints::update(float deltaTime, sf::RenderWindow& window) {
    updatePositions(window);

    // Update hover states
    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos);
    updateButtonHover(mousePos);

    glitchRenderer.update(deltaTime);
}

void FreePoints::updateButtonHover(sf::Vector2f mousePos) {
    for (auto& skillLine : skillLines) {
        if (skillLine) {
            bool plusHovered = skillLine->plusButton->contains(mousePos);
            bool minusHovered = skillLine->minusButton->contains(mousePos);

            skillLine->plusButton->setHovered(plusHovered && canAddPoint(skillLine->type));
            skillLine->minusButton->setHovered(minusHovered && canRemovePoint(skillLine->type));
        }
    }
}

void FreePoints::render(sf::RenderWindow& window) {
    // Background
    if (backgroundSprite.has_value()) {
        auto windowSize = window.getSize();
        auto textureSize = backgroundTexture.getSize();

        float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
        float scaleY = static_cast<float>(windowSize.y) / textureSize.y;

        backgroundSprite->setScale(sf::Vector2f(scaleX, scaleY));
        window.draw(backgroundSprite.value());
    }

    // Glitch background effects
    glitchRenderer.renderBackground(window, backgroundTexture);

    // UI Elements
    glitchRenderer.renderGlitchText(window, *titleText, "NEUROCIPHER REBOOT");
    window.draw(*titleText);

    window.draw(*remainingPointsText);

    // Skill lines
    for (const auto& skillLine : skillLines) {
        if (skillLine) {
            skillLine->render(window);
        }
    }

    // Glitch effects
    glitchRenderer.renderGlitchLines(window, 15);
    glitchRenderer.setBackgroundDarkening(true, 0.5f);
    glitchRenderer.setAnalogGlitch(true);
    glitchRenderer.setCyberpunkSquares(true);
    glitchRenderer.renderCyberpunkSquares(window, 8);
}

void FreePoints::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    if (event.getIf<sf::Event::MouseButtonPressed>()) {
        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
        sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
        handleButtonClick(worldPos);
    }

    if (const auto* keyEvent = event.getIf<sf::Event::KeyPressed>()) {
        if (keyEvent->scancode == sf::Keyboard::Scancode::Enter) {
            if (remainingPoints == 0) {
                std::cout << "Character creation completed! Moving to next screen...\n";
                nextScene = std::make_unique<AppearanceScene>(config);
                finished = true;
            }
            else {
                std::cout << "Please distribute all points before continuing. Remaining: " << remainingPoints << "\n";
            }
        }
    }
}

void FreePoints::handleButtonClick(sf::Vector2f mousePos) {
    for (auto& skillLine : skillLines) {
        if (skillLine) {
            if (skillLine->plusButton->contains(mousePos) && canAddPoint(skillLine->type)) {
                skillLine->plusButton->onClick();
            }
            else if (skillLine->minusButton->contains(mousePos) && canRemovePoint(skillLine->type)) {
                skillLine->minusButton->onClick();
            }
        }
    }
}

bool FreePoints::canAddPoint(SkillType type) const {
    return remainingPoints > 0;
}

bool FreePoints::canRemovePoint(SkillType type) const {
    size_t index = static_cast<size_t>(type);
    return skillValues[index] > 0;
}

void FreePoints::addPoint(SkillType type) {
    if (canAddPoint(type)) {
        size_t index = static_cast<size_t>(type);
        skillValues[index]++;
        remainingPoints--;

        skillLines[index]->currentValue = skillValues[index];
        skillLines[index]->updateValue();
        updateRemainingPointsDisplay();

        std::cout << "Added point to skill " << static_cast<int>(type)
            << ". Remaining: " << remainingPoints << std::endl;
    }
}

void FreePoints::removePoint(SkillType type) {
    if (canRemovePoint(type)) {
        size_t index = static_cast<size_t>(type);
        skillValues[index]--;
        remainingPoints++;

        skillLines[index]->currentValue = skillValues[index];
        skillLines[index]->updateValue();
        updateRemainingPointsDisplay();

        std::cout << "Removed point from skill " << static_cast<int>(type)
            << ". Remaining: " << remainingPoints << std::endl;
    }
}

void FreePoints::updateRemainingPointsDisplay() {
    remainingPointsText->setString("Remaining Points: " + std::to_string(remainingPoints));

    // Change color based on remaining points
    if (remainingPoints == 0) {
        remainingPointsText->setFillColor(sf::Color::Green);
    }
    else if (remainingPoints < 5) {
        remainingPointsText->setFillColor(sf::Color::Yellow);
    }
    else {
        remainingPointsText->setFillColor(TEXT_COLOR);
    }
}

void FreePoints::updatePositions(sf::RenderWindow& window) {
    auto windowSize = window.getSize();

    float scaleX = static_cast<float>(windowSize.x) / BASE_WIDTH;
    float scaleY = static_cast<float>(windowSize.y) / BASE_HEIGHT;
    float scale = std::min(scaleX, scaleY);

    // Update title position
    titleText->setCharacterSize(static_cast<unsigned int>(TITLE_SIZE * scale));
    titleText->setPosition(sf::Vector2f(100.0f * scaleX, 50.0f * scaleY));

    // Update remaining points position
    remainingPointsText->setCharacterSize(static_cast<unsigned int>(TEXT_SIZE * scale));
    remainingPointsText->setPosition(sf::Vector2f(100.0f * scaleX, 120.0f * scaleY));

    // Update skill lines positions
    for (size_t i = 0; i < skillLines.size(); ++i) {
        if (skillLines[i]) {
            float yPos = (200.0f + i * 60.0f) * scaleY;
            sf::Vector2f basePos(100.0f * scaleX, yPos);

            // Update text positions and sizes
            skillLines[i]->nameText->setCharacterSize(static_cast<unsigned int>(TEXT_SIZE * scale));
            skillLines[i]->nameText->setPosition(basePos);

            skillLines[i]->valueText->setCharacterSize(static_cast<unsigned int>(TEXT_SIZE * scale));
            skillLines[i]->valueText->setPosition(sf::Vector2f(basePos.x + 300 * scale, basePos.y));

            // Update button positions and sizes
            sf::Vector2f buttonSize(30 * scale, 30 * scale);

            skillLines[i]->minusButton->shape.setSize(buttonSize);
            skillLines[i]->minusButton->shape.setPosition(sf::Vector2f(basePos.x + 350 * scale, basePos.y - 3 * scale));

            skillLines[i]->plusButton->shape.setSize(buttonSize);
            skillLines[i]->plusButton->shape.setPosition(sf::Vector2f(basePos.x + 390 * scale, basePos.y - 3 * scale));
            skillLines[i]->plusButton->text->setCharacterSize(static_cast<unsigned int>(20 * scale));
            skillLines[i]->minusButton->text->setCharacterSize(static_cast<unsigned int>(20 * scale));

            // Recenter button text
            auto centerButtonText = [](SkillButton& button, sf::Vector2f buttonPos, sf::Vector2f buttonSize) {
                if (button.text) {
                    sf::FloatRect textBounds = button.text->getLocalBounds();
                    button.text->setPosition(sf::Vector2f(
                        buttonPos.x + (buttonSize.x - textBounds.size.x) / 2.0f - textBounds.position.x,
                        buttonPos.y + (buttonSize.y - textBounds.size.y) / 2.0f - textBounds.position.y
                    ));
                }
                };

            centerButtonText(*skillLines[i]->minusButton, skillLines[i]->minusButton->shape.getPosition(), buttonSize);
            centerButtonText(*skillLines[i]->plusButton, skillLines[i]->plusButton->shape.getPosition(), buttonSize);
        }
    }
}

bool FreePoints::isFinished() const {
    return finished;
}

std::unique_ptr<Scene> FreePoints::extractNextScene() {
    return std::move(nextScene);
}

