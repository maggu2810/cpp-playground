#include "slog/slog.hxx"

int main() {
    slog::log({}, slog::level::info, "{} {}", "hello world", 42);
    slog::info({}, "{} {}", "hello world", 42);
    slog::warn({}, "{} {}", "hello world", 42);

    return 0;
}
