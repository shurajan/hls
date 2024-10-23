//
// Created by Alexander Bralnin on 23.10.2024.
//
#ifndef ROOMINFO_H
#define ROOMINFO_H

#include <string>
#include <optional>
#include <nlohmann/json.hpp>
#include <iostream>

// Структура для хранения извлеченных данных
struct RoomInfo {
    std::string roomStatus;
    std::string broadcasterUsername;
    std::string hlsSource;
};

// Функция для обработки JSON-ответа и извлечения нужных полей
inline std::optional<RoomInfo> processJsonResponse(const std::string& jsonResponse) {
    try {
        // Парсинг JSON-ответа
        nlohmann::json parsedResponse = nlohmann::json::parse(jsonResponse);

        // Извлечение значений нужных полей
        RoomInfo info;
        info.roomStatus = parsedResponse.value("room_status", "unknown");
        info.broadcasterUsername = parsedResponse.value("broadcaster_username", "unknown");
        info.hlsSource = parsedResponse.value("hls_source", "not available");

        return info; // Возвращаем заполненный объект
    } catch (const std::exception& e) {
        std::cerr << "Failed to process JSON response: " << e.what() << std::endl;
        return std::nullopt;
    }
}

#endif // ROOMINFO_H
