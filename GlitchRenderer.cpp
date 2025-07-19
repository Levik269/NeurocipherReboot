#include "GlitchRenderer.h"
#include <random>
#include <iostream>
#include <cstdlib>

GlitchRenderer::GlitchRenderer() {
    originalBackgroundPos = sf::Vector2f(0.f, 0.f);
}

void GlitchRenderer::update(float deltaTime) {
    // Обновляем таймер для фона
    backgroundGlitchTimer += deltaTime;
    if (backgroundGlitchTimer > 0.8f) {
        backgroundGlitchActive = (rand() % 2 == 0); // 50% шанс
        backgroundGlitchTimer = 0.f;
    }

    // Обновляем таймер для текста
    textGlitchTimer += deltaTime;
    if (textGlitchTimer > 1.0f) {
        textGlitchActive = (rand() % 2 == 0); // 50% шанс
        textGlitchTimer = 0.f;
    }
}

void GlitchRenderer::renderBackground(sf::RenderWindow& window, sf::Texture& texture) {
    sf::Sprite backgroundSprite(texture);
    auto windowSize = window.getSize();
    auto textureSize = texture.getSize();

    // Масштабирование
    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
    float scale = std::max(scaleX, scaleY);

    backgroundSprite.setScale(sf::Vector2f(scaleX, scaleY));

    // Применяем глич эффект
    if (backgroundGlitchActive) {
        float offsetX = getRandomOffset(backgroundIntensity * 8.0f);
        float offsetY = getRandomOffset(backgroundIntensity * 8.0f);
        backgroundSprite.setPosition(sf::Vector2f(offsetX, offsetY));
    }
    else {
        backgroundSprite.setPosition(originalBackgroundPos);
    }

    window.draw(backgroundSprite);
}

void GlitchRenderer::setBackgroundGlitch(bool enabled, float intensity) {
    backgroundGlitchActive = enabled;
    backgroundIntensity = intensity;
}

void GlitchRenderer::renderGlitchText(sf::RenderWindow& window, sf::Text& mainText, const std::string& text) {
    // Создаем глич-версию текста если нужно
    if (!glitchText || currentFont != &mainText.getFont()) {
        currentFont = const_cast<sf::Font*>(&mainText.getFont());
        glitchText = std::make_unique<sf::Text>(*currentFont);
        glitchText->setString(text);
        glitchText->setCharacterSize(mainText.getCharacterSize());
        glitchText->setFillColor(sf::Color(139, 0, 0));
    }

    // Применяем глич эффект к основному тексту
    if (textGlitchActive) {
        float offsetX = getRandomOffset(textIntensity * 5.0f);
        float offsetY = getRandomOffset(textIntensity * 5.0f);

        auto originalPos = mainText.getPosition();
        mainText.setPosition(sf::Vector2f(originalPos.x + offsetX, originalPos.y + offsetY));

        // Рендерим глич-версию со смещением
        glitchText->setPosition(sf::Vector2f(originalPos.x - offsetX + 2.0f, originalPos.y - offsetY + 2.0f));
        window.draw(*glitchText);
    }
}

void GlitchRenderer::setTextGlitch(bool enabled, float intensity) {
    textGlitchActive = enabled;
    textIntensity = intensity;
}

void GlitchRenderer::renderGlitchLines(sf::RenderWindow& window, int lineCount) {
    if (!screenGlitchEnabled) return;

    sf::VertexArray lines(sf::PrimitiveType::Lines);
    auto windowSize = window.getSize();

    for (int i = 0; i < lineCount; ++i) {
        int y = rand() % windowSize.y;
        sf::Color glitchColor = getGlitchColor();

        sf::Vertex v1;
        v1.position = sf::Vector2f(0.f, static_cast<float>(y));
        v1.color = glitchColor;

        sf::Vertex v2;
        v2.position = sf::Vector2f(static_cast<float>(windowSize.x), static_cast<float>(y));
        v2.color = glitchColor;

        lines.append(v1);
        lines.append(v2);
    }

    window.draw(lines);
}

void GlitchRenderer::setScreenGlitch(bool enabled) {
    screenGlitchEnabled = enabled;
}

void GlitchRenderer::renderHoverGlitch(sf::RenderWindow& window, const sf::FloatRect& bounds) {
    float x = bounds.position.x;
    float y = bounds.position.y;
    float w = bounds.size.x;
    float h = bounds.size.y;

    sf::RectangleShape glitch(sf::Vector2f(
        w * (0.3f + 0.2f * (rand() % 3)),
        2.f
    ));
    glitch.setFillColor(sf::Color::Red);
    glitch.setPosition(sf::Vector2f(
        x + static_cast<float>(rand() % static_cast<int>(w / 2)),
        y + 5.f + static_cast<float>(rand() % static_cast<int>(h - 10.f))
    ));

    window.draw(glitch);
}

float GlitchRenderer::getRandomOffset(float intensity) const {
    return static_cast<float>((rand() % static_cast<int>(intensity * 2)) - intensity);
}

sf::Color GlitchRenderer::getGlitchColor() const {
    int brightness = 100 + (rand() % 155);
    return sf::Color(brightness, 0, 0);
}