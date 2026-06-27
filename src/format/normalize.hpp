#pragma once

#include <string>
#include <string_view>

std::string normalize_to_nfc(const std::string_view utf8_text);
