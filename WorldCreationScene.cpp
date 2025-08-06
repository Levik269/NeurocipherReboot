#include "MainMenuScene.h"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <memory>
#include <iostream>
#include <variant>
#include <cstdlib>
#include <ctime>
#include "Scene.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics.hpp>
#include <CharacterCreationScenes.h>
#include "CharacterSpecializationScene.h"
#include "GlitchRenderer.h"
#include "WorldCreationScene.h"

WorldButton::WorldButton(GameConfig& config) : config(config) {
    bool fontLoaded = false;

    std::vector<std::string> fontPaths = {
        "assets/fonts/digital-7 (italic).ttf",
        "arial.ttf",
        "../assets/fonts/arial.ttf",
        "C:\\Windows\\Fonts\\arial.ttf",
        "C:\\Windows\\Fonts\\calibri.ttf"
    };

    for (const auto& path : fontPaths) {
        if (font.openFromFile(path)) { 
            fontLoaded = true;
            std::cout << "Font loaded successfully from: " << path << std::endl;
            break;
        }
    }

    if (!fontLoaded) {
        std::cerr << "Warning: Could not load any font file. Text may not display correctly." << std::endl;
    }

    if (!backgroundTexture.loadFromFile("assets/textures/menu.png")) {
        std::cerr << "Failed to load background image.\n";
    }
    else {
        backgroundSprite.emplace(backgroundTexture);
    }

    // ИСПРАВЛЕНО: В SFML 3 sf::Text требует font в конструкторе
    WorldText = std::make_unique<sf::Text>(font, "CHOOSE THE WORLD TYPE");
    WorldText->setCharacterSize(100);
    WorldText->setFillColor(sf::Color(139, 0, 0)); // setFillColor вместо setColor
    WorldText->setPosition(sf::Vector2f(10.f, 10.f));

    // Если используется menuItems, убедись, что оно объявлено (vector<sf::Drawable*> menuItems;)
    menuItems.push_back(WorldText.get());

    // Загрузка кнопок (обновлено для хранения текстур)
    for (const auto& [label, texturePath] : origins) {
        auto tex = std::make_shared<sf::Texture>();
        if (!tex->loadFromFile(texturePath)) {
            std::cerr << "Failed to load texture for " << label << ": " << texturePath << std::endl;
        }
        textures.push_back(tex); // сохраняем чтобы не удалялись

        WorldButton button(font);
        button.setTexture(tex);
        button.label = label;
        button.labelText.setString(label);
        worldButton.push_back(std::move(button));
    }

    // Настройка глитч эффектов
    glitchRenderer.setTextGlitch(true, 1.0f);  // Включаем глитч для текста
    glitchRenderer.setAnalogGlitch(true);       // Включаем аналоговый глитч
}

void WorldButton::update(float dt, sf::RenderWindow& window) {
    glitchRenderer.update(dt);
    updatePositions(window);

    sf::Vector2f mouseWorld = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    for (auto& btn : worldButton) {
        if (btn.getBounds().contains(mouseWorld)) {
            btn.border.setOutlineColor(sf::Color::Yellow);
            // Можно добавить дополнительные эффекты при наведении
        }
        else {
            btn.border.setOutlineColor(sf::Color::Green);
        }
    }
}

void WorldButton::render(sf::RenderWindow& window) {
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
    // Рендерим главный заголовок с глитч эффектом
    //glitchRenderer.renderGlitchText(window, *OriginText, "CHOOSE YOUR ORIGIN");
    window.draw(*OriginText);

    std::cout << "Rendering buttons..." << std::endl;
    for (auto& btn : originButtons) {
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

void WorldButton::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    // ИСПРАВЛЕНО: В SFML 3 новый API для событий
    if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        sf::Vector2f worldPos = window.mapPixelToCoords({ mousePressed->position.x, mousePressed->position.y });

        for (const auto& btn : originButtons) {
            if (btn.contains(worldPos)) {
                std::cout << btn.label << " selected\n";
                //nextScene = std::make_unique<CharacterSpecialization>(config);
                finished = true;
                break;
            }
        }
    }
    else if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        // ИСПРАВЛЕНО: Scoped enum для клавиш
        if (keyPressed->code == sf::Keyboard::Key::Enter) {
            std::cout << "Enter key pressed\n";
        }
    }
}

bool WorldButton::isFinished() const {
    return finished;
}

std::unique_ptr<Scene> WorldButton::extractNextScene() {
    return std::move(nextScene);
}

void WorldButton::updatePositions(sf::RenderWindow& window) {
    auto windowSize = window.getSize();

    const float baseWidth = 1280.f;
    const float baseHeight = 720.f;

    float scaleX = static_cast<float>(windowSize.x) / baseWidth;
    float scaleY = static_cast<float>(windowSize.y) / baseHeight;
    float scale = std::min(scaleX, scaleY);

    const float baseX = 50.f;
    const float baseY = 180.f;
    const float buttonWidth = 200.f;
    const float buttonHeight = 300.f;
    const float spacing = 5.f;

    for (size_t i = 0; i < worldButton.size(); ++i) {
        float x = (baseX + i * (buttonHeight + spacing)) * scaleX;
        float y = baseY * scaleY;

        // ИСПРАВЛЕНО: sf::Vector2f вместо отдельных параметров
        worldButton[i].setPosition({ x, y });
        worldButton[i].setSize({ buttonWidth * scaleX, buttonHeight * scaleY });
        worldButton[i].labelText.setCharacterSize(static_cast<unsigned int>(36 * scale));
        worldButton[i].labelText.setPosition({ x + 10.f, y + buttonHeight * scaleY + 10.f });
    }
}