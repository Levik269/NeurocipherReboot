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
#include "SettingsScene.h"
#include "CharacterCreationScenes.h"
#include <CharacterFreePointsDistributionScene.h>
#include <CharacterAppearance.h>
#include "SaveManager.h"
#include "SceneManager.h"

CharacterPart::CharacterPart() : sprite(texture) {}

bool CharacterPart::loadFromFile(const std::string& path) {
    if (texture.loadFromFile(path)) {
        sprite.setTexture(texture);
        isLoaded = true;
        std::cout << "Successfully loaded: " << path << std::endl;
        return true;
    }
    std::cerr << "Failed to load: " << path << std::endl;
    return false;
}

void CharacterPart::setPosition(sf::Vector2f pos) {
    sprite.setPosition(pos);
}

void CharacterPart::setScale(sf::Vector2f scale) {
    sprite.setScale(scale);
}

// ModularCharacterSpriteManager Implementation
ModularCharacterSpriteManager::ModularCharacterSpriteManager() 
    : fallbackSprite(fallbackTexture) {  // Инициализация в списке инициализации
    currentPartIndices.fill(0);
    createFallbackTexture();
}

void ModularCharacterSpriteManager::createFallbackTexture() {
    // Создаем простую текстуру-заглушку
    sf::Image fallbackImage({ 64, 64 }, sf::Color::Red);

    // Рисуем простой квадрат с границей
    for (unsigned int x = 0; x < 64; ++x) {
        for (unsigned int y = 0; y < 64; ++y) {
            if (x < 4 || x >= 60 || y < 4 || y >= 60) {
                fallbackImage.setPixel({ x, y }, sf::Color::White);
            }
            else {
                fallbackImage.setPixel({ x, y }, sf::Color(200, 100, 100));
            }
        }
    }

    if (fallbackTexture.loadFromImage(fallbackImage)) {
        fallbackSprite.setTexture(fallbackTexture);
        fallbackLoaded = true;
        std::cout << "Fallback texture created successfully" << std::endl;
    }
}

bool ModularCharacterSpriteManager::loadPartCategory(PartType partType, const std::string& folderName,
    const std::vector<std::string>& partNames) {
    size_t typeIndex = static_cast<size_t>(partType);
    characterParts[typeIndex].clear();

    std::cout << "Loading " << folderName << " parts..." << std::endl;

    for (const auto& partName : partNames) {
        CharacterPart part;

        // Попробуем разные варианты путей
        std::vector<std::string> possiblePaths = {
            "assets/character_parts/" + folderName + "/" + partName + ".png",
            "assets/characters/" + folderName + "/" + partName + ".png",
            "assets/sprites/character/" + folderName + "/" + partName + ".png",
            "character_parts/" + folderName + "/" + partName + ".png"
        };

        bool loaded = false;
        for (const auto& path : possiblePaths) {
            if (part.loadFromFile(path)) {
                loaded = true;
                break;
            }
        }

        if (loaded) {
            characterParts[typeIndex].push_back(std::move(part));
        }
        else {
            std::cerr << "Could not load any variant of: " << partName << std::endl;
        }
    }

    std::cout << "Loaded " << characterParts[typeIndex].size() << " " << folderName << " parts" << std::endl;
    return !characterParts[typeIndex].empty();
}

bool ModularCharacterSpriteManager::loadCharacterParts() {
    std::cout << "Starting to load character parts..." << std::endl;

    bool allLoaded = true;

    // Пробуем загрузить базовые части
    allLoaded &= loadPartCategory(PartType::Base, "base", {
        "male_light", "male_medium", "male_dark",
        "female_light", "female_medium", "female_dark",
        "other_light", "other_medium", "other_dark"
        });

    // Если базовые части не загрузились, попробуем упрощенные варианты
    if (characterParts[static_cast<size_t>(PartType::Base)].empty()) {
        std::cout << "Trying simplified base parts..." << std::endl;
        allLoaded &= loadPartCategory(PartType::Base, "base", {
            "male", "female", "body"
            });
    }

    allLoaded &= loadPartCategory(PartType::Hair, "hair", {
        "short_black", "short_brown", "short_blonde",
        "long_black", "long_brown", "long_blonde",
        "curly_black", "curly_brown", "curly_blonde"
        });

    // Упрощенные варианты для волос
    if (characterParts[static_cast<size_t>(PartType::Hair)].empty()) {
        allLoaded &= loadPartCategory(PartType::Hair, "hair", {
            "hair1", "hair2", "hair3"
            });
    }

    allLoaded &= loadPartCategory(PartType::Eyes, "eyes", {
        "blue", "green", "brown", "gray"
        });

    if (characterParts[static_cast<size_t>(PartType::Eyes)].empty()) {
        allLoaded &= loadPartCategory(PartType::Eyes, "eyes", {
            "eyes1", "eyes2"
            });
    }

    allLoaded &= loadPartCategory(PartType::Face, "face", {
        "round", "oval", "square"
        });

    if (characterParts[static_cast<size_t>(PartType::Face)].empty()) {
        allLoaded &= loadPartCategory(PartType::Face, "face", {
            "face1", "face2"
            });
    }

    partsLoaded = true; // Даже если не все загрузилось, можем показывать что есть
    updatePartPositions();

    std::cout << "Character parts loading completed. Success: " << (allLoaded ? "Yes" : "Partial") << std::endl;
    return allLoaded;
}

