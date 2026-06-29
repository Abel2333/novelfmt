#include <filesystem>
#include <optional>
#include <string_view>

#include "cli/parse_cli.hpp"
#include "common/try.hpp"
#include "format/pipeline.hpp"
#include "io/read_file.hpp"
#include "io/write_file.hpp"

#include <fmt/base.h>
#include <fmt/format.h>
#include <re2/re2.h>
#include <unicode/brkiter.h>
#include <unicode/locid.h>
#include <unicode/normalizer2.h>
#include <unicode/ubrk.h>
#include <unicode/unistr.h>

namespace {

novelfmt::Result<void> run(int argc, char* argv[]) {
    auto options = parse_cli(argc, argv);

    std::optional<std::string> content;
    NOVELFMT_TRY_ASSIGN(content, read_file(options.input_path));

    if (!content.has_value()) {
        NOVELFMT_TRY(write_stdout(fmt::format("The file {} is empty", options.input_path.string())));
        return {};
    }

    auto content_view = std::string_view(*content);

    auto formatted_content = run_pipeline(content_view, options);

    if (options.output_path.has_value()) {
        NOVELFMT_TRY(write_file(options.output_path.value(), formatted_content));
        return {};
    }

    NOVELFMT_TRY(write_stdout(fmt::format("Formatted Content:\n {}", formatted_content)));
    return {};
}

} // namespace

int main(int argc, char* argv[]) {
    auto result = run(argc, argv);
    if (!result) {
        const auto& error = result.error();
        fmt::print(stderr, "fatal: {}\n", error.message);
        return error.exit_code;
    }

    return 0;
}
