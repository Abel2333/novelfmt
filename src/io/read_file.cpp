#include "read_file.hpp"

#include <filesystem>
#include <fstream>
#include <optional>
#include <string>

#include "common/result.hpp"

novelfmt::Result<std::optional<std::string>> read_file(const std::filesystem::path& path) {
    const auto file_size = std::filesystem::file_size(path);

    if (file_size == 0) {
        return std::optional<std::string>{};
    }

    std::ifstream file(path, std::ios::binary);
    if (!file) {
        return novelfmt::MakeUnexpected(novelfmt::ErrorKind::Io, "cannot open file: {}",
                                        path.string());
    }

    std::string content(file_size, '\0');
    file.read(content.data(), static_cast<std::streamsize>(file_size));
    if (file.fail() && !file.eof()) {
        return novelfmt::MakeUnexpected(novelfmt::ErrorKind::Io, "error reading file: {}",
                                        path.string());
    }

    content.resize(static_cast<std::size_t>(file.gcount()));
    return content;
}
