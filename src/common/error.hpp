#pragma once

#include <cstdlib>
#include <string>
#include <utility>

#include "fmt/base.h"
#include "fmt/format.h"

namespace novelfmt {

/**
 * @brief Prints a fatal message and aborts the process.
 *
 * @param fmt_str Format string for the fatal message body.
 * @param args Format arguments consumed by `fmt_str`.
 */
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

/**
 * @brief Builds an `Error` with the default exit code.
 *
 * @param kind High-level category of the failure.
 * @param fmt_str Format string for the user-facing message.
 * @param args Format arguments consumed by `fmt_str`.
 * @return Structured error value for propagation through `Result`.
 */
template <typename... Args>
[[nodiscard]] Error MakeError(ErrorKind kind, fmt::format_string<Args...> fmt_str, Args&&... args) {
    return Error{
        .kind = kind,
        .message = fmt::format(fmt_str, std::forward<Args>(args)...),
        .exit_code = 1,
    };
}

/**
 * @brief Builds an `Error` with an explicit process exit code.
 *
 * @param kind High-level category of the failure.
 * @param exit_code Exit code to surface from `main`.
 * @param fmt_str Format string for the user-facing message.
 * @param args Format arguments consumed by `fmt_str`.
 * @return Structured error value for propagation through `Result`.
 */
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
