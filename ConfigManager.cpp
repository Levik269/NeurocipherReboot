#include "ConfigManager.h"
#include <fstream>
#include <sstream>

GameConfig ConfigManager::load() {
    GameConfig cfg;
    std::ifstream in("config.cfg");
    if (!in.is_open()) return cfg;

    std::string key;
    while (in >> key) {
        if (key == "width") in >> cfg.width;
        else if (key == "height") in >> cfg.height;
        else if (key == "fullscreen") in >> cfg.fullscreen;
        else if (key == "vsync") in >> cfg.vsync;
    }
    return cfg;
}

void ConfigManager::save(const GameConfig& cfg) {
    std::ofstream out("config.cfg");
    out << "width " << cfg.width << "\n"
        << "height " << cfg.height << "\n"
        << "fullscreen " << (cfg.fullscreen ? 1 : 0) << "\n"
        << "vsync " << (cfg.vsync ? 1 : 0) << "\n";
}
