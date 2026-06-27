#include <cstddef>
#include <string>
#include <string_view>

#include "cli/options.hpp"
#include "format/heading.hpp"
#include "format/normalize.hpp"

std::string run_pipeline(std::string_view text, const Options& options) {
    std::string result;
    result.reserve(text.size());

    auto normalized_text = normalize_to_nfc(text);
    // `substr()` method of std::string will build and return an new string
    // thus use std::string_view here
    auto text_view = std::string_view(normalized_text);

    for (std::size_t start = 0; start < normalized_text.size();) {
        auto end = text_view.find('\n', start);
        auto line = text_view.substr(start, (end == std::string_view::npos ? end : end - start));

        start = (end == std::string_view::npos ? normalized_text.size() : end + 1);

        auto formatted_line = format_heading(line, options.heading_mode);

        if (formatted_line.has_value()) {
            result += formatted_line.value() + "\n";
        } else {
            result += line;
        }
        result += "\n";
    }

    return result;
}
