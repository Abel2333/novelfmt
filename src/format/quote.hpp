#pragma once

#include <string>

namespace novelfmt {

/**
 * @brief Normalizes quotation marks across the full text buffer.
 *
 * The implementation repairs malformed quote direction, normalizes glyph choice, and carries
 * limited dialogue state across paragraph boundaries.
 *
 * @param text UTF-8 text buffer to normalize in place.
 */
void normalize_quotes(std::string& text);

} // namespace novelfmt
