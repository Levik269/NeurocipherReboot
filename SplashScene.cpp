// В SplashScene.cpp
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

    titleText = std::make_unique<sf::Text>(font, "PRESS ANY KEY TO CONTINUE", 48);
    titleText->setCharacterSize(24);              // Увеличенный размер текста
    titleText->setFillColor(sf::Color(139, 0, 0));  // цвет

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
    // ничего
}

void SplashScene::render(sf::RenderWindow& window) {
    auto size = window.getSize();

    if (backgroundSprite) {
        sf::Vector2f scale(
            static_cast<float>(size.x) / static_cast<float>(backgroundTexture.getSize().x),
            static_cast<float>(size.y) / static_cast<float>(backgroundTexture.getSize().y)
        );
        backgroundSprite->setScale(scale);
        backgroundSprite->setPosition(sf::Vector2f(0.f, 0.f));
        window.draw(*backgroundSprite);
    }

    if (titleText) {
        auto bounds = titleText->getLocalBounds();
        titleText->setOrigin({
            bounds.position.x + bounds.size.x / 2.f,
            bounds.position.y + bounds.size.y / 2.f
            });
        titleText->setPosition({ float(size.x) / 2.f, float(size.y) * 0.85f });
        window.draw(*titleText);
    }

    sf::VertexArray lines(sf::PrimitiveType::Lines);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> y_dist(0, size.y);
    std::uniform_int_distribution<int> brightness(100, 255);


    for (int i = 0; i < 15; ++i) {
        int y = y_dist(gen);
        sf::Color glitchColor(brightness(gen), 0, 0);

        sf::Vertex v1;
        v1.position = sf::Vector2f(0.f, static_cast<float>(y));
        v1.color = glitchColor;

        sf::Vertex v2;
        v2.position = sf::Vector2f(static_cast<float>(size.x), static_cast<float>(y));
        v2.color = glitchColor;

        lines.append(v1);
        lines.append(v2);
    }

    window.draw(lines);
    
}


bool SplashScene::isFinished() const {
    return finished;
}



