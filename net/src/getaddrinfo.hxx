//
// Created by maggu2810 on 11/8/24.
//

#ifndef GETADDRINFO_HXX
#define GETADDRINFO_HXX

#include <expected>
#include <memory>
#include <netdb.h>

namespace net {
    std::expected<std::shared_ptr<addrinfo>, const char *> getaddrinfo(const char *node,
                                                                       const char *service,
                                                                       const addrinfo &hints);
}

#endif //GETADDRINFO_HXX
