#include "StreamDownloadManager.h"
#include <NetworkClient.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main() {
    // Создаем NetworkClient (например, через SOCKS5-прокси)
    network::NetworkClient network_client = network::NetworkClient();
    network::NetworkClient mtls_network_client = network::NetworkClient("client-cert.pem", "client-key.pem", "ca-cert.pem");

    // Создаем менеджер загрузки потоков
    StreamDownloadManager manager(network_client);

    // Пример добавления задач на загрузку потоков с REST API
    std::vector<std::string> api_urls = {
        "url1",
        "url2"
    };

    for (const auto& api_url : api_urls) {
        // Запрашиваем JSON по API URL
        std::string json_response = mtls_network_client.fetch(api_url);
        if (json_response.empty()) {
            std::cerr << "Не удалось загрузить JSON: " << api_url << std::endl;
            continue;
        }

        // Парсим JSON и извлекаем поле hls_source
        json parsed_json = json::parse(json_response);
        if (!parsed_json.contains("hls_source")) {
            std::cerr << "Поле hls_source отсутствует в JSON: " << api_url << std::endl;
            continue;
        }
        std::string playlist_url = parsed_json["hls_source"].get<std::string>();

        // Добавляем задачу на загрузку
        manager.add_task(playlist_url, "output" + std::to_string(&api_url - &api_urls[0]) + ".ts", "Max");
    }

    // Поддерживаем приложение активным, пока задачи выполняются
    while (!manager.is_stopped()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Завершаем все задачи при выходе
    manager.stop_all();

    std::cout << "Все загрузки завершены." << std::endl;
    return 0;
}
