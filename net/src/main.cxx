//
// Created by maggu2810 on 11/6/24.
//

#include <iostream>

#include "iface.hxx"
#include "logging.hxx"
#include "net.hxx"
#include "sock.hxx"
#include "udpsock.hxx"

int main(int argc, char *argv[]) {
    int sock = udpsock::create_bound_udp_socket(13400);
    if (sock < 0) { return EXIT_FAILURE; };
    if (auto [ipv4, ipv6] = udpsock::prepare_for_recv_info(sock); !ipv4 && !ipv6) {
        return EXIT_FAILURE;
    }
    while (true) {
        std::uint8_t buf[1500];
        std::variant<std::monostate, in_addr, in6_addr> host_addr;
        sockaddr_storage ss_peer; // NOLINT(*-pro-type-member-init)
        if (udpsock::recvfromadv(sock, buf, sizeof(buf), &host_addr, &ss_peer) == -1) {
            break;
        }
        const auto iface = iface::get_ifacename(host_addr);
        LOGD("host address: {}, peer: {}, iface: {}", net::to_string(host_addr), sock::to_string(ss_peer), iface ? *iface: "unknown");
    }
    return EXIT_SUCCESS;
}
