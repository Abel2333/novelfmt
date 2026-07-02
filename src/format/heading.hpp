#pragma once

#include <optional>
#include <string>
#include <string_view>

#include "cli/options.hpp"

namespace novelfmt {

/**
 * @brief Detects and rewrites a paragraph as a Markdown heading when it matches a title pattern.
 *
 * @param raw Raw paragraph text.
 * @param mode Heading style to emit when a match is found.
 * @return Formatted heading text, or `std::nullopt` when the paragraph is not a heading.
 */
std::optional<std::string> format_heading(std::string_view raw, HeadingMode mode);

} // namespace novelfmt
