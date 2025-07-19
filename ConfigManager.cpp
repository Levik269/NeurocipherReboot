#include "ConfigManager.h"
#include <fstream>
#include <sstream>
const std::vector<Resolution> AVAILABLE_RESOLUTIONS = {
    {1280, 720, "1280x720 (HD)"},
    {1366, 768, "1366x768"},
    {1600, 900, "1600x900"},
    {1920, 1080, "1920x1080 (Full HD)"},
    {2560, 1080, "2560x1080 (UW Full HD)"},
    {2560, 1440, "2560x1440 (QHD)"},
    {3440, 1440, "3440x1440 (UW QHD)"},
    {3840, 1600, "3840x1600 (UW QHD+)"},
    {3840, 2160, "3840x2160 (4K)"},
    {5120, 1440, "5120x1440 (Super UW)"}
};
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
