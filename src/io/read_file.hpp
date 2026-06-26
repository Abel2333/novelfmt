#pragma once

#include <filesystem>
#include <optional>
#include <string>

std::optional<std::string> read_file(const std::filesystem::path& path);
