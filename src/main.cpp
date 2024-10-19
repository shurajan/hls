#include "LiveStreamSegments.h"
#include "StreamDownloader.h"
#include <iostream>
#include <NetworkClient.h>
#include <NetworkClientSocks5.h>
#include <thread>
#include <vector>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp> // Библиотека для работы с JSON

using json = nlohmann::json;

int main() {
    // Инициализация libcurl
    CURLcode global_init = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (global_init != CURLE_OK) {
        std::cerr << "Ошибка инициализации libcurl: " << curl_easy_strerror(global_init) << std::endl;
        return 1;
    }

    // Список URL для загрузки JSON-данных, которые содержат поле hls_source
    std::vector<std::string> json_urls = {
        "url1",
        "url2"
    };

    // Имя выходного файла (можно добавлять суффикс для каждого плейлиста)
    std::string base_output_file = "output_stream";

    // Функция для загрузки потока с плейлиста
    auto download_playlist = [](const std::string& json_url, const std::string& output_file) {
        try {
            // Создаем NetworkClientSocks5 для работы с прокси
            network::NetworkClientSocks5 network_client("127.0.0.1", 1080);

            // Загружаем JSON-ответ
            std::string json_response = network_client.fetch(json_url);
            if (json_response.empty()) {
                std::cerr << "Не удалось загрузить JSON: " << json_url << std::endl;
                return;
            }

            // Парсим JSON и извлекаем поле hls_source
            json parsed_json = json::parse(json_response);
            if (!parsed_json.contains("hls_source")) {
                std::cerr << "Поле hls_source отсутствует в JSON: " << json_url << std::endl;
                return;
            }
            std::string playlist_url = parsed_json["hls_source"].get<std::string>();

            // Создаем объект парсера и загрузчика
            const std::string resolution = "Max";
            m3u8::LiveStreamSegments parser(&network_client, playlist_url, resolution);
            m3u8::StreamDownloader downloader(parser);

            // Запуск загрузки
            downloader.download_stream(playlist_url, output_file);
            std::cout << "Загрузка завершена для: " << playlist_url << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Ошибка при загрузке " << json_url << ": " << e.what() << std::endl;
        }
    };

    // Создаем потоки для загрузки каждого плейлиста
    std::vector<std::thread> threads;
    for (size_t i = 0; i < json_urls.size(); ++i) {
        std::string output_file = base_output_file + std::to_string(i) + ".ts";
        threads.emplace_back(download_playlist, json_urls[i], output_file);
    }

    // Ожидаем завершения всех потоков
    for (auto& thread : threads) {
        thread.join();
    }

    // Освобождение глобальных ресурсов libcurl
    curl_global_cleanup();

    std::cout << "Все загрузки завершены." << std::endl;
    return 0;
}
