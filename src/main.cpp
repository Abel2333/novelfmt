#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "CLI/CLI.hpp"
#include "re2/re2.h"
#include "unicode/brkiter.h"
#include "unicode/locid.h"
#include "unicode/normalizer2.h"
#include "unicode/ubrk.h"
#include "unicode/unistr.h"

namespace {

[[noreturn]] void Fail(const std::string &message, UErrorCode status = U_ZERO_ERROR) {
    std::cerr << message;
    if (U_FAILURE(status)) {
        std::cerr << ": " << u_errorName(status);
    }
    std::cerr << '\n';
    std::exit(1);
}

std::string NormalizeToNfc(const std::string &utf8_text) {
    UErrorCode status = U_ZERO_ERROR;
    const icu::Normalizer2 *normalizer = icu::Normalizer2::getNFCInstance(status);
    if (U_FAILURE(status) || normalizer == nullptr) {
        Fail("Failed to create ICU normalizer", status);
    }

    icu::UnicodeString input = icu::UnicodeString::fromUTF8(utf8_text);
    icu::UnicodeString normalized;
    normalizer->normalize(input, normalized, status);
    if (U_FAILURE(status)) {
        Fail("Failed to normalize UTF-8 text", status);
    }

    std::string normalized_utf8;
    normalized.toUTF8String(normalized_utf8);
    return normalized_utf8;
}

std::vector<std::string> SegmentWordsZh(const std::string &utf8_text) {
    UErrorCode status = U_ZERO_ERROR;
    std::unique_ptr<icu::BreakIterator> iterator(
        icu::BreakIterator::createWordInstance(icu::Locale::getChinese(), status));
    if (U_FAILURE(status) || iterator == nullptr) {
        Fail("Failed to create ICU word iterator", status);
    }

    icu::UnicodeString text = icu::UnicodeString::fromUTF8(utf8_text);
    iterator->setText(text);

    std::vector<std::string> words;
    for (int32_t start = iterator->first(), end = iterator->next(); end != icu::BreakIterator::DONE;
         start = end, end = iterator->next()) {
        if (iterator->getRuleStatus() == UBRK_WORD_NONE) {
            continue;
        }

        std::string token;
        text.tempSubStringBetween(start, end).toUTF8String(token);
        words.push_back(token);
    }

    return words;
}

std::vector<std::string> FindChapterTitles(const std::string &utf8_text) {
    static const re2::RE2 chapter_regex(
        R"((?:^|\n)(第[0-9一二三四五六七八九十百千两〇零]+章[^\n]*))");
    if (!chapter_regex.ok()) {
        Fail("Failed to compile chapter regex");
    }

    re2::StringPiece input(utf8_text);
    std::string chapter_title;
    std::vector<std::string> matches;
    while (RE2::FindAndConsume(&input, chapter_regex, &chapter_title)) {
        matches.push_back(chapter_title);
    }
    return matches;
}

} // namespace

int main(int argc, char **argv) {
    CLI::App app{"novelfmt - novel format tools"};
    app.set_version_flag("--version", "0.1.0");

    std::string input_path;
    app.add_option("input", input_path, "input file location")
        ->required()
        ->check(CLI::ExistingFile);

    CLI11_PARSE(app, argc, argv);

    const auto real_input_path = std::filesystem::path{input_path};
    const auto file_size = std::filesystem::file_size(real_input_path);

    if (file_size == 0)
        return 0;

    std::ifstream file(real_input_path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("cannot open file: " + real_input_path.string());
    }

    std::string content(file_size, '\0');
    file.read(content.data(), static_cast<std::streamsize>(file_size));
    if (file.fail() && !file.eof()) {
        throw std::runtime_error("error reading file: " + real_input_path.string());
    }

    content.resize(static_cast<std::size_t>(file.gcount()));
    content.erase(std::remove(content.begin(), content.end(), '\r'), content.end());

    const std::string normalized = NormalizeToNfc(content);
    const std::vector<std::string> chapters = FindChapterTitles(normalized);
    const std::vector<std::string> words = SegmentWordsZh(normalized);

    std::cout << "Normalized text:\n" << normalized << "\n\n";

    std::cout << "Detected chapter titles:\n";
    for (const auto &chapter : chapters) {
        std::cout << "  - " << chapter << '\n';
    }

    std::cout << "\nSegmented words:\n";
    for (const auto &word : words) {
        std::cout << "  [" << word << "]\n";
    }

    return 0;
}
