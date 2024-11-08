//
// Created by maggu2810 on 11/8/24.
//

#include "getifaddrs.hxx"

#include "to_string.hxx"

namespace net {
    std::expected<std::shared_ptr<ifaddrs>, std::string> getifaddrs() {
        ifaddrs *ifap{};
        if (int rv = getifaddrs(&ifap); rv != 0) {
            return std::unexpected(to_string(errno));
        }
        return std::shared_ptr<ifaddrs>{ifap, freeifaddrs};
    }
}
