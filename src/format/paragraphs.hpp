#pragma once

#include <string_view>
#include <vector>

namespace novelfmt {

/**
 * @brief Splits text into paragraph views using `\n` as the paragraph boundary.
 *
 * Empty paragraphs between adjacent newlines are preserved. A trailing newline does not produce
 * an extra empty paragraph.
 *
 * @param text Source text to partition.
 * @return Views into `text`, one per logical paragraph.
 */
auto split_paragraphs(std::string_view text) -> std::vector<std::string_view>;

} // namespace novelfmt
