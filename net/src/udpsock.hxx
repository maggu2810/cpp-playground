//
// Created by maggu2810 on 11/6/24.
//

#ifndef UDPSERVER_HXX
#define UDPSERVER_HXX

#include "inaddr_storage.hxx"

#include <cstdint>
#include <utility>
#include <sys/types.h>
#include <sys/socket.h>

namespace udpsock {
    int create_bound_udp_socket(std::uint16_t port);

    std::pair<bool, bool> prepare_for_recv_info(int sock);

    ssize_t recvfromadv(int sockfd, void *buf, size_t len,
                        inaddr_storage *host_addr,
                        sockaddr_storage *peer_addr);
} // udpserver

#endif //UDPSERVER_HXX
