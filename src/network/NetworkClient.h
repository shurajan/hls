#ifndef NETWORK_CLIENT_H
#define NETWORK_CLIENT_H

#include "NetworkClientBase.h"
#include <string>

namespace network {

    class NetworkClient : public NetworkClientBase {
    public:
        // Конструктор по умолчанию
        NetworkClient();

        // Конструктор для mTLS
        NetworkClient(const std::string& clientCertPath,
                      const std::string& clientKeyPath,
                      const std::string& caCertPath);

        // Переопределенный метод fetch
        std::string fetch(const std::string& url) override;

    private:
        // Пути к сертификатам для mTLS
        std::string clientCertPath_;
        std::string clientKeyPath_;
        std::string caCertPath_;

        // Флаг, указывающий, использовать ли mTLS
        bool useMtls_;
    };

} // namespace network

#endif // NETWORK_CLIENT_H
