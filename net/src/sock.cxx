//
// Created by maggu2810 on 11/6/24.
//

#include "sock.hxx"

#include "net.hxx"

#include <sstream>

namespace sock {
    std::expected<std::shared_ptr<addrinfo>, const char *> getaddrinfo(const char *node,
                                                                       const char *service,
                                                                       const addrinfo &hints) {
        addrinfo *servinfo{};
        if (int rv = getaddrinfo(node, service, &hints, &servinfo); rv != 0) {
            return std::unexpected(gai_strerror(rv));
        }
        return std::shared_ptr<addrinfo>{servinfo, freeaddrinfo};
    }

    std::string to_string(const sockaddr &sa) {
        std::stringstream ss;
        ss << std::format("family: {}", sa.sa_family);
        if (sa.sa_family == AF_INET) {
            const auto sas = reinterpret_cast<const sockaddr_in *>(&sa);
            ss << std::format(" [AF_INET], address: {}, port: {}", net::to_string(sas->sin_addr), sas->sin_port);
        } else if (sa.sa_family == AF_INET6) {
            const auto sas = reinterpret_cast<const sockaddr_in6 *>(&sa);
            ss << std::format(" [AF_INET&], address: {}, port: {}, scope_id: {}, flowinfo: {}",
                              net::to_string(sas->sin6_addr), sas->sin6_port, sas->sin6_scope_id, sas->sin6_flowinfo);
        } else {
            ss << " [not handled]";
        }
        return ss.str();
    }
} // sock

