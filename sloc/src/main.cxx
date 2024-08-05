#include <format>
#include <source_location>
#include <iostream>


#if 0
//#include <spdlog/spdlog.h>


namespace logging {
    template<class... Args>
    class FormatStringWithLocation {
    public:
        constexpr FormatStringWithLocation(const std::format_string<Args...> &_fmt,
                                           const std::source_location &_sloc = std::source_location::current()) : fmt(
                _fmt), sloc(_sloc) {}

        const std::format_string<Args...> &fmt;
        const std::source_location sloc;
    };

    template<class... Args>
    void log(std::format_string<Args...> fmt, Args &&... args) {
        std::cout << std::format(fmt, std::forward<Args>(args)...) << "\n";
    }


    [[nodiscard]] constexpr auto
    get_log_source_location(const std::source_location &location = std::source_location::current()) {
        return spdlog::source_loc{location.file_name(), static_cast<std::int32_t>(location.line()),
                                  location.function_name()};
    }

#if 0
    struct format_with_location {
        std::string_view value;
        spdlog::source_loc loc;

        template<typename String>
        format_with_location(const String &s, const std::source_location &location = std::source_location::current())
                : value{s}, loc{get_log_source_location(location)} {}
    };

    template<typename... Args>
    void warn(format_with_location fmt, Args &&...args) {
#if 1
        spdlog::default_logger_raw()->log(fmt.loc, spdlog::level::warn, fmt::runtime(fmt.value),
                                          std::forward<Args>(args)...);
#else
        spdlog::default_logger_raw()->log(fmt.loc, spdlog::level::warn,
                                          std::vformat(fmt.value, std::make_format_args(std::forward<Args>(args)...)));
#endif
    }
#endif

    std::shared_ptr<spdlog::logger> _create() {
        auto logger = spdlog::default_logger();
        logger->set_level(spdlog::level::trace);
        // https://github.com/gabime/spdlog/wiki/3.-Custom-formatting
        logger->set_pattern("[%H:%M:%S %z] [thread %t] [%s:%#] [%!] %v");
        return logger;
    }

    std::shared_ptr<spdlog::logger> logger() {
        static auto instance = _create();
        return instance;
    }

}  // namespace logging

#define LOG_TRACE(...) LOG(spdlog::level::trace, __VA_ARGS__)
#define LOGL_TRACE(logger, ...) LOGL(logger, spdlog::level::trace, __VA_ARGS__)
#define LOG_DEBUG(...) LOG(spdlog::level::debug, __VA_ARGS__)
#define LOGL_DEBUG(logger, ...) LOGL(logger, spdlog::level::debug, __VA_ARGS__)
#define LOG_INFO(...) LOG(spdlog::level::info, __VA_ARGS__)
#define LOGL_INFO(logger, ...) LOGL(logger, spdlog::level::info, __VA_ARGS__)
#define LOG_WARN(...) LOG(spdlog::level::warn, __VA_ARGS__)
#define LOGL_WARN(logger, ...) LOGL(logger, spdlog::level::warn, __VA_ARGS__)
#define LOG_ERROR(...) LOG(spdlog::level::err, __VA_ARGS__)
#define LOGL_ERROR(logger, ...) LOGL(logger, spdlog::level::err, __VA_ARGS__)
#define LOG_CRITICAL(...) LOG(spdlog::level::critical, __VA_ARGS__)
#define LOGL_CRITICAL(logger, ...) LOGL(logger, spdlog::level::critical, __VA_ARGS__)

#define LOG(level, ...) LOGL(logging::logger(), level, __VA_ARGS__)
//#define LOGL(logger, level, ...) logger->log(level, __VA_ARGS__)
#define LOGL(logger, level, ...) logger->log(logging::get_log_source_location(), level, __VA_ARGS__)

//#define LOGE(_A1, _A2, ...) do { if constexpr (std::is_same_v<std::shared_ptr<spdlog::logger>, decltype(_A1)>) { _A1->log(logging::get_log_source_location(), _A2, __VA_ARGS__); } else { logging::logger()->log(logging::get_log_source_location(), _A1, _A2, __VA_ARGS__);} } while(0)
#define LOGTEST(...) logging::fwd(logging::get_log_source_location(), logging::logger(), spdlog::level::warn, __VA_ARGS__)

void foo(int x) {
    //LOGE(spdlog::level::warn, "hello {}", x);
    LOG_ERROR("hello {}", x);
}

int main() {
    auto logger = spdlog::default_logger_raw();

    auto value = 42;

    LOGL_ERROR(logger, "hello {}", value);
    LOG_ERROR("hello {}", value);

    foo(3);
    std::cout << std::format("{} {}", "hello word", 42);

    return 0;
}
#else

#define LOGGING_IMPL_SLOC_ENABLED 1

namespace logging {
    namespace impl {
        class Opaque {
        public:
            consteval Opaque(const std::source_location sloc =
#if LOGGING_IMPL_SLOC_ENABLED
                    std::source_location::current()
#else
                    {}
#endif
                    ) : m_sloc(sloc) {}

            [[nodiscard]] constexpr const std::source_location &sloc() const { return m_sloc; };
        private:
            const std::source_location m_sloc;
        };
    }

    enum class Level {
        warn, info
    };

    template<class... Args>
    void log(impl::Opaque opaque, Level lvl, std::format_string<Args...> fmt, Args &&... args) {
        std::cout << "level: " << static_cast<int>(lvl) << " | " << opaque.sloc().line() << " => "
                  << std::format(fmt, std::forward<Args>(args)...) << "\n";
    }

    template<class... Args>
    void info(impl::Opaque opaque, std::format_string<Args...> fmt, Args &&... args) {
        log(opaque, Level::info, fmt, std::forward<Args>(args)...);
    }

    template<class... Args>
    void warn(impl::Opaque opaque, std::format_string<Args...> fmt, Args &&... args) {
        log(opaque, Level::warn, fmt, std::forward<Args>(args)...);
    }



}


int main() {
    //std::cout << std::format("{} {}", "hello word", 42);
    logging::log({}, logging::Level::info, "{} {}", "hello word", 42);
    logging::info({}, "{} {}", "hello word", 42);
    logging::warn({}, "{} {}", "hello word", 42);

    return 0;
}

#endif