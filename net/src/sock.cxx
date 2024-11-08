//
// Created by maggu2810 on 11/6/24.
//

#include "sock.hxx"

#include <cstring>

#include "getaddrinfo.hxx"

#include "logging.hxx"
#include "to_string.hxx"

#include <fcntl.h>

namespace {
    int socket_type_to_ai_sock_type(net::socket_type socket_type) {
        switch (socket_type) {
            case net::socket_type::tcp: return SOCK_STREAM;
            case net::socket_type::udp: return SOCK_DGRAM;
        }
        std::unreachable();
    }

    int socket_type_to_ai_protocol(net::socket_type socket_type) {
        switch (socket_type) {
            case net::socket_type::tcp: return IPPROTO_TCP;
            case net::socket_type::udp: return IPPROTO_UDP;
        }
        std::unreachable();
    }

    void logd(addrinfo *ai) {
        for (addrinfo *p = ai; p != nullptr; p = p->ai_next) {
            LOGD("addrinfo: flags: {}, family: {}, socktype: {}, protocol: {}, addr: {}, canonname: {}",
                 p->ai_flags, p->ai_family, p->ai_socktype, p->ai_protocol,
                 p->ai_addr ? net::to_string(*p->ai_addr) : "<nullptr>",
                 p->ai_canonname ? p->ai_canonname : "<nullptr>");
        }
    }
}

namespace net {
    int create_bound_socket(socket_type socket_type, std::uint16_t port, bool non_blocking, bool reuse_addr) {
        addrinfo hints{};
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = socket_type_to_ai_sock_type(socket_type);
        hints.ai_flags = AI_NUMERICSERV | AI_PASSIVE;

        auto servinfo = getaddrinfo(nullptr, std::to_string(port).c_str(), hints);
        if (!servinfo) {
            LOGE("getaddrinfo: {}", servinfo.error());
            return -1;
        }

        logd(servinfo->get());

        int sockfd = [](addrinfo *servinfo, bool non_blocking, bool reuse_addr) {
            for (addrinfo *p = servinfo; p != nullptr; p = p->ai_next) {
                const int socket_flags = SOCK_CLOEXEC | (non_blocking ? SOCK_NONBLOCK : 0);
                const int sockfd = socket(p->ai_family, p->ai_socktype | socket_flags,
                                          p->ai_protocol);
                if (sockfd == -1) {
                    LOGE("socket: {}", to_string(errno));
                    continue;
                }

                if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
                    close(sockfd);
                    LOGE("bind: {}", to_string(errno));
                    continue;
                }

                if (reuse_addr) {
                    const int enable = 1;
                    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1) {
                        close(sockfd);
                        LOGE("setsockopt: {}", to_string(errno));
                        continue;
                    }
                }

                return sockfd;
            }
            return -1;
        }(servinfo.value().get(), non_blocking, reuse_addr);

        if (sockfd == -1) {
            return -1;
        }

        return sockfd;
    }

    int create_connected_socket(socket_type socket_type, const std::string &host, std::uint16_t port,
                                bool non_blocking) {
        addrinfo hints{};
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = socket_type_to_ai_sock_type(socket_type);
        hints.ai_flags = AI_NUMERICSERV;

        auto servinfo = getaddrinfo(host.c_str(), std::to_string(port).c_str(), hints);
        if (!servinfo) {
            LOGE("getaddrinfo: {}", servinfo.error());
            return -1;
        }

        logd(servinfo->get());

        int sockfd = [](addrinfo *servinfo, bool non_blocking) {
            for (addrinfo *p = servinfo; p != nullptr; p = p->ai_next) {
                constexpr int socket_flags = SOCK_CLOEXEC;
                const int sockfd = socket(p->ai_family, p->ai_socktype | socket_flags,
                                          p->ai_protocol);
                if (sockfd == -1) {
                    LOGE("socket: {}", to_string(errno));
                    continue;
                }

                if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
                    close(sockfd);
                    LOGE("connect: {}", to_string(errno));
                    continue;
                }

                if (non_blocking) {
                    if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) | O_NONBLOCK) == -1) {
                        close(sockfd);
                        continue;
                    }
                }

                return sockfd;
            }
            return -1;
        }(servinfo.value().get(), non_blocking);

        if (sockfd == -1) {
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
                LOGD("ipv4 pktinfo: {}, {}, {}", pktinfo->ipi_ifindex, to_string(pktinfo->ipi_spec_dst),
                     to_string(pktinfo->ipi_addr));
                if (my_sockaddr_storage_peer.ss_family != AF_INET) {
                    LOGE("mismatch between pktinfo and source address family");
                    return -1;
                }
                if (host_addr) {
                    *host_addr = pktinfo->ipi_spec_dst;
                }
            } else if (my_cmsghdr->cmsg_level == IPPROTO_IPV6 && my_cmsghdr->cmsg_type == IPV6_PKTINFO) {
                const auto pktinfo = reinterpret_cast<in6_pktinfo *>(CMSG_DATA(my_cmsghdr));
                LOGD("ipv6 pktinfo: {}, {}", pktinfo->ipi6_ifindex, to_string(pktinfo->ipi6_addr));
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
            LOGD("ipv4 sockaddr: {}, {}", to_string(sockaddr->sin_addr), sockaddr->sin_port);
        } else if (my_sockaddr_storage_peer.ss_family == AF_INET6) {
            const auto sockaddr = reinterpret_cast<sockaddr_in6 *>(&my_sockaddr_storage_peer);
            LOGD("ipv6 sockaddr: {}, {}, {}, {}", to_string(sockaddr->sin6_addr), sockaddr->sin6_port,
                 sockaddr->sin6_flowinfo,
                 sockaddr->sin6_scope_id);
        } else {
            LOGD("unknown sockaddr type");
            return -1;
        }

        if (peer_addr) {
            static_assert(sizeof(*peer_addr) == sizeof(my_sockaddr_storage_peer));
            std::memcpy(peer_addr, &my_sockaddr_storage_peer, sizeof(my_sockaddr_storage_peer));
        }

        return rv;
    }
} // udpserver
