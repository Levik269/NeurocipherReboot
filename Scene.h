//scene.h
#pragma once
#include <SFML/Graphics.hpp>

class Scene {
public:
    virtual ~Scene() = default;
    virtual void update(float dt, sf::RenderWindow& window) = 0;
    virtual void render(sf::RenderWindow& window) = 0;
    virtual void handleEvent(const sf::Event& event, sf::RenderWindow& window) = 0;
    virtual bool isFinished() const = 0;
};