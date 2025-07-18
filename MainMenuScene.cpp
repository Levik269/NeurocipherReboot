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

    glitchTitleText = std::make_unique<sf::Text>(font);
    glitchTitleText->setString("NEUROCIPHER REBOOT");
    glitchTitleText->setFillColor(sf::Color(139, 0, 0));
    
    // Добавляем в меню
    menuItems.push_back(startText.get());
    menuItems.push_back(loadText.get());
    menuItems.push_back(optionsText.get());
    menuItems.push_back(exitText.get());
    menuItems.push_back(glitchTitleText.get());
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

    // Глич эффект на заголовке
    glitchTimer += deltaTime;

    if (glitchTimer > 1.f) {
        glitchActive = rand() % 3 == 0; // иногда включается
        glitchTimer = 0.f;
    }

    if (glitchActive) {
        float offsetX = static_cast<float>((rand() % 5) - 2);
        float offsetY = static_cast<float>((rand() % 5) - 2);
        auto currentPos = titleText->getPosition();
        titleText->setPosition({ 100.f + offsetX, 100.f + offsetY });
        glitchTitleText->setPosition({ 102.f - offsetX, 102.f - offsetY });
    }
    else {
        titleText->setPosition({ 100.f, 100.f });
    }

    // Глич линии при наведении
    glitchLines.clear();
    if (hoveredIndex != -1 && hoveredIndex < menuItems.size()) {
        auto bounds = menuItems[hoveredIndex]->getGlobalBounds();
        float x = bounds.position.x;
        float y = bounds.position.y;
        float w = bounds.size.x;
        float h = bounds.size.y;
        sf::RectangleShape glitch(sf::Vector2f(w * (0.3f + 0.2f * (rand() % 3)), 2.f));
        glitch.setFillColor(sf::Color::Red);
        glitch.setPosition(sf::Vector2f(
            x + static_cast<float>(rand() % static_cast<int>(w / 2)),
            y + 5.f + static_cast<float>(rand() % static_cast<int>(h - 10.f))
        ));
        glitchLines.push_back(glitch);
    }
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

    if (glitchActive)
        window.draw(*glitchTitleText);

    window.draw(*titleText);
    window.draw(*startText);
    window.draw(*loadText);
    window.draw(*optionsText);
    window.draw(*exitText);

    for (const auto& line : glitchLines)
        window.draw(line);
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
    glitchTitleText->setCharacterSize(static_cast<unsigned int>(baseTitleSize * scale));

    // Применяем масштаб к позициям
    titleText->setPosition(sf::Vector2f(baseX * scaleX, baseY * scaleY));
    startText->setPosition(sf::Vector2f(baseX * scaleX, (baseY + 200.0f) * scaleY));
    loadText->setPosition(sf::Vector2f(baseX * scaleX, (baseY + 250.0f) * scaleY));
    optionsText->setPosition(sf::Vector2f(baseX * scaleX, (baseY + 300.0f) * scaleY));
    exitText->setPosition(sf::Vector2f(baseX * scaleX, (baseY + 350.0f) * scaleY));
    glitchTitleText->setPosition(sf::Vector2f((baseX + 2.0f) * scaleX, (baseY + 2.0f) * scaleY));
}