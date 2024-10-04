
#include "PlaylistParserSiteCbr.h"
#include <regex>
#include <sstream>
#include <iostream>
#include <limits>
#include <vector>
#include <stdexcept>

namespace m3u8 {
// Вспомогательная функция для удаления суффикса "playlist.m3u8" из URL и получения базового URL
std::string get_base_url(const std::string& playlist_url) {
    std::string suffix = "playlist.m3u8";
    std::size_t pos = playlist_url.rfind(suffix);
    if (pos != std::string::npos) {
        return playlist_url.substr(0, pos); // Обрезаем строку до "playlist.m3u8"
    }
    return playlist_url; // Если суффикс не найден, возвращаем оригинальный URL
}

// Функция для парсинга сегментов .ts из содержимого плейлиста
std::vector<std::string> parse_ts_segments(const std::string& playlist_content, const std::string& base_url) {
    std::vector<std::string> segment_urls;
    std::istringstream stream(playlist_content);
    std::string line;

    while (std::getline(stream, line)) {
        // Ищем строки, которые содержат .ts (это сегменты)
        if (line.find(".ts") != std::string::npos) {
            // Объединяем базовый URL с относительным путем к сегменту
            segment_urls.push_back(base_url + line);
        }
    }

    return segment_urls;
}

//Конструктор
PlaylistParserSiteCbr::PlaylistParserSiteCbr(network::NetworkClientBase* client)
    : PlaylistParserBase(client) {
}

// Основная функция парсинга плейлиста
std::vector<std::string> PlaylistParserSiteCbr::parse_m3u8_playlist(const std::string& playlist_url, Resolution resolution) {

    std::string playlist_content;
    try {
        playlist_content = client_->fetch(playlist_url); // Загрузка плейлиста по URL
        std::cout << "Playlist content: " << playlist_content << std::endl;
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to fetch playlist: " + std::string(e.what()));
    }

    // Получаем базовый URL, удаляя "playlist.m3u8"
    std::string base_url = get_base_url(playlist_url);

    // Регулярное выражение для поиска строк с разрешением и URL
    std::regex stream_regex(R"#(RESOLUTION=(\d+)x(\d+))#");
    std::smatch match;
    std::vector<std::pair<int, std::string>> streams;
    std::string selected_playlist_url;
    int best_quality = (resolution == Resolution::Max) ? 0 : std::numeric_limits<int>::max();

    // Проходим по всем строкам плейлиста
    std::istringstream stream(playlist_content);
    std::string line;
    while (std::getline(stream, line)) {
        if (std::regex_search(line, match, stream_regex)) {
            int height = std::stoi(match[2]);

            if (resolution != Resolution::Max && resolution != Resolution::Min && height == get_resolutions_map()[resolution]) {
                std::getline(stream, line); // Следующая строка — это относительный URL
                selected_playlist_url = base_url + line;
                break;
            }

            streams.emplace_back(height, line);
        }
    }

    // Если задано Max или Min, выбираем соответствующий URL
    if (selected_playlist_url.empty()) {
        if (resolution == Resolution::Max) {
            for (const auto& [height, url] : streams) {
                if (height > best_quality) {
                    best_quality = height;
                    selected_playlist_url = base_url + url;
                }
            }
        } else if (resolution == Resolution::Min) {
            for (const auto& [height, url] : streams) {
                if (height < best_quality) {
                    best_quality = height;
                    selected_playlist_url = base_url + url;
                }
            }
        }
    }

    // Загружаем новый плейлист (в котором содержатся ссылки на .ts файлы)
    std::string new_playlist_content;
    try {
        new_playlist_content = client_->fetch(selected_playlist_url); // Загружаем следующий плейлист
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to fetch .ts playlist: " + std::string(e.what()));
    }

    // Парсим .ts сегменты из нового плейлиста
    return parse_ts_segments(new_playlist_content, base_url);
}
}
