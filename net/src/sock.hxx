//
// Created by maggu2810 on 11/6/24.
//

#ifndef SOCK_HXX
#define SOCK_HXX

#include <expected>
#include <memory>
#include <netdb.h>

namespace sock {
    std::expected<std::shared_ptr<addrinfo>, const char *> getaddrinfo(const char *node,
                                                                       const char *service,
                                                                       const addrinfo &hints);

    std::string to_string(const sockaddr &sa);

    inline std::string to_string(const sockaddr_storage &sa) {
        return to_string(reinterpret_cast<const sockaddr &>(sa));
    }

    inline std::string to_string(const sockaddr_in &sa) {
        return to_string(reinterpret_cast<const sockaddr &>(sa));
    }

    inline std::string to_string(const sockaddr_in6 &sa) {
        return to_string(reinterpret_cast<const sockaddr &>(sa));
    }
} // sock

#endif //SOCK_HXX
