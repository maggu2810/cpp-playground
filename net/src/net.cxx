//
// Created by maggu2810 on 11/7/24.
//

#include "net.hxx"

#include <format>
#include <stdexcept>
#include <arpa/inet.h>

#include "visit_utils.hxx"

namespace {
    template<typename INADDR, int AF, int SZ>
    std::string inet_ntop_wrapper(const INADDR &address) {
        char buf[SZ];
        if (::inet_ntop(AF, &address, buf, sizeof(buf)) == nullptr) {
            throw std::runtime_error(std::format(
                "Failed to convert address to string (expected because we supply correct values) [errno: {}]", errno));
        }
        return {buf};
    }
}

namespace net {
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
} // net