void ModularCharacterSpriteManager::setPartIndex(PartType partType, int index) {
    size_t typeIndex = static_cast<size_t>(partType);
    if (index >= 0 && index < static_cast<int>(characterParts[typeIndex].size())) {
        currentPartIndices[typeIndex] = index;
        updatePartPositions();
        std::cout << "Set part " << static_cast<int>(partType) << " to index " << index << std::endl;
    }
}

int ModularCharacterSpriteManager::getPartCount(PartType partType) const {
    size_t typeIndex = static_cast<size_t>(partType);
    return static_cast<int>(characterParts[typeIndex].size());
}

int ModularCharacterSpriteManager::getCurrentPartIndex(PartType partType) const {
    size_t typeIndex = static_cast<size_t>(partType);
    return currentPartIndices[typeIndex];
}

void ModularCharacterSpriteManager::updatePartPositions() {
    for (size_t typeIndex = 0; typeIndex < static_cast<size_t>(PartType::COUNT); ++typeIndex) {
        if (currentPartIndices[typeIndex] < static_cast<int>(characterParts[typeIndex].size())) {
            auto& part = characterParts[typeIndex][currentPartIndices[typeIndex]];
            part.setPosition(basePosition);
            part.setScale({ baseScale, baseScale });
        }
    }

    // Обновляем fallback sprite
    if (fallbackLoaded) {
        fallbackSprite.setPosition(basePosition);
        fallbackSprite.setScale({ baseScale, baseScale });
    }
}

void ModularCharacterSpriteManager::updateCharacterSprite(const CharacterAppearance& appearance) {
    if (!partsLoaded) {
        std::cout << "Parts not loaded, cannot update character sprite" << std::endl;
        return;
    }

    // Безопасное определение индексов с проверкой границ
    int baseIndex = std::min(appearance.gender * 3 + appearance.skinTone,
        getPartCount(PartType::Base) - 1);
    if (baseIndex >= 0) setPartIndex(PartType::Base, baseIndex);

    int hairIndex = std::min(appearance.hairType * 3 + appearance.hairColor,
        getPartCount(PartType::Hair) - 1);
    if (hairIndex >= 0) setPartIndex(PartType::Hair, hairIndex);

    int eyeIndex = std::min(0, getPartCount(PartType::Eyes) - 1);
    if (eyeIndex >= 0) setPartIndex(PartType::Eyes, eyeIndex);

    int faceIndex = std::min(appearance.faceType, getPartCount(PartType::Face) - 1);
    if (faceIndex >= 0) setPartIndex(PartType::Face, faceIndex);

    std::cout << "Updated character sprite with indices: "
        << baseIndex << ", " << hairIndex << ", " << eyeIndex << ", " << faceIndex << std::endl;
}

