#include "SaveManager.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <regex>

// Константы для файловой системы
const std::string SaveManager::SAVES_DIRECTORY = "saves";
const std::string SaveManager::PLAYER_DATA_FILE = "player.dat";
const std::string SaveManager::GAME_DATA_FILE = "game.dat";
const std::string SaveManager::SETTINGS_FILE = "settings.dat";

// Реализация PlayerData
std::string PlayerData::serialize() const {
    std::stringstream ss;
    ss << "name=" << name << "\n";
    ss << "level=" << level << "\n";
    ss << "experience=" << experience << "\n";
    ss << "health=" << health << "\n";
    ss << "maxHealth=" << maxHealth << "\n";
    ss << "strength=" << strength << "\n";
    ss << "intelligence=" << intelligence << "\n";
    ss << "agility=" << agility << "\n";
    ss << "playtime=" << std::fixed << std::setprecision(2) << playtime << "\n";
    return ss.str();
}

void PlayerData::deserialize(const std::string& data) {
    std::istringstream iss(data);
    std::string line;

    while (std::getline(iss, line)) {
        size_t pos = line.find('=');
        if (pos == std::string::npos) continue;

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        if (key == "name") name = value;
        else if (key == "level") level = std::stoi(value);
        else if (key == "experience") experience = std::stoi(value);
        else if (key == "health") health = std::stoi(value);
        else if (key == "maxHealth") maxHealth = std::stoi(value);
        else if (key == "strength") strength = std::stoi(value);
        else if (key == "intelligence") intelligence = std::stoi(value);
        else if (key == "agility") agility = std::stoi(value);
        else if (key == "playtime") playtime = std::stof(value);
    }
}

// Реализация SaveManager
SaveManager::SaveManager() {
    savesPath = std::filesystem::current_path() / SAVES_DIRECTORY;

    // Создаем директорию сохранений если она не существует
    try {
        if (!std::filesystem::exists(savesPath)) {
            std::filesystem::create_directories(savesPath);
            std::cout << "Created saves directory: " << savesPath << std::endl;
        }
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error creating saves directory: " << e.what() << std::endl;
    }
}

bool SaveManager::createNewSave(const std::string& saveName, const PlayerData& playerData) {
    if (!isValidSaveName(saveName)) {
        std::cerr << "Invalid save name: " << saveName << std::endl;
        return false;
    }

    if (saveExists(saveName)) {
        std::cerr << "Save already exists: " << saveName << std::endl;
        return false;
    }

    try {
        // Создаем директорию для сохранения
        if (!createSaveDirectory(saveName)) {
            return false;
        }

        std::filesystem::path saveDir = savesPath / saveName;

        // Создаем файлы по умолчанию
        if (!createDefaultFiles(saveDir, playerData)) {
            // Если не удалось создать файлы, удаляем директорию
            std::filesystem::remove_all(saveDir);
            return false;
        }

        std::cout << "Successfully created new save: " << saveName << std::endl;
        return true;

    }
    catch (const std::exception& e) {
        std::cerr << "Error creating save: " << e.what() << std::endl;
        return false;
    }
}

