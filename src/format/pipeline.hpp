#pragma once

#include <string_view>

#include "cli/options.hpp"

std::string run_pipeline(std::string_view text, const Options& options);
