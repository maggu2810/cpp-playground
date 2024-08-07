cmake_minimum_required(VERSION 3.28.2)
LIST(PREPEND CMAKE_PREFIX_PATH "/home/maggu2810/workspace/projects/oss/fmt_8.1.1.install")
find_package(fmt REQUIRED)
add_executable(sloc
        src/main.cxx
        src/slog/level.hxx
        src/slog/slog_impl.hxx
        src/slog/slog.hxx
        src/slog/slog_impl.cxx
)
target_compile_features(sloc PRIVATE cxx_std_17)
target_link_libraries(sloc PRIVATE systemd fmt::fmt)