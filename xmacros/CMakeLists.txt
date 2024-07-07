cmake_minimum_required(VERSION 3.28.2)

set(TARGET xmacros)

project(${TARGET})

add_executable(${TARGET}
        src/main.cxx
        include/macros/overloading.h
        include/macros/enum_utils.h)
target_compile_features(${TARGET} PRIVATE cxx_std_23)
target_include_directories(${TARGET} PRIVATE include)