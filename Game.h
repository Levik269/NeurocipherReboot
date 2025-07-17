#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include "SceneManager.h"

class Game {
public:
    Game();
    void run();

private:
    sf::RenderWindow window;
    SceneManager sceneManager;  
    GameConfig cfg;
};
