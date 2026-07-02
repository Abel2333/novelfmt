#include "format/paragraphs.hpp"

#include <string_view>
#include <vector>

namespace novelfmt {

auto split_paragraphs(std::string_view text) -> std::vector<std::string_view> {
    std::vector<std::string_view> paragraphs;

    for (std::size_t start = 0; start < text.size();) {
        const auto end = text.find('\n', start);
        paragraphs.push_back(text.substr(start, end == std::string_view::npos ? end : end - start));
        start = (end == std::string_view::npos ? text.size() : end + 1);
    }

    return paragraphs;
}

} // namespace novelfmt
