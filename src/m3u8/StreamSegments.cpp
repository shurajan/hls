#include "StreamSegments.h"
#include "M3U8Parser.h"
#include "ResolutionWrapper.h"
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
    bool StreamSegments::isFullURL(const std::string &uri) {
        return uri.find("http://") == 0 || uri.find("https://") == 0;
    }

    // Реализация метода initializeMediaPlaylist
    void StreamSegments::initializeMediaPlaylist(const std::string &masterPlaylistURI, Resolution resolution) {
        baseURL = getBaseURL(masterPlaylistURI); // Извлекаем базовый URL

        // Получаем содержимое мастер-листа через сетевой клиент
        std::string masterPlaylistContent = networkClient->fetch(masterPlaylistURI);

        // Парсинг мастер-листа
        MasterPlaylist masterParser;
        masterParser.parse(masterPlaylistContent);

        // Поиск максимального и минимального разрешений
        std::map<int, std::string> resolutionMap;
        for (const auto &stream: masterParser.getStreams()) {
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
            std::string targetResolution = ResolutionWrapper::toString(resolution);
            for (const auto &stream: masterParser.getStreams()) {
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

    void StreamSegments::initializeMediaPlaylist(const std::string &masterPlaylistURI, const std::string &resolution) {
        Resolution resolutionValue = ResolutionWrapper::fromString(resolution);
        this->initializeMediaPlaylist(masterPlaylistURI, resolutionValue);
    }

    std::vector<std::string> StreamSegments::get_segments() {
        std::vector<std::string> segmentURIs;

        // Обновляем медиаплейлист для live потоков
        mediaPlaylistContent = networkClient->fetch(mediaPlaylistURI);

        // Парсинг медиаплейлиста
        MediaPlaylist mediaParser;
        mediaParser.parse(mediaPlaylistContent);

        // Строим полный URL для каждого сегмента, если это не полный URL
        for (const auto &segment: mediaParser.getSegments()) {
            if (!isFullURL(segment.uri)) {
                segmentURIs.push_back(baseURL + segment.uri); // Полный URL на сегмент
            } else {
                segmentURIs.push_back(segment.uri); // Если это полный URL, добавляем его напрямую
            }
        }

        return segmentURIs;
    }
} // namespace m3u8
