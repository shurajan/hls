//
// Created by sasha on 05.10.2024.
//

#ifndef LIVESTREAMSEGMENTS_H
#define LIVESTREAMSEGMENTS_H

#include "StreamSegments.h"

namespace m3u8 {

    class LiveStreamSegments : public StreamSegments {
    public:
        // Конструктор принимает ссылку на мастер-лист, разрешение и сетевой клиент
        LiveStreamSegments(const std::string &masterPlaylistURI, Resolution resolution, network::NetworkClientBase* network_client);

        // Реализация метода для получения сегментов
        std::vector<std::string> get_segments() override;
    };
}
#endif //LIVESTREAMSEGMENTS_H
