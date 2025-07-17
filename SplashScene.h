// В SplashScene.h
#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include "Scene.h"

class SplashScene : public Scene {
public:
    SplashScene();

    void handleEvent(const sf::Event& event, sf::RenderWindow& window) override;
    void update(float dt, sf::RenderWindow& window) override;
    void render(sf::RenderWindow& window) override;
    bool isFinished() const override;

private:
    bool finished = false;
    bool fontLoaded = false;

    sf::Font font;
    std::unique_ptr<sf::Text> titleText;

    sf::Texture backgroundTexture;  // просто объект
    std::unique_ptr<sf::Sprite> backgroundSprite; // указатель для Sprite
};


