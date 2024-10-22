#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <chrono>
#include <csignal>
#include <atomic>
#include <StreamDownloader.h>
#include "network/NetworkClient.h" // Предполагается, что этот файл содержит определение класса NetworkClient

// Глобальные статические константы для путей к сертификатам mTLS
static const std::string clientCertPath = "client.crt";
static const std::string clientKeyPath = "client.key";
static const std::string caCertPath = "ca.crt";

// Флаг для отслеживания завершения работы программы
std::atomic<bool> keepRunning(true);

// Обработчик сигнала для обработки SIGINT (Ctrl-C)
void signalHandler(int signal) {
    if (signal == SIGINT) {
        keepRunning = false;
        std::cout << "\nReceived Ctrl-C, stopping all threads..." << std::endl;
    }
}

// Функция, которую будут выполнять потоки
void threadFunction(int threadNumber, const std::string& url) {
    try {
        // Создаем отдельный объект NetworkClient для каждого потока
        network::NetworkClient client(clientCertPath, clientKeyPath, caCertPath);

        while (keepRunning) {
            std::cout << "Thread " << threadNumber << " is fetching URL: " << url << std::endl;

            // Выполняем запрос и получаем JSON-ответ
            std::string response = client.fetch(url);

            // Обрабатываем JSON-ответ
            if (!response.empty()) {
                std::cout << "Thread " << threadNumber << " received response: " << response << std::endl;
            } else {
                std::cout << "Thread " << threadNumber << " received an empty response." << std::endl;
            }

            // Ожидаем 5 минут перед следующим запросом
            std::this_thread::sleep_for(std::chrono::minutes(1));
        }
    } catch (const std::exception& e) {
        std::cerr << "Thread " << threadNumber << " encountered an error: " << e.what() << std::endl;
    }

    std::cout << "Thread " << threadNumber << " is stopping." << std::endl;
}

int main() {
    // Устанавливаем обработчик сигнала для обработки SIGINT (Ctrl-C)
    std::signal(SIGINT, signalHandler);

    // Список URL-адресов для проверки
    std::vector<std::string> urlList = {
        "https://api.example.com/data1",
        "https://api.example.com/data2",
        "https://api.example.com/data3",
        "https://api.example.com/data4",
        "https://api.example.com/data5"
    };

    // Вектор для хранения потоков
    std::vector<std::thread> threads;
    int numThreads = urlList.size();

    // Создание потоков для каждого URL
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(threadFunction, i, urlList[i]);
    }

    // Ожидание завершения всех потоков
    for (auto& thread : threads) {
        thread.join();
    }

    std::cout << "All threads have finished. Program is exiting." << std::endl;
    return 0;
}
