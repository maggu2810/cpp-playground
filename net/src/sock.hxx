//
// Created by maggu2810 on 11/6/24.
//

#ifndef UDPSERVER_HXX
#define UDPSERVER_HXX

#include "inaddr_storage.hxx"

#include <cstdint>
#include <string>
#include <utility>
#include <sys/types.h>
#include <sys/socket.h>

#include "socket_type.hxx"

namespace net {
    int create_bound_socket(socket_type socket_type, std::uint16_t port, bool non_blocking = false, bool reuse_addr = false);

    int create_connected_socket(socket_type socket_type, const std::string& host, std::uint16_t port, bool non_blocking = false);

    std::pair<bool, bool> prepare_for_recv_info(int sock);

    ssize_t recvfromadv(int sockfd, void *buf, size_t len,
                        inaddr_storage *host_addr,
                        sockaddr_storage *peer_addr);
} // udpserver

#endif //UDPSERVER_HXX
