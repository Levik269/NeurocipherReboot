//CharacterSpecializationScene.h
#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "Scene.h"
#include "Config.h"
#include "GlitchRenderer.h"

struct SpecButton {
    std::optional<sf::Sprite> sprite;
    sf::Text labelText;
    sf::RectangleShape border;
    std::string label;
    std::shared_ptr<sf::Texture> texture;

    SpecButton(const sf::Font& font)
        : labelText(font, "")
    {
        border.setFillColor(sf::Color::Transparent);
        border.setOutlineThickness(1.f);
        border.setOutlineColor(sf::Color::Green);
        labelText.setCharacterSize(32);
        labelText.setFillColor(sf::Color(139, 0, 0));
    }

    void setTexture(std::shared_ptr<sf::Texture> tex) {
        texture = tex;
        if (texture && texture->getSize().x > 0) {
            sprite = sf::Sprite(*texture); 
        }
    }

    void setPosition(const sf::Vector2f& pos) {
        if (sprite) sprite->setPosition(pos);
        border.setPosition(pos);
        labelText.setPosition({ pos.x + 10.f, pos.y + border.getSize().y + 10.f });
    }

    void setSize(const sf::Vector2f& size) {
        border.setSize(size);
        if (texture && texture->getSize().x > 0 && sprite) {
            float scaleX = size.x / static_cast<float>(texture->getSize().x);
            float scaleY = size.y / static_cast<float>(texture->getSize().y);
            sprite->setScale({ scaleX, scaleY });
        }
    }

    sf::FloatRect getBounds() const {
        return border.getGlobalBounds();
    }

    bool contains(const sf::Vector2f& point) const {
        return getBounds().contains(point);
    }
};

class CharacterSpecialization : public Scene {
private:

    std::vector<std::pair<std::string, std::string>> spec = {
        {"Hacker", "assets/textures/z.png"},
        {"Mercenary", "assets/textures/corpo.png"},
        {"Trader", "assets/textures/street.png"},
        {"Engineer", "assets/textures/tech.png" },
        {"Soldier", "assets/textures/z.png" },
        {"Detective", "assets/textures/corpo.png"}
    };

    sf::Texture backgroundTexture;
    std::optional<sf::Sprite> backgroundSprite;
    std::vector<SpecButton> SpecButtons;
    std::vector<std::shared_ptr<sf::Texture>> textures; 
    sf::Font font;
    GameConfig& config;
    std::unique_ptr<sf::Text> SpecializationText;
    GlitchRenderer glitchRenderer;
    std::unique_ptr<Scene> nextScene;
    bool finished = false;
    std::vector<sf::Drawable*> menuItems;
    int hoveredIndex = -1;
    void updatePositions(sf::RenderWindow& window);
public:
    CharacterSpecialization(GameConfig& config);
    void update(float deltaTime, sf::RenderWindow& window) override;
    void render(sf::RenderWindow& window) override;
    void handleEvent(const sf::Event& event, sf::RenderWindow& window) override;
    bool isFinished() const override;
    std::unique_ptr<Scene> extractNextScene();
};