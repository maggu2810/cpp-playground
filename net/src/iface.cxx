//
// Created by maggu2810 on 11/6/24.
//

#include "iface.hxx"

#include "logging.hxx"
#include "visit_utils.hxx"

#include <cstring>
#include <ifaddrs.h>
#include <netdb.h>
#include <netinet/in.h>

namespace {
    template<typename ADDRESS_TYPE, std::size_t OFFSET>
    const ADDRESS_TYPE *get_address(const sockaddr &sa) {
        return reinterpret_cast<const ADDRESS_TYPE *>(reinterpret_cast<const std::uint8_t *>(&sa) + OFFSET);
    }

    template<int FAMILY, typename ADDRESS_TYPE, std::size_t OFFSET>
    bool match_sa_address(const sockaddr &sa, const ADDRESS_TYPE &address) {
        if (sa.sa_family != FAMILY) {
            return false;
        }

        const auto ifaceAddress = get_address<ADDRESS_TYPE, OFFSET>(sa);

        if (memcmp(ifaceAddress, &address, sizeof(ADDRESS_TYPE)) != 0) {
            return false;
        }

        return true;
    }

    template<int FAMILY, typename ADDRESS_TYPE, std::size_t OFFSET>
    bool match_sa_sa(const sockaddr &sa1, const sockaddr &sa2) {
        if (sa1.sa_family != FAMILY || sa2.sa_family != FAMILY) {
            return false;
        }

        const auto ifaceAddress1 = get_address<ADDRESS_TYPE, OFFSET>(sa1);
        const auto ifaceAddress2 = get_address<ADDRESS_TYPE, OFFSET>(sa2);
        if (memcmp(ifaceAddress1, ifaceAddress2, sizeof(ADDRESS_TYPE)) != 0) {
            LOGD("Address does not match, use next");
            return false;
        }

        return true;
    }

    template<int FAMILY, typename ADDRESS_TYPE, std::size_t OFFSET>
    std::optional<std::string> get_iface_name(const ADDRESS_TYPE &address) {
        const auto ifap = iface::getifaddrs();
        if (!ifap) { return {}; }

        for (ifaddrs *ifa = ifap.value().get(); ifa != nullptr; ifa = ifa->ifa_next) {
            if (ifa->ifa_addr == nullptr) {
                continue;
            }
            if (match_sa_address<FAMILY, ADDRESS_TYPE, OFFSET>(*ifa->ifa_addr, address)) {
                return ifa->ifa_name;
            }
        }

        return {};
    }
}

namespace iface {
    std::expected<std::shared_ptr<ifaddrs>, std::string> getifaddrs() {
        ifaddrs *ifap{};
        if (int rv = getifaddrs(&ifap); rv != 0) {
            const auto err = errno;
            char buffer[128];
            return std::unexpected(strerror_r(err, buffer, sizeof(buffer)));
        }
        return std::shared_ptr<ifaddrs>{ifap, freeifaddrs};
    }

    std::optional<std::string> get_ifacename(const inaddr_storage &address) {
        using R = std::optional<std::string>;
        return std::visit(overloaded{
                              [](const in_addr &arg) -> R { return get_ifacename(arg); },
                              [](const in6_addr &arg)-> R { return get_ifacename(arg); },
                              [](const std::monostate &arg)-> R { return {}; },
                          }, address);
    }


    std::optional<std::string> get_ifacename(const in_addr &address) {
        return get_iface_name<AF_INET, in_addr, offsetof(sockaddr_in, sin_addr)>(address);
    }

    std::optional<std::string> get_ifacename(const in6_addr &address) {
        return get_iface_name<AF_INET6, in6_addr, offsetof(sockaddr_in6, sin6_addr)>(address);
    }

    std::optional<std::string> get_ifacename(const sockaddr &sa) {
        const auto ifap = getifaddrs();
        if (!ifap) { return {}; }

        for (ifaddrs *ifa = ifap.value().get(); ifa != nullptr; ifa = ifa->ifa_next) {
            if (ifa->ifa_addr == nullptr) {
                LOGD("Interface address unknown");
            }
            if (ifa->ifa_addr->sa_family != sa.sa_family) {
                continue;
            }
            if (match_sa_sa<AF_INET, in_addr, offsetof(sockaddr_in, sin_addr)>(*ifa->ifa_addr, sa) ||
                match_sa_sa<AF_INET6, in6_addr, offsetof(sockaddr_in6, sin6_addr)>(*ifa->ifa_addr, sa)) {
                return ifa->ifa_name;
            }
        }
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
