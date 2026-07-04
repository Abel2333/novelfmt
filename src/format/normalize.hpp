#pragma once

#include <string>
#include <string_view>
#include "common/result.hpp"

namespace novelfmt {

/**
 * @brief Converts a UTF-8 buffer to NFC normalization form.
 *
 * @param utf8_text Source UTF-8 text.
 * @param normalized_text Output buffer that receives normalized UTF-8 text.
 * @return Success or a Unicode normalization error.
 */
Result<void> normalize_to_nfc(std::string_view utf8_text, std::string& normalized_text);

} // namespace novelfmt
