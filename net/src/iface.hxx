//
// Created by maggu2810 on 11/6/24.
//

#ifndef IFACEUTILS_HXX
#define IFACEUTILS_HXX

#include "inaddr_storage.hxx"

#include <expected>
#include <ifaddrs.h>
#include <memory>
#include <optional>
#include <string>
#include <sys/socket.h>

namespace iface {
    std::expected<std::shared_ptr<ifaddrs>, std::string> getaddrinfo();

    // get interface name by address

    std::optional<std::string> get_ifacename(const inaddr_storage &address);

    std::optional<std::string> get_ifacename(const in_addr &address);

    std::optional<std::string> get_ifacename(const in6_addr &address);

    // get interface name by sockaddr

    std::optional<std::string> get_ifacename(const sockaddr &sa);

    std::optional<std::string> get_ifacename(const sockaddr_storage &sa) {
        return get_ifacename(reinterpret_cast<const sockaddr &>(sa));
    }

    std::optional<std::string> get_ifacename(const sockaddr_in &sa) {
        return get_ifacename(reinterpret_cast<const sockaddr &>(sa));
    }

    std::optional<std::string> get_ifacename(const sockaddr_in6 &sa) {
        return get_ifacename(reinterpret_cast<const sockaddr &>(sa));
    }

    // get interface name by socket

    std::optional<std::string> get_ifacename(int sock);
}


#endif //IFACEUTILS_HXX
