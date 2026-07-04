#pragma once

#include <filesystem>
#include <string_view>

#include "common/result.hpp"

namespace novelfmt {

/**
 * @brief Writes text directly to a file path.
 *
 * @param path Destination file path.
 * @param content UTF-8 text to write.
 * @return Success or an IO error.
 */
Result<void> write_file(const std::filesystem::path& path, std::string_view content);

/**
 * @brief Replaces a file by writing through a temporary sibling path.
 *
 * @param path Destination file path.
 * @param content UTF-8 text to write.
 * @return Success or an IO error.
 */
Result<void> atomic_write_file(const std::filesystem::path& path, std::string_view content);

/**
 * @brief Writes text to standard output.
 *
 * @param content UTF-8 text to print.
 * @return Success or an IO error.
 */
Result<void> write_stdout(std::string_view content);

} // namespace novelfmt
