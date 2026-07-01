#pragma once

#include <tl/expected.hpp>

#define NOVELFMT_CONCAT_INNER(x, y) x##y
#define NOVELFMT_CONCAT(x, y) NOVELFMT_CONCAT_INNER(x, y)

#define NOVELFMT_TRY(expr)                                                                         \
    do {                                                                                           \
        auto NOVELFMT_CONCAT(_novelfmt_result_, __LINE__) = (expr);                                \
        if (!NOVELFMT_CONCAT(_novelfmt_result_, __LINE__)) {                                       \
            return tl::make_unexpected(                                                            \
                std::move(NOVELFMT_CONCAT(_novelfmt_result_, __LINE__)).error());                  \
        }                                                                                          \
    } while (false)

#define NOVELFMT_TRY_ASSIGN(lhs, expr)                                                             \
    do {                                                                                           \
        auto NOVELFMT_CONCAT(_novelfmt_result_, __LINE__) = (expr);                                \
        if (!NOVELFMT_CONCAT(_novelfmt_result_, __LINE__)) {                                       \
            return tl::make_unexpected(                                                            \
                std::move(NOVELFMT_CONCAT(_novelfmt_result_, __LINE__)).error());                  \
        }                                                                                          \
        lhs = std::move(NOVELFMT_CONCAT(_novelfmt_result_, __LINE__)).value();                     \
    } while (false)
