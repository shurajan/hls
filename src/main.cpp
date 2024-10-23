
#include <iostream>
#include "AppConfig.h"
#include "App.h"

int main(int argc, char* argv[]) {
    std::string configFile = "config.json"; // Имя файла по умолчанию

    // Если параметр командной строки указан, используем его
    if (argc > 1) {
        configFile = argv[1];
    }

    // Настройка обработчика сигналов
    App::setupSignalHandler();

    try {
        // Загружаем конфигурацию
        AppConfig appConfig(configFile);

        // Создаем и запускаем приложение с использованием загруженной конфигурации
        App app(appConfig);
        app.run();
    } catch (const std::exception& e) {
        // В случае ошибки выводим сообщение и выходим из приложения
        std::cerr << "Error: " << e.what() << std::endl;
        return 1; // Код возврата 1 указывает на ошибку
    }

    return 0;
}