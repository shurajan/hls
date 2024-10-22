#ifndef NETWORK_CLIENT_H
#define NETWORK_CLIENT_H

#include <string>
#include <curl/curl.h>

namespace network {

    class NetworkClient {
    public:
        // Конструктор по умолчанию
        NetworkClient();

        // Конструктор для mTLS
        NetworkClient(std::string  clientCertPath,
                      std::string  clientKeyPath,
                      std::string  caCertPath);

        // Переопределенный метод fetch
        std::string fetch(const std::string& url);

    private:
        // Пути к сертификатам для mTLS
        std::string clientCertPath_;
        std::string clientKeyPath_;
        std::string caCertPath_;

        // Флаг, указывающий, использовать ли mTLS
        bool useMtls_;

        // Статические методы для обработки данных
        static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
        static size_t HeaderCallback(void* contents, size_t size, size_t nmemb, void* userp);

        // Метод для инициализации CURL
        CURL* initCurl();

        // Метод для распаковки данных в формате gzip
        static std::string decompress_gzip(const std::string& compressed_data);
    };

} // namespace network

#endif // NETWORK_CLIENT_H
