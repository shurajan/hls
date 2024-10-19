#ifndef NETWORK_CLIENT_SOCKS5_H
#define NETWORK_CLIENT_SOCKS5_H

#include "NetworkClientBase.h"

namespace network {

    class NetworkClientSocks5 : public NetworkClientBase {
    public:
        NetworkClientSocks5(const std::string& proxy_host, int proxy_port);
        NetworkClientSocks5(const std::string& proxy_host,
                            int proxy_port,
                            const std::string& clientCertPath,
                            const std::string& clientKeyPath,
                            const std::string& caCertPath);

        std::string fetch(const std::string& url) override;

    private:
        std::string proxy_host_;
        int proxy_port_;

        // Пути к сертификатам для mTLS
        std::string clientCertPath_;
        std::string clientKeyPath_;
        std::string caCertPath_;

        // Флаг, указывающий, использовать ли mTLS
        bool useMtls_;
    };

} // namespace network

#endif // NETWORK_CLIENT_SOCKS5_H
