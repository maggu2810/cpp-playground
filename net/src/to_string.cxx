//
// Created by maggu2810 on 11/8/24.
//

#include "to_string.hxx"

#include <sstream>
#include <cstring>
#include <arpa/inet.h>

#include "getaddrinfo.hxx"
#include "overloaded.hxx"

namespace {
    template<typename INADDR, int AF, int SZ>
    std::string inet_ntop_wrapper(const INADDR &address) {
        char buf[SZ];
        if (::inet_ntop(AF, &address, buf, sizeof(buf)) == nullptr) {
            const auto errnum = errno;
            throw std::runtime_error(std::format(
                "Failed to convert address to string (expected because we supply correct values) [errno: {}]", errnum));
        }
        return {buf};
    }
}

namespace net {
    std::string strerrnum(int errnum) {
        char buffer[128];
        const char *str = ::strerror_r(errnum, buffer, sizeof(buffer));
        return std::format("[{}] {}", errnum, str);
    }

    std::string to_string(const inaddr_storage &address) {
        using R = std::string;
        return std::visit(overloaded{
                              [](const in_addr &arg) -> R { return to_string(arg); },
                              [](const in6_addr &arg)-> R { return to_string(arg); },
                              [](const std::monostate &arg)-> R { return "uninitialized"; },
                          }, address);
    }

    std::string to_string(const in_addr &address) {
        return inet_ntop_wrapper<in_addr, AF_INET, INET_ADDRSTRLEN>(address);
    }

    std::string to_string(const in6_addr &address) {
        return inet_ntop_wrapper<in6_addr, AF_INET6, INET6_ADDRSTRLEN>(address);
    }

    std::string to_string(const sockaddr &sa, socklen_t addrlen) {
        char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
        if (getnameinfo(&sa, addrlen, hbuf, sizeof(hbuf), sbuf, sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV) != 0) {
            const auto errnum = errno;
            throw std::runtime_error(std::format(
                "getnameinfo failed [errno: {}]", errnum));
        }
        std::stringstream ss;
        ss << std::format("family: {}, host: {}, port: {}", sa.sa_family, hbuf, sbuf);
        if (sa.sa_family == AF_INET) {
            const auto sas = reinterpret_cast<const sockaddr_in *>(&sa);
            ss << std::format(", [AF_INET]", to_string(sas->sin_addr), sas->sin_port);
        } else if (sa.sa_family == AF_INET6) {
            const auto sas = reinterpret_cast<const sockaddr_in6 *>(&sa);
            ss << std::format(", [AF_INET6], scope_id: {}, flowinfo: {}",
                              sas->sin6_scope_id, sas->sin6_flowinfo);
        }
        return ss.str();
    }
}
