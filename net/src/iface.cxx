//
// Created by maggu2810 on 11/6/24.
//

#include "iface.hxx"

#include "cleanup.hxx"
#include "logging.hxx"

#include <cstring>
#include <ifaddrs.h>
#include <netdb.h>
#include <netinet/in.h>

namespace iface {
    std::expected<std::shared_ptr<ifaddrs>, std::string> getaddrinfo() {
        ifaddrs *ifap{};
        if (int rv = getifaddrs(&ifap); rv != 0) {
            const auto err = errno;
            char buffer[128];
            return std::unexpected(strerror_r(err, buffer, sizeof(buffer)));
        }
        return std::shared_ptr<ifaddrs>{ifap, freeifaddrs};
    }

    template<int FAMILY, typename ADDRESS_TYPE, typename SOCKADDRESS_TYPE, std::size_t OFFSET>
    std::optional<std::string> get_iface_name(const ADDRESS_TYPE &address) {
        const auto ifap = getaddrinfo();
        if (!ifap) { return {}; }

        for (ifaddrs *ifa = ifap.value().get(); ifa != nullptr; ifa = ifa->ifa_next) {
            if (ifa->ifa_addr == nullptr) {
                continue;
            }
            //LOGD("get_iface_name: sa_family: {}, name: {}", ifa->ifa_addr->sa_family, ifa->ifa_name);
            if (ifa->ifa_addr->sa_family == FAMILY) {
                const auto saIface = reinterpret_cast<const SOCKADDRESS_TYPE *>(ifa->ifa_addr);
                const auto ifaceAddress = reinterpret_cast<const ADDRESS_TYPE *>(
                    reinterpret_cast<const std::uint8_t *>(saIface) + OFFSET);
                if (memcmp(ifaceAddress, &address, sizeof(ADDRESS_TYPE)) != 0) {
                    continue;
                }
            } else {
                continue;
            }
            return {ifa->ifa_name};
        }

        return {};
    }

    template<class... Ts>
    struct overloaded : Ts... {
        using Ts::operator()...;
    };

    std::optional<std::string> get_ifacename(const inaddr_storage &address) {
        using R = std::optional<std::string>;
        return std::visit(overloaded{
                              [](const in_addr &arg) -> R { return get_ifacename(arg); },
                              [](const in6_addr &arg)-> R { return get_ifacename(arg); },
                              [](const std::monostate &arg)-> R { return {}; },
                          }, address);
    }


    std::optional<std::string> get_ifacename(const in_addr &address) {
        return get_iface_name<AF_INET, in_addr, sockaddr_in, offsetof(sockaddr_in, sin_addr)>(address);
    }

    std::optional<std::string> get_ifacename(const in6_addr &address) {
        return get_iface_name<AF_INET6, in6_addr, sockaddr_in6, offsetof(sockaddr_in6, sin6_addr)>(address);
    }

    std::optional<std::string> get_ifacename(const sockaddr &sa) {
        const auto ifap = getaddrinfo();
        if (!ifap) { return {}; }

        for (ifaddrs *ifa = ifap.value().get(); ifa != nullptr; ifa = ifa->ifa_next) {
            LOGD("Check interface: %s", ifa->ifa_name);
            if (ifa->ifa_addr == nullptr) {
                LOGD("Interface address unknown");
                continue;
            }
            if (ifa->ifa_addr->sa_family != sa.sa_family) {
                LOGD("Address family does not match, use next");
                continue;
            }
            if (ifa->ifa_addr->sa_family == AF_INET) {
                LOGD("Found IPv4 address %s", ifa->ifa_name);
                const auto *saIface = reinterpret_cast<const sockaddr_in *>(ifa->ifa_addr);
                const auto *saSock = reinterpret_cast<const sockaddr_in *>(&sa);
                if (memcmp(&saIface->sin_addr, &saSock->sin_addr, sizeof(in_addr)) != 0) {
                    LOGD("Address does not match, use next");
                    continue;
                }
            } else if (ifa->ifa_addr->sa_family == AF_INET6) {
                LOGD("Found IPv6 address %s", ifa->ifa_name);
                const auto *saIface = reinterpret_cast<const sockaddr_in6 *>(ifa->ifa_addr);
                const auto *saSock = reinterpret_cast<const sockaddr_in6 *>(&sa);
                if (memcmp(&saIface->sin6_addr, &saSock->sin6_addr, sizeof(in6_addr)) != 0) {
                    LOGD("Address does not match, use next");
                    continue;
                }
            } else {
                LOGD("Unhandled address family");
                continue;
            }
            LOGD("Found");
            return {ifa->ifa_name};
        }
        LOGD("No match found");
        return {};
    }

    std::optional<std::string> get_ifacename(const int sock) {
        sockaddr_storage sas; // NOLINT(*-pro-type-member-init)
        socklen_t sasLen = sizeof(sas);
        if (getsockname(sock, reinterpret_cast<sockaddr *>(&sas), &sasLen) != 0) {
            return {};
        }
        return get_ifacename(sas);
    }
}
