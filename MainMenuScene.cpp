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
#include "SaveManager.h"

MainMenuScene::MainMenuScene(GameConfig& config) : config(config) {
    // Инициализация генератора случайных чисел
    std::srand(std::time(nullptr));

    // Загрузка шрифта в SFML 3.1
    bool fontLoaded = false;

    // Попробуем разные пути для шрифта
    std::vector<std::string> fontPaths = {
        "assets/fonts/digital-7 (italic).ttf",
        "arial.ttf",
        "../assets/fonts/arial.ttf",
        "C:\\Windows\\Fonts\\arial.ttf",  // Системный шрифт Windows
        "C:\\Windows\\Fonts\\calibri.ttf" // Альтернативный системный шрифт
    };

    for (const auto& path : fontPaths) {
        if (font.openFromFile(path)) {
            fontLoaded = true;
            std::cout << "Font loaded successfully from: " << path << std::endl;
            break;
        }
    }

    if (!backgroundTexture.loadFromFile("assets/textures/menu.png")) {
        std::cerr << "Failed to load background image.\n";
    }
    else {
        // Правильная инициализация для std::optional<sf::Sprite>
        backgroundSprite.emplace(backgroundTexture);

    }

    if (!fontLoaded) {
        std::cerr << "Warning: Could not load any font file. Text may not display correctly." << std::endl;
    }

    // Правильная инициализация sf::Text для SFML 3.1
    titleText = std::make_unique<sf::Text>(font);
    titleText->setString("NEUROCIPHER REBOOT");
    titleText->setFillColor(sf::Color(139, 0, 0));

    startText = std::make_unique<sf::Text>(font);
    startText->setString("Start game");
    startText->setFillColor(sf::Color(139, 0, 0));

    loadText = std::make_unique<sf::Text>(font);
    loadText->setString("Load game");
    loadText->setFillColor(sf::Color(139, 0, 0));

    optionsText = std::make_unique<sf::Text>(font);
    optionsText->setString("Options");
    optionsText->setFillColor(sf::Color(139, 0, 0));

    exitText = std::make_unique<sf::Text>(font);
    exitText->setString("Exit");
    exitText->setFillColor(sf::Color(139, 0, 0));
    
    // Добавляем в меню
    menuItems.push_back(startText.get());
    menuItems.push_back(loadText.get());
    menuItems.push_back(optionsText.get());
    menuItems.push_back(exitText.get());
    
}

void MainMenuScene::update(float deltaTime, sf::RenderWindow& window) {

    // Hover update
    updatePositions(window);
    hoveredIndex = -1;
    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos);
    for (size_t i = 0; i < menuItems.size(); ++i) {
        if (menuItems[i]->getGlobalBounds().contains(mousePos)) {
            hoveredIndex = static_cast<int>(i);
            break;
        }
    }

    glitchRenderer.update(deltaTime);
}

void MainMenuScene::render(sf::RenderWindow& window) {
    if (backgroundSprite.has_value()) {
        auto windowSize = window.getSize();
        auto textureSize = backgroundTexture.getSize();

        // Вычисляем масштаб для заполнения всего окна
        float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
        float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
        float scale = std::max(scaleX, scaleY);

        backgroundSprite->setScale(sf::Vector2f(scale, scale));

        float spriteWidth = static_cast<float>(textureSize.x) * scale;
        float spriteHeight = static_cast<float>(textureSize.y) * scale;
        float offsetX = (static_cast<float>(windowSize.x) - spriteWidth) / 2.f;
        float offsetY = (static_cast<float>(windowSize.y) - spriteHeight) / 2.f;

        backgroundSprite->setScale(sf::Vector2f(scaleX, scaleY));
        window.draw(backgroundSprite.value());
    }

    // Рендерим фон с глич-эффектом
    glitchRenderer.renderBackground(window, backgroundTexture);

    // Рендерим заголовок с глич-эффектом
    glitchRenderer.renderGlitchText(window, *titleText, "NEUROCIPHER REBOOT");
    window.draw(*titleText);

    // Остальные элементы меню...
    window.draw(*startText);
    window.draw(*loadText);
    window.draw(*optionsText);
    window.draw(*exitText);

    // Глич-линии при наведении
    if (hoveredIndex != -1 && hoveredIndex < menuItems.size()) {
        glitchRenderer.renderHoverGlitch(window, menuItems[hoveredIndex]->getGlobalBounds());
    }

    // Общие глич-линии на экране
    glitchRenderer.renderGlitchLines(window, 15);

    // Включение затемнения на 30%
    glitchRenderer.setBackgroundDarkening(true, 0.3f);

    // Включение аналогового глитча
    glitchRenderer.setAnalogGlitch(true);

    // Включение киберпанк квадратов
    glitchRenderer.setCyberpunkSquares(true);

    // В цикле рендеринга
    glitchRenderer.renderCyberpunkSquares(window, 8); // 8 квадратов
}

