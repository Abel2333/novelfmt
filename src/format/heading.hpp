#pragma once

#include <optional>
#include <string>
#include <string_view>

#include "cli/options.hpp"

std::optional<std::string> format_heading(std::string_view raw, HeadingMode mode);
