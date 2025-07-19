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
        if (font.loadFromFile(path)) {
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
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            if (selectedIndex >= 0 && selectedIndex < static_cast<int>(buttons.size())) {
                handleSelection(selectedIndex);
            }
        }
    }

    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
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
    sf::RectangleShape buttonBg(sf::Vector2f(bounds.width, bounds.height));
    buttonBg.setPosition(sf::Vector2f(bounds.left, bounds.top));
    buttonBg.setFillColor(isSelected ? highlightButtonColor : backgroundButtonColor);
    buttonBg.setOutlineThickness(2.0f);
    buttonBg.setOutlineColor(isSelected ? highlightColor : normalColor);

    window.draw(buttonBg);

    // Draw button text
    sf::Text buttonText;
    buttonText.setFont(font);
    buttonText.setString(text);
    buttonText.setFillColor(isSelected ? highlightColor : normalColor);
    buttonText.setCharacterSize(20);

    // Center text in button
    sf::FloatRect textBounds = buttonText.getLocalBounds();
    buttonText.setPosition(
        bounds.left + (bounds.width - textBounds.width) / 2.0f - textBounds.left,
        bounds.top + (bounds.height - textBounds.height) / 2.0f - textBounds.top
    );
    window.draw(buttonText);
}

// OriginSelectionScene implementation
OriginSelectionScene::OriginSelectionScene(GameConfig& config, CharacterStats& character)
    : CharacterCreationScene(config, character) {

    origins = { Origin::Corporate, Origin::Street, Origin::Nomad, Origin::Academic };

    titleText = std::make_unique<sf::Text>();
    descriptionText = std::make_unique<sf::Text>();
    loreText = std::make_unique<sf::Text>();
    bonusesText = std::make_unique<sf::Text>();
    instructionText = std::make_unique<sf::Text>();

    // Set font for all texts
    titleText->setFont(font);
    descriptionText->setFont(font);
    loreText->setFont(font);
    bonusesText->setFont(font);
    instructionText->setFont(font);

    // Setup button data
    buttonTexts = { "Corporate", "Street", "Nomad", "Academic" };
    buttons.resize(buttonTexts.size());
    for (auto& button : buttons) {
        button.setFillColor(backgroundButtonColor);
    }

    // Add texts to UI elements
    uiTexts.push_back(titleText.get());
    uiTexts.push_back(descriptionText.get());
    uiTexts.push_back(loreText.get());
    uiTexts.push_back(bonusesText.get());
    uiTexts.push_back(instructionText.get());

    setupUI(sf::RenderWindow(sf::VideoMode(1280, 720), "temp"));
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

void OriginSelectionScene::update(float deltaTime, sf::RenderWindow& window) {
    CharacterCreationScene::update(deltaTime, window);

    // Update description based on hovered item
    if (selectedIndex >= 0 && selectedIndex < static_cast<int>(origins.size())) {
        updateDescription(selectedIndex);
    }
}

// BackgroundSelectionScene implementation
BackgroundSelectionScene::BackgroundSelectionScene(GameConfig& config, CharacterStats& character)
    : CharacterCreationScene(config, character) {

    backgrounds = { Background::Military, Background::Criminal, Background::Academic, Background::Corporate };

    titleText = std::make_unique<sf::Text>();
    descriptionText = std::make_unique<sf::Text>();
    loreText = std::make_unique<sf::Text>();
    bonusesText = std::make_unique<sf::Text>();
    instructionText = std::make_unique<sf::Text>();

    // Set font for all texts
    titleText->setFont(font);
    descriptionText->setFont(font);
    loreText->setFont(font);
    bonusesText->setFont(font);
    instructionText->setFont(font);

    // Setup button data
    buttonTexts = { "Military", "Criminal", "Academic", "Corporate" };
    buttons.resize(buttonTexts.size());
    for (auto& button : buttons) {
        button.setFillColor(backgroundButtonColor);
    }

    // Add texts to UI elements
    uiTexts.push_back(titleText.get());
    uiTexts.push_back(descriptionText.get());
    uiTexts.push_back(loreText.get());
    uiTexts.push_back(bonusesText.get());
    uiTexts.push_back(instructionText.get());

    setupUI(sf::RenderWindow(sf::VideoMode(1280, 720), "temp"));
}

void BackgroundSelectionScene::setupUI(sf::RenderWindow& window) {
    auto windowSize = window.getSize();
    float scaleX = static_cast<float>(windowSize.x) / 1280.0f;
    float scaleY = static_cast<float>(windowSize.y) / 720.0f;

    // Title
    titleText->setString("Choose Your Background");
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
    instructionText->setString("Click to select your character's background");
    instructionText->setFillColor(sf::Color(150, 150, 150));
    instructionText->setCharacterSize(static_cast<unsigned int>(16 * std::min(scaleX, scaleY)));
    instructionText->setPosition(50.0f * scaleX, 600.0f * scaleY);

    updateDescription(0); // Show first option by default
}

void BackgroundSelectionScene::handleSelection(int index) {
    if (index >= 0 && index < static_cast<int>(backgrounds.size())) {
        character.background = backgrounds[index];
        character.applyBackgroundBonuses();

        // Move to skill distribution
        nextScene = std::make_unique<SkillDistributionScene>(config, character);
        finished = true;
    }
}

void BackgroundSelectionScene::updateDescription(int index) {
    if (index >= 0 && index < static_cast<int>(backgrounds.size())) {
        BackgroundInfo info = CharacterStats::getBackgroundInfo(backgrounds[index]);

        descriptionText->setString(info.name + ": " + info.description);
        loreText->setString(info.lore);

        std::string bonusText = "Bonuses:\n";
        for (const auto& bonus : info.bonuses) {
            bonusText += "• " + bonus + "\n";
        }
        bonusesText->setString(bonusText);
    }
}

void BackgroundSelectionScene::update(float deltaTime, sf::RenderWindow& window) {
    CharacterCreationScene::update(deltaTime, window);

    // Update description based on hovered item
    if (selectedIndex >= 0 && selectedIndex < static_cast<int>(backgrounds.size())) {
        updateDescription(selectedIndex);
    }
}

// SkillDistributionScene implementation
SkillDistributionScene::SkillDistributionScene(GameConfig& config, CharacterStats& character)
    : CharacterCreationScene(config, character) {

    titleText = std::make_unique<sf::Text>();
    pointsText = std::make_unique<sf::Text>();
    instructionText = std::make_unique<sf::Text>();
    skillInfoText = std::make_unique<sf::Text>();
    finishButtonText = std::make_unique<sf::Text>();

    // Set font for all texts
    titleText->setFont(font);
    pointsText->setFont(font);
    instructionText->setFont(font);
    skillInfoText->setFont(font);
    finishButtonText->setFont(font);

    // Add texts to UI elements
    uiTexts.push_back(titleText.get());
    uiTexts.push_back(pointsText.get());
    uiTexts.push_back(instructionText.get());
    uiTexts.push_back(skillInfoText.get());

    setupUI(sf::RenderWindow(sf::VideoMode(1280, 720), "temp"));
    updateSkillButtons();
}

void SkillDistributionScene::setupUI(sf::RenderWindow& window) {
    auto windowSize = window.getSize();
    float scaleX = static_cast<float>(windowSize.x) / 1280.0f;
    float scaleY = static_cast<float>(windowSize.y) / 720.0f;

    // Title
    titleText->setString("Distribute Skill Points");
    titleText->setFillColor(normalColor);
    titleText->setCharacterSize(static_cast<unsigned int>(48 * std::min(scaleX, scaleY)));
    titleText->setPosition(50.0f * scaleX, 50.0f * scaleY);

    // Points remaining
    pointsText->setFillColor(normalColor);
    pointsText->setCharacterSize(static_cast<unsigned int>(24 * std::min(scaleX, scaleY)));
    pointsText->setPosition(50.0f * scaleX, 100.0f * scaleY);

    // Instructions
    instructionText->setString("Click + to increase skills, - to decrease. Each level costs more points.");
    instructionText->setFillColor(sf::Color(150, 150, 150));
    instructionText->setCharacterSize(static_cast<unsigned int>(16 * std::min(scaleX, scaleY)));
    instructionText->setPosition(50.0f * scaleX, 650.0f * scaleY);

    // Skill info
    skillInfoText->setFillColor(sf::Color(200, 200, 200));
    skillInfoText->setCharacterSize(static_cast<unsigned int>(18 * std::min(scaleX, scaleY)));
    skillInfoText->setPosition(700.0f * scaleX, 150.0f * scaleY);

    // Finish button
    finishButton.setSize(sf::Vector2f(150.0f * scaleX, 40.0f * scaleY));
    finishButton.setPosition(1000.0f * scaleX, 650.0f * scaleY);
    finishButton.setFillColor(backgroundButtonColor);
    finishButton.setOutlineThickness(2.0f);
    finishButton.setOutlineColor(normalColor);

    finishButtonText->setString("Finish");
    finishButtonText->setFillColor(normalColor);
    finishButtonText->setCharacterSize(static_cast<unsigned int>(20 * std::min(scaleX, scaleY)));
}

void SkillDistributionScene::handleSelection(int index) {
    // This will be handled in the overridden handleEvent method
}

void SkillDistributionScene::updateSkillButtons() {
    pointsText->setString("Skill Points Remaining: " + std::to_string(character.skillPoints));

    // Update finish button visibility
    showFinishButton = (character.skillPoints == 0);

    if (showFinishButton) {
        finishButton.setFillColor(sf::Color(0, 100, 0, 180));
        finishButtonText->setFillColor(sf::Color::Green);
    }
    else {
        finishButton.setFillColor(backgroundButtonColor);
        finishButtonText->setFillColor(normalColor);
    }
}

void SkillDistributionScene::updateSkillInfo(int index) {
    // Update skill information display when hovering over skills
    if (index >= 0 && index < static_cast<int>(skillButtons.size())) {
        const auto& skillBtn = skillButtons[index];
        std::string info = skillBtn.skillName + " - " + skillBtn.subskillName + "\n";
        info += "Current Level: " + std::to_string(skillBtn.currentLevel) + "\n";
        info += "Next Level Cost: " + std::to_string(skillBtn.cost) + " points\n";
        info += skillBtn.canAfford ? "Can afford upgrade" : "Cannot afford upgrade";
        skillInfoText->setString(info);
    }
}

void SkillDistributionScene::render(sf::RenderWindow& window) {
    CharacterCreationScene::render(window);

    // Render skill trees
    auto windowSize = window.getSize();
    float scaleX = static_cast<float>(windowSize.x) / 1280.0f;
    float scaleY = static_cast<float>(windowSize.y) / 720.0f;

    sf::Vector2f startPos(50.0f * scaleX, 150.0f * scaleY);
    float yOffset = 0;

    for (const auto& [skillName, skill] : character.skills) {
        drawSkillTree(window, skillName, skill, sf::Vector2f(startPos.x, startPos.y + yOffset));
        yOffset += 120.0f * scaleY;
    }

    // Render finish button
    if (showFinishButton) {
        window.draw(finishButton);

        // Center text in button
        sf::FloatRect buttonBounds = finishButton.getGlobalBounds();
        sf::FloatRect textBounds = finishButtonText->getLocalBounds();
        finishButtonText->setPosition(
            buttonBounds.left + (buttonBounds.width - textBounds.width) / 2.0f - textBounds.left,
            buttonBounds.top + (buttonBounds.height - textBounds.height) / 2.0f - textBounds.top
        );
        window.draw(*finishButtonText);
    }
}

void SkillDistributionScene::drawSkillTree(sf::RenderWindow& window, const std::string& skillName,
    const Skill& skill, sf::Vector2f position) {

    auto windowSize = window.getSize();
    float scaleX = static_cast<float>(windowSize.x) / 1280.0f;
    float scaleY = static_cast<float>(windowSize.y) / 720.0f;

    // Draw skill name
    sf::Text skillNameText;
    skillNameText.setFont(font);
    skillNameText.setString(skillName);
    skillNameText.setFillColor(normalColor);
    skillNameText.setCharacterSize(static_cast<unsigned int>(22 * std::min(scaleX, scaleY)));
    skillNameText.setPosition(position);
    window.draw(skillNameText);

    // Draw subskills
    float yOffset = 30.0f * scaleY;
    for (const auto& [subskillName, level] : skill.subskills) {
        sf::Text subskillText;
        subskillText.setFont(font);
        subskillText.setString("  " + subskillName + ": " + std::to_string(level));
        subskillText.setFillColor(sf::Color(200, 200, 200));
        subskillText.setCharacterSize(static_cast<unsigned int>(18 * std::min(scaleX, scaleY)));
        subskillText.setPosition(position.x, position.y + yOffset);
        window.draw(subskillText);

        // Draw + and - buttons
        sf::RectangleShape plusButton(sf::Vector2f(20.0f * scaleX, 20.0f * scaleY));
        plusButton.setPosition(position.x + 300.0f * scaleX, position.y + yOffset);
        plusButton.setFillColor(sf::Color(0, 150, 0, 180));
        plusButton.setOutlineThickness(1.0f);
        plusButton.setOutlineColor(sf::Color::Green);
        window.draw(plusButton);

        sf::Text plusText;
        plusText.setFont(font);
        plusText.setString("+");
        plusText.setFillColor(sf::Color::White);
        plusText.setCharacterSize(static_cast<unsigned int>(14 * std::min(scaleX, scaleY)));
        sf::FloatRect plusBounds = plusButton.getGlobalBounds();
        sf::FloatRect plusTextBounds = plusText.getLocalBounds();
        plusText.setPosition(
            plusBounds.left + (plusBounds.width - plusTextBounds.width) / 2.0f - plusTextBounds.left,
            plusBounds.top + (plusBounds.height - plusTextBounds.height) / 2.0f - plusTextBounds.top
        );
        window.draw(plusText);

        if (level > 0) {
            sf::RectangleShape minusButton(sf::Vector2f(20.0f * scaleX, 20.0f * scaleY));
            minusButton.setPosition(position.x + 330.0f * scaleX, position.y + yOffset);
            minusButton.setFillColor(sf::Color(150, 0, 0, 180));
            minusButton.setOutlineThickness(1.0f);
            minusButton.setOutlineColor(sf::Color::Red);
            window.draw(minusButton);

            sf::Text minusText;
            minusText.setFont(font);
            minusText.setString("-");
            minusText.setFillColor(sf::Color::White);
            minusText.setCharacterSize(static_cast<unsigned int>(14 * std::min(scaleX, scaleY)));
            sf::FloatRect minusBounds = minusButton.getGlobalBounds();
            sf::FloatRect minusTextBounds = minusText.getLocalBounds();
            minusText.setPosition(
                minusBounds.left + (minusBounds.width - minusTextBounds.width) / 2.0f - minusTextBounds.left,
                minusBounds.top + (minusBounds.height - minusTextBounds.height) / 2.0f - minusTextBounds.top
            );
            window.draw(minusText);
        }

        yOffset += 25.0f * scaleY;
    }
}

void SkillDistributionScene::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));

            // Check finish button click
            if (showFinishButton && finishButton.getGlobalBounds().contains(mousePos)) {
                nextScene = std::make_unique<CharacterNameScene>(config, character);
                finished = true;
                return;
            }

            // Handle skill button clicks (implement skill point distribution logic here)
            // This would involve checking mouse position against skill +/- buttons
        }
    }

    CharacterCreationScene::handleEvent(event, window);
}

