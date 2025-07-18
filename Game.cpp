#include "Game.h"
#include <optional>
#include "Config.h"
#include "ConfigManager.h"
#include <SFML/Window.hpp>
#include <SFML/Config.hpp>
#include <SFML/Graphics.hpp>
#include <cstdint>

Game::Game() : sceneManager(ConfigManager::load()) {
    GameConfig cfg = ConfigManager::load();

    sf::VideoMode mode(sf::Vector2u(cfg.width, cfg.height));

    if (cfg.fullscreen) {
        window.create(mode, "Neurocipher Reboot", sf::State::Fullscreen);
    }
    else {
        window.create(mode, "Neurocipher Reboot");
    }

    window.setVerticalSyncEnabled(cfg.vsync);
    sceneManager = SceneManager(cfg);
    sceneManager.setGame(this);
}

void Game::run() {
    sf::Clock clock;

    while (window.isOpen()) {
        
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            sceneManager.handleEvent(*event, window);  
        }

        float dt = clock.restart().asSeconds();
        sceneManager.update(dt, window);  

        window.clear();
        sceneManager.render(window);  
        window.display();
    }
}

void Game::updateWindow() {
    cfg = ConfigManager::load();

    sf::VideoMode mode(sf::Vector2u(cfg.width, cfg.height));

    if (cfg.fullscreen) {
        window.create(mode, "Neurocipher Reboot", sf::State::Fullscreen);
    }
    else {
        window.create(mode, "Neurocipher Reboot");
    }

    window.setVerticalSyncEnabled(cfg.vsync);
}