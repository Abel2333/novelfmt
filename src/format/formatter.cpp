#include "format/formatter.hpp"

#include <string>

#include "format/heading.hpp"
#include "format/paragraphs.hpp"

namespace novelfmt {

void formatter_text(std::string_view text_view, std::string& result, const Options& options) {
    for (const auto line : split_paragraphs(text_view)) {
        auto formatted_line = format_heading(line, options.heading_mode);

        if (formatted_line.has_value()) {
            result += formatted_line.value();
        } else {
            result += line;
        }
        result += "\n\n";
    }
}

} // namespace novelfmt
