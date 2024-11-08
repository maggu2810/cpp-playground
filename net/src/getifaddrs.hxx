//
// Created by maggu2810 on 11/8/24.
//

#ifndef GETIFADDRS_HXX
#define GETIFADDRS_HXX

#include <expected>
#include <ifaddrs.h>
#include <memory>

namespace net {
    std::expected<std::shared_ptr<ifaddrs>, std::string> getifaddrs();
}

#endif //GETIFADDRS_HXX
