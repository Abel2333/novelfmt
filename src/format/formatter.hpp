#pragma once

#include <string>
#include <string_view>

#include "cli/options.hpp"

namespace novelfmt {

/**
 * @brief Formats a normalized text buffer paragraph by paragraph.
 *
 * @param text_view Source text split logically by `\n`.
 * @param result Output buffer appended with formatted paragraphs.
 * @param options Active formatter options.
 */
void formatter_text(std::string_view text_view, std::string& result, const Options& options);

} // namespace novelfmt
