#ifndef STREAMSEGMENTS_H
#define STREAMSEGMENTS_H

#include <string>
#include <vector>
#include "network/NetworkClientBase.h"  // Предполагается, что есть такой класс для сетевых запросов

namespace m3u8 {

    // Перечисление разрешений
    enum class Resolution {
        Max,
        Min,
        P144,
        P240,
        P360,
        P480,
        P540,
        P720,
        P1080,
        P1440,
        P2160
    };

    // Базовый класс для сегментов потока
    class StreamSegments {
    protected:
        std::string baseURL;  // Базовая ссылка, извлеченная из мастер-листа
        std::string mediaPlaylistURI;  // Ссылка на медиаплейлист
        std::string mediaPlaylistContent;  // Содержимое медиаплейлиста
        network::NetworkClientBase* networkClient;  // Указатель на сетевой клиент

        // Функция для извлечения базовой части URL (до последнего "/")
        std::string getBaseURL(const std::string &url);

        // Проверка, является ли это полный URL или относительный путь (chunk)
        bool isFullURL(const std::string& uri);

        // Логика парсинга мастер-листа и выбор медиаплейлиста на основе разрешения
        void initializeMediaPlaylist(const std::string &masterPlaylistURI, Resolution resolution);

        // Преобразование разрешения в строку
        std::string resolutionToString(Resolution resolution);

        // Преобразование строки в разрешение
        Resolution stringToResolution(const std::string& resolutionStr);

    public:
        StreamSegments(network::NetworkClientBase* network_client) : networkClient(network_client) {}
        virtual ~StreamSegments() = default;

        // Виртуальная функция для получения ссылок на сегменты
        virtual std::vector<std::string> get_segments() = 0;
    };

}

#endif // STREAMSEGMENTS_H
