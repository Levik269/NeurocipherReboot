#include "CharacterCreationScenes.h"
#include <iostream>
#include <sstream>
#include <algorithm>

// Base CharacterCreationScene implementation
CharacterCreationScene::CharacterCreationScene(GameConfig& config, CharacterStats& character)
    : config(config), character(character) {

    // Load font
    std::vector<std::string> fontPaths = {
        "assets/fonts/digital-7 (italic).ttf",
        "arial.ttf",
        "C:\\Windows\\Fonts\\arial.ttf",
        "C:\\Windows\\Fonts\\calibri.ttf"
    };

    bool fontLoaded = false;
    for (const auto& path : fontPaths) {
        if (font.openFromFile(path)) {
            fontLoaded = true;
            break;
        }
    }

    if (!fontLoaded) {
        std::cerr << "Warning: Could not load font for character creation." << std::endl;
    }
}

void CharacterCreationScene::update(float deltaTime, sf::RenderWindow& window) {
    glitchRenderer.update(deltaTime);
    updatePositions(window);

    // Update hover state
    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos);

    selectedIndex = -1;
    for (size_t i = 0; i < buttons.size(); ++i) {
        if (buttons[i].getGlobalBounds().contains(mousePos)) {
            selectedIndex = static_cast<int>(i);
            break;
        }
    }
}

void CharacterCreationScene::render(sf::RenderWindow& window) {
    // Render background with glitch effect
    glitchRenderer.renderGlitchLines(window, 8);

    // Render UI elements
    for (const auto& text : uiTexts) {
        window.draw(*text);
    }

    // Render buttons with hover effects
    for (size_t i = 0; i < buttons.size(); ++i) {
        bool isSelected = static_cast<int>(i) == selectedIndex;
        drawButton(window, buttons[i].getGlobalBounds(), buttonTexts[i], isSelected, isSelected);

        if (isSelected) {
            glitchRenderer.renderHoverGlitch(window, buttons[i].getGlobalBounds());
        }
    }
}

void CharacterCreationScene::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (selectedIndex >= 0 && selectedIndex < static_cast<int>(buttons.size())) {
            handleSelection(selectedIndex);
        }
    }

    if (const auto* keyEvent = event.getIf<sf::Event::KeyPressed>()) {
        if (keyEvent->scancode == sf::Keyboard::Scancode::Escape) {
            // Go back or exit
            finished = true;
        }
    }
}

void CharacterCreationScene::updatePositions(sf::RenderWindow& window) {
    auto windowSize = window.getSize();
    float scaleX = static_cast<float>(windowSize.x) / 1280.0f;
    float scaleY = static_cast<float>(windowSize.y) / 720.0f;

    // Update button positions and sizes
    for (size_t i = 0; i < buttons.size(); ++i) {
        buttons[i].setSize(sf::Vector2f(300.0f * scaleX, 50.0f * scaleY));
        buttons[i].setPosition(100.0f * scaleX, (200.0f + i * 70.0f) * scaleY);
    }
}

void CharacterCreationScene::drawButton(sf::RenderWindow& window, const sf::FloatRect& bounds,
    const std::string& text, bool isSelected, bool isHovered) {
    // Draw button background
    sf::RectangleShape buttonBg(sf::Vector2f(bounds.size.x, bounds.size.y));
    buttonBg.setPosition(sf::Vector2f(bounds.left, bounds.top));
    buttonBg.setFillColor(isSelected ? highlightButtonColor : backgroundButtonColor);
    buttonBg.setOutlineThickness(2.0f);
    buttonBg.setOutlineColor(isSelected ? highlightColor : normalColor);

    window.draw(buttonBg);

    // Draw button text
    sf::Text buttonText(font);
    buttonText.setString(text);
    buttonText.setFillColor(isSelected ? highlightColor : normalColor);
    buttonText.setCharacterSize(20);

    // Center text in button
    sf::FloatRect textBounds = buttonText.getGlobalBounds();
    buttonText.setPosition(
        bounds.left + (bounds.width - textBounds.width) / 2.0f,
        bounds.top + (bounds.height - textBounds.height) / 2.0f
    );
    window.draw(buttonText);
}

