//CharacterCreationScene.cpp
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
#include <CharacterCreationScenes.h>
#include <CharacterSpecializationScene.h>
#include "GlitchRenderer.h"
#include <CharacterFreePointsDistributionScene.h>

CharacterSpecialization::CharacterSpecialization(GameConfig& config) : config(config) {

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
    //Hacker, Mercenary, Trader, Technician, StreetDoctor, Detective
    SpecializationText = std::make_unique<sf::Text>(font, "CHOOSE YOUR SPECIALIZATION");
    SpecializationText->setCharacterSize(100);
    SpecializationText->setFillColor(sf::Color(139, 0, 0)); // setFillColor вместо setColor
    SpecializationText->setPosition(sf::Vector2f(10.f, 10.f));

    for (const auto& [label, texturePath] : spec) {
        auto tex = std::make_shared<sf::Texture>();
        if (!tex->loadFromFile(texturePath)) {
            std::cerr << "Failed to load texture for " << label << ": " << texturePath << std::endl;
        }
        textures.push_back(tex); 

        SpecButton button(font);
        button.setTexture(tex);
        button.label = label;
        button.labelText.setString(label);
        SpecButtons.push_back(std::move(button));
    }
    // Настройка глитч эффектов
    glitchRenderer.setTextGlitch(true, 1.0f);  // Включаем глитч для текста
    glitchRenderer.setAnalogGlitch(true);       // Включаем аналоговый глитч
}

void CharacterSpecialization::update(float dt, sf::RenderWindow& window) {
    glitchRenderer.update(dt);
    updatePositions(window);

    // Получаем позицию мыши и проверяем, что она в пределах окна
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2u windowSize = window.getSize();

    if (mousePos.x >= 0 && mousePos.y >= 0 &&
        mousePos.x < static_cast<int>(windowSize.x) &&
        mousePos.y < static_cast<int>(windowSize.y)) {

        sf::Vector2f mouseWorld = window.mapPixelToCoords(mousePos);

        for (auto& btn : SpecButtons) {
            if (btn.getBounds().contains(mouseWorld)) {
                btn.border.setOutlineColor(sf::Color::Yellow);
            }
            else {
                btn.border.setOutlineColor(sf::Color::Green);
            }
        }
    }
    else {
        // Если мышь вне окна, сбрасываем все кнопки в обычное состояние
        for (auto& btn : SpecButtons) {
            btn.border.setOutlineColor(sf::Color::Green);
        }
    }
}

void CharacterSpecialization::render(sf::RenderWindow& window) {
    std::cout << "Rendering background..." << std::endl;
    if (backgroundSprite.has_value()) {
        auto windowSize = window.getSize();
        auto textureSize = backgroundTexture.getSize();
        float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
        float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
        float scale = std::max(scaleX, scaleY);
        backgroundSprite->setScale(sf::Vector2f(scale, scale));
        window.draw(backgroundSprite.value());
    }

    std::cout << "Rendering glitch..." << std::endl;
    glitchRenderer.renderBackground(window, backgroundTexture);

    std::cout << "Rendering origin text..." << std::endl;
    window.draw(*SpecializationText);

    std::cout << "Rendering buttons..." << std::endl;
    for (auto& btn : SpecButtons) {
        // Рендерим спрайты кнопок
        if (btn.sprite) window.draw(*btn.sprite);
        window.draw(btn.border);

        // Рендерим текст кнопок с глитч эффектом
        glitchRenderer.renderGlitchText(window, btn.labelText, btn.label);
        window.draw(btn.labelText);

        // Добавляем hover глитч эффект при наведении мыши
        sf::Vector2f mouseWorld = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        if (btn.getBounds().contains(mouseWorld)) {
            glitchRenderer.renderHoverGlitch(window, btn.getBounds());
        }
    }

    std::cout << "Render complete.\n";
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

void CharacterSpecialization::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        sf::Vector2f worldPos = window.mapPixelToCoords({ mousePressed->position.x, mousePressed->position.y });

        for (const auto& btn : SpecButtons) {
            if (btn.contains(worldPos)) {
                std::cout << btn.label << " selected\n";
                nextScene = std::make_unique<FreePoints>(config);
                std::cout << "Next scene should be here" << std::endl;
                finished = true;
                break;
            }
        }
    }
    else if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Enter) {
            std::cout << "Enter key pressed\n";
        }
    }
}

bool CharacterSpecialization::isFinished() const {
    return finished;
}

std::unique_ptr<Scene> CharacterSpecialization::extractNextScene() {
    return std::move(nextScene);
}

void CharacterSpecialization::updatePositions(sf::RenderWindow& window) {
    auto windowSize = window.getSize();

    const float baseWidth = 1280.f;
    const float baseHeight = 720.f;

    float scaleX = static_cast<float>(windowSize.x) / baseWidth;
    float scaleY = static_cast<float>(windowSize.y) / baseHeight;
    float scale = std::min(scaleX, scaleY);

    const float baseX = 50.f;
    const float baseY = 180.f;
    const float buttonWidth = 130.f;
    const float buttonHeight = 195.f;
    const float spacing = 5.f;

    for (size_t i = 0; i < SpecButtons.size(); ++i) {
        float x = (baseX + i * (buttonHeight + spacing)) * scaleX;
        float y = baseY * scaleY;
        SpecButtons[i].setPosition({ x, y });
        SpecButtons[i].setSize({ buttonWidth * scaleX, buttonHeight * scaleY });
        SpecButtons[i].labelText.setCharacterSize(static_cast<unsigned int>(36 * scale));
        SpecButtons[i].labelText.setPosition({ x + 10.f, y + buttonHeight * scaleY + 10.f });
    }
}