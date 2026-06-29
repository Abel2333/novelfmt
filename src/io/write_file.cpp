#include "write_file.hpp"

#include <filesystem>
#include <system_error>

#include "common/error.hpp"
#include "common/result.hpp"
#include "fmt/core.h"
#include "fmt/os.h"

namespace {
novelfmt::Result<void> write_all(const std::filesystem::path& path, std::string_view content) {
    try {
        auto out = fmt::output_file(path.string());
        out.print("{}", content);
    } catch (const std::system_error& e) {
        return novelfmt::MakeUnexpected(novelfmt::ErrorKind::Io, "cannot write file: {} - {}",
                                        path.string(), e.what());
    }

    return {};
}
} // namespace

novelfmt::Result<void> write_file(const std::filesystem::path& path, std::string_view content) {
    return write_all(path, content);
}

std::filesystem::path make_temp_path(const std::filesystem::path& target) {
    auto temp = target;
    temp += ".tmp";
    return temp;
}

novelfmt::Result<void> atomic_write_file(const std::filesystem::path& path,
                                         std::string_view content) {
    const auto temp_path = make_temp_path(path);

    auto write_result = write_all(temp_path, content);
    if (!write_result) {
        return write_result;
    }

    std::error_code ec;
    std::filesystem::rename(temp_path, path, ec);
    if (!ec) {
        return {};
    }

    std::filesystem::remove(path, ec);
    ec.clear();
    std::filesystem::rename(temp_path, path, ec);
    if (ec) {
        std::filesystem::remove(temp_path);
        return novelfmt::MakeUnexpected(novelfmt::ErrorKind::Io, "failed to replace file: {}",
                                        path.string());
    }

    return {};
}

novelfmt::Result<void> write_stdout(std::string_view content) {
    try {
        fmt::print("{}", content);
    } catch (const std::system_error& e) {
        return novelfmt::MakeUnexpected(novelfmt::ErrorKind::Io, "failed to write to stdout: {}",
                                        e.what());
    }

    return {};
}
