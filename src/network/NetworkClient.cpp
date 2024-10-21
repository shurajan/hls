#include "NetworkClient.h"
#include <algorithm>
#include <iostream>
#include <zlib.h>
#include <curl/curl.h>

namespace network {
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

    CURL* NetworkClient::initCurl() {
        return curl_easy_init();
    }

    size_t NetworkClient::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }

    size_t NetworkClient::HeaderCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        std::string header((char*)contents, size * nmemb);
        std::string* headers = static_cast<std::string*>(userp);
        headers->append(header);
        return size * nmemb;
    }


    std::string NetworkClient::decompress_gzip(const std::string& compressed_data) {
        const size_t bufferSize = 32768;
        char buffer[bufferSize];
        std::string decompressed_data;

        z_stream zs;
        zs.zalloc = Z_NULL;
        zs.zfree = Z_NULL;
        zs.opaque = Z_NULL;
        zs.avail_in = static_cast<uInt>(compressed_data.size());
        zs.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(compressed_data.data()));

        if (inflateInit2(&zs, 16 + MAX_WBITS) != Z_OK) {
            throw std::runtime_error("Failed to initialize zlib for gzip decompression.");
        }

        int ret;
        do {
            zs.avail_out = bufferSize;
            zs.next_out = reinterpret_cast<Bytef*>(buffer);

            ret = inflate(&zs, Z_NO_FLUSH);
            if (ret != Z_OK && ret != Z_STREAM_END) {
                inflateEnd(&zs);
                throw std::runtime_error("Error during gzip decompression.");
            }

            decompressed_data.append(buffer, bufferSize - zs.avail_out);
        } while (ret != Z_STREAM_END);

        inflateEnd(&zs);
        return decompressed_data;
    }

} // namespace network