void ModularCharacterSpriteManager::render(sf::RenderWindow& window, sf::Vector2f position, float scale) {
    // Адаптивное позиционирование для разных разрешений
    auto windowSize = window.getSize();
    float scaleX = static_cast<float>(windowSize.x) / 1280.0f;
    float scaleY = static_cast<float>(windowSize.y) / 720.0f;
    float adaptiveScale = std::min(scaleX, scaleY) * scale;

    // Адаптивная позиция
    sf::Vector2f adaptivePosition = {
        position.x * scaleX,
        position.y * scaleY
    };

    basePosition = adaptivePosition;
    baseScale = adaptiveScale;
    updatePartPositions();

    bool anyPartRendered = false;

    // Рендерим части в правильном порядке
    std::array<PartType, 4> renderOrder = {
        PartType::Base,
        PartType::Face,
        PartType::Eyes,
        PartType::Hair
    };

    for (PartType partType : renderOrder) {
        size_t typeIndex = static_cast<size_t>(partType);
        int currentIndex = currentPartIndices[typeIndex];

        if (currentIndex >= 0 && currentIndex < static_cast<int>(characterParts[typeIndex].size())) {
            const auto& part = characterParts[typeIndex][currentIndex];
            if (part.isLoaded) {
                window.draw(part.sprite);
                anyPartRendered = true;
            }
        }
    }

    // Если ничего не отрендерилось, показываем fallback
    if (!anyPartRendered && fallbackLoaded) {
        std::cout << "No parts rendered, showing fallback at position: "
            << adaptivePosition.x << ", " << adaptivePosition.y << std::endl;
        window.draw(fallbackSprite);
    }
}

void ModularCharacterSpriteManager::randomizeAppearance() {
    if (!partsLoaded) return;

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    for (size_t i = 0; i < static_cast<size_t>(PartType::COUNT); ++i) {
        if (!characterParts[i].empty()) {
            int randomIndex = std::rand() % characterParts[i].size();
            currentPartIndices[i] = randomIndex;
        }
    }

    updatePartPositions();
    std::cout << "Randomized appearance" << std::endl;
}

CharacterAppearance ModularCharacterSpriteManager::getAppearanceFromParts() const {
    CharacterAppearance appearance;

    if (partsLoaded) {
        int baseIndex = getCurrentPartIndex(PartType::Base);
        if (baseIndex >= 0) {
            appearance.gender = baseIndex / 3;
            appearance.skinTone = baseIndex % 3;
        }

        int hairIndex = getCurrentPartIndex(PartType::Hair);
        if (hairIndex >= 0) {
            appearance.hairType = hairIndex / 3;
            appearance.hairColor = hairIndex % 3;
        }

        appearance.faceType = getCurrentPartIndex(PartType::Face);
    }

    return appearance;
}

bool ModularCharacterSpriteManager::arePartsLoaded() const {
    return partsLoaded;
}

void ModularCharacterSpriteManager::printDebugInfo() const {
    std::cout << "=== Character Parts Debug Info ===" << std::endl;
    std::cout << "Parts loaded: " << (partsLoaded ? "Yes" : "No") << std::endl;

    const char* partNames[] = { "Base", "Hair", "Eyes", "Face" };
    for (size_t i = 0; i < static_cast<size_t>(PartType::COUNT); ++i) {
        std::cout << partNames[i] << ": " << characterParts[i].size()
            << " parts, current index: " << currentPartIndices[i] << std::endl;
    }
    std::cout << "=================================" << std::endl;
}

// AppearanceButton Implementation (без изменений)
AppearanceButton::AppearanceButton(const sf::Font& font, const std::string& buttonText,
    sf::Vector2f position, sf::Vector2f size) {
    shape.setSize(size);
    shape.setPosition(position);
    shape.setFillColor(sf::Color(50, 50, 50, 180));
    shape.setOutlineColor(sf::Color(139, 0, 0));
    shape.setOutlineThickness(2.0f);

    text = std::make_unique<sf::Text>(font);
    text->setString(buttonText);
    text->setFillColor(sf::Color(139, 0, 0));
    text->setCharacterSize(20);

    sf::FloatRect textBounds = text->getGlobalBounds();
    text->setPosition({
        position.x + (size.x - textBounds.size.x) / 2.0f,
        position.y + (size.y - textBounds.size.y) / 2.0f
        });
}

bool AppearanceButton::contains(sf::Vector2f point) const {
    return shape.getGlobalBounds().contains(point);
}

void AppearanceButton::setHovered(bool hovered) {
    isHovered = hovered;
    if (hovered) {
        shape.setFillColor(sf::Color(80, 80, 80, 200));
        shape.setOutlineColor(sf::Color(200, 50, 50));
    }
    else {
        shape.setFillColor(sf::Color(50, 50, 50, 180));
        shape.setOutlineColor(sf::Color(139, 0, 0));
    }
}

void AppearanceButton::handleClick() {
    if (onClick) {
        onClick();
    }
}

void AppearanceButton::render(sf::RenderWindow& window) {
    window.draw(shape);
    if (text) {
        window.draw(*text);
    }
}

