#ifndef STREAMSEGMENTS_H
#define STREAMSEGMENTS_H

#include <string>
#include <vector>
#include "network/NetworkClientBase.h"
#include "ResolutionWrapper.h"

namespace m3u8 {

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

        // Логика парсинга мастер-листа и выбор медиаплейлиста на основе разрешения
        void initializeMediaPlaylist(const std::string &masterPlaylistURI, const std::string &resolution);

    public:
        StreamSegments(network::NetworkClientBase* network_client) : networkClient(network_client) {}
        virtual ~StreamSegments() = default;

        // Виртуальная функция для получения ссылок на сегменты
        virtual std::vector<std::string> get_segments() = 0;
    };

}

#endif // STREAMSEGMENTS_H