// CharacterNameScene implementation
CharacterNameScene::CharacterNameScene(GameConfig& config, CharacterStats& character)
    : CharacterCreationScene(config, character) {

    titleText = std::make_unique<sf::Text>();
    inputDisplayText = std::make_unique<sf::Text>();
    instructionText = std::make_unique<sf::Text>();

    // Set font for all texts
    titleText->setFont(font);
    inputDisplayText->setFont(font);
    instructionText->setFont(font);

    // Add texts to UI elements
    uiTexts.push_back(titleText.get());
    uiTexts.push_back(inputDisplayText.get());
    uiTexts.push_back(instructionText.get());

    setupUI(sf::RenderWindow(sf::VideoMode(1280, 720), "temp"));
}

void CharacterNameScene::setupUI(sf::RenderWindow& window) {
    auto windowSize = window.getSize();
    float scaleX = static_cast<float>(windowSize.x) / 1280.0f;
    float scaleY = static_cast<float>(windowSize.y) / 720.0f;

    // Title
    titleText->setString("Enter Character Name");
    titleText->setFillColor(normalColor);
    titleText->setCharacterSize(static_cast<unsigned int>(48 * std::min(scaleX, scaleY)));
    titleText->setPosition(50.0f * scaleX, 50.0f * scaleY);

    // Input box
    inputBox.setSize(sf::Vector2f(500.0f * scaleX, 50.0f * scaleY));
    inputBox.setPosition(400.0f * scaleX, 300.0f * scaleY);
    inputBox.setFillColor(backgroundButtonColor);
    inputBox.setOutlineThickness(2.0f);
    inputBox.setOutlineColor(normalColor);

    // Input text
    inputDisplayText->setFillColor(normalColor);
    inputDisplayText->setCharacterSize(static_cast<unsigned int>(24 * std::min(scaleX, scaleY)));
    inputDisplayText->setPosition(410.0f * scaleX, 315.0f * scaleY);

    // Instructions
    instructionText->setString("Type your character's name and press Enter");
    instructionText->setFillColor(sf::Color(150, 150, 150));
    instructionText->setCharacterSize(static_cast<unsigned int>(20 * std::min(scaleX, scaleY)));
    instructionText->setPosition(400.0f * scaleX, 250.0f * scaleY);

    updateInputDisplay();
}

void CharacterNameScene::handleSelection(int index) {
    // Not used in this scene
}

void CharacterNameScene::updateInputDisplay() {
    std::string displayText = inputText;
    if (showCursor && isInputActive) {
        displayText += "|";
    }
    inputDisplayText->setString(displayText);
}

void CharacterNameScene::update(float deltaTime, sf::RenderWindow& window) {
    CharacterCreationScene::update(deltaTime, window);

    // Update cursor blink
    cursorBlinkTime += deltaTime;
    if (cursorBlinkTime >= 0.5f) {
        showCursor = !showCursor;
        cursorBlinkTime = 0.0f;
        updateInputDisplay();
    }
}
