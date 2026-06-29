#pragma once

#include <utility>

#include <tl/expected.hpp>

#include "common/error.hpp"

namespace novelfmt {

template <class T>
using Result = tl::expected<T, Error>;

template <typename... Args>
[[nodiscard]] tl::unexpected<Error> MakeUnexpected(
    ErrorKind kind,
    fmt::format_string<Args...> fmt_str,
    Args&&... args) {
    return tl::make_unexpected(MakeError(kind, fmt_str, std::forward<Args>(args)...));
}

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
