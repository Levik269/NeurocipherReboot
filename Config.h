// Config.h
#pragma once
#include <SFML/Window.hpp>

struct GameConfig {
    unsigned int width = 1280;
    unsigned int height = 720;
    bool fullscreen = false;
    bool vsync = true;
};

struct Resolution {
    unsigned int width;
    unsigned int height;
    std::string name;
};

extern const std::vector<Resolution> AVAILABLE_RESOLUTIONS;