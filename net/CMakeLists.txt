cmake_minimum_required(VERSION 3.28.2)

set(TARGET net)

project(${TARGET})

add_executable(${TARGET}
        src/cleanup.cxx
        src/cleanup.hxx
        src/getaddrinfo.hxx
        src/getaddrinfo.cxx
        src/getifaddrs.hxx
        src/getifaddrs.cxx
        src/iface.cxx
        src/iface.hxx
        src/inaddr_storage.hxx
        src/logging.hxx
        src/main.cxx
        src/overloaded.hxx
        src/sock.cxx
        src/sock.hxx
        src/socket_type.hxx
        src/to_string.cxx
        src/to_string.hxx
)

target_compile_features(${TARGET} PRIVATE cxx_std_23)