// AppearanceConfigLine Implementation (адаптивный)
AppearanceConfigLine::AppearanceConfigLine(const sf::Font& font, AppearanceType appearanceType,
    const std::string& name, const std::vector<std::string>& availableOptions,
    sf::Vector2f position, float scale)
    : type(appearanceType), options(availableOptions) {

    nameText = std::make_unique<sf::Text>(font);
    nameText->setString(name);
    nameText->setFillColor(sf::Color(139, 0, 0));
    nameText->setCharacterSize(static_cast<unsigned int>(24 * scale));
    nameText->setPosition(position);

    valueText = std::make_unique<sf::Text>(font);
    valueText->setFillColor(sf::Color(200, 200, 200));
    valueText->setCharacterSize(static_cast<unsigned int>(24 * scale));
    valueText->setPosition({ position.x + 250 * scale, position.y });

    sf::Vector2f buttonSize(35 * scale, 30 * scale);

    prevButton = std::make_unique<AppearanceButton>(
        font, "<",
        sf::Vector2f(position.x + 400 * scale, position.y),
        buttonSize
    );

    nextButton = std::make_unique<AppearanceButton>(
        font, ">",
        sf::Vector2f(position.x + 440 * scale, position.y),
        buttonSize
    );

    prevButton->setOnClick([this]() { previousOption(); });
    nextButton->setOnClick([this]() { nextOption(); });

    updateValueDisplay();
}

void AppearanceConfigLine::setValue(int value) {
    if (value >= 0 && value < static_cast<int>(options.size())) {
        currentIndex = value;
        updateValueDisplay();
        if (onValueChanged) {
            onValueChanged(currentIndex);
        }
    }
}

void AppearanceConfigLine::updateValueDisplay() {
    if (!options.empty() && currentIndex >= 0 && currentIndex < static_cast<int>(options.size())) {
        if (valueText) {
            valueText->setString(options[currentIndex]);
        }
    }
}

void AppearanceConfigLine::previousOption() {
    if (currentIndex > 0) {
        currentIndex--;
    }
    else {
        currentIndex = static_cast<int>(options.size()) - 1;
    }
    updateValueDisplay();
    if (onValueChanged) {
        onValueChanged(currentIndex);
    }
}

void AppearanceConfigLine::nextOption() {
    if (currentIndex < static_cast<int>(options.size()) - 1) {
        currentIndex++;
    }
    else {
        currentIndex = 0;
    }
    updateValueDisplay();
    if (onValueChanged) {
        onValueChanged(currentIndex);
    }
}

void AppearanceConfigLine::updateHover(sf::Vector2f mousePos) {
    if (prevButton) prevButton->setHovered(prevButton->contains(mousePos));
    if (nextButton) nextButton->setHovered(nextButton->contains(mousePos));
}

void AppearanceConfigLine::handleClick(sf::Vector2f mousePos) {
    if (prevButton && prevButton->contains(mousePos)) {
        prevButton->handleClick();
    }
    if (nextButton && nextButton->contains(mousePos)) {
        nextButton->handleClick();
    }
}

void AppearanceConfigLine::render(sf::RenderWindow& window) {
    if (nameText) window.draw(*nameText);
    if (valueText) window.draw(*valueText);
    if (prevButton) prevButton->render(window);
    if (nextButton) nextButton->render(window);
}

void AppearanceConfigLine::updatePositions(sf::Vector2f position, float scale) {
    if (nameText) {
        nameText->setPosition(position);
        nameText->setCharacterSize(static_cast<unsigned int>(24 * scale));
    }

    if (valueText) {
        valueText->setPosition({ position.x + 250 * scale, position.y });
        valueText->setCharacterSize(static_cast<unsigned int>(24 * scale));
    }
}

// Исправленная AppearanceScene
AppearanceScene::AppearanceScene(GameConfig& config) : config(config) {
    std::cout << "Initializing AppearanceScene..." << std::endl;

    loadResources();
    initializeAppearanceConfigs();
    initializeUI();
    setupConfigLines();

    // КРИТИЧЕСКИ ВАЖНО: Инициализируем модульный менеджер спрайтов
    if (!modularSpriteManager.loadCharacterParts()) {
        std::cerr << "Warning: Failed to load some character parts" << std::endl;
    }

    modularSpriteManager.printDebugInfo();

    characterData.randomize();
    updateCharacterDisplay();

    std::cout << "AppearanceScene initialized successfully" << std::endl;
}

