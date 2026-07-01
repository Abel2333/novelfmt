#pragma once

#include <filesystem>
#include <string_view>

#include "common/result.hpp"

namespace novelfmt {

Result<void> write_file(const std::filesystem::path& path, std::string_view content);

Result<void> atomic_write_file(const std::filesystem::path& path, std::string_view content);

Result<void> write_stdout(std::string_view content);

} // namespace novelfmt
