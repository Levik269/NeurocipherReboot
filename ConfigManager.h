// ConfigManager.h
#pragma once
#include "Config.h"

class ConfigManager {
public:
    static GameConfig load();
    static void save(const GameConfig& cfg);
};