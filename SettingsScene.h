#pragma once
#include "Scene.h"
#include "Config.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "GlitchRenderer.h"
class SettingsScene : public Scene {
public:
    SettingsScene(GameConfig& config);
    void update(float dt, sf::RenderWindow& window) override;
    void render(sf::RenderWindow& window) override;
    void handleEvent(const sf::Event& event, sf::RenderWindow& window) override;
    bool isFinished() const override;

private:
    GlitchRenderer glitchRenderer;
    int currentResolutionIndex = 0;
    GameConfig& config;
    sf::Font font;
    std::vector<std::unique_ptr<sf::Text>> options;
    int selectedIndex = 0;
    bool finished = false;

    void updateTexts(sf::RenderWindow& window);
    std::vector<sf::Text*> menuItems;
    int hoveredIndex = -1;
    void updatePositions(sf::RenderWindow& window);

    sf::Texture backgroundTexture;
    std::optional<sf::Sprite> backgroundSprite;
    
    
};