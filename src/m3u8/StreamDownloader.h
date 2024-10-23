#ifndef M3U8_STREAM_DOWNLOADER_H
#define M3U8_STREAM_DOWNLOADER_H

#include "StreamSegments.h"
#include <string>
#include <vector>
#include <unordered_set>
#include <fstream>
#include <chrono>
#include <thread>
#include <iostream>
#include <curl/curl.h>
#include <atomic>

namespace m3u8 {

class StreamDownloader {
public:
    // Конструктор принимает ссылку на глобальную переменную завершения
    StreamDownloader(StreamSegments& parser, std::atomic<bool>& global_keep_running)
        : parser_(parser), keepRunning(global_keep_running) {}

    void download_stream(const std::string& output_file) {
        if (!initialize_curl()) {
            std::cerr << "Ошибка инициализации cURL" << std::endl;
            return;
        }

        std::ofstream ts_file(output_file, std::ios::binary | std::ios::app);
        if (!ts_file.is_open()) {
            std::cerr << "Ошибка открытия файла для записи" << std::endl;
            cleanup_curl();
            return;
        }

        std::unordered_set<std::string> downloaded_segments;

        // Основной цикл загрузки потоков, пока не получен сигнал остановки
        while (keepRunning) {
            std::vector<std::string> ts_links;
            if (!fetch_segments(parser_, ts_links)) {
                std::cerr << "Не удалось получить сегменты. Завершение загрузки." << std::endl;
                break;
            }

            bool success = process_segments(ts_file, ts_links);
            if (success) {
                downloaded_segments.insert(ts_links.begin(), ts_links.end());
                std::cout << "Новые сегменты успешно загружены." << std::endl;
            } else {
                std::cerr << "Не удалось загрузить все сегменты. Завершение загрузки." << std::endl;
                break;
            }
        }

        ts_file.close();
        cleanup_curl();
        std::cout << "Загрузка завершена." << std::endl;
    }

private:
    StreamSegments& parser_;
    std::atomic<bool>& keepRunning;
    CURL* curl_ = nullptr;

    bool initialize_curl() {
        curl_ = curl_easy_init();
        return curl_ != nullptr;
    }

    void cleanup_curl() {
        if (curl_) {
            curl_easy_cleanup(curl_);
            curl_ = nullptr;
        }
    }

    bool process_segments(std::ofstream& ts_file, const std::vector<std::string>& ts_links) {
        int retries = 5;
        for (int attempt = 1; attempt <= retries; ++attempt) {
            bool all_segments_loaded = true;
            for (const auto& ts_link : ts_links) {
                if (!download_segment(ts_link, ts_file)) {
                    all_segments_loaded = false;
                    std::cerr << "Попытка " << attempt << " из " << retries << ": ошибка загрузки сегмента " << ts_link << std::endl;
                    break;
                }
            }
            if (all_segments_loaded) {
                return true;
            }
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
        return false;
    }

    bool fetch_segments(StreamSegments& parser, std::vector<std::string>& ts_links) {
        int failed_attempts = 0;
        while (failed_attempts < 10) {
            ts_links = parser.get_segments();
            if (!ts_links.empty()) {
                return true;
            }
            failed_attempts++;
            std::cerr << "Нет доступных сегментов в данный момент. Попытка "
                      << failed_attempts << " из 10." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
        std::cerr << "Не удалось получить новые сегменты 10 раз подряд. Завершение попыток." << std::endl;
        return false;
    }

    bool download_segment(const std::string& url, std::ofstream& ts_file) {
        curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &ts_file);

        CURLcode res = curl_easy_perform(curl_);
        if (res != CURLE_OK) {
            std::cerr << "Ошибка загрузки сегмента: " << url << " - " << curl_easy_strerror(res) << std::endl;
            return false;
        }
        std::cout << "Загружен сегмент: " << url << std::endl;
        return true;
    }

    static size_t write_data(void* ptr, size_t size, size_t nmemb, std::ofstream* file) {
        file->write(static_cast<char*>(ptr), size * nmemb);
        return size * nmemb;
    }
};

} // namespace m3u8

#endif // M3U8_STREAM_DOWNLOADER_H