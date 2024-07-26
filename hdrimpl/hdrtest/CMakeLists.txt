cmake_minimum_required(VERSION 3.28.2)

set(TARGETS hdrtest_bar hdrtest_foo hdrtest_bar_foo hdrtest_foo_bar)
foreach(TARGET IN LISTS TARGETS)
    add_executable(${TARGET}
            src/main.cxx
    )
endforeach()

target_link_libraries(hdrtest_bar PRIVATE bar)
target_link_libraries(hdrtest_foo PRIVATE foo)
target_link_libraries(hdrtest_bar_foo PRIVATE bar foo)
target_link_libraries(hdrtest_foo_bar PRIVATE foo bar)

target_compile_definitions(hdrtest_bar PRIVATE BAR_ENABLED)
target_compile_definitions(hdrtest_bar_foo PRIVATE BAR_ENABLED)
target_compile_definitions(hdrtest_foo PRIVATE FOO_ENABLED)
target_compile_definitions(hdrtest_foo_bar PRIVATE FOO_ENABLED)
