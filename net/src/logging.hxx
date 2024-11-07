//
// Created by maggu2810 on 11/6/24.
//

#ifndef LOG_HXX
#define LOG_HXX

#include <iostream>
#include <ostream>
#include <format>
#include <valarray>

#define LOGD(...) logging::debug(__VA_ARGS__)
#define LOGE(...) logging::err(__VA_ARGS__)

namespace logging {
    enum class level {
        emergency, alert, critical, error, warning, notice, info, debug
    };

    namespace impl {
        inline void log(level lvl, const std::string &str) {
            std::cout << str << std::endl;
        }

        template<class... Args>
        constexpr void log(level lvl, std::format_string<Args...> fmt,
                           Args &&... args) {
            log(lvl, std::format(fmt, std::forward<Args>(args)...));
        }
    }


    template<class... Args>
    constexpr void log(level lvl, std::format_string<Args...> fmt, Args &&... args) {
        impl::log(lvl, fmt, std::forward<Args>(args)...);
    }

    template<class... Args>
    constexpr void emerg(std::format_string<Args...> fmt, Args &&... args) {
        impl::log(level::emergency, fmt, std::forward<Args>(args)...);
    }

    template<class... Args>
    constexpr void alert(std::format_string<Args...> fmt, Args &&... args) {
        impl::log(level::alert, fmt, std::forward<Args>(args)...);
    }

    template<class... Args>
    constexpr void crit(std::format_string<Args...> fmt, Args &&... args) {
        impl::log(level::critical, fmt, std::forward<Args>(args)...);
    }

    template<class... Args>
    constexpr void err(std::format_string<Args...> fmt, Args &&... args) {
        impl::log(level::error, fmt, std::forward<Args>(args)...);
    }

    template<class... Args>
    constexpr void warn(std::format_string<Args...> fmt, Args &&... args) {
        impl::log(level::warning, fmt, std::forward<Args>(args)...);
    }

    template<class... Args>
    constexpr void notice(std::format_string<Args...> fmt, Args &&... args) {
        impl::log(level::notice, fmt, std::forward<Args>(args)...);
    }

    template<class... Args>
    constexpr void info(std::format_string<Args...> fmt, Args &&... args) {
        impl::log(level::info, fmt, std::forward<Args>(args)...);
    }

    template<class... Args>
    constexpr void debug(std::format_string<Args...> fmt, Args &&... args) {
        impl::log(level::debug, fmt, std::forward<Args>(args)...);
    }
}

#endif //LOG_HXX
