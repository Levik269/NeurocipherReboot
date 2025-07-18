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
    titleText->setCharacterSize(76);
    titleText->setFillColor(sf::Color(139, 0, 0));
    

    startText = std::make_unique<sf::Text>(font);
    startText->setString("Start game");
    startText->setCharacterSize(24);
    startText->setFillColor(sf::Color(139, 0, 0));
    

    loadText = std::make_unique<sf::Text>(font);
    loadText->setString("Load game");
    loadText->setCharacterSize(24);
    loadText->setFillColor(sf::Color(139, 0, 0));
    

    optionsText = std::make_unique<sf::Text>(font);
    optionsText->setString("Options");
    optionsText->setCharacterSize(24);
    optionsText->setFillColor(sf::Color(139, 0, 0));
    

    exitText = std::make_unique<sf::Text>(font);
    exitText->setString("Exit");
    exitText->setCharacterSize(24);
    exitText->setFillColor(sf::Color(139, 0, 0));
    

    glitchTitleText = std::make_unique<sf::Text>(font);
    glitchTitleText->setString("NEUROCIPHER REBOOT");
    glitchTitleText->setCharacterSize(76);
    glitchTitleText->setFillColor(sf::Color(139, 0, 0));

    updatePositions();
    // Добавляем в меню
    menuItems.push_back(startText.get());
    menuItems.push_back(loadText.get());
    menuItems.push_back(optionsText.get());
    menuItems.push_back(exitText.get());
    menuItems.push_back(glitchTitleText.get());
}

void MainMenuScene::update(float deltaTime, sf::RenderWindow& window) {

    // Hover update
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
        titleText->setPosition({ 100.f + offsetX, 100.f + offsetY });
        glitchTitleText->setPosition({ 102.f - offsetX, 102.f - offsetY });
    }
    else {
        titleText->setPosition({ 100.f, 100.f });
    }

    // Глич линии при наведении
    glitchLines.clear();
    if (hoveredIndex != -1) {
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
    float baseX = config.width * 0.078f;  // ~100px для 1280px
    float baseY = config.height * 0.139f; // ~100px для 720px
    float menuSpacing = config.height * 0.069f; // ~50px для 720px

    titleText->setPosition(sf::Vector2f(baseX, baseY));
    startText->setPosition(sf::Vector2f(baseX, baseY + config.height * 0.278f)); // ~200px для 720px
    loadText->setPosition(sf::Vector2f(baseX, baseY + config.height * 0.347f)); // ~250px для 720px
    optionsText->setPosition(sf::Vector2f(baseX, baseY + config.height * 0.417f)); // ~300px для 720px
    exitText->setPosition(sf::Vector2f(baseX, baseY + config.height * 0.486f)); // ~350px для 720px
}

