//
// Created by Alexander Bralnin on 23.10.2024.
//
// AppConfig.cpp
#include "AppConfig.h"
#include <fstream>
#include <stdexcept>

using json = nlohmann::json;

AppConfig::AppConfig(const std::string& configFile) {
    loadConfig(configFile);
}

const MtlsConfig& AppConfig::getMtlsConfig() const {
    return mtlsConfig;
}

const std::vector<std::string>& AppConfig::getUrlList() const {
    return urlList;
}

void AppConfig::loadConfig(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Unable to open configuration file: " + filename);
    }

    json j;
    file >> j;

    // Проверяем наличие mTLS-конфигурации
    if (!j.contains("mtlsConfig") || !j["mtlsConfig"].is_object()) {
        throw std::runtime_error("Invalid or missing 'mtlsConfig' in configuration file: " + filename);
    }
    mtlsConfig.clientCertPath = j["mtlsConfig"].value("clientCertPath", "");
    mtlsConfig.clientKeyPath = j["mtlsConfig"].value("clientKeyPath", "");
    mtlsConfig.caCertPath = j["mtlsConfig"].value("caCertPath", "");

    if (mtlsConfig.clientCertPath.empty() || mtlsConfig.clientKeyPath.empty() || mtlsConfig.caCertPath.empty()) {
        throw std::runtime_error("Incomplete 'mtlsConfig' in configuration file: " + filename);
    }

    // Проверяем наличие списка URL-адресов
    if (!j.contains("urlList") || !j["urlList"].is_array()) {
        throw std::runtime_error("Invalid or missing 'urlList' in configuration file: " + filename);
    }

    urlList = j["urlList"].get<std::vector<std::string>>();
}