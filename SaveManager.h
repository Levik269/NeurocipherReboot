// SaveManager.h
#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <chrono>

struct PlayerData {
    std::string name;
    int level = 1;
    int experience = 0;
    int health = 100;
    int maxHealth = 100;
    int strength = 10;
    int intelligence = 10;
    int agility = 10;
    float playtime = 0.0f; // в часах

    // Методы для сериализации
    std::string serialize() const;
    void deserialize(const std::string& data);
};

struct SaveSlot {
    std::string slotName;
    std::string playerName;
    std::string creationDate;
    std::string lastPlayedDate;
    float playtime;
    int level;
    std::filesystem::path savePath;
    bool isValid;

    SaveSlot() : level(1), playtime(0.0f), isValid(false) {}
};

class SaveManager {
public:
    SaveManager();

    // Основные методы
    bool createNewSave(const std::string& saveName, const PlayerData& playerData);
    bool loadSave(const std::string& saveName, PlayerData& playerData);
    bool saveCurrent(const std::string& saveName, const PlayerData& playerData);
    bool deleteSave(const std::string& saveName);

    // Получение информации о сохранениях
    std::vector<SaveSlot> getAllSaveSlots();
    bool saveExists(const std::string& saveName);

    // Утилиты
    std::string generateUniqueSaveName(const std::string& baseName);
    bool isValidSaveName(const std::string& name);

    // Константы
    static const std::string SAVES_DIRECTORY;
    static const std::string PLAYER_DATA_FILE;
    static const std::string GAME_DATA_FILE;
    static const std::string SETTINGS_FILE;

private:
    std::filesystem::path savesPath;

    // Вспомогательные методы
    bool createSaveDirectory(const std::string& saveName);
    bool createDefaultFiles(const std::filesystem::path& savePath, const PlayerData& playerData);
    std::string getCurrentTimeString();
    SaveSlot createSaveSlot(const std::filesystem::path& savePath);
    bool validateSaveDirectory(const std::filesystem::path& savePath);
};