#pragma once

#include <filesystem>
#include <optional>
#include <string>

#include "common/result.hpp"

namespace novelfmt {

/**
 * @brief Reads an input file into memory.
 *
 * @param path Path to the source file.
 * @return File contents on success, `std::nullopt` for an empty file, or an IO error.
 */
Result<std::optional<std::string>> read_file(const std::filesystem::path& path);

} // namespace novelfmt
