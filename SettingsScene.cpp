#include "SettingsScene.h"
#include "ConfigManager.h"
#include <string>
#include <stdexcept>
#include <iostream>  // для std::cerr
#include <cstdlib> 
SettingsScene::SettingsScene(GameConfig& configRef) : config(configRef) {
    if (!font.openFromFile("assets/fonts/digital-7 (italic).ttf")) {
        throw std::runtime_error("Failed to load font");
    }
    // Убираем updateTexts() из конструктора, так как у нас ещё нет окна
    // Найти текущее разрешение в списке
    for (size_t i = 0; i < AVAILABLE_RESOLUTIONS.size(); ++i) {
        if (AVAILABLE_RESOLUTIONS[i].width == config.width &&
            AVAILABLE_RESOLUTIONS[i].height == config.height) {
            currentResolutionIndex = static_cast<int>(i);
            break;
        }
    }
    // Загрузка фона (используем тот же что и в главном меню)
    if (backgroundTexture.loadFromFile("assets/textures/SettingsMenu.png")) {
        backgroundSprite.emplace(backgroundTexture);
    }
    else {
        std::cerr << "Failed to load background image for settings.\n";
    }
}

void SettingsScene::update(float dt, sf::RenderWindow& window) {
    glitchRenderer.update(dt);
    updateTexts(window);
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
}

void SettingsScene::render(sf::RenderWindow& window) {
    // Рендерим фон с глич-эффектом
    glitchRenderer.renderBackground(window, backgroundTexture);

    // Остальные элементы...
    for (const auto& text : options) {
        window.draw(*text);
    }
}

void SettingsScene::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    // Обработка мыши
    if (const auto* mouseClick = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (hoveredIndex != -1) {
            selectedIndex = hoveredIndex;

            if (selectedIndex < 3) {
                // Клик по настройкам - переключаем значение
                switch (selectedIndex) {
                case 0:
                    currentResolutionIndex = (currentResolutionIndex + 1) % static_cast<int>(AVAILABLE_RESOLUTIONS.size());
                    config.width = AVAILABLE_RESOLUTIONS[currentResolutionIndex].width;
                    config.height = AVAILABLE_RESOLUTIONS[currentResolutionIndex].height;
                    break;
                case 1:
                    config.fullscreen = !config.fullscreen;
                    break;
                case 2:
                    config.vsync = !config.vsync;
                    break;
                }
            }
            else if (selectedIndex == 3) {
                // Клик по "Save & Back"
                ConfigManager::save(config);
                finished = true;
            }
            updateTexts(window);
        }
    }

    // Синхронизация selectedIndex с hoveredIndex при движении мыши
    if (hoveredIndex != -1) {
        selectedIndex = hoveredIndex;
    }

    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        switch (key->scancode) {
        case sf::Keyboard::Scancode::Down:
            selectedIndex = (selectedIndex + 1) % 4;
            updateTexts(window);
            break;
        case sf::Keyboard::Scancode::Up:
            selectedIndex = (selectedIndex + 3) % 4;
            updateTexts(window);
            break;
        case sf::Keyboard::Scancode::Left:
        case sf::Keyboard::Scancode::Right:
            switch (selectedIndex) {
            case 0:
                if (key->scancode == sf::Keyboard::Scancode::Left) {
                    currentResolutionIndex = (currentResolutionIndex - 1 + static_cast<int>(AVAILABLE_RESOLUTIONS.size())) % static_cast<int>(AVAILABLE_RESOLUTIONS.size());
                }
                else if (key->scancode == sf::Keyboard::Scancode::Right) {
                    currentResolutionIndex = (currentResolutionIndex + 1) % static_cast<int>(AVAILABLE_RESOLUTIONS.size());
                }
                config.width = AVAILABLE_RESOLUTIONS[currentResolutionIndex].width;
                config.height = AVAILABLE_RESOLUTIONS[currentResolutionIndex].height;
                break;
            case 1:
                config.fullscreen = !config.fullscreen;
                break;
            case 2:
                config.vsync = !config.vsync;
                break;
            }
            updateTexts(window);
            break;
        case sf::Keyboard::Scancode::Enter:
            if (selectedIndex == 3) {
                ConfigManager::save(config);
                finished = true;
            }
            break;
        default: break;
        }
    }
}

bool SettingsScene::isFinished() const {
    return finished;
}

void SettingsScene::updateTexts(sf::RenderWindow& window) {
    auto windowSize = window.getSize();

    // Базовые размеры для масштабирования
    float baseWidth = 1280.0f;
    float baseHeight = 720.0f;
    float baseTextSize = 24.0f;
    float baseX = 100.0f;
    float baseY = 100.0f;
    float baseSpacing = 40.0f;

    // Вычисляем масштаб на основе текущего размера окна
    float scaleX = static_cast<float>(windowSize.x) / baseWidth;
    float scaleY = static_cast<float>(windowSize.y) / baseHeight;
    float scale = std::min(scaleX, scaleY);

    options.clear();

    auto makeOption = [&](const std::string& label, const std::string& value, bool selected) {
        auto text = std::make_unique<sf::Text>(font);
        text->setString(label + ": " + value);
        text->setCharacterSize(static_cast<unsigned int>(baseTextSize * scale));
        text->setPosition(sf::Vector2f(
            baseX * scaleX,
            (baseY + baseSpacing * static_cast<float>(options.size())) * scaleY
        ));
        text->setFillColor(selected ? sf::Color::Red : sf::Color::White);
        return text;
        };

    std::string resStr = AVAILABLE_RESOLUTIONS[currentResolutionIndex].name;
    options.push_back(makeOption("Resolution", resStr, selectedIndex == 0));
    options.push_back(makeOption("Fullscreen", config.fullscreen ? "ON" : "OFF", selectedIndex == 1));
    options.push_back(makeOption("VSync", config.vsync ? "ON" : "OFF", selectedIndex == 2));

    auto back = std::make_unique<sf::Text>(font);
    back->setString("Save & Back");
    back->setCharacterSize(static_cast<unsigned int>(baseTextSize * scale));
    back->setPosition(sf::Vector2f(
        baseX * scaleX,
        (baseY + baseSpacing * static_cast<float>(options.size())) * scaleY
    ));
    back->setFillColor(selectedIndex == 3 ? sf::Color::Red : sf::Color::White);
    options.push_back(std::move(back));
    // Обновляем menuItems для работы с мышью
    menuItems.clear();
    for (auto& option : options) {
        menuItems.push_back(option.get());
    }
}
void SettingsScene::updatePositions(sf::RenderWindow& window) {
    // Обновляем цвета на основе наведения мыши
    for (size_t i = 0; i < options.size(); ++i) {
        if (static_cast<int>(i) == hoveredIndex) {
            options[i]->setFillColor(sf::Color::Red);  // Наведение мышью
        }
        else if (static_cast<int>(i) == selectedIndex) {
            options[i]->setFillColor(sf::Color::Yellow);  // Выбрано клавиатурой
        }
        else {
            options[i]->setFillColor(sf::Color::White);  // Обычный цвет
        }
    }
}