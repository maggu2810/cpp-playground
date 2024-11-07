//
// Created by maggu2810 on 11/6/24.
//

#include "udpsock.hxx"

#include "sock.hxx"

#include <cstring>
#include <netdb.h>
#include <arpa/inet.h>

#include "logging.hxx"

namespace udpsock {
    int create_bound_udp_socket(std::uint16_t port) {
        addrinfo hints{};
        hints.ai_family = AF_UNSPEC; // AF_UNSPEC, AF_INET, AF_INET6
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_flags = AI_PASSIVE; // use my IP

        auto servinfo = sock::getaddrinfo(nullptr, std::to_string(port).c_str(), hints);
        if (!servinfo) {
            LOGE("getaddrinfo: {}", servinfo.error());
            return -1;
        }

        // loop through all the results and bind to the first we can
        int sockfd = [](addrinfo *servinfo) {
            for (addrinfo *p = servinfo; p != nullptr; p = p->ai_next) {
                const int sockfd = socket(p->ai_family, p->ai_socktype,
                                          p->ai_protocol);
                if (sockfd == -1) {
                    perror("socket");
                    continue;
                }

                if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
                    close(sockfd);
                    perror("bind");
                    continue;
                }

                const int enable = 1;
                if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1) {
                    close(sockfd);
                    perror("setsockopt");
                    continue;
                }

                return sockfd;
            }
            return -1;
        }(servinfo.value().get());

        if (sockfd == -1) {
            fprintf(stderr, "listener: failed to bind socket\n");
            return -1;
        }


        return sockfd;
    }

    std::pair<bool, bool> prepare_for_recv_info(int sock) {
        const int enable = 1;
        const bool ipv4 = setsockopt(sock, IPPROTO_IP, IP_PKTINFO, &enable, sizeof(enable)) == -1;
        if (!ipv4) {
            perror("setsockopt, ipv4 pktinfo");
        }
        const bool ipv6 = setsockopt(sock, IPPROTO_IPV6, IPV6_RECVPKTINFO, &enable, sizeof(enable)) == -1;
        if (!ipv6) {
            perror("setsockopt, ipv6 pktinfo");
        }
        return std::make_pair(ipv4, ipv6);
    }

    ssize_t recvfromadv(int sockfd, void *buf, size_t len,
                        inaddr_storage *host_addr,
                        sockaddr_storage *peer_addr) {
#if 0
        sockaddr_storage my_sockaddr_storage_host{};
        socklen_t my_sockaddr_storage_host_len = sizeof(my_sockaddr_storage_host);
        if (getsockname(sockfd, reinterpret_cast<sockaddr *>(&my_sockaddr_storage_host),
                        &my_sockaddr_storage_host_len) == 0) {
            const auto err = errno;
            // EPROTONOSUPPORT
            perror("getsockname");
            return -1;
        }
#endif

        sockaddr_storage my_sockaddr_storage_peer{};
        iovec my_iovec[1]{{.iov_base = buf, .iov_len = len}};
        std::uint8_t buffer[CMSG_SPACE(sizeof(in_pktinfo)) + CMSG_SPACE(sizeof(in6_pktinfo)) + 1000];

        msghdr my_msghdr{
            .msg_name = &my_sockaddr_storage_peer,
            .msg_namelen = sizeof(my_sockaddr_storage_peer),
            .msg_iov = my_iovec,
            .msg_iovlen = 1,
            .msg_control = buffer,
            .msg_controllen = sizeof(buffer),
            .msg_flags = 0
        };

        const ssize_t rv = recvmsg(sockfd, &my_msghdr, MSG_CMSG_CLOEXEC /*| MSG_DONTWAIT*/);
        if (rv == -1) {
            return -1;
        }

        for (cmsghdr *my_cmsghdr = CMSG_FIRSTHDR(&my_msghdr); my_cmsghdr != nullptr;
             my_cmsghdr = CMSG_NXTHDR(&my_msghdr, my_cmsghdr)) {
            if (my_cmsghdr->cmsg_level == IPPROTO_IP && my_cmsghdr->cmsg_type == IP_PKTINFO) {
                const auto pktinfo = reinterpret_cast<in_pktinfo *>(CMSG_DATA(my_cmsghdr));
                char ipi_spec_dst[INET_ADDRSTRLEN];
                if (inet_ntop(AF_INET, &pktinfo->ipi_spec_dst, ipi_spec_dst, sizeof(ipi_spec_dst)) == nullptr) {
                    perror("inet_ntop");
                    return -1;
                }
                char ipi_addr[INET_ADDRSTRLEN];
                if (inet_ntop(AF_INET, &pktinfo->ipi_addr, ipi_addr, sizeof(ipi_addr)) == nullptr) {
                    perror("inet_ntop");
                    return -1;
                }
                LOGD("ipv4 pktinfo: {}, {}, {}", pktinfo->ipi_ifindex, ipi_spec_dst, ipi_addr);
                if (my_sockaddr_storage_peer.ss_family != AF_INET) {
                    LOGE("mismatch between pktinfo and source address family");
                    return -1;
                }
                if (host_addr) {
                    *host_addr = pktinfo->ipi_spec_dst;
                }
            } else if (my_cmsghdr->cmsg_level == IPPROTO_IPV6 && my_cmsghdr->cmsg_type == IPV6_PKTINFO) {
                const auto pktinfo = reinterpret_cast<in6_pktinfo *>(CMSG_DATA(my_cmsghdr));
                char ipi6_addr[INET6_ADDRSTRLEN];
                if (inet_ntop(AF_INET6, &pktinfo->ipi6_addr, ipi6_addr, sizeof(ipi6_addr)) == nullptr) {
                    perror("inet_ntop");
                    return -1;
                }
                LOGD("ipv6 pktinfo: {}, {}", pktinfo->ipi6_ifindex, ipi6_addr);
                if (my_sockaddr_storage_peer.ss_family != AF_INET6) {
                    LOGE("mismatch between pktinfo and source address family");
                    return -1;
                }
                if (host_addr) {
                    *host_addr = pktinfo->ipi6_addr;
                }
            } else {
                LOGD("unhandled cmsg: level: {}, type: {}", my_cmsghdr->cmsg_level, my_cmsghdr->cmsg_type);
            }
        }

        if (my_sockaddr_storage_peer.ss_family == AF_INET) {
            const auto sockaddr = reinterpret_cast<sockaddr_in *>(&my_sockaddr_storage_peer);
            char sin_addr[INET_ADDRSTRLEN];
            if (inet_ntop(AF_INET, &sockaddr->sin_addr, sin_addr, sizeof(sin_addr)) == nullptr) {
                perror("inet_ntop");
                return -1;
            }
            LOGD("ipv4 sockaddr: {}, {}", sin_addr, sockaddr->sin_port);
        } else if (my_sockaddr_storage_peer.ss_family == AF_INET6) {
            const auto sockaddr = reinterpret_cast<sockaddr_in6 *>(&my_sockaddr_storage_peer);
            char sin6_addr[INET6_ADDRSTRLEN];
            if (inet_ntop(AF_INET6, &sockaddr->sin6_addr, sin6_addr, sizeof(sin6_addr)) == nullptr) {
                perror("inet_ntop");
                return -1;
            }
            LOGD("ipv6 sockaddr: {}, {}, {}, {}", sin6_addr, sockaddr->sin6_port, sockaddr->sin6_flowinfo,
                 sockaddr->sin6_scope_id);
        } else {
            LOGD("unknown sockaddr type");
            return -1;
        }

        if (peer_addr) {
            static_assert(sizeof(*peer_addr) == sizeof(my_sockaddr_storage_peer));
            memcpy(peer_addr, &my_sockaddr_storage_peer, sizeof(my_sockaddr_storage_peer));
        }
        return rv;
    }

