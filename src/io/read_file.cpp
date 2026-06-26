#include "read_file.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <optional>
#include <stdexcept>
#include <string>

#include "fmt/format.h"

std::optional<std::string> read_file(const std::filesystem::path& path) {
    const auto file_size = std::filesystem::file_size(path);

    if (file_size == 0) {
        return std::nullopt;
    }

    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error(fmt::format("cannot open file: {}", path.string()));
    }

    std::string content(file_size, '\0');
    file.read(content.data(), static_cast<std::streamsize>(file_size));
    if (file.fail() && !file.eof()) {
        throw std::runtime_error(fmt::format("error reading file: {}", path.string()));
    }

    content.resize(static_cast<std::size_t>(file.gcount()));
    content.erase(std::remove(content.begin(), content.end(), '\r'), content.end());

    return content;
}
