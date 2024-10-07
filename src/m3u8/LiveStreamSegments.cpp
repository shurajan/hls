//
// Created by sasha on 05.10.2024.
//

#include "LiveStreamSegments.h"
#include "StreamSegments.h"
#include "network/NetworkClientBase.h"
#include "M3U8Parser.h"
#include "ResolutionWrapper.h"

namespace m3u8 {

    // Реализация метода для получения сегментов
    std::vector<std::string> LiveStreamSegments::get_segments() {
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
}
