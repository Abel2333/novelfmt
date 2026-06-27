#include <filesystem>
#include <string_view>

#include "cli/parse_cli.hpp"
#include "format/pipeline.hpp"
#include "io/read_file.hpp"
#include "io/write_file.hpp"

#include <fmt/base.h>
#include <re2/re2.h>
#include <unicode/brkiter.h>
#include <unicode/locid.h>
#include <unicode/normalizer2.h>
#include <unicode/ubrk.h>
#include <unicode/unistr.h>

int main(int argc, char* argv[]) {
    auto options = parse_cli(argc, argv);

    auto content = read_file(options.input_path);

    if (!content.has_value()) {
        fmt::print("The file {} is empty", options.input_path.string());
        return 0;
    }

    auto content_view = std::string_view(*content);

    auto formatted_content = run_pipeline(content_view, options);

    if (options.output_path.has_value()) {
        write_file(options.output_path.value(), formatted_content);
    } else {
        fmt::print("Formatted Content:\n {}", formatted_content);
    }

    return 0;
}
