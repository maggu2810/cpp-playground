// Copyright [2024] [maggu2810]
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

//
// Created by maggu2810 on 8/5/24.
//

#ifndef CPP_PLAYGROUND_LOGGING_HXX
#define CPP_PLAYGROUND_LOGGING_HXX

#include "wrap_fmt.hxx"
#include "slog_impl.hxx"


namespace slog {
    template<class... Args>
    constexpr void log(impl::Opaque opaque, level lvl, slog_fmt_ns::format_string<Args...> fmt, Args &&... args) {
        impl::log(opaque, lvl, fmt, std::forward<Args>(args)...);
    }

    template<class... Args>
    constexpr void emerg(impl::Opaque opaque, slog_fmt_ns::format_string<Args...> fmt, Args &&... args) {
        impl::log(opaque, level::emergency, fmt, std::forward<Args>(args)...);
    }

    template<class... Args>
    constexpr void alert(impl::Opaque opaque, slog_fmt_ns::format_string<Args...> fmt, Args &&... args) {
        impl::log(opaque, level::alert, fmt, std::forward<Args>(args)...);
    }

    template<class... Args>
    constexpr void crit(impl::Opaque opaque, slog_fmt_ns::format_string<Args...> fmt, Args &&... args) {
        impl::log(opaque, level::critical, fmt, std::forward<Args>(args)...);
    }

    template<class... Args>
    constexpr void err(impl::Opaque opaque, slog_fmt_ns::format_string<Args...> fmt, Args &&... args) {
        impl::log(opaque, level::error, fmt, std::forward<Args>(args)...);
    }

    template<class... Args>
    constexpr void warn(impl::Opaque opaque, slog_fmt_ns::format_string<Args...> fmt, Args &&... args) {
        impl::log(opaque, level::warning, fmt, std::forward<Args>(args)...);
    }

    template<class... Args>
    constexpr void notice(impl::Opaque opaque, slog_fmt_ns::format_string<Args...> fmt, Args &&... args) {
        impl::log(opaque, level::notice, fmt, std::forward<Args>(args)...);
    }

    template<class... Args>
    constexpr void info(impl::Opaque opaque, slog_fmt_ns::format_string<Args...> fmt, Args &&... args) {
        impl::log(opaque, level::info, fmt, std::forward<Args>(args)...);
    }

    template<class... Args>
    constexpr void debug(impl::Opaque opaque, slog_fmt_ns::format_string<Args...> fmt, Args &&... args) {
        impl::log(opaque, level::debug, fmt, std::forward<Args>(args)...);
    }
}

#endif //CPP_PLAYGROUND_LOGGING_HXX
