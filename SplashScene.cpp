#include "SplashScene.h"
#include <iostream>
#include <random>
#include <SFML/Graphics.hpp>

SplashScene::SplashScene()
{
    std::vector<std::string> fontPaths = {
        "assets/fonts/digital-7 (italic).ttf",
        "arial.ttf",
        "../assets/fonts/arial.ttf",
        "C:\\Windows\\Fonts\\arial.ttf",
        "C:\\Windows\\Fonts\\calibri.ttf"
    };

    for (const auto& path : fontPaths) {
        if (font.openFromFile(path)) {  // SFML 3.x - openFromFile
            fontLoaded = true;
            std::cout << "Font loaded from: " << path << std::endl;
            break;
        }
    }

    if (!fontLoaded) {
        std::cerr << "Warning: Could not load any font file. Text may not display correctly." << std::endl;
    }

    titleText = std::make_unique<sf::Text>(font, "PRESS ANY KEY TO CONTINUE", 24);
    titleText->setFillColor(sf::Color(139, 0, 0));

    if (backgroundTexture.loadFromFile("assets/textures/splash_background.jpg")) {
        backgroundSprite = std::make_unique<sf::Sprite>(backgroundTexture);
    }
    else {
        std::cerr << "Failed to load splash background image." << std::endl;
    }
}

void SplashScene::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    if (event.is<sf::Event::KeyPressed>() || event.is<sf::Event::MouseButtonPressed>()) {
        finished = true;
    }
}

void SplashScene::update(float dt, sf::RenderWindow& window) {
    // Обновляем позиции для корректного масштабирования
    updatePositions(window);
}

void SplashScene::render(sf::RenderWindow& window) {
    auto windowSize = window.getSize();

    // Рендеринг фона с корректным масштабированием
    if (backgroundSprite) {
        auto textureSize = backgroundTexture.getSize();

        // Вычисляем масштаб для заполнения всего окна с сохранением пропорций
        float scaleX = static_cast<float>(windowSize.x) / static_cast<float>(textureSize.x);
        float scaleY = static_cast<float>(windowSize.y) / static_cast<float>(textureSize.y);
        float scale = std::max(scaleX, scaleY);

        backgroundSprite->setScale(sf::Vector2f(scale, scale));

        // Центрируем спрайт
        float spriteWidth = static_cast<float>(textureSize.x) * scale;
        float spriteHeight = static_cast<float>(textureSize.y) * scale;
        float offsetX = (static_cast<float>(windowSize.x) - spriteWidth) / 2.f;
        float offsetY = (static_cast<float>(windowSize.y) - spriteHeight) / 2.f;

        backgroundSprite->setPosition(sf::Vector2f(offsetX, offsetY));
        window.draw(*backgroundSprite);
    }

    // Рендеринг текста с корректным позиционированием
    if (titleText) {
        window.draw(*titleText);
    }

    // Глич-эффект линий
    sf::VertexArray lines(sf::PrimitiveType::Lines);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> y_dist(0, windowSize.y);
    std::uniform_int_distribution<int> brightness(100, 255);

    for (int i = 0; i < 15; ++i) {
        int y = y_dist(gen);
        sf::Color glitchColor(brightness(gen), 0, 0);

        sf::Vertex v1;
        v1.position = sf::Vector2f(0.f, static_cast<float>(y));
        v1.color = glitchColor;

        sf::Vertex v2;
        v2.position = sf::Vector2f(static_cast<float>(windowSize.x), static_cast<float>(y));
        v2.color = glitchColor;

        lines.append(v1);
        lines.append(v2);
    }

    window.draw(lines);
}

bool SplashScene::isFinished() const {
    return finished;
}

void SplashScene::updatePositions(sf::RenderWindow& window) {
    if (!titleText) return;

    auto windowSize = window.getSize();

    // Базовые размеры для масштабирования
    float baseWidth = 1280.0f;
    float baseHeight = 720.0f;
    float baseTextSize = 24.0f;

    // Вычисляем масштаб на основе текущего размера окна
    float scaleX = static_cast<float>(windowSize.x) / baseWidth;
    float scaleY = static_cast<float>(windowSize.y) / baseHeight;
    float scale = std::min(scaleX, scaleY); // Используем минимальный масштаб для сохранения пропорций

    // Применяем масштаб к размеру текста
    titleText->setCharacterSize(static_cast<unsigned int>(baseTextSize * scale));

    // Центрируем текст по горизонтали и размещаем в нижней части экрана
    auto bounds = titleText->getLocalBounds();
    titleText->setOrigin(sf::Vector2f(
        bounds.position.x + bounds.size.x / 2.f,
        bounds.position.y + bounds.size.y / 2.f
    ));

    // Позиционируем текст в нижней части экрана (85% от высоты)
    titleText->setPosition(sf::Vector2f(
        static_cast<float>(windowSize.x) / 2.f,
        static_cast<float>(windowSize.y) * 0.85f
    ));
}


