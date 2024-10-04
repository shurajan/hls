#ifndef NETWORK_CLIENT_SOCKS5_H
#define NETWORK_CLIENT_SOCKS5_H

#include "NetworkClientBase.h"

namespace network {

    class NetworkClientSocks5 : public NetworkClientBase {
    public:
        NetworkClientSocks5(const std::string& proxy_host, int proxy_port);
        std::string fetch(const std::string& url) override;

    private:
        std::string proxy_host_;
        int proxy_port_;
    };

} // namespace network

#endif // NETWORK_CLIENT_SOCKS5_H
