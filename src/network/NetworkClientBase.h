#ifndef NETWORK_CLIENT_BASE_H
#define NETWORK_CLIENT_BASE_H

#include <string>
#include <curl/curl.h>

namespace network {

    class NetworkClientBase {
    public:
        NetworkClientBase();
        virtual ~NetworkClientBase();

        // Статические методы для обработки данных
        static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
        static size_t HeaderCallback(void* contents, size_t size, size_t nmemb, void* userp);

        // Виртуальный метод для выполнения сетевого запроса
        virtual std::string fetch(const std::string& url) = 0;

    protected:
        // Метод для инициализации CURL
        CURL* initCurl();

        // Метод для распаковки данных в формате gzip
        std::string decompress_gzip(const std::string& compressed_data);
    };

} // namespace network

#endif // NETWORK_CLIENT_BASE_H
