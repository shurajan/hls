//
// Created by Alexander Bralnin on 23.10.2024.
//

#include "App.h"
#include <iostream>
#include <chrono>
#include <csignal>
#include "network/NetworkClient.h"
#include "RoomInfo.h"
#include "StreamDownloader.h"
#include <curl/curl.h>

static std::atomic<bool> globalKeepRunning(true); // Глобальная переменная для отслеживания завершения

App::App(const AppConfig& config)
    : appConfig(config), keepRunning(true) {
}

void App::setupSignalHandler() {
    std::signal(SIGINT, App::signalHandler);
}

void App::signalHandler(int signal) {
    if (signal == SIGINT) {
        globalKeepRunning = false;
        std::cout << "\nReceived Ctrl-C, stopping all threads..." << std::endl;
    }
}

void App::run() {
    if (curl_global_init(CURL_GLOBAL_ALL) != 0) {
        std::cerr << "Ошибка инициализации cURL" << std::endl;
        return;
    }
    // Получаем настройки mTLS
    const MtlsConfig& mtlsConfig = appConfig.getMtlsConfig();

    // Получаем список URL-адресов
    const std::vector<std::string>& urlList = appConfig.getUrlList();

    // Создаем потоки для каждого URL
    int numThreads = urlList.size();
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(&App::threadFunction, this, i, urlList[i]);
        std::this_thread::sleep_for(std::chrono::seconds(8));
    }

    // Ожидаем завершения всех потоков
    for (auto& thread : threads) {
        thread.join();
    }

    curl_global_cleanup();
    std::cout << "All threads have finished. Program is exiting." << std::endl;
}

void App::threadFunction(int threadNumber, const std::string& url) {
    try {
        // Создаем отдельный объект NetworkClient для каждого потока
        network::NetworkClient client(
            appConfig.getMtlsConfig().clientCertPath,
            appConfig.getMtlsConfig().clientKeyPath,
            appConfig.getMtlsConfig().caCertPath
        );

        while (globalKeepRunning) {
            std::cout << "Thread " << threadNumber << " is fetching URL: " << url << std::endl;

            // Выполняем запрос и получаем JSON-ответ
            std::string response = client.fetch(url);

            // Обрабатываем JSON-ответ
            try {
                if (!response.empty()) {
                    // Попытка обработать JSON-ответ
                    auto roomInfoOpt = processJsonResponse(response);
                    if (roomInfoOpt) {
                        // Если данные успешно извлечены, выводим их
                        const RoomInfo& roomInfo = *roomInfoOpt;
                        std::cout << "Room Status: " << roomInfo.roomStatus << std::endl;
                        std::cout << "Broadcaster Username: " << roomInfo.broadcasterUsername << std::endl;
                        std::cout << "HLS Source: " << roomInfo.hlsSource << std::endl;

                        if (!roomInfo.hlsSource.empty()) {
                            network::NetworkClient networkClient = network::NetworkClient();
                            m3u8::StreamSegments streamSegments = m3u8::StreamSegments(&networkClient, roomInfo.hlsSource, "Max");
                            m3u8::StreamDownloader streamDownloader(streamSegments, globalKeepRunning);
                            auto file_name = generateFilename(roomInfo.broadcasterUsername);
                            streamDownloader.download_stream(file_name);
                        }
                    } else {
                        // В случае ошибки при обработке JSON
                        std::cerr << "Failed to retrieve room information." << std::endl;
                    }

                } else {
                    std::cout << "Thread " << threadNumber << " received an empty response." << std::endl;
                }
            } catch (const std::exception& e) {
                // Обработка исключений при парсинге или обработке JSON
                std::cerr << "Thread " << threadNumber << " encountered a JSON processing error: " << e.what() << std::endl;
            }

            // Ожидаем 1 минуту перед следующим запросом
            std::this_thread::sleep_for(std::chrono::minutes(1));
        }
    } catch (const std::exception& e) {
        std::cerr << "Thread " << threadNumber << " encountered an error: " << e.what() << std::endl;
    }

    std::cout << "Thread " << threadNumber << " is stopping." << std::endl;
}