#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <random>
#include "Scene.h"
#include "Config.h"
#include "GlitchRenderer.h"
#include <functional>
#include <optional>
#include <array>
#include <map>       
#include <cstdlib>   
#include <ctime>  

struct GameConfig;
class GlitchRenderer;
class Scene; // ДОБАВЛЕНО: базовый класс сцен

// ДОБАВЛЕНО: Полное определение CharacterAppearance
struct CharacterAppearance {
    int gender = 0;
    int hairType = 0;
    int hairColor = 0;
    int skinTone = 0;
    int faceType = 0;
    int bodyType = 0;

    void randomize() {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
        gender = std::rand() % 3;
        hairType = std::rand() % 3;
        hairColor = std::rand() % 3;
        skinTone = std::rand() % 3;
        faceType = std::rand() % 3;
        bodyType = std::rand() % 3;
    }
};

enum class AppearanceType {
    Gender = 0,
    HairType,
    HairColor,
    SkinTone,
    FaceType,
    BodyType,
    COUNT
};

enum class PartType {
    Base = 0,
    Hair,
    Eyes,
    Face,
    COUNT
};

// Структура для хранения конфигурации внешности
struct AppearanceConfig {
    std::string name;
    std::vector<std::string> options;
};

// Кнопка для интерфейса внешности
class AppearanceButton {
private:
    sf::RectangleShape shape;
    std::unique_ptr<sf::Text> text;
    bool isHovered = false;
    std::function<void()> onClick;

public:
    AppearanceButton(const sf::Font& font, const std::string& buttonText,
        sf::Vector2f position, sf::Vector2f size);

    bool contains(sf::Vector2f point) const;
    void setHovered(bool hovered);
    void handleClick();
    void render(sf::RenderWindow& window);

    void setOnClick(std::function<void()> callback) {
        onClick = callback;
    }
};

// Строка конфигурации внешности
class AppearanceConfigLine {
private:
    AppearanceType type;
    std::vector<std::string> options;
    int currentIndex = 0;

    std::unique_ptr<sf::Text> nameText;
    std::unique_ptr<sf::Text> valueText;
    std::unique_ptr<AppearanceButton> prevButton;
    std::unique_ptr<AppearanceButton> nextButton;

    std::function<void(int)> onValueChanged;

    void updateValueDisplay();
    void previousOption();
    void nextOption();

public:
    AppearanceConfigLine(const sf::Font& font, AppearanceType appearanceType,
        const std::string& name, const std::vector<std::string>& availableOptions,
        sf::Vector2f position, float scale = 1.0f);

    void setValue(int value);
    int getValue() const { return currentIndex; }

    void updateHover(sf::Vector2f mousePos);
    void handleClick(sf::Vector2f mousePos);
    void render(sf::RenderWindow& window);
    void updatePositions(sf::Vector2f position, float scale);

    void setOnValueChanged(std::function<void(int)> callback) {
        onValueChanged = callback;
    }
};

// Часть персонажа для модульной системы
struct CharacterPart {
    sf::Texture texture;
    sf::Sprite sprite;
    bool isLoaded = false;

    CharacterPart();
    bool loadFromFile(const std::string& path);
    void setPosition(sf::Vector2f pos);
    void setScale(sf::Vector2f scale);
};

// Модульный менеджер спрайтов персонажа
class ModularCharacterSpriteManager {
private:
    std::array<std::vector<CharacterPart>, static_cast<size_t>(PartType::COUNT)> characterParts;
    std::array<int, static_cast<size_t>(PartType::COUNT)> currentPartIndices;

    sf::Vector2f basePosition{ 0, 0 };
    float baseScale = 1.0f;
    bool partsLoaded = false;

    // Fallback текстуры для отладки
    sf::Texture fallbackTexture;
    sf::Sprite fallbackSprite;
    bool fallbackLoaded = false;

    bool loadPartCategory(PartType partType, const std::string& folderName,
        const std::vector<std::string>& partNames);
    void createFallbackTexture();

public:
    ModularCharacterSpriteManager();

    bool loadCharacterParts();
    void setPartIndex(PartType partType, int index);
    int getPartCount(PartType partType) const;
    int getCurrentPartIndex(PartType partType) const;
    void updatePartPositions();
    void updateCharacterSprite(const CharacterAppearance& appearance);
    void render(sf::RenderWindow& window, sf::Vector2f position, float scale = 1.0f);
    void randomizeAppearance();
    CharacterAppearance getAppearanceFromParts() const;
    bool arePartsLoaded() const;
    void printDebugInfo() const;
};

// Устаревший менеджер спрайтов (для совместимости)
class CharacterSpriteManager {
private:
    std::map<std::string, sf::Texture> textures; // ИСПРАВЛЕНО: теперь std::map доступен

public:
    bool loadTexture(const std::string& path);
    std::string generateSpritePath(const CharacterAppearance& appearance);
    void updateCharacterSprite(const CharacterAppearance& appearance);
    void render(sf::RenderWindow& window, sf::Vector2f position, float scale = 1.0f);
};

// Основная сцена внешности персонажа
class AppearanceScene : public Scene { // ИСПРАВЛЕНО: наследуется от Scene
private:
    GameConfig& config;

    // Ресурсы
    sf::Font font;
    sf::Texture backgroundTexture;
    std::optional<sf::Sprite> backgroundSprite;

    // UI элементы
    std::unique_ptr<sf::Text> titleText;
    std::unique_ptr<AppearanceButton> randomizeButton;
    std::unique_ptr<AppearanceButton> confirmButton;
    std::vector<std::unique_ptr<AppearanceConfigLine>> configLines;

    // Данные персонажа
    CharacterAppearance characterData;
    std::vector<AppearanceConfig> appearanceConfigs;

    // Менеджеры
    ModularCharacterSpriteManager modularSpriteManager;
    CharacterSpriteManager spriteManager; // Устаревший, для совместимости
    GlitchRenderer glitchRenderer;

    // Состояние сцены
    bool finished = false;
    std::unique_ptr<Scene> nextScene = nullptr; // ДОБАВЛЕНО: для следующей сцены

    // Методы инициализации
    void loadResources();
    void initializeAppearanceConfigs();
    void initializeUI();
    void setupConfigLines();

    // Методы обновления
    void updatePositions(sf::RenderWindow& window);
    void updateHoverStates(sf::Vector2f mousePos);
    void updateCharacterDisplay();

    // Обработчики событий
    void handleMouseClick(sf::Vector2f mousePos);
    void onAppearanceValueChanged(AppearanceType type, int value);
    void randomizeAppearance();
    void confirmSelection();

public:
    AppearanceScene(GameConfig& config);

    void update(float deltaTime, sf::RenderWindow& window) override; // ДОБАВЛЕНО: override
    void render(sf::RenderWindow& window) override; // ДОБАВЛЕНО: override
    void handleEvent(const sf::Event& event, sf::RenderWindow& window) override; // ДОБАВЛЕНО: override

    bool isFinished() const override { return finished; } // ДОБАВЛЕНО: override
    const CharacterAppearance& getCharacterData() const { return characterData; }

    // ДОБАВЛЕНО: метод для извлечения следующей сцены
    std::unique_ptr<Scene> extractNextScene() {
        return std::move(nextScene);
    }
};