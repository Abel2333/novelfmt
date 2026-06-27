#include "format/heading.hpp"

#include <optional>
#include <string>

#include <fmt/format.h>
#include <re2/re2.h>
#include <re2/stringpiece.h>

std::optional<std::string> format_heading(std::string_view raw, HeadingMode mode) {
    static const re2::RE2 heading_regex(R"(^\s*()"
                                        R"(第[0-9一二三四五六七八九十百千两〇零]+[章节卷部回])"
                                        R"(|[0-9一二三四五六七八九十百千两〇零]+[章节卷部回])"
                                        R"(|序章|终章|尾声|番外篇?|楔子|引子|后记|前言|简介|序言)"
                                        R"()\s*(.*?)\s*$)");

    std::string marker; // chapter marker
    std::string title;
    re2::StringPiece input(raw.data(), raw.size());

    if (!RE2::FullMatch(input, heading_regex, &marker, &title)) {
        return std::nullopt;
    }

    if (title.empty()) {
        return fmt::format("## {}", marker);
    }
    return fmt::format("## {} {}", marker, title);
}