#if 0
    // get sockaddr, IPv4 or IPv6:
    void *get_in_addr(struct sockaddr *sa) {
        if (sa->sa_family == AF_INET) {
            return &(((struct sockaddr_in *) sa)->sin_addr);
        }

        return &(((struct sockaddr_in6 *) sa)->sin6_addr);
    }

    int main(void) {
        int sockfd;
        struct addrinfo hints, *servinfo, *p;
        int rv;
        int numbytes;
        struct sockaddr_storage their_addr;
        char buf[MAXBUFLEN];
        socklen_t addr_len;
        char s[INET6_ADDRSTRLEN];

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC; // AF_UNSPEC, AF_INET, AF_INET6
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_flags = AI_PASSIVE; // use my IP

        if ((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            return 1;
        }

        // loop through all the results and bind to the first we can
        for (p = servinfo; p != NULL; p = p->ai_next) {
            if ((sockfd = socket(p->ai_family, p->ai_socktype,
                                 p->ai_protocol)) == -1) {
                perror("listener: socket");
                continue;
            }

            if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
                close(sockfd);
                perror("listener: bind");
                continue;
            }

            break;
        }

        if (p == NULL) {
            fprintf(stderr, "listener: failed to bind socket\n");
            return 2;
        }

        freeaddrinfo(servinfo);

        printf("listener: waiting to recvfrom...\n");

        addr_len = sizeof their_addr;
        if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN - 1, 0,
                                 (struct sockaddr *) &their_addr, &addr_len)) == -1) {
            perror("recvfrom");
            exit(1);
        }

        printf("listener: got packet from %s\n",
               inet_ntop(their_addr.ss_family,
                         get_in_addr((struct sockaddr *) &their_addr),
                         s, sizeof s));
        printf("listener: packet is %d bytes long\n", numbytes);
        buf[numbytes] = '\0';
        printf("listener: packet contains \"%s\"\n", buf);

        close(sockfd);

        return 0;
    }
#endif
} // udpserver
