#pragma once

#include <string>
#include <string_view>

#include "cli/options.hpp"

void formatter_text(std::string_view text_view, std::string& result, const Options& options);
