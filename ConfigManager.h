// ConfigManager.h
#pragma once
#include "Config.h"
#include <vector>

class ConfigManager {
public:
    static GameConfig load();
    static void save(const GameConfig& cfg);
};