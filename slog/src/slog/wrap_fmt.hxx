//
// Created by maggu2810 on 8/7/24.
//

#ifndef WRAP_FMT_HXX
#define WRAP_FMT_HXX

#define SLOG_IMPL_STD_FORMAT 0

#if SLOG_IMPL_STD_FORMAT
#include <format>
#define slog_fmt_ns std
#else
#include <fmt/format.h>
#define slog_fmt_ns fmt
#endif

#endif //WRAP_FMT_HXX
