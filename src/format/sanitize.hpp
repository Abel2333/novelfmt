#pragma once

#include <string>

namespace novelfmt {

/**
 * @brief Applies text-level punctuation and whitespace cleanup after structural normalization.
 *
 * @param text UTF-8 text buffer to sanitize in place.
 */
void sanitize_text(std::string& text);

} // namespace novelfmt
