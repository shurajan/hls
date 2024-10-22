#ifndef STREAM_DOWNLOADER_H
#define STREAM_DOWNLOADER_H

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
    StreamDownloader(StreamSegments& parser, std::atomic<bool>& stop_signal)
        : parser_(parser), stop_signal_(stop_signal) {}

    void download_stream(const std::string& playlist_url, const std::string& output_file) {
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

        while (!stop_signal_) {
            // Получаем ссылки на сегменты
            std::vector<std::string> ts_links = parser_.get_segments();
            if (ts_links.empty()) {
                std::cerr << "Нет доступных сегментов в данный момент." << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(2));
                continue;
            }

            // Попытка загрузить сегменты
            bool success = process_segments(ts_file, ts_links);
            if (success) {
                // Если загрузка успешна, добавляем сегменты в загруженные
                downloaded_segments.insert(ts_links.begin(), ts_links.end());
                std::cout << "Новые сегменты успешно загружены." << std::endl;
            } else {
                // Если загрузка не удалась, прерываем работу
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
    std::atomic<bool>& stop_signal_;
    CURL* curl_ = nullptr;

    bool initialize_curl() {
        curl_ = curl_easy_init();
        return curl_ != nullptr;
    }

    void cleanup_curl() {
        if (curl_) {
            curl_easy_cleanup(curl_);
        }
    }

    bool process_segments(std::ofstream& ts_file, const std::vector<std::string>& ts_links) {
        int retries = 5;
        for (int attempt = 1; attempt <= retries; ++attempt) {
            bool all_segments_loaded = true;
            for (const auto& ts_link : ts_links) {
                // Попытка загрузить сегмент
                if (!download_segment(ts_link, ts_file)) {
                    all_segments_loaded = false;
                    std::cerr << "Попытка " << attempt << " из " << retries << ": ошибка загрузки сегмента " << ts_link << std::endl;
                    break; // Прерываем текущую попытку, чтобы попробовать снова
                }
            }
            if (all_segments_loaded) {
                return true; // Все сегменты успешно загружены
            }
            std::this_thread::sleep_for(std::chrono::seconds(2)); // Ожидание перед следующей попыткой
        }
        return false; // Не удалось загрузить все сегменты за 5 попыток
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

}

#endif // STREAM_DOWNLOADER_H