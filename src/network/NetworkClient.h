#ifndef NETWORK_CLIENT_H
#define NETWORK_CLIENT_H

#include "NetworkClientBase.h"

namespace network {

    class NetworkClient : public NetworkClientBase {
    public:
        NetworkClient();
        std::string fetch(const std::string& url) override;
    };

} // namespace network

#endif // NETWORK_CLIENT_H
