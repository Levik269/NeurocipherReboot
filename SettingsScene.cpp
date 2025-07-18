#include "SettingsScene.h"
#include "ConfigManager.h"
#include <string>
#include <stdexcept>

SettingsScene::SettingsScene(GameConfig& configRef) : config(configRef) {
    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
        throw std::runtime_error("Failed to load font");
    }
    // Убираем updateTexts() из конструктора, так как у нас ещё нет окна
}

void SettingsScene::update(float dt, sf::RenderWindow& window) {
    // Обновляем позиции для корректного масштабирования
    updateTexts(window);
}

void SettingsScene::render(sf::RenderWindow& window) {
    for (const auto& text : options) {
        window.draw(*text);
    }
}

void SettingsScene::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
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
                if (config.width == 1280) {
                    config.width = 1920;
                    config.height = 1080;
                }
                else {
                    config.width = 1280;
                    config.height = 720;
                }
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

    std::string resStr = std::to_string(config.width) + "x" + std::to_string(config.height);
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
}