#include "SceneManager.h"
#include "MainMenuScene.h"
#include "SplashScene.h"
#include <SFML/Window.hpp>
#include "SettingsScene.h"  
#include "ConfigManager.h"  
#include "Game.h"  
#include "CharacterCreationScenes.h"
#include "CharacterSpecializationScene.h"
#include "CharacterFreePointsDistributionScene.h"  // Добавим include
#include "CharacterAppearance.h"  // Добавим include

SceneManager::SceneManager(const GameConfig& config) : config(config) {
    currentScene = std::make_unique<SplashScene>();
}

void SceneManager::update(float dt, sf::RenderWindow& window) {
    if (currentScene) {
        currentScene->update(dt, window);
        if (currentScene->isFinished()) {
            // SplashScene переходит в MainMenuScene
            if (dynamic_cast<SplashScene*>(currentScene.get())) {
                currentScene = std::make_unique<MainMenuScene>(config);
            }
            // MainMenuScene может отдавать следующую сцену
            else if (auto* mainMenu = dynamic_cast<MainMenuScene*>(currentScene.get())) {
                auto nextScene = mainMenu->extractNextScene();
                if (nextScene) {
                    currentScene = std::move(nextScene);
                }
                else {
                    currentScene.reset();
                }
            }
            // SettingsScene — возврат в главное меню после обновления
            else if (dynamic_cast<SettingsScene*>(currentScene.get())) {
                config = ConfigManager::load();
                if (game) {
                    game->updateWindow();
                }
                currentScene = std::make_unique<MainMenuScene>(config);
            }
            // Обработка CharacterOrigin
            else if (auto* origin = dynamic_cast<CharacterOrigin*>(currentScene.get())) {
                auto nextScene = origin->extractNextScene();
                if (nextScene) {
                    currentScene = std::move(nextScene);
                }
                else {
                    currentScene = std::make_unique<MainMenuScene>(config);
                }
            }
            // Обработка CharacterSpecialization
            else if (auto* specialization = dynamic_cast<CharacterSpecialization*>(currentScene.get())) {
                auto nextScene = specialization->extractNextScene();
                if (nextScene) {
                    currentScene = std::move(nextScene);
                }
                else {
                    currentScene = std::make_unique<MainMenuScene>(config);
                }
            }
            // ДОБАВЛЯЕМ: Обработка FreePoints
            else if (auto* freePoints = dynamic_cast<FreePoints*>(currentScene.get())) {
                auto nextScene = freePoints->extractNextScene();
                if (nextScene) {
                    std::cout << "SceneManager: Transitioning from FreePoints to next scene" << std::endl;
                    currentScene = std::move(nextScene);
                }
                else {
                    std::cout << "SceneManager: No next scene from FreePoints, returning to main menu" << std::endl;
                    currentScene = std::make_unique<MainMenuScene>(config);
                }
            }
            // ДОБАВЛЯЕМ: Обработка AppearanceScene
            else if (auto* appearance = dynamic_cast<AppearanceScene*>(currentScene.get())) {
                auto nextScene = appearance->extractNextScene();
                if (nextScene) {
                    std::cout << "SceneManager: Transitioning from AppearanceScene to next scene" << std::endl;
                    currentScene = std::move(nextScene);
                }
                else {
                    std::cout << "SceneManager: AppearanceScene completed, returning to main menu" << std::endl;
                    currentScene = std::make_unique<MainMenuScene>(config);
                }
            }
            else {
                // Для любых других сцен — fallback в главное меню
                std::cout << "SceneManager: Unknown scene finished, returning to main menu" << std::endl;
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

