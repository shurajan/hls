#include "StreamSegments.h"
#include "M3U8Parser.h"
#include <iostream>
#include <map>
#include <stdexcept>

namespace m3u8 {

// Функция для извлечения базовой части URL (до последнего "/")
std::string StreamSegments::getBaseURL(const std::string &url) {
    auto pos = url.find_last_of('/');
    if (pos != std::string::npos) {
        return url.substr(0, pos + 1); // Оставляем "/" в конце для удобства
    }
    return url;
}

// Проверка, является ли это полный URL или относительный путь (chunk)
bool StreamSegments::isFullURL(const std::string& uri) {
    return uri.find("http://") == 0 || uri.find("https://") == 0;
}

// Преобразование разрешения в строку
std::string StreamSegments::resolutionToString(Resolution resolution) {
    switch (resolution) {
        case Resolution::P144: return "256x144";
        case Resolution::P240: return "426x240";
        case Resolution::P360: return "640x360";
        case Resolution::P480: return "854x480";
        case Resolution::P540: return "960x540";
        case Resolution::P720: return "1280x720";
        case Resolution::P1080: return "1920x1080";
        case Resolution::P1440: return "2560x1440";
        case Resolution::P2160: return "3840x2160";
        default: return "";
    }
}

// Преобразование строки в разрешение
Resolution StreamSegments::stringToResolution(const std::string& resolutionStr) {
    if (resolutionStr == "256x144") return Resolution::P144;
    if (resolutionStr == "426x240") return Resolution::P240;
    if (resolutionStr == "640x360") return Resolution::P360;
    if (resolutionStr == "854x480") return Resolution::P480;
    if (resolutionStr == "960x540") return Resolution::P540;
    if (resolutionStr == "1280x720") return Resolution::P720;
    if (resolutionStr == "1920x1080") return Resolution::P1080;
    if (resolutionStr == "2560x1440") return Resolution::P1440;
    if (resolutionStr == "3840x2160") return Resolution::P2160;
    return Resolution::Min;  // По умолчанию, если не найдено, возвращаем минимальное разрешение
}

// Реализация метода initializeMediaPlaylist
void StreamSegments::initializeMediaPlaylist(const std::string &masterPlaylistURI, Resolution resolution) {
    baseURL = getBaseURL(masterPlaylistURI);  // Извлекаем базовый URL

    // Получаем содержимое мастер-листа через сетевой клиент
    std::string masterPlaylistContent = networkClient->fetch(masterPlaylistURI);

    // Парсинг мастер-листа
    MasterPlaylist masterParser;
    masterParser.parse(masterPlaylistContent);

    // Поиск максимального и минимального разрешений
    std::map<int, std::string> resolutionMap;
    for (const auto& stream : masterParser.getStreams()) {
        std::string resString = stream.resolution;
        int width = std::stoi(resString.substr(0, resString.find('x')));
        int height = std::stoi(resString.substr(resString.find('x') + 1));
        resolutionMap[width * height] = stream.uri;
    }

    // Логика выбора медиаплейлиста в зависимости от разрешения
    if (resolution == Resolution::Max) {
        mediaPlaylistURI = resolutionMap.rbegin()->second; // Максимальное разрешение
    } else if (resolution == Resolution::Min) {
        mediaPlaylistURI = resolutionMap.begin()->second; // Минимальное разрешение
    } else {
        std::string targetResolution = resolutionToString(resolution);
        for (const auto& stream : masterParser.getStreams()) {
            if (stream.resolution == targetResolution) {
                mediaPlaylistURI = stream.uri;
                break;
            }
        }
    }

    if (mediaPlaylistURI.empty()) {
        throw std::runtime_error("Не удалось найти плейлист с заданным разрешением.");
    }

    // Строим полную ссылку на медиаплейлист, если это не полный URL
    if (!isFullURL(mediaPlaylistURI)) {
        mediaPlaylistURI = baseURL + mediaPlaylistURI;
    }

    // Получаем содержимое медиаплейлиста через сетевой клиент
    mediaPlaylistContent = networkClient->fetch(mediaPlaylistURI);
}

} // namespace m3u8
