﻿#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "Scene.h" 
#include <SFML/Config.hpp>
#include <SFML/Window.hpp>
#include "Config.h"
#include "ConfigManager.h"
class Game;
class SceneManager {
public:
    SceneManager(const GameConfig& config);
    ~SceneManager() = default;

    // Запретить копирование
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;

    // Разрешить перемещение
    SceneManager(SceneManager&&) = default;
    SceneManager& operator=(SceneManager&&) = default;
    
    void update(float deltaTime, sf::RenderWindow& window);
    void render(sf::RenderWindow& window);
    void handleEvent(const sf::Event& event, sf::RenderWindow& window);

    bool isFinished() const {
        return !currentScene;
    }
    
    GameConfig& getConfig() { return config; }
    void setGame(Game* gamePtr) { game = gamePtr; }


private:
    std::unique_ptr<Scene> currentScene; 
    GameConfig config;
    Game* game = nullptr;
};
