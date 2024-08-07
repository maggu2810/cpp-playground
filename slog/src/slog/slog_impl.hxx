//
// Created by maggu2810 on 8/5/24.
//

#ifndef CPP_PLAYGROUND_LOGGING_IMPL_HXX
#define CPP_PLAYGROUND_LOGGING_IMPL_HXX

#include "wrap_fmt.hxx"

#define LOGGING_IMPL_SLOC_ENABLED 0
#define LOGGING_IMPL_TID_ENABLED 1

#define SLOG_IMPL_STD_SLOC 0

#if SLOG_IMPL_STD_SLOC
#include <source_location>
using slog_impl_source_location = std::source_location;
#else
#include <cstdint>

struct slog_impl_source_location {
    constexpr std::uint_least32_t
    line() const noexcept { return 0u; }

    constexpr uint_least32_t
    column() const noexcept { return 0u; }

    constexpr const char *
    file_name() const noexcept { return ""; }

    constexpr const char *
    function_name() const noexcept { return ""; }
};
#endif

#include <thread>
#include <optional>

#include "level.hxx"


namespace slog::impl {
    class Opaque {
    public:
        constexpr Opaque(const std::optional<slog_impl_source_location> sloc = {
#if LOGGING_IMPL_SLOC_ENABLED
                             std::source_location::current()
#endif
                         }, const std::optional<std::thread::id> tid = {
#if LOGGING_IMPL_TID_ENABLED
                             std::this_thread::get_id()
#endif
                         }) : m_sloc(sloc), m_tid(tid) {
        }

        [[nodiscard]] constexpr const std::optional<slog_impl_source_location> &sloc() const { return m_sloc; };

        [[nodiscard]] constexpr const std::optional<std::thread::id> &tid() const { return m_tid; };

    private:
        const std::optional<slog_impl_source_location> m_sloc;
        const std::optional<std::thread::id> m_tid;
    };

    void log(const impl::Opaque &opaque, level lvl, const std::string &str);

    template<class... Args>
    constexpr void log(const impl::Opaque &opaque, level lvl, slog_fmt_ns::format_string<Args...> fmt,
                       Args &&... args) {
        log(opaque, lvl, slog_fmt_ns::format(fmt, std::forward<Args>(args)...));
    }
}

#endif //CPP_PLAYGROUND_LOGGING_IMPL_HXX
