#pragma once

#include <filesystem>
#include <string_view>

void write_file(const std::filesystem::path& path, std::string_view content);

void atomic_write_file(const std::filesystem::path& path, std::string_view content);

void write_stdout(std::string_view content);
