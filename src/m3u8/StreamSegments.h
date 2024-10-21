#ifndef STREAMSEGMENTS_H
#define STREAMSEGMENTS_H

#include <string>
#include <vector>
#include "network/NetworkClient.h"
#include "ResolutionWrapper.h"

namespace m3u8 {
    // Базовый класс для сегментов потока
    class StreamSegments {
    public:
        template<typename ResolutionType>
        StreamSegments(network::NetworkClient *network_client,
                       const std::string &masterPlaylistURI,
                       ResolutionType resolution) {
            networkClient = network_client;
            if constexpr (std::is_same<ResolutionType, Resolution>::value) {
                initializeMediaPlaylist(masterPlaylistURI, resolution);
            } else if constexpr (std::is_same<ResolutionType, std::string>::value) {
                initializeMediaPlaylist(masterPlaylistURI, resolution);
            } else if constexpr (std::is_same<ResolutionType, const char *>::value) {
                initializeMediaPlaylist(masterPlaylistURI, std::string(resolution));
            } else {
                static_assert(std::is_same<ResolutionType, Resolution>::value ||
                              std::is_same<ResolutionType, std::string>::value ||
                              std::is_same<ResolutionType, const char *>::value,
                              "Unsupported resolution type");
            }
        }
        ~StreamSegments() = default;

        // Виртуальная функция для получения ссылок на сегменты
        std::vector<std::string> get_segments();
    private:
        std::string baseURL; // Базовая ссылка, извлеченная из мастер-листа
        std::string mediaPlaylistURI; // Ссылка на медиаплейлист
        std::string mediaPlaylistContent; // Содержимое медиаплейлиста
        network::NetworkClient *networkClient; // Указатель на сетевой клиент

        // Функция для извлечения базовой части URL (до последнего "/")
        std::string getBaseURL(const std::string &url);

        // Проверка, является ли это полный URL или относительный путь (chunk)
        bool isFullURL(const std::string &uri);

        // Логика парсинга мастер-листа и выбор медиаплейлиста на основе разрешения
        void initializeMediaPlaylist(const std::string &masterPlaylistURI, Resolution resolution);

        // Логика парсинга мастер-листа и выбор медиаплейлиста на основе разрешения
        void initializeMediaPlaylist(const std::string &masterPlaylistURI, const std::string &resolution);
    };
}

#endif // STREAMSEGMENTS_H
