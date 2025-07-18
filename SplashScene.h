// SplashScene.h
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

    sf::Texture backgroundTexture;
    std::unique_ptr<sf::Sprite> backgroundSprite;

    void updatePositions(sf::RenderWindow& window);
};


