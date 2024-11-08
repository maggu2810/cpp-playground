//
// Created by maggu2810 on 11/8/24.
//

#include "getaddrinfo.hxx"

namespace net {
    std::expected<std::shared_ptr<addrinfo>, const char *> getaddrinfo(const char *node,
                                                                       const char *service,
                                                                       const addrinfo &hints) {
        addrinfo *servinfo{};
        if (int rv = getaddrinfo(node, service, &hints, &servinfo); rv != 0) {
            return std::unexpected(gai_strerror(rv));
        }
        return std::shared_ptr<addrinfo>{servinfo, freeaddrinfo};
    }
}
