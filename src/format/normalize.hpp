#pragma once

#include <string>
#include <string_view>

void normalize_to_nfc(std::string_view utf8_text, std::string& normalized_text);