bool SaveManager::loadSave(const std::string& saveName, PlayerData& playerData) {
    if (!saveExists(saveName)) {
        std::cerr << "Save does not exist: " << saveName << std::endl;
        return false;
    }

    try {
        std::filesystem::path playerFile = savesPath / saveName / PLAYER_DATA_FILE;

        std::ifstream file(playerFile);
        if (!file.is_open()) {
            std::cerr << "Cannot open player data file: " << playerFile << std::endl;
            return false;
        }

        std::string content((std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>());
        file.close();

        playerData.deserialize(content);

        // Обновляем время последнего доступа
        std::filesystem::path settingsFile = savesPath / saveName / SETTINGS_FILE;
        std::ofstream settings(settingsFile, std::ios::app);
        if (settings.is_open()) {
            settings << "lastPlayed=" << getCurrentTimeString() << "\n";
            settings.close();
        }

        std::cout << "Successfully loaded save: " << saveName << std::endl;
        return true;

    }
    catch (const std::exception& e) {
        std::cerr << "Error loading save: " << e.what() << std::endl;
        return false;
    }
}

bool SaveManager::saveCurrent(const std::string& saveName, const PlayerData& playerData) {
    if (!saveExists(saveName)) {
        std::cerr << "Save does not exist: " << saveName << std::endl;
        return false;
    }

    try {
        std::filesystem::path playerFile = savesPath / saveName / PLAYER_DATA_FILE;

        std::ofstream file(playerFile);
        if (!file.is_open()) {
            std::cerr << "Cannot open player data file for writing: " << playerFile << std::endl;
            return false;
        }

        file << playerData.serialize();
        file.close();

        // Обновляем время последнего сохранения
        std::filesystem::path settingsFile = savesPath / saveName / SETTINGS_FILE;
        std::ofstream settings(settingsFile, std::ios::app);
        if (settings.is_open()) {
            settings << "lastSaved=" << getCurrentTimeString() << "\n";
            settings.close();
        }

        std::cout << "Successfully saved: " << saveName << std::endl;
        return true;

    }
    catch (const std::exception& e) {
        std::cerr << "Error saving: " << e.what() << std::endl;
        return false;
    }
}

bool SaveManager::deleteSave(const std::string& saveName) {
    if (!saveExists(saveName)) {
        std::cerr << "Save does not exist: " << saveName << std::endl;
        return false;
    }

    try {
        std::filesystem::path saveDir = savesPath / saveName;
        std::filesystem::remove_all(saveDir);

        std::cout << "Successfully deleted save: " << saveName << std::endl;
        return true;

    }
    catch (const std::exception& e) {
        std::cerr << "Error deleting save: " << e.what() << std::endl;
        return false;
    }
}

std::vector<SaveSlot> SaveManager::getAllSaveSlots() {
    std::vector<SaveSlot> slots;

    try {
        if (!std::filesystem::exists(savesPath)) {
            return slots;
        }

        for (const auto& entry : std::filesystem::directory_iterator(savesPath)) {
            if (entry.is_directory()) {
                SaveSlot slot = createSaveSlot(entry.path());
                if (slot.isValid) {
                    slots.push_back(slot);
                }
            }
        }

        // Сортируем по времени последней игры (новые первыми)
        std::sort(slots.begin(), slots.end(), [](const SaveSlot& a, const SaveSlot& b) {
            return a.lastPlayedDate > b.lastPlayedDate;
            });

    }
    catch (const std::exception& e) {
        std::cerr << "Error scanning saves: " << e.what() << std::endl;
    }

    return slots;
}

bool SaveManager::saveExists(const std::string& saveName) {
    std::filesystem::path saveDir = savesPath / saveName;
    return std::filesystem::exists(saveDir) &&
        std::filesystem::is_directory(saveDir) &&
        validateSaveDirectory(saveDir);
}

std::string SaveManager::generateUniqueSaveName(const std::string& baseName) {
    std::string cleanName = baseName;

    // Очищаем имя от недопустимых символов
    std::regex invalid_chars(R"([<>:"/\\|?*])");
    cleanName = std::regex_replace(cleanName, invalid_chars, "_");

    if (cleanName.empty()) {
        cleanName = "NewSave";
    }

    // Если имя не занято, возвращаем его
    if (!saveExists(cleanName)) {
        return cleanName;
    }

    // Иначе добавляем числовой суффикс
    int counter = 1;
    std::string uniqueName;
    do {
        uniqueName = cleanName + "_" + std::to_string(counter);
        counter++;
    } while (saveExists(uniqueName) && counter < 1000);

    return uniqueName;
}

