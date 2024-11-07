//
// Created by maggu2810 on 11/7/24.
//

#ifndef INADDR_STORAGE_HXX
#define INADDR_STORAGE_HXX

#include <variant>
#include <netinet/in.h>

using inaddr_storage =std::variant<std::monostate, in_addr, in6_addr>;


#endif //INADDR_STORAGE_HXX
