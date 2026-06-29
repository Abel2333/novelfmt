#pragma once

#include <filesystem>
#include <optional>
#include <string>

#include "common/result.hpp"

using novelfmt::Result;

Result<std::optional<std::string>> read_file(const std::filesystem::path& path);
