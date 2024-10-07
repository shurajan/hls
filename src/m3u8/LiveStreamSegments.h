//
// Created by sasha on 05.10.2024.
//

#ifndef LIVESTREAMSEGMENTS_H
#define LIVESTREAMSEGMENTS_H

#include "StreamSegments.h"

namespace m3u8 {
    class LiveStreamSegments : public StreamSegments {
    public:
        template<typename ResolutionType>
        LiveStreamSegments(network::NetworkClientBase *network_client, const std::string &masterPlaylistURI, ResolutionType resolution)
            : StreamSegments(network_client) {

            if constexpr (std::is_same<ResolutionType, Resolution>::value) {
                initializeMediaPlaylist(masterPlaylistURI, resolution);
            } else if constexpr (std::is_same<ResolutionType, std::string>::value) {
                initializeMediaPlaylist(masterPlaylistURI, resolution);
            } else if constexpr (std::is_same<ResolutionType, const char*>::value) {
                initializeMediaPlaylist(masterPlaylistURI, std::string(resolution));
            } else {
                static_assert(std::is_same<ResolutionType, Resolution>::value ||
                              std::is_same<ResolutionType, std::string>::value ||
                              std::is_same<ResolutionType, const char*>::value,
                              "Unsupported resolution type");
            }
        }


        std::vector<std::string> get_segments() override;
    };
}
#endif //LIVESTREAMSEGMENTS_H
