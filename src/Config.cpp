#include "Config.h"
#include <fstream>
#include <iostream>

// NOTE: You need to download json.hpp from https://github.com/nlohmann/json/releases/latest/download/json.hpp
// and place it in the lib/ directory
#include "../lib/json.hpp"

using json = nlohmann::json;

// Initialize static members with default values
int Config::defaultInterval = 60;  // 60 minutes default
std::string Config::logFilePath = "logs/fqdn_blocker.log";
std::string Config::auditStorePath = "data/audit_store.json";

bool Config::Load(const std::string& configPath) {
    try {
        std::ifstream configFile(configPath);
        if (!configFile.is_open()) {
            std::cerr << "Configuration file not found, using defaults: " << configPath << std::endl;
            return false;
        }

        json configJson;
        configFile >> configJson;
        configFile.close();

        // Load configuration values
        if (configJson.contains("defaultInterval")) {
            defaultInterval = configJson["defaultInterval"];
        }
        if (configJson.contains("logFilePath")) {
            logFilePath = configJson["logFilePath"];
        }
        if (configJson.contains("auditStorePath")) {
            auditStorePath = configJson["auditStorePath"];
        }

        std::cout << "Configuration loaded successfully from: " << configPath << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading configuration: " << e.what() << std::endl;
        return false;
    }
}

bool Config::Save(const std::string& configPath) {
    try {
        json configJson;
        configJson["defaultInterval"] = defaultInterval;
        configJson["logFilePath"] = logFilePath;
        configJson["auditStorePath"] = auditStorePath;

        std::ofstream configFile(configPath);
        if (!configFile.is_open()) {
            std::cerr << "Failed to open configuration file for writing: " << configPath << std::endl;
            return false;
        }

        configFile << configJson.dump(4);  // Pretty print with 4 spaces
        configFile.close();

        std::cout << "Configuration saved successfully to: " << configPath << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error saving configuration: " << e.what() << std::endl;
        return false;
    }
}

int Config::GetDefaultInterval() {
    return defaultInterval;
}

void Config::SetDefaultInterval(int intervalMinutes) {
    defaultInterval = intervalMinutes;
}

std::string Config::GetLogFilePath() {
    return logFilePath;
}

void Config::SetLogFilePath(const std::string& path) {
    logFilePath = path;
}

std::string Config::GetAuditStorePath() {
    return auditStorePath;
}

void Config::SetAuditStorePath(const std::string& path) {
    auditStorePath = path;
}
