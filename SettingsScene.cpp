#include "SettingsScene.h"
#include "ConfigManager.h"
#include <string>

SettingsScene::SettingsScene(GameConfig& configRef) : config(configRef) {
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        throw std::runtime_error("Failed to load font");
    }
    updateTexts();
}

void SettingsScene::update(float, sf::RenderWindow&) {
    // Nothing to animate for now
}

void SettingsScene::render(sf::RenderWindow& window) {
    for (const auto& text : options) {
        window.draw(*text);
    }
}

void SettingsScene::handleEvent(const sf::Event& event, sf::RenderWindow&) {
    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        switch (key->scancode) {
        case sf::Keyboard::Scancode::Down:
            selectedIndex = (selectedIndex + 1) % 4;
            updateTexts();
            break;
        case sf::Keyboard::Scancode::Up:
            selectedIndex = (selectedIndex + 3) % 4;
            updateTexts();
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
            updateTexts();
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

void SettingsScene::updateTexts() {
    options.clear();

    auto makeOption = [&](const std::string& label, const std::string& value, bool selected) {
        auto text = std::make_unique<sf::Text>(label + ": " + value, font, 24);
        text->setPosition({ 100.f, static_cast<float>(100 + 40 * options.size()) });
        text->setFillColor(selected ? sf::Color::Red : sf::Color::White);
        return text;
        };

    std::string resStr = std::to_string(config.width) + "x" + std::to_string(config.height);
    options.push_back(makeOption("Resolution", resStr, selectedIndex == 0));
    options.push_back(makeOption("Fullscreen", config.fullscreen ? "ON" : "OFF", selectedIndex == 1));
    options.push_back(makeOption("VSync", config.vsync ? "ON" : "OFF", selectedIndex == 2));

    auto back = std::make_unique<sf::Text>("Save & Back", font, 24);
    back->setPosition({ 100.f, static_cast<float>(100 + 40 * options.size()) });
    back->setFillColor(selectedIndex == 3 ? sf::Color::Red : sf::Color::White);
    options.push_back(std::move(back));
}