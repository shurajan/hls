#include "NetworkClient.h"
#include <algorithm>
#include <iostream>

namespace network {

    size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }

    size_t HeaderCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        std::string header((char*)contents, size * nmemb);
        std::string* headers = static_cast<std::string*>(userp);
        headers->append(header);
        return size * nmemb;
    }

    // Конструктор по умолчанию
    NetworkClient::NetworkClient() : useMtls_(false) {}

    // Конструктор для mTLS
    NetworkClient::NetworkClient(const std::string& clientCertPath,
                                 const std::string& clientKeyPath,
                                 const std::string& caCertPath)
        : clientCertPath_(clientCertPath), clientKeyPath_(clientKeyPath), caCertPath_(caCertPath), useMtls_(true) {}

    // Реализация метода fetch
    std::string NetworkClient::fetch(const std::string& url) {
        CURL* curl = initCurl();
        std::string readBuffer;
        std::string headersBuffer;

        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
            curl_easy_setopt(curl, CURLOPT_HEADERDATA, &headersBuffer);

            // Настройки для mTLS, если включен флаг useMtls_
            if (useMtls_) {
                curl_easy_setopt(curl, CURLOPT_SSLCERT, clientCertPath_.c_str());
                curl_easy_setopt(curl, CURLOPT_SSLKEY, clientKeyPath_.c_str());
                curl_easy_setopt(curl, CURLOPT_CAINFO, caCertPath_.c_str());

                // Если нужно, можно указать пароль для приватного ключа
                // curl_easy_setopt(curl, CURLOPT_KEYPASSWD, "private_key_password");
            }

            CURLcode res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << "\n";
            } else {
                // Приводим заголовки к нижнему регистру
                std::transform(headersBuffer.begin(), headersBuffer.end(), headersBuffer.begin(), ::tolower);

                if (headersBuffer.find("content-encoding: gzip") != std::string::npos) {
                    try {
                        readBuffer = decompress_gzip(readBuffer);
                    } catch (const std::exception& e) {
                        std::cerr << "Decompression failed: " << e.what() << "\n";
                    }
                }
            }

            curl_easy_cleanup(curl);
        }

        return readBuffer;
    }

} // namespace network
