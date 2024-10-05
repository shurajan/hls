//
// Created by sasha on 05.10.2024.
//

#ifndef FIXEDSTREAMSEGMENTS_H
#define FIXEDSTREAMSEGMENTS_H

#include "StreamSegments.h"

namespace m3u8 {

    class FixedStreamSegments : public StreamSegments {
    public:
        // Конструктор принимает ссылку на мастер-лист, разрешение и сетевой клиент
        FixedStreamSegments(const std::string &masterPlaylistURI, Resolution resolution, network::NetworkClientBase* network_client);

        // Реализация метода для получения сегментов
        std::vector<std::string> get_segments() override;
    };

}

#endif //FIXEDSTREAMSEGMENTS_H
