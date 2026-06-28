#include "format/formatter.hpp"

#include <string>

#include "format/heading.hpp"

void formatter_text(std::string_view text_view, std::string& result, const Options& options) {
    for (std::size_t start = 0; start < text_view.size();) {
        auto end = text_view.find('\n', start);
        auto line = text_view.substr(start, (end == std::string_view::npos ? end : end - start));

        start = (end == std::string_view::npos ? text_view.size() : end + 1);

        auto formatted_line = format_heading(line, options.heading_mode);

        if (formatted_line.has_value()) {
            result += formatted_line.value();
        } else {
            result += line;
        }
        result += "\n\n";
    }
}
