//
// Created by Alexander Bralnin on 23.10.2024.
//

// AppConfig.h
#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

struct MtlsConfig {
    std::string clientCertPath;
    std::string clientKeyPath;
    std::string caCertPath;
};

class AppConfig {
public:
    explicit AppConfig(const std::string& configFile);

    // Возвращает mTLS конфигурацию
    const MtlsConfig& getMtlsConfig() const;

    // Возвращает список URL-адресов
    const std::vector<std::string>& getUrlList() const;

private:
    MtlsConfig mtlsConfig;
    std::vector<std::string> urlList;

    // Функция для загрузки конфигурации из JSON-файла
    void loadConfig(const std::string& filename);
};

#endif // APPCONFIG_H
