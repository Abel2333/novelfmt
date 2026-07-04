#include "parse_cli.hpp"

#include <cstdlib>
#include <filesystem>
#include <string>

#include "CLI/CLI.hpp"

namespace novelfmt {

Options parse_cli(int argc, char* argv[]) {

    CLI::App app{"novelfmt - novel format tools"};
    std::string input_path;
    std::string output_path;
    bool in_place = false;
    bool dry_run = false;

    app.set_version_flag("--version", "0.1.0");

    app.add_option("input", input_path, "input file path")->required()->check(CLI::ExistingFile);

    std::string heading_mode_str = "chapter";
    app.add_option("--heading-mode", heading_mode_str,
                   "heading level: chapter (##) or volume-chapter (###)")
        ->check(CLI::IsMember{{"chapter", "volume-chapter"}});

    app.add_option("-o,--output", output_path, "output file path (default is stdout)");
    app.add_flag("--in-place", in_place, "rewrite the input file");
    app.add_flag("--dry-run", dry_run, "parse and report without writing output");

    // use Try-Catch to show the help menu when parse failed
    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        std::exit(app.exit(e));
    }

    // Build final Options for other parts of this project
    Options options;
    options.input_path = std::filesystem::path{input_path};
    if (!output_path.empty()) {
        options.output_path.emplace(std::filesystem::path{output_path});
    }
    options.in_place = in_place;
    options.dry_run = dry_run;

    if (heading_mode_str == "chapter") {
        options.heading_mode = HeadingMode::ChapterOnly;
    } else if (heading_mode_str == "volume-chapter") {
        options.heading_mode = HeadingMode::VolumeAndChapter;
    }

    return options;
}

} // namespace novelfmt
