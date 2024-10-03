#ifndef NETWORK_CLIENT_H
#define NETWORK_CLIENT_H

#include <string>

class NetworkClient {
public:
    // Конструктор
    NetworkClient();

    // Метод для загрузки данных с URL
    std::string fetch(const std::string& url);

    // Публичный метод для распаковки gzip-данных
    std::string decompress_gzip(const std::string& compressed_data);

private:
    // Другие приватные методы и члены класса
};

#endif // NETWORK_CLIENT_H
