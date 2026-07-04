#include "format/quote.hpp"

#include <array>
#include <string_view>

#include "format/paragraphs.hpp"

#include <unicode/unistr.h>
#include <unicode/utypes.h>

namespace {

constexpr UChar32 kPrimaryOpen = U'「';
constexpr UChar32 kPrimaryClose = U'」';
constexpr UChar32 kSecondaryOpen = U'『';
constexpr UChar32 kSecondaryClose = U'』';

constexpr std::array<UChar32, 5> kPrimaryQuoteCandidates = {U'"', U'“', U'”', U'「', U'」'};
constexpr std::array<UChar32, 5> kSecondaryQuoteCandidates = {U'\'', U'‘', U'’', U'『', U'』'};

enum class QuoteState {
    Outside,
    InsidePrimary,
    InsideSecondary,
};

template <std::size_t N>
bool contains_code_point(const std::array<UChar32, N>& candidates, UChar32 ch) {
    for (const auto candidate : candidates) {
        if (candidate == ch) {
            return true;
        }
    }

    return false;
}

bool is_primary_quote_candidate(UChar32 ch) {
    return contains_code_point(kPrimaryQuoteCandidates, ch);
}

bool is_secondary_quote_candidate(UChar32 ch) {
    return contains_code_point(kSecondaryQuoteCandidates, ch);
}

bool is_any_quote_candidate(UChar32 ch) {
    return is_primary_quote_candidate(ch) || is_secondary_quote_candidate(ch);
}

/**
 * @brief Maps one raw quote candidate to a normalized quote glyph and updates the nesting state.
 *
 * The current implementation only tracks paragraph-local nesting depth:
 * `Outside -> InsidePrimary -> InsideSecondary`. Any unmatched or ambiguous candidate is folded
 * back into the nearest stable state instead of being passed through unchanged.
 *
 * @param ch Raw quote candidate from the source paragraph.
 * @param quote_state Current paragraph-local quote state.
 * @return Normalized quote glyph to append to the output buffer.
 */
UChar32 classify_quote(UChar32 ch, QuoteState& quote_state) {
    if (quote_state == QuoteState::Outside && is_primary_quote_candidate(ch)) {
        quote_state = QuoteState::InsidePrimary;
        return kPrimaryOpen;
    }

    if (quote_state == QuoteState::InsidePrimary && is_primary_quote_candidate(ch)) {
        quote_state = QuoteState::Outside;
        return kPrimaryClose;
    }

    if (quote_state == QuoteState::InsidePrimary && is_secondary_quote_candidate(ch)) {
        quote_state = QuoteState::InsideSecondary;
        return kSecondaryOpen;
    }

    if (quote_state == QuoteState::InsideSecondary && is_secondary_quote_candidate(ch)) {
        quote_state = QuoteState::InsidePrimary;
        return kSecondaryClose;
    }

    if (quote_state == QuoteState::Outside) {
        quote_state = QuoteState::InsidePrimary;
        return kPrimaryOpen;
    }

    if (quote_state == QuoteState::InsidePrimary) {
        quote_state = QuoteState::Outside;
        return kPrimaryClose;
    }

    quote_state = QuoteState::InsidePrimary;
    return kSecondaryClose;
}

/**
 * @brief Normalizes all quote characters within a single paragraph.
 *
 * @param paragraph Raw UTF-8 paragraph text.
 * @return Normalized paragraph text.
 */
std::string normalize_paragraph(std::string_view paragraph) {
    QuoteState quote_state = QuoteState::Outside;
    icu::UnicodeString output;

    const auto input = icu::UnicodeString::fromUTF8(paragraph);

    for (int32_t index = 0; index < input.length(); index = input.moveIndex32(index, 1)) {
        const auto ch = input.char32At(index);

        if (!is_any_quote_candidate(ch)) {
            output.append(ch);
            continue;
        }

        output.append(classify_quote(ch, quote_state));
    }

    std::string text;
    output.toUTF8String(text);
    return text;
}

} // namespace

namespace novelfmt {

/**
 * @brief Normalizes quotation marks across the full text buffer.
 *
 * The text is processed paragraph by paragraph. Paragraph boundaries are preserved, but quote
 * state is reset for each paragraph.
 *
 * @param text UTF-8 text buffer to normalize in place.
 */
void normalize_quotes(std::string& text) {
    const auto paragraphs = split_paragraphs(text);

    std::string normalized_text;
    normalized_text.reserve(text.size());

    for (std::size_t index = 0; index < paragraphs.size(); ++index) {
        if (!paragraphs[index].empty()) {
            normalized_text += normalize_paragraph(paragraphs[index]);
        }

        if (index + 1 < paragraphs.size()) {
            normalized_text += '\n';
        }
    }

    text = std::move(normalized_text);
}

} // namespace novelfmt
