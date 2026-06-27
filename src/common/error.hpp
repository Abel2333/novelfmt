#pragma once

#include <cstdlib>
#include <stdexcept>
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

template <typename... Args>
[[noreturn]] void Throw(fmt::format_string<Args...> fmt_str, Args&&... args) {
    throw std::runtime_error(fmt::format(fmt_str, std::forward<Args>(args)...));
}

} // namespace novelfmt
