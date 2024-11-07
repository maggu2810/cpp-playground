cmake_minimum_required(VERSION 3.28.2)

set(TARGET net)

project(${TARGET})

add_executable(${TARGET}
        src/cleanup.cxx
        src/cleanup.hxx
        src/iface.cxx
        src/iface.hxx
        src/logging.hxx
        src/main.cxx
        src/sock.cxx
        src/sock.hxx
        src/udpsock.cxx
        src/udpsock.hxx
        src/net.cxx
        src/net.hxx
        src/visit_utils.hxx
)

target_compile_features(${TARGET} PRIVATE cxx_std_23)
