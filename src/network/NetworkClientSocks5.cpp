#include "NetworkClientSocks5.h"
#include <algorithm>
#include <sstream>
#include <iostream>

namespace network {
NetworkClientSocks5::NetworkClientSocks5(const std::string& proxy_host, int proxy_port)
    : proxy_host_(proxy_host), proxy_port_(proxy_port), useMtls_(false){}

NetworkClientSocks5::NetworkClientSocks5(const std::string& proxy_host,
                                         int proxy_port,
                                         const std::string& clientCertPath,
                                         const std::string& clientKeyPath,
                                         const std::string& caCertPath)
    : proxy_host_(proxy_host), proxy_port_(proxy_port),
      clientCertPath_(clientCertPath), clientKeyPath_(clientKeyPath),
      caCertPath_(caCertPath), useMtls_(true){}

std::string NetworkClientSocks5::fetch(const std::string& url) {
    CURL* curl = initCurl();
    std::string readBuffer;
    std::string headersBuffer;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Установка прокси-сервера SOCKS5 с разрешением через прокси
        std::string proxy_string = proxy_host_ + ":" + std::to_string(proxy_port_);
        curl_easy_setopt(curl, CURLOPT_PROXY, proxy_string.c_str());
        curl_easy_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5_HOSTNAME);

        // Установка TLS версии
        curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_3);

        // Отключение проверки SSL для отладки
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

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
    } else {
        std::cerr << "Failed to initialize CURL.\n";
    }

    return readBuffer;
}

} // namespace network
