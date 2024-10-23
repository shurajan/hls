//
// Created by Alexander Bralnin on 23.10.2024.
//

#// App.h
#ifndef APP_H
#define APP_H

#include <vector>
#include <thread>
#include "AppConfig.h"

class App {
public:
    // Конструктор принимает ссылку на объект AppConfig
    explicit App(const AppConfig& config);

    // Запуск приложения
    void run();

    // Статический метод для установки обработчика сигналов
    static void setupSignalHandler();

private:
    const AppConfig& appConfig;
    std::vector<std::thread> threads;
    std::atomic<bool> keepRunning;

    // Метод для обработки сигнала SIGINT
    static void signalHandler(int signal);

    // Функция, выполняемая в потоке для обработки URL
    void threadFunction(int threadNumber, const std::string& url);
};

#endif // APP_H