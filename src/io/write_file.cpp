#include "write_file.hpp"

#include <filesystem>
#include <stdexcept>
#include <system_error>

#include "fmt/core.h"
#include "fmt/format.h"
#include "fmt/os.h"

namespace {
void write_all(const std::filesystem::path& path, std::string_view content) {
    try {
        auto out = fmt::output_file(path.string());
        out.print("{}", content);
    } catch (const std::system_error& e) {
        throw std::runtime_error(
            fmt::format("cannot write file: {} - {}", path.string(), e.what()));
    }
}
} // namespace

void write_file(const std::filesystem::path& path, std::string_view content) {
    write_all(path, content);
}

std::filesystem::path make_temp_path(const std::filesystem::path& target) {
    auto temp = target;
    temp += ".tmp";
    return temp;
}

void atomic_write_file(const std::filesystem::path& path, std::string_view content) {
    const auto temp_path = make_temp_path(path);

    write_all(temp_path, content);

    std::error_code ec;
    std::filesystem::rename(temp_path, path, ec);
    if (!ec) {
        return;
    }

    std::filesystem::remove(path, ec);
    ec.clear();
    std::filesystem::rename(temp_path, path, ec);
    if (ec) {
        std::filesystem::remove(temp_path);
        throw std::runtime_error(fmt::format("failed to replace file: {}", path.string()));
    }
}

void write_stdout(std::string_view content) {
    try {
        fmt::print("{}", content);
    } catch (const std::system_error& e) {
        throw std::runtime_error(fmt::format("failed to write to stdout: {}", e.what()));
    }
}