bool SaveManager::isValidSaveName(const std::string& name) {
    if (name.empty() || name.length() > 50) {
        return false;
    }

    // Проверяем на недопустимые символы
    std::regex invalid_chars(R"([<>:"/\\|?*])");
    if (std::regex_search(name, invalid_chars)) {
        return false;
    }

    // Проверяем на зарезервированные имена Windows
    std::vector<std::string> reserved = { "CON", "PRN", "AUX", "NUL",
                                        "COM1", "COM2", "COM3", "COM4",
                                        "LPT1", "LPT2", "LPT3", "LPT4" };

    std::string upperName = name;
    std::transform(upperName.begin(), upperName.end(), upperName.begin(), ::toupper);

    for (const auto& res : reserved) {
        if (upperName == res) {
            return false;
        }
    }

    return true;
}

bool SaveManager::createSaveDirectory(const std::string& saveName) {
    try {
        std::filesystem::path saveDir = savesPath / saveName;
        return std::filesystem::create_directory(saveDir);
    }
    catch (const std::exception& e) {
        std::cerr << "Error creating save directory: " << e.what() << std::endl;
        return false;
    }
}

bool SaveManager::createDefaultFiles(const std::filesystem::path& savePath, const PlayerData& playerData) {
    try {
        // Создаем файл данных игрока
        std::ofstream playerFile(savePath / PLAYER_DATA_FILE);
        if (!playerFile.is_open()) {
            return false;
        }
        playerFile << playerData.serialize();
        playerFile.close();

        // Создаем файл игровых данных
        std::ofstream gameFile(savePath / GAME_DATA_FILE);
        if (!gameFile.is_open()) {
            return false;
        }
        gameFile << "# Game data file\n";
        gameFile << "version=1.0\n";
        gameFile << "created=" << getCurrentTimeString() << "\n";
        gameFile.close();

        // Создаем файл настроек сохранения
        std::ofstream settingsFile(savePath / SETTINGS_FILE);
        if (!settingsFile.is_open()) {
            return false;
        }
        settingsFile << "# Save settings\n";
        settingsFile << "created=" << getCurrentTimeString() << "\n";
        settingsFile << "lastPlayed=" << getCurrentTimeString() << "\n";
        settingsFile.close();

        return true;

    }
    catch (const std::exception& e) {
        std::cerr << "Error creating default files: " << e.what() << std::endl;
        return false;
    }
}

std::string SaveManager::getCurrentTimeString() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

SaveSlot SaveManager::createSaveSlot(const std::filesystem::path& savePath) {
    SaveSlot slot;
    slot.savePath = savePath;
    slot.slotName = savePath.filename().string();

    try {
        // Проверяем валидность директории сохранения
        if (!validateSaveDirectory(savePath)) {
            return slot;
        }

        // Читаем данные игрока
        std::ifstream playerFile(savePath / PLAYER_DATA_FILE);
        if (playerFile.is_open()) {
            std::string content((std::istreambuf_iterator<char>(playerFile)),
                std::istreambuf_iterator<char>());
            playerFile.close();

            PlayerData tempData;
            tempData.deserialize(content);
            slot.playerName = tempData.name;
            slot.level = tempData.level;
            slot.playtime = tempData.playtime;
        }

        // Читаем настройки сохранения
        std::ifstream settingsFile(savePath / SETTINGS_FILE);
        if (settingsFile.is_open()) {
            std::string line;
            while (std::getline(settingsFile, line)) {
                size_t pos = line.find('=');
                if (pos == std::string::npos) continue;

                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);

                if (key == "created") {
                    slot.creationDate = value;
                }
                else if (key == "lastPlayed") {
                    slot.lastPlayedDate = value;
                }
            }
            settingsFile.close();
        }

        slot.isValid = true;

    }
    catch (const std::exception& e) {
        std::cerr << "Error reading save slot: " << e.what() << std::endl;
    }

    return slot;
}

bool SaveManager::validateSaveDirectory(const std::filesystem::path& savePath) {
    return std::filesystem::exists(savePath / PLAYER_DATA_FILE) &&
        std::filesystem::exists(savePath / GAME_DATA_FILE) &&
        std::filesystem::exists(savePath / SETTINGS_FILE);
}