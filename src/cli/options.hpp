#pragma once

#include <filesystem>
#include <optional>

namespace novelfmt {

enum class HeadingMode { ChapterOnly, VolumeAndChapter };

enum class ReportLevel { Silent, Summary, Verbose };

struct Options {
    std::filesystem::path input_path;
    std::optional<std::filesystem::path> output_path;

    bool in_place = false;
    bool dry_run = false;

    bool detect_titles = true;
    HeadingMode heading_mode = HeadingMode::ChapterOnly;

    bool normalize_quotes = true;
    bool normalize_line_endings = true;

    ReportLevel report_level = ReportLevel::Summary;
};

} // namespace novelfmt
