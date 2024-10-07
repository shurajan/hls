//
// Created by sasha on 05.10.2024.
//

#ifndef FIXEDSTREAMSEGMENTS_H
#define FIXEDSTREAMSEGMENTS_H

#include "StreamSegments.h"

namespace m3u8 {

    class FixedStreamSegments : public StreamSegments {
    public:
        template<typename ResolutionType>
        FixedStreamSegments(network::NetworkClientBase *network_client, const std::string &masterPlaylistURI, ResolutionType resolution)
            : StreamSegments(network_client) {

            if constexpr (std::is_same<ResolutionType, Resolution>::value) {
                // Логика для enum class Resolution
                initializeMediaPlaylist(masterPlaylistURI, resolution);
            } else if constexpr (std::is_same<ResolutionType, std::string>::value) {
                // Логика для строки (const std::string&)
                initializeMediaPlaylist(masterPlaylistURI, resolution);
            } else if constexpr (std::is_same<ResolutionType, const char*>::value) {
                // Преобразуем строковый литерал в std::string и вызываем соответствующую логику
                initializeMediaPlaylist(masterPlaylistURI, std::string(resolution));
            } else {
                static_assert(std::is_same<ResolutionType, Resolution>::value ||
                              std::is_same<ResolutionType, std::string>::value ||
                              std::is_same<ResolutionType, const char*>::value,
                              "Unsupported resolution type");
            }
        }
        // Реализация метода для получения сегментов
        std::vector<std::string> get_segments() override;
    };

}

#endif //FIXEDSTREAMSEGMENTS_H
