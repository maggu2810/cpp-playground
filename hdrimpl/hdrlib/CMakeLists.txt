cmake_minimum_required(VERSION 3.28.2)

add_library(hdrlib INTERFACE
    include/hdrlib/Simple.hxx
)

target_include_directories(hdrlib INTERFACE include)