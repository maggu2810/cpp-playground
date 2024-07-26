cmake_minimum_required(VERSION 3.28.2)

add_library(foo
        include/foo/Foo.hxx
        src/Foo.cxx
)

target_include_directories(foo PUBLIC include)
target_compile_definitions(foo PRIVATE HDRLIB_X=6)
target_link_libraries(foo PRIVATE hdrlib)