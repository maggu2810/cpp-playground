//
// Created by maggu2810 on 11/8/24.
//

#ifndef TO_STRING_HXX
#define TO_STRING_HXX

#include "inaddr_storage.hxx"

#include <string>

namespace net {
std::string to_string(int errnum);

    std::string to_string(const inaddr_storage& address);
    std::string to_string(const in_addr &address);
    std::string to_string(const in6_addr &address);

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
}

#endif //TO_STRING_HXX
