#include "Game.h"
#include <optional>
#include "Config.h"
#include "ConfigManager.h"
#include <SFML/Window.hpp>
#include <SFML/Config.hpp>
#include <SFML/Graphics.hpp>
#include <cstdint>
Game::Game() {
    GameConfig cfg = ConfigManager::load();
    auto style = sf::WindowStyle::Fullscreen;
    sf::VideoMode mode(sf::Vector2u(cfg.width, cfg.height));
    window.create(mode, "Neurocipher Reboot", style);
    window.setVerticalSyncEnabled(cfg.vsync);
    sceneManager = SceneManager(cfg);
}


void Game::run() {
    sf::Clock clock;

    while (window.isOpen()) {
        // Handle events - SFML 3.x style
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            sceneManager.handleEvent(*event, window);
        }


        // Update game logic
        float dt = clock.restart().asSeconds();
        sceneManager.update(dt, window);

        // Render
        window.clear();
        sceneManager.render(window);
        window.display();
    }
}