void MainMenuScene::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    if (event.getIf<sf::Event::MouseButtonPressed>()) {
        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
        sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);

        for (size_t i = 0; i < menuItems.size(); ++i) {
            if (menuItems[i]->getGlobalBounds().contains(worldPos)) {
                switch (i) {
                case 0:
                    std::cout << "Start Game clicked\n";
                    onStartGameClicked();
                    break;
                case 1:
                    std::cout << "Load Game clicked\n";
                    break;
                case 2:
                    std::cout << "Options clicked\n";
                    nextScene = std::make_unique<SettingsScene>(config);
                    finished = true;
                    break;
                case 3:
                    window.close(); // Выход
                    break;
                }
            }
        }
    }

    if (const auto* keyEvent = event.getIf<sf::Event::KeyPressed>()) {
        if (keyEvent->scancode == sf::Keyboard::Scancode::Enter) {
            std::cout << "Enter key pressed\n";
        }
    }
}

bool MainMenuScene::isFinished() const {
    return finished; // Главное меню не завершает себя само
}

std::unique_ptr<Scene> MainMenuScene::extractNextScene() {
    return std::move(nextScene);
}

void MainMenuScene::updatePositions() {
}

void MainMenuScene::updatePositions(sf::RenderWindow& window) {
    auto windowSize = window.getSize();

    // Базовые размеры для масштабирования
    float baseWidth = 1280.0f;
    float baseHeight = 720.0f;

    // Вычисляем масштаб на основе текущего размера окна
    float scaleX = static_cast<float>(windowSize.x) / baseWidth;
    float scaleY = static_cast<float>(windowSize.y) / baseHeight;
    float scale = std::min(scaleX, scaleY); // Используем минимальный масштаб для сохранения пропорций

    // Базовые позиции и размеры
    float baseX = 100.0f;
    float baseY = 100.0f;
    float baseTitleSize = 76.0f;
    float baseMenuSize = 24.0f;
    float baseMenuSpacing = 50.0f;

    // Применяем масштаб к размерам текста
    titleText->setCharacterSize(static_cast<unsigned int>(baseTitleSize * scale));
    startText->setCharacterSize(static_cast<unsigned int>(baseMenuSize * scale));
    loadText->setCharacterSize(static_cast<unsigned int>(baseMenuSize * scale));
    optionsText->setCharacterSize(static_cast<unsigned int>(baseMenuSize * scale));
    exitText->setCharacterSize(static_cast<unsigned int>(baseMenuSize * scale));
    

    // Применяем масштаб к позициям
    titleText->setPosition(sf::Vector2f(baseX * scaleX, baseY * scaleY));
    startText->setPosition(sf::Vector2f(baseX * scaleX, (baseY + 200.0f) * scaleY));
    loadText->setPosition(sf::Vector2f(baseX * scaleX, (baseY + 250.0f) * scaleY));
    optionsText->setPosition(sf::Vector2f(baseX * scaleX, (baseY + 300.0f) * scaleY));
    exitText->setPosition(sf::Vector2f(baseX * scaleX, (baseY + 350.0f) * scaleY));
    
}

void MainMenuScene::onStartGameClicked() {
    // Базовое имя сохранения
    std::string baseSaveName = "NewSave";

    // Генерируем уникальное имя, чтобы не перезаписывать существующие
    std::string uniqueSaveName = saveManager.generateUniqueSaveName(baseSaveName);

    PlayerData newPlayerData;
    newPlayerData.name = "Player"; // можно запросить позже у игрока
    newPlayerData.level = 1;
    newPlayerData.experience = 0;
    newPlayerData.health = 100;
    newPlayerData.maxHealth = 100;
    newPlayerData.strength = 10;
    newPlayerData.intelligence = 10;
    newPlayerData.agility = 10;
    newPlayerData.playtime = 0.f;

    if (!saveManager.createNewSave(uniqueSaveName, newPlayerData)) {
        std::cerr << "Error creating new save!" << std::endl;
        return;
    }

    std::cout << "New save created: " << uniqueSaveName << std::endl;

    // Загружаем сейв (опционально, можно использовать newPlayerData напрямую)
    PlayerData loadedData;
    if (!saveManager.loadSave(uniqueSaveName, loadedData)) {
        std::cerr << "Error to load save!" << std::endl;
        return;
    }

    // Переходим к следующей сцене — созданию персонажа или игре
    nextScene = std::make_unique<CharacterOrigin>(config);
    finished = true;
}

