#pragma once

#include <utility>

#include <tl/expected.hpp>

#include "common/error.hpp"

namespace novelfmt {

template <class T>
using Result = tl::expected<T, Error>;

/**
 * @brief Wraps a formatted error into `tl::unexpected`.
 *
 * @param kind High-level category of the failure.
 * @param fmt_str Format string for the user-facing message.
 * @param args Format arguments consumed by `fmt_str`.
 * @return Unexpected result payload for `Result<T>`.
 */
template <typename... Args>
[[nodiscard]] tl::unexpected<Error> MakeUnexpected(
    ErrorKind kind,
    fmt::format_string<Args...> fmt_str,
    Args&&... args) {
    return tl::make_unexpected(MakeError(kind, fmt_str, std::forward<Args>(args)...));
}

/**
 * @brief Wraps a formatted error with an explicit exit code into `tl::unexpected`.
 *
 * @param kind High-level category of the failure.
 * @param exit_code Exit code to surface from `main`.
 * @param fmt_str Format string for the user-facing message.
 * @param args Format arguments consumed by `fmt_str`.
 * @return Unexpected result payload for `Result<T>`.
 */
template <typename... Args>
[[nodiscard]] tl::unexpected<Error> MakeUnexpected(
    ErrorKind kind,
    int exit_code,
    fmt::format_string<Args...> fmt_str,
    Args&&... args) {
    return tl::make_unexpected(
        MakeError(kind, exit_code, fmt_str, std::forward<Args>(args)...));
}

} // namespace novelfmt
