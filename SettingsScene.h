#pragma once
#include "Scene.h"
#include "Config.h" // для GameConfig
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

class SettingsScene : public Scene {
public:
    SettingsScene(GameConfig& config);
    void update(float dt, sf::RenderWindow& window) override;
    void render(sf::RenderWindow& window) override;
    void handleEvent(const sf::Event& event, sf::RenderWindow& window) override;
    bool isFinished() const override;

    std::unique_ptr<Scene> getNextScene(); // <-- добавим это, если нужна передача сцены обратно

private:
    GameConfig& config;
    sf::Font font;
    std::vector<std::unique_ptr<sf::Text>> options;
    int selectedIndex = 0;
    bool finished = false;

    void applySelection();
    void updateTexts();
};
