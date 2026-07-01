#pragma once

#include <string>
#include <string_view>
#include "common/result.hpp"

namespace novelfmt {

Result<void> normalize_to_nfc(std::string_view utf8_text, std::string& normalized_text);

} // namespace novelfmt