void AppearanceScene::loadResources() {
    std::vector<std::string> fontPaths = {
        "assets/fonts/digital-7 (italic).ttf",
        "assets/fonts/arial.ttf",
        "C:\\Windows\\Fonts\\arial.ttf"
    };

    bool fontLoaded = false;
    for (const auto& path : fontPaths) {
        if (font.openFromFile(path)) {
            fontLoaded = true;
            std::cout << "Font loaded from: " << path << std::endl;
            break;
        }
    }

    if (!fontLoaded) {
        std::cerr << "Warning: Could not load font file." << std::endl;
    }

    if (!backgroundTexture.loadFromFile("assets/textures/menu.png")) {
        std::cerr << "Failed to load background texture." << std::endl;
    }
    else {
        backgroundSprite = sf::Sprite(backgroundTexture);
    }
}

void AppearanceScene::initializeAppearanceConfigs() {
    appearanceConfigs = {
        {"Gender", {"Male", "Female", "Other"}},
        {"Hair Style", {"Short", "Long", "Curly"}},
        {"Hair Color", {"Black", "Brown", "Blonde"}},
        {"Skin Tone", {"Light", "Medium", "Dark"}},
        {"Face Type", {"Round", "Oval", "Square"}},
        {"Body Type", {"Slim", "Athletic", "Heavy"}}
    };
}

void AppearanceScene::initializeUI() {
    titleText = std::make_unique<sf::Text>(font);
    titleText->setString("CHARACTER APPEARANCE");
    titleText->setFillColor(sf::Color(139, 0, 0));
    titleText->setCharacterSize(48);

    randomizeButton = std::make_unique<AppearanceButton>(
        font, "RANDOMIZE",
        sf::Vector2f(100, 500),
        sf::Vector2f(120, 40)
    );
    randomizeButton->setOnClick([this]() { randomizeAppearance(); });

    confirmButton = std::make_unique<AppearanceButton>(
        font, "CONFIRM",
        sf::Vector2f(250, 500),
        sf::Vector2f(120, 40)
    );
    confirmButton->setOnClick([this]() { confirmSelection(); });
}

void AppearanceScene::setupConfigLines() {
    configLines.clear();

    for (size_t i = 0; i < appearanceConfigs.size(); ++i) {
        auto line = std::make_unique<AppearanceConfigLine>(
            font,
            static_cast<AppearanceType>(i),
            appearanceConfigs[i].name,
            appearanceConfigs[i].options,
            sf::Vector2f(100, 150 + i * 50)
        );

        AppearanceType type = static_cast<AppearanceType>(i);
        line->setOnValueChanged([this, type](int value) {
            onAppearanceValueChanged(type, value);
            });

        configLines.push_back(std::move(line));
    }

    if (configLines.size() >= 6) {
        configLines[0]->setValue(characterData.gender);
        configLines[1]->setValue(characterData.hairType);
        configLines[2]->setValue(characterData.hairColor);
        configLines[3]->setValue(characterData.skinTone);
        configLines[4]->setValue(characterData.faceType);
        configLines[5]->setValue(characterData.bodyType);
    }
}

void AppearanceScene::update(float deltaTime, sf::RenderWindow& window) {
    updatePositions(window);

    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos);

    updateHoverStates(mousePos);
    glitchRenderer.update(deltaTime);
}

void AppearanceScene::updatePositions(sf::RenderWindow& window) {
    auto windowSize = window.getSize();
    float scaleX = static_cast<float>(windowSize.x) / 1280.0f;
    float scaleY = static_cast<float>(windowSize.y) / 720.0f;
    float scale = std::min(scaleX, scaleY);

    if (titleText) {
        titleText->setCharacterSize(static_cast<unsigned int>(48 * scale));
        titleText->setPosition({ 100 * scaleX, 50 * scaleY });
    }

    if (backgroundSprite.has_value()) {
        backgroundSprite->setScale({ scaleX, scaleY });
    }

    // Обновляем позиции элементов интерфейса адаптивно
    for (size_t i = 0; i < configLines.size(); ++i) {
        sf::Vector2f newPos(100 * scaleX, (150 + i * 50) * scaleY);
        configLines[i]->updatePositions(newPos, scale);
    }
}

