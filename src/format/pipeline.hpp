#pragma once

#include <string_view>

#include "cli/options.hpp"
#include "common/result.hpp"

novelfmt::Result<std::string> run_pipeline(std::string_view text, const Options& options);
