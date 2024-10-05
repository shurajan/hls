
#ifndef STREAM_DOWNLOADER_H
#define STREAM_DOWNLOADER_H

#include "StreamSegments.h"
#include <string>
#include <vector>
#include <fstream>
#include <csignal>
#include <chrono>
#include <thread>
#include <iostream>
#include <curl/curl.h>

namespace m3u8 {

class StreamDownloader {
public:
    StreamDownloader(StreamSegments& parser) : parser_(parser) {}
    
    void download_stream(const std::string& playlist_url, Resolution resolution, const std::string& output_file) {
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

        while (!stop_signal_) {
            // Получаем ссылки на сегменты
            std::vector<std::string> ts_links = parser_.get_segments();
            
            // Загружаем сегменты по очереди
            for (const auto& ts_link : ts_links) {
                if (stop_signal_) break;

                curl_easy_setopt(curl, CURLOPT_URL, ts_link.c_str());
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ts_file);

                res = curl_easy_perform(curl);

                if (res != CURLE_OK) {
                    std::cerr << "Ошибка загрузки сегмента: " << ts_link << " - " << curl_easy_strerror(res) << std::endl;
                } else {
                    std::cout << "Загружен сегмент: " << ts_link << std::endl;
                }
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
    
    static void stop() {
        stop_signal_ = true;
    }

private:
    StreamSegments& parser_;
    inline static bool stop_signal_ = false;

    static size_t write_data(void* ptr, size_t size, size_t nmemb, std::ofstream* file) {
        file->write(static_cast<char*>(ptr), size * nmemb);
        return size * nmemb;
    }
};

}

#endif // STREAM_DOWNLOADER_H