// OriginSelectionScene implementation
OriginSelectionScene::OriginSelectionScene(GameConfig& config, CharacterStats& character)
    : CharacterCreationScene(config, character) {

    origins = { Origin::Corporate, Origin::Street, Origin::Nomad, Origin::Academic };

    titleText = std::make_unique<sf::Text>(font);
    descriptionText = std::make_unique<sf::Text>(font);
    loreText = std::make_unique<sf::Text>(font);
    bonusesText = std::make_unique<sf::Text>(font);
    instructionText = std::make_unique<sf::Text>(font);

    // Setup button data
    buttonTexts = { "Corporate", "Street", "Nomad", "Academic" };
    buttons.resize(buttonTexts.size());
    for (auto& button : buttons) {
        button.setFillColor(backgroundButtonColor);
    }

    setupUI(sf::RenderWindow(sf::VideoMode(sf::Vector2u(1280, 720)), "temp"));
}

void OriginSelectionScene::setupUI(sf::RenderWindow& window) {
    auto windowSize = window.getSize();
    float scaleX = static_cast<float>(windowSize.x) / 1280.0f;
    float scaleY = static_cast<float>(windowSize.y) / 720.0f;

    // Title
    titleText->setString("Choose Your Origin");
    titleText->setFillColor(normalColor);
    titleText->setCharacterSize(static_cast<unsigned int>(48 * std::min(scaleX, scaleY)));
    titleText->setPosition(50.0f * scaleX, 50.0f * scaleY);

    // Description area
    descriptionText->setFillColor(normalColor);
    descriptionText->setCharacterSize(static_cast<unsigned int>(24 * std::min(scaleX, scaleY)));
    descriptionText->setPosition(500.0f * scaleX, 150.0f * scaleY);

    // Lore text
    loreText->setFillColor(sf::Color(180, 180, 180));
    loreText->setCharacterSize(static_cast<unsigned int>(18 * std::min(scaleX, scaleY)));
    loreText->setPosition(500.0f * scaleX, 200.0f * scaleY);

    // Bonuses text
    bonusesText->setFillColor(sf::Color(0, 255, 100));
    bonusesText->setCharacterSize(static_cast<unsigned int>(20 * std::min(scaleX, scaleY)));
    bonusesText->setPosition(500.0f * scaleX, 350.0f * scaleY);

    // Instructions
    instructionText->setString("Click to select your character's origin");
    instructionText->setFillColor(sf::Color(150, 150, 150));
    instructionText->setCharacterSize(static_cast<unsigned int>(16 * std::min(scaleX, scaleY)));
    instructionText->setPosition(50.0f * scaleX, 600.0f * scaleY);

    updateDescription(0); // Show first option by default
}

void OriginSelectionScene::handleSelection(int index) {
    if (index >= 0 && index < static_cast<int>(origins.size())) {
        character.origin = origins[index];
        character.applyOriginBonuses();

        // Move to background selection
        nextScene = std::make_unique<BackgroundSelectionScene>(config, character);
        finished = true;
    }
}

void OriginSelectionScene::updateDescription(int index) {
    if (index >= 0 && index < static_cast<int>(origins.size())) {
        OriginInfo info = CharacterStats::getOriginInfo(origins[index]);

        descriptionText->setString(info.name + ": " + info.description);
        loreText->setString(info.lore);

        std::string bonusText = "Bonuses:\n";
        for (const auto& bonus : info.bonuses) {
            bonusText += "• " + bonus + "\n";
        }
        bonusesText->setString(bonusText);
    }
}

// BackgroundSelectionScene implementation
BackgroundSelectionScene::BackgroundSelectionScene(GameConfig& config, CharacterStats& character)
    : CharacterCreationScene(config, character) {

}
