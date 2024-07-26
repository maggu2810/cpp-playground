cmake_minimum_required(VERSION 3.28.2)
project(hdrimpl)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

add_subdirectory(bar)
add_subdirectory(foo)
add_subdirectory(hdrlib)
add_subdirectory(hdrtest)