#include "GlitchRenderer.h"
#include <random>
#include <iostream>
#include <cstdlib>
#include <cstdint>

GlitchRenderer::GlitchRenderer() {
    originalBackgroundPos = sf::Vector2f(0.f, 0.f);

    // Инициализация новых переменных
    backgroundDarkeningEnabled = false;
    darkeningIntensity = 0.5f;
    cyberpunkSquaresEnabled = false;
    squareGlitchTimer = 0.f;
    analogGlitchEnabled = false;
    analogOffsetX = 0.f;
    analogOffsetY = 0.f;
    analogTimer = 0.f;
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

    // Обновляем таймер для киберпанк квадратов
    squareGlitchTimer += deltaTime;
    if (squareGlitchTimer > 0.1f + (rand() % 200) / 1000.0f) { // Случайная частота 0.1-0.3 секунды
        squareGlitchTimer = 0.f;
    }

    // Обновляем аналоговый глитч (более натуральная тряска)
    analogTimer += deltaTime;
    if (analogTimer > 0.05f) { // Обновляем каждые 50мс для плавности
        if (analogGlitchEnabled && (rand() % 4 == 0)) { // 25% шанс сработать
            analogOffsetX = getRandomOffset(backgroundIntensity * 12.0f);
            analogOffsetY = getRandomOffset(backgroundIntensity * 3.0f); // Меньше по Y для реалистичности
        }
        else {
            analogOffsetX *= 0.8f; // Плавное затухание
            analogOffsetY *= 0.8f;
        }
        analogTimer = 0.f;
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

    // Применяем аналоговый глитч (более натуральный)
    if (analogGlitchEnabled) {
        backgroundSprite.setPosition(sf::Vector2f(analogOffsetX, analogOffsetY));

        // Создаем "след" от старого изображения с другим цветом
        sf::Sprite ghostSprite = backgroundSprite;
        ghostSprite.setPosition(sf::Vector2f(-analogOffsetX * 0.3f, -analogOffsetY * 0.3f));
        ghostSprite.setColor(sf::Color(255, 100, 100, 80)); // Красноватый призрачный след
        window.draw(ghostSprite);

        // Еще один слой для усиления эффекта
        sf::Sprite ghostSprite2 = backgroundSprite;
        ghostSprite2.setPosition(sf::Vector2f(analogOffsetX * 0.5f, analogOffsetY * 0.5f));
        ghostSprite2.setColor(sf::Color(100, 255, 100, 60)); // Зеленоватый след
        window.draw(ghostSprite2);
    }
    // Старый глитч эффект (оставляем для совместимости)
    else if (backgroundGlitchActive) {
        float offsetX = getRandomOffset(backgroundIntensity * 8.0f);
        float offsetY = getRandomOffset(backgroundIntensity * 8.0f);
        backgroundSprite.setPosition(sf::Vector2f(offsetX, offsetY));
    }
    else {
        backgroundSprite.setPosition(originalBackgroundPos);
    }

    window.draw(backgroundSprite);

    // Применяем затемнение фона
    if (backgroundDarkeningEnabled) {
        sf::RectangleShape darkOverlay(sf::Vector2f(windowSize.x, windowSize.y));
        darkOverlay.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(255 * darkeningIntensity)));
        window.draw(darkOverlay);
    }
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

    auto originalPos = mainText.getPosition();

    // Применяем аналоговый глитч к тексту (текст "едет" вместе с фоном)
    if (analogGlitchEnabled) {
        // Основной текст смещается вместе с фоном
        mainText.setPosition(sf::Vector2f(originalPos.x + analogOffsetX, originalPos.y + analogOffsetY));

        // Призрачные следы текста остаются на старых позициях
        sf::Text ghostText1 = mainText;
        ghostText1.setPosition(originalPos);
        ghostText1.setFillColor(sf::Color(255, 100, 100, 120));
        window.draw(ghostText1);

        sf::Text ghostText2 = mainText;
        ghostText2.setPosition(sf::Vector2f(originalPos.x - analogOffsetX * 0.2f, originalPos.y - analogOffsetY * 0.2f));
        ghostText2.setFillColor(sf::Color(100, 255, 100, 100));
        window.draw(ghostText2);
    }
    // Старый глитч эффект для текста
    else if (textGlitchActive) {
        float offsetX = getRandomOffset(textIntensity * 5.0f);
        float offsetY = getRandomOffset(textIntensity * 5.0f);

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

// Новые функции

void GlitchRenderer::setBackgroundDarkening(bool enabled, float intensity) {
    backgroundDarkeningEnabled = enabled;
    darkeningIntensity = std::max(0.0f, std::min(1.0f, intensity)); // Ограничиваем от 0 до 1
}

void GlitchRenderer::setCyberpunkSquares(bool enabled) {
    cyberpunkSquaresEnabled = enabled;
}

void GlitchRenderer::setAnalogGlitch(bool enabled) {
    analogGlitchEnabled = enabled;
    if (!enabled) {
        analogOffsetX = 0.f;
        analogOffsetY = 0.f;
    }
}

void GlitchRenderer::renderCyberpunkSquares(sf::RenderWindow& window, int squareCount) {
    if (!cyberpunkSquaresEnabled) return;

    auto windowSize = window.getSize();

    // Рендерим только если прошло достаточно времени (создает эффект мелькания)
    if (squareGlitchTimer < 0.03f) { // Квадраты видны только 30мс
        for (int i = 0; i < squareCount; ++i) {
            // Случайный размер квадрата
            float size = 10.f + (rand() % 80);

            // Случайная позиция
            float x = rand() % static_cast<int>(windowSize.x - size);
            float y = rand() % static_cast<int>(windowSize.y - size);

            sf::RectangleShape square(sf::Vector2f(size, size));

            // Случайный цвет в киберпанк стиле
            sf::Color colors[] = {
                sf::Color(255, 0, 255, 180),   // Магента
                sf::Color(0, 255, 255, 180),   // Циан
                sf::Color(255, 255, 0, 180),   // Желтый
                sf::Color(255, 0, 0, 180),     // Красный
                sf::Color(0, 255, 0, 180),     // Зеленый
                sf::Color(255, 255, 255, 160), // Белый
            };

            square.setFillColor(colors[rand() % 6]);
            square.setPosition(sf::Vector2f(x, y));

            window.draw(square);
        }
    }
}

float GlitchRenderer::getRandomOffset(float intensity) const {
    return static_cast<float>((rand() % static_cast<int>(intensity * 2)) - intensity);
}

sf::Color GlitchRenderer::getGlitchColor() const {
    int brightness = 100 + (rand() % 155);
    return sf::Color(brightness, 0, 0);
}