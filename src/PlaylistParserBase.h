//
// Created by sasha on 10/3/24.
//

#ifndef PLAYLIST_PARSER_BASE_H
#define PLAYLIST_PARSER_BASE_H

#include <string>
#include <map>
#include <NetworkClientBase.h>
#include <vector>

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

// Абстрактный класс для анализа плейлистов
class PlaylistParserBase {
public:
    PlaylistParserBase(network::NetworkClientBase* client) : client_(client) {}
    virtual ~PlaylistParserBase() = default;

    // Виртуальный метод для разбора плейлиста
    virtual std::vector<std::string>  parse_m3u8_playlist(const std::string& playlist, Resolution resolution) = 0;

protected:
    network::NetworkClientBase* client_;  // Указатель на сетевой клиент
    // Метод для получения карты разрешений
    std::map<Resolution, int> get_resolutions_map() const {
        return {
                {Resolution::P144, 144},
                {Resolution::P240, 240},
                {Resolution::P360, 360},
                {Resolution::P480, 480},
                {Resolution::P540, 540},
                {Resolution::P720, 720},
                {Resolution::P1080, 1080},
                {Resolution::P1440, 1440},
                {Resolution::P2160, 2160}
        };
    }
};

#endif // PLAYLIST_PARSER_BASE_H