void AppearanceScene::updateHoverStates(sf::Vector2f mousePos) {
    for (auto& line : configLines) {
        line->updateHover(mousePos);
    }

    if (randomizeButton) randomizeButton->setHovered(randomizeButton->contains(mousePos));
    if (confirmButton) confirmButton->setHovered(confirmButton->contains(mousePos));
}

void AppearanceScene::render(sf::RenderWindow& window) {
    if (backgroundSprite.has_value()) {
        window.draw(backgroundSprite.value());
    }
    glitchRenderer.renderBackground(window, backgroundTexture);

    if (titleText) {
        window.draw(*titleText);
    }

    for (auto& line : configLines) {
        line->render(window);
    }

    // ИСПОЛЬЗУЕМ modularSpriteManager вместо spriteManager
    modularSpriteManager.render(window, { 600, 200 }, 2.0f);

    if (randomizeButton) randomizeButton->render(window);
    if (confirmButton) confirmButton->render(window);

    glitchRenderer.renderGlitchLines(window, 10);
    glitchRenderer.setBackgroundDarkening(true, 0.2f);
}

void AppearanceScene::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>()) {
        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
        sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos);
        handleMouseClick(mousePos);
    }

    if (const auto* keyEvent = event.getIf<sf::Event::KeyPressed>()) {
        if (keyEvent->scancode == sf::Keyboard::Scancode::Enter) {
            confirmSelection();
        }
        if (keyEvent->scancode == sf::Keyboard::Scancode::R) {
            randomizeAppearance();
        }
        if (keyEvent->scancode == sf::Keyboard::Scancode::D) {
            // Отладочная информация по нажатию D
            modularSpriteManager.printDebugInfo();
        }
    }
}

void AppearanceScene::handleMouseClick(sf::Vector2f mousePos) {
    for (auto& line : configLines) {
        line->handleClick(mousePos);
    }

    if (randomizeButton && randomizeButton->contains(mousePos)) {
        randomizeButton->handleClick();
    }

    if (confirmButton && confirmButton->contains(mousePos)) {
        confirmButton->handleClick();
    }
}

void AppearanceScene::onAppearanceValueChanged(AppearanceType type, int value) {
    switch (type) {
    case AppearanceType::Gender:
        characterData.gender = value;
        break;
    case AppearanceType::HairType:
        characterData.hairType = value;
        break;
    case AppearanceType::HairColor:
        characterData.hairColor = value;
        break;
    case AppearanceType::SkinTone:
        characterData.skinTone = value;
        break;
    case AppearanceType::FaceType:
        characterData.faceType = value;
        break;
    case AppearanceType::BodyType:
        characterData.bodyType = value;
        break;
    default:
        break;
    }

    updateCharacterDisplay();
}

void AppearanceScene::randomizeAppearance() {
    characterData.randomize();

    if (configLines.size() >= 6) {
        configLines[0]->setValue(characterData.gender);
        configLines[1]->setValue(characterData.hairType);
        configLines[2]->setValue(characterData.hairColor);
        configLines[3]->setValue(characterData.skinTone);
        configLines[4]->setValue(characterData.faceType);
        configLines[5]->setValue(characterData.bodyType);
    }

    updateCharacterDisplay();
    std::cout << "Appearance randomized" << std::endl;
}

void AppearanceScene::confirmSelection() {
    std::cout << "Character appearance confirmed!" << std::endl;
    std::cout << "Gender: " << characterData.gender << std::endl;
    std::cout << "Hair: " << characterData.hairType << ", Color: " << characterData.hairColor << std::endl;
    std::cout << "Skin: " << characterData.skinTone << ", Face: " << characterData.faceType << std::endl;
    std::cout << "Body: " << characterData.bodyType << std::endl;

    finished = true;
}

void AppearanceScene::updateCharacterDisplay() {
    modularSpriteManager.updateCharacterSprite(characterData);
    std::cout << "Character display updated" << std::endl;
}

// Заглушки для совместимости с старым кодом
void CharacterSpriteManager::updateCharacterSprite(const CharacterAppearance& appearance) {
    std::cout << "Legacy CharacterSpriteManager called - consider using ModularCharacterSpriteManager" << std::endl;
}

void CharacterSpriteManager::render(sf::RenderWindow& window, sf::Vector2f position, float scale) {
    std::cout << "Legacy CharacterSpriteManager render called" << std::endl;
}

bool CharacterSpriteManager::loadTexture(const std::string& path) {
    return false;
}

std::string CharacterSpriteManager::generateSpritePath(const CharacterAppearance& appearance) {
    return "";
}