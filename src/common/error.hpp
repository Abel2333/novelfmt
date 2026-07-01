#pragma once

#include <cstdlib>
#include <string>
#include <utility>

#include "fmt/base.h"
#include "fmt/format.h"

namespace novelfmt {

template <typename... Args>
[[noreturn]] void Fail(fmt::format_string<Args...> fmt_str, Args&&... args) {
    fmt::print(stderr, "fatal: ");
    fmt::print(stderr, fmt_str, std::forward<Args>(args)...);
    fmt::print(stderr, "\n");
    std::fflush(stderr);
    std::abort();
}

enum class ErrorKind { Cli, Io, Format, Internal };

struct Error {
    ErrorKind kind;
    std::string message;
    int exit_code = 1;
};

template <typename... Args>
[[nodiscard]] Error MakeError(ErrorKind kind, fmt::format_string<Args...> fmt_str, Args&&... args) {
    return Error{
        .kind = kind,
        .message = fmt::format(fmt_str, std::forward<Args>(args)...),
        .exit_code = 1,
    };
}

template <typename... Args>
[[nodiscard]] Error MakeError(ErrorKind kind, int exit_code, fmt::format_string<Args...> fmt_str,
                              Args&&... args) {
    return Error{
        .kind = kind,
        .message = fmt::format(fmt_str, std::forward<Args>(args)...),
        .exit_code = exit_code,
    };
}

} // namespace novelfmt
