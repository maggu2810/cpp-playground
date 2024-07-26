cmake_minimum_required(VERSION 3.28.2)

add_library(bar
        include/bar/Bar.hxx
        src/Bar.cxx
)

target_include_directories(bar PUBLIC include)
target_compile_definitions(bar PRIVATE HDRLIB_X=5)
target_link_libraries(bar PRIVATE hdrlib)