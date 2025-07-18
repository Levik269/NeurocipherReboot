#include "SceneManager.h"
#include "MainMenuScene.h"
#include "SplashScene.h"
#include <SFML/Window.hpp>
#include "SettingsScene.h"  
#include "ConfigManager.h"  
#include "Game.h"  

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
            else if (auto* mainMenu = dynamic_cast<MainMenuScene*>(currentScene.get())) {
                auto nextScene = mainMenu->extractNextScene();
                if (nextScene) {
                    currentScene = std::move(nextScene);
                }
                else {
                    currentScene.reset();
                }
            }
            else if (dynamic_cast<SettingsScene*>(currentScene.get())) {
                // Когда выходим из настроек, обновляем конфигурацию и окно
                config = ConfigManager::load();
                if (game) {
                    game->updateWindow();
                }
                currentScene = std::make_unique<MainMenuScene>(config);
            }
            else {
                currentScene = std::make_unique<MainMenuScene>(config);
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

