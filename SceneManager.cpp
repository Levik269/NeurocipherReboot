#include "SceneManager.h"
#include "MainMenuScene.h"
#include "SplashScene.h"
#include <SFML/Window.hpp>

SceneManager::SceneManager(const GameConfig& config) : config(config) {
    currentScene = std::make_unique<SplashScene>();
}

void SceneManager::update(float dt, sf::RenderWindow& window) {
    if (currentScene) {
        currentScene->update(dt, window);

        
        if (currentScene->isFinished()) {
            
            if (dynamic_cast<SplashScene*>(currentScene.get())) {
                currentScene = std::make_unique<MainMenuScene>(config);
            }
            else {
                currentScene.reset();
            }
        }
    }
}

void SceneManager::render(sf::RenderWindow& window) {
    if (currentScene)
        currentScene->render(window);
}

void SceneManager::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    if (currentScene) {
        currentScene->handleEvent(event, window); 
    }
}

