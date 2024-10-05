//
// Created by sasha on 05.10.2024.
//

#ifndef M3U8PARSER_H
#define M3U8PARSER_H

#include <string>
#include <vector>

namespace m3u8 {

    // Структура для хранения информации о потоке в Master Playlist
    struct StreamInfo {
        int bandwidth;
        std::string name;
        std::string codecs;
        std::string resolution;
        std::string uri;
    };

    // Структура для хранения информации о сегментах в Media Playlist
    struct SegmentInfo {
        float duration;
        std::string uri;
    };

    // Класс для работы с Master Playlist
    class MasterPlaylist {
    private:
        std::vector<StreamInfo> streams; // Список потоков

    public:
        // Парсинг мастер-плейлиста
        void parse(const std::string& m3u8_content);

        // Получение всех потоков
        const std::vector<StreamInfo>& getStreams() const;

        // Печать списка потоков (для отладки)
        void print() const;
    };

    // Класс для работы с Media Playlist
    class MediaPlaylist {
    private:
        std::vector<SegmentInfo> segments; // Список сегментов

    public:
        // Парсинг медиаплейлиста
        void parse(const std::string& m3u8_content);

        // Получение всех сегментов
        const std::vector<SegmentInfo>& getSegments() const;

        // Печать списка сегментов (для отладки)
        void print() const;
    };

} // namespace m3u8

#endif //M3U8PARSER_H
