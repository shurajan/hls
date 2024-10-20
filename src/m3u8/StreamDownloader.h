#ifndef STREAM_DOWNLOADER_H
#define STREAM_DOWNLOADER_H

#include "StreamSegments.h"
#include <string>
#include <vector>
#include <unordered_set>
#include <fstream>
#include <csignal>
#include <chrono>
#include <thread>
#include <iostream>
#include <curl/curl.h>
#include <atomic>

namespace m3u8 {

class StreamDownloader {
public:
    StreamDownloader(StreamSegments& parser, std::atomic<bool>& stop_signal)
        : parser_(parser), stop_signal_(stop_signal) {}

    void download_stream(const std::string& playlist_url, const std::string& output_file) {
        CURL* curl;
        CURLcode res;
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();

        if (!curl) {
            std::cerr << "Ошибка инициализации cURL" << std::endl;
            return;
        }

        std::ofstream ts_file(output_file, std::ios::binary | std::ios::app);
        if (!ts_file.is_open()) {
            std::cerr << "Ошибка открытия файла для записи" << std::endl;
            return;
        }

        // Множество для отслеживания загруженных сегментов
        std::unordered_set<std::string> downloaded_segments;
        int max_retries = 5;  // Максимальное количество попыток при ошибках

        while (!stop_signal_) {
            // Получаем ссылки на сегменты
            std::vector<std::string> ts_links = parser_.get_segments();

            // Проверка наличия новых сегментов
            if (ts_links.empty()) {
                std::cerr << "Сегменты недоступны. Проверьте доступность плейлиста." << std::endl;
                if (--max_retries <= 0) {
                    std::cerr << "Превышено максимальное количество попыток. Завершение загрузки." << std::endl;
                    break;
                }
                std::this_thread::sleep_for(std::chrono::seconds(2));
                continue;
            }

            // Сброс счетчика попыток при успешном получении сегментов
            max_retries = 5;

            // Загружаем сегменты по очереди
            for (const auto& ts_link : ts_links) {
                if (stop_signal_) break;

                // Пропускаем сегмент, если он уже был загружен
                if (downloaded_segments.find(ts_link) != downloaded_segments.end()) {
                    std::cout << "Сегмент уже загружен: " << ts_link << std::endl;
                    continue;
                }

                // Настройка CURL для загрузки сегмента
                curl_easy_setopt(curl, CURLOPT_URL, ts_link.c_str());
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ts_file);

                // Выполнение запроса
                res = curl_easy_perform(curl);

                if (res != CURLE_OK) {
                    std::cerr << "Ошибка загрузки сегмента: " << ts_link << " - " << curl_easy_strerror(res) << std::endl;
                    if (--max_retries <= 0) {
                        std::cerr << "Превышено максимальное количество попыток. Завершение загрузки." << std::endl;
                        break;
                    }
                } else {
                    std::cout << "Загружен сегмент: " << ts_link << std::endl;
                    // Добавляем сегмент в множество загруженных
                    downloaded_segments.insert(ts_link);
                    max_retries = 5;  // Сброс счетчика попыток после успешной загрузки
                }

                if (stop_signal_) break;

                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }

            // Ожидание появления новых сегментов
            if (!stop_signal_) {
                std::cout << "Ожидание появления новых сегментов..." << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
        }

        ts_file.close();
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        std::cout << "Загрузка завершена." << std::endl;
    }

private:
    StreamSegments& parser_;
    std::atomic<bool>& stop_signal_; // Ссылка на флаг завершения

    static size_t write_data(void* ptr, size_t size, size_t nmemb, std::ofstream* file) {
        file->write(static_cast<char*>(ptr), size * nmemb);
        return size * nmemb;
    }
};

}

#endif // STREAM_DOWNLOADER_H
