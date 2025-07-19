//MainMenuScene.h
#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "Scene.h"
#include "Config.h"
#include "GlitchRenderer.h"
class MainMenuScene : public Scene {
private:
    sf::Texture backgroundTexture;
    std::optional<sf::Sprite> backgroundSprite;
    sf::Font font;
    GameConfig& config;
    std::unique_ptr<sf::Text> titleText;
    std::unique_ptr<sf::Text> startText;
    std::unique_ptr<sf::Text> loadText;
    std::unique_ptr<sf::Text> optionsText;
    std::unique_ptr<sf::Text> exitText;

    GlitchRenderer glitchRenderer;
    std::unique_ptr<Scene> nextScene;
    std::vector<sf::Text*> menuItems;
    
    int hoveredIndex = -1;

    
    bool finished = false;
    
    void updatePositions();
    void updatePositions(sf::RenderWindow& window);
public:
    MainMenuScene(GameConfig& config);
    void update(float deltaTime, sf::RenderWindow& window) override;
    void render(sf::RenderWindow& window) override;
    void handleEvent(const sf::Event& event, sf::RenderWindow& window) override;
    bool isFinished() const override;
    std::unique_ptr<Scene> extractNextScene();
};

