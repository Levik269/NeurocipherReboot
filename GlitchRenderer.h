//GlitchRenderer.h
#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include <memory>
#include <vector>

class GlitchRenderer {
public:
    GlitchRenderer();

    // Обновление эффектов
    void update(float deltaTime);

    // Глич эффекты для фона
    void renderBackground(sf::RenderWindow& window, sf::Texture& texture);
    void setBackgroundGlitch(bool enabled, float intensity = 1.0f);

    // Глич эффекты для текста
    void renderGlitchText(sf::RenderWindow& window, sf::Text& mainText, const std::string& text);
    void setTextGlitch(bool enabled, float intensity = 1.0f);

    // Глич линии на экране
    void renderGlitchLines(sf::RenderWindow& window, int lineCount = 15);
    void setScreenGlitch(bool enabled);

    // Глич эффект при наведении на элемент
    void renderHoverGlitch(sf::RenderWindow& window, const sf::FloatRect& bounds);

    void setBackgroundDarkening(bool enabled, float intensity = 0.5f);
    void setCyberpunkSquares(bool enabled);
    void setAnalogGlitch(bool enabled);
    void renderCyberpunkSquares(sf::RenderWindow& window, int squareCount = 5);

private:
    // Таймеры и состояния
    float backgroundGlitchTimer = 0.f;
    float textGlitchTimer = 0.f;
    bool backgroundGlitchActive = false;
    bool textGlitchActive = false;
    bool screenGlitchEnabled = true;

    // Интенсивности эффектов
    float backgroundIntensity = 1.0f;
    float textIntensity = 1.0f;

    // Вспомогательные объекты
    std::unique_ptr<sf::Text> glitchText;
    sf::Font* currentFont = nullptr;
    sf::Vector2f originalBackgroundPos;

    // Вспомогательные функции
    float getRandomOffset(float intensity) const;
    sf::Color getGlitchColor() const;

    bool backgroundDarkeningEnabled;
    float darkeningIntensity;

    bool cyberpunkSquaresEnabled;
    float squareGlitchTimer;

    bool analogGlitchEnabled;
    float analogOffsetX;
    float analogOffsetY;
    float analogTimer;
};