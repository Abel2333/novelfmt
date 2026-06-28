#include "format/sanitize.hpp"

#include <string>

#include <re2/re2.h>

void sanitize_text(std::string& text) {
    // Normalize line endings: \r\n or bare \r -> \n
    RE2::GlobalReplace(&text, R"(\r\n|\r)", "\n");

    // Collapse consecutive newlines into a single \n
    RE2::GlobalReplace(&text, R"(\n+)", "\n");
    // Remove whitespace except newlines (ASCII space/tab, CJK fullwidth space U+3000, NBSP U+00A0)
    RE2::GlobalReplace(&text, R"([ \t\x{3000}\x{00A0}]+)", "");
    // Collapse 4+ dashes into 3 (em-dash usage)
    RE2::GlobalReplace(&text, R"(-{4,})", "---");
    // Collapse 3+ tilde/wave dash into 2
    RE2::GlobalReplace(&text, R"(~{3,})", "~~");
    RE2::GlobalReplace(&text, R"(～{3,})", "～～");
    // Period/Chinese period -> ellipsis: 3 -> …, 4+ -> …
    RE2::GlobalReplace(&text, R"([。.]{4,})", "……");
    RE2::GlobalReplace(&text, R"([。.]{3})", "…");
    // Collapse 3+ existing ellipsis chars into 2
    RE2::GlobalReplace(&text, R"(…{3,})", "……");
}
