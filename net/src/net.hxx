//
// Created by maggu2810 on 11/7/24.
//

#ifndef HXX
#define HXX

#include "inaddr_storage.hxx"

#include <string>

namespace net {
    std::string to_string(const inaddr_storage& address);
    std::string to_string(const in_addr &address);
    std::string to_string(const in6_addr &address);
} // net

#endif //HXX
