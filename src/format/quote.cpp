#include "format/quote.hpp"

#include <array>
#include <optional>
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
constexpr std::array<UChar32, 6> kOpeningContextPunct = {U'：', U'（', U'【', U'《', U'〈', U'('};
constexpr std::array<UChar32, 12> kClosingContextPunct = {
    U'，', U'。', U'！', U'？', U'；', U'：', U'、', U'）', U'】', U'》', U'〉', U')',
};
constexpr std::array<UChar32, 4> kSentencePunct = {U'。', U'！', U'？', U'…'};
constexpr std::array<UChar32, 5> kSpaceLikeChars = {U' ', U'\t', U'\n', U'\r', U'　'};

enum class QuoteState {
    Outside,
    InsidePrimary,
    InsideSecondary,
};

enum class QuoteEvent {
    Open,
    Close,
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

bool is_word_like(UChar32 ch) {
    const bool is_ascii_lower = ch >= U'a' && ch <= U'z';
    const bool is_ascii_upper = ch >= U'A' && ch <= U'Z';
    const bool is_ascii_digit = ch >= U'0' && ch <= U'9';
    const bool is_cjk_unified = ch >= 0x4E00 && ch <= 0x9FFF;
    const bool is_cjk_extension_a = ch >= 0x3400 && ch <= 0x4DBF;

    return is_ascii_lower || is_ascii_upper || is_ascii_digit || is_cjk_unified ||
           is_cjk_extension_a;
}

bool is_opening_context_punct(UChar32 ch) { return contains_code_point(kOpeningContextPunct, ch); }

bool is_closing_context_punct(UChar32 ch) { return contains_code_point(kClosingContextPunct, ch); }

bool is_sentence_punct(UChar32 ch) { return contains_code_point(kSentencePunct, ch); }

bool is_space_like(UChar32 ch) { return contains_code_point(kSpaceLikeChars, ch); }

auto first_non_space_index(const icu::UnicodeString& paragraph) -> std::optional<int32_t> {
    for (int32_t index = 0; index < paragraph.length(); index = paragraph.moveIndex32(index, 1)) {
        if (!is_space_like(paragraph.char32At(index))) {
            return index;
        }
    }

    return std::nullopt;
}

auto last_non_space_index(const icu::UnicodeString& paragraph) -> std::optional<int32_t> {
    for (int32_t index = paragraph.length(); index > 0;) {
        index = paragraph.moveIndex32(index, -1);
        if (!is_space_like(paragraph.char32At(index))) {
            return index;
        }
    }

    return std::nullopt;
}

auto next_non_space_index_after(const icu::UnicodeString& paragraph, int32_t index)
    -> std::optional<int32_t> {
    for (int32_t cursor = paragraph.moveIndex32(index, 1); cursor < paragraph.length();
         cursor = paragraph.moveIndex32(cursor, 1)) {
        if (!is_space_like(paragraph.char32At(cursor))) {
            return cursor;
        }
    }

    return std::nullopt;
}

auto previous_non_space_code_point(const icu::UnicodeString& paragraph, int32_t index)
    -> std::optional<UChar32> {
    for (int32_t cursor = paragraph.moveIndex32(index, -1); cursor >= 0;
         cursor = paragraph.moveIndex32(cursor, -1)) {
        const auto ch = paragraph.char32At(cursor);
        if (!is_space_like(ch)) {
            return ch;
        }
        if (cursor == 0) {
            break;
        }
    }

    return std::nullopt;
}

auto next_non_space_code_point(const icu::UnicodeString& paragraph, int32_t index)
    -> std::optional<UChar32> {
    for (int32_t cursor = paragraph.moveIndex32(index, 1); cursor < paragraph.length();
         cursor = paragraph.moveIndex32(cursor, 1)) {
        const auto ch = paragraph.char32At(cursor);
        if (!is_space_like(ch)) {
            return ch;
        }
    }

    return std::nullopt;
}

/**
 * @brief Locates where to synthesize a missing outer opening quote inside one paragraph.
 *
 * This handles the common OCR / source-text failure mode where a paragraph only retains one
 * trailing primary quote candidate. In that case we reopen the dialogue either at paragraph start
 * or immediately after the latest opening-context punctuation such as `：`.
 *
 * @param paragraph Current paragraph in ICU's UTF-16 representation.
 * @return Code unit index where a synthetic primary opening quote should be inserted, or
 *         `std::nullopt` when no single-paragraph repair is warranted.
 */
auto find_synthetic_primary_open_index(const icu::UnicodeString& paragraph) -> std::optional<int32_t> {
    const auto leading_index = first_non_space_index(paragraph);
    if (!leading_index.has_value()) {
        return std::nullopt;
    }

    int primary_count = 0;
    std::optional<int32_t> primary_index;
    std::optional<int32_t> last_opening_context_index;

    for (int32_t index = 0; index < paragraph.length(); index = paragraph.moveIndex32(index, 1)) {
        const auto ch = paragraph.char32At(index);
        if (is_primary_quote_candidate(ch)) {
            ++primary_count;
            primary_index = index;
        }
        if (is_opening_context_punct(ch)) {
            last_opening_context_index = index;
        }
    }

    if (primary_count != 1 || !primary_index.has_value()) {
        return std::nullopt;
    }

    const auto trailing_index = last_non_space_index(paragraph);
    if (!trailing_index.has_value() || trailing_index.value() != primary_index.value()) {
        return std::nullopt;
    }

    if (last_opening_context_index.has_value()) {
        const auto after_context =
            next_non_space_index_after(paragraph, last_opening_context_index.value());
        if (after_context.has_value() && after_context.value() < primary_index.value()) {
            return after_context;
        }
    }

    return leading_index;
}

/**
 * @brief Classifies one quote candidate as an opening or closing event within the current paragraph.
 *
 * The classifier first applies hard rules based on the candidate family and current nesting state,
 * then falls back to lightweight local-context scoring around the previous and next non-space code
 * points.
 *
 * @param paragraph Current paragraph in ICU's UTF-16 representation.
 * @param index Code unit index of the current candidate.
 * @param ch Current candidate code point.
 * @param state Quote nesting state before this candidate is applied.
 * @return Whether the candidate should be treated as an opening or closing quote event.
 */
auto classify_quote_event(const icu::UnicodeString& paragraph,
                          int32_t index,
                          UChar32 ch,
                          QuoteState state) -> QuoteEvent {
    const auto previous = previous_non_space_code_point(paragraph, index);
    const auto next = next_non_space_code_point(paragraph, index);

    if (is_secondary_quote_candidate(ch)) {
        return state == QuoteState::InsideSecondary ? QuoteEvent::Close : QuoteEvent::Open;
    }

    if (is_primary_quote_candidate(ch)) {
        if (state == QuoteState::InsidePrimary) {
            if (previous.has_value() && is_opening_context_punct(previous.value()) &&
                next.has_value() && is_word_like(next.value())) {
                return QuoteEvent::Open;
            }
            return QuoteEvent::Close;
        }
        if (state == QuoteState::InsideSecondary) {
            return QuoteEvent::Close;
        }
        return QuoteEvent::Open;
    }

    int open_score = 0;
    int close_score = 0;

    if (index == 0) {
        open_score += 2;
    }
    if (!previous.has_value()) {
        open_score += 2;
    } else {
        if (is_opening_context_punct(previous.value())) {
            open_score += 3;
        }
        if (is_word_like(previous.value()) || is_sentence_punct(previous.value())) {
            close_score += 2;
        }
    }

    if (!next.has_value()) {
        close_score += 2;
    } else {
        if (is_word_like(next.value())) {
            open_score += 1;
        }
        if (is_closing_context_punct(next.value()) || is_sentence_punct(next.value())) {
            close_score += 2;
        }
    }

    if (open_score > close_score) {
        return QuoteEvent::Open;
    }
    if (close_score > open_score) {
        return QuoteEvent::Close;
    }

    return state == QuoteState::Outside ? QuoteEvent::Open : QuoteEvent::Close;
}

/**
 * @brief Appends the normalized quote glyph for the given event and current nesting state.
 *
 * @param output Paragraph output buffer.
 * @param state Quote state before applying the event.
 * @param event Quote event classified from the source text.
 */
void append_normalized_quote(icu::UnicodeString& output, QuoteState state, QuoteEvent event) {
    if (event == QuoteEvent::Open) {
        output.append(state == QuoteState::InsidePrimary ? kSecondaryOpen : kPrimaryOpen);
        return;
    }

    output.append(state == QuoteState::InsideSecondary ? kSecondaryClose : kPrimaryClose);
}

/**
 * @brief Applies a classified quote event to the paragraph state machine.
 *
 * @param output Paragraph output buffer.
 * @param state Quote state before the current event.
 * @param event Quote event classified from local context.
 * @return Quote state after applying the event.
 */
auto apply_quote_event(icu::UnicodeString& output, QuoteState state, QuoteEvent event) -> QuoteState {
    if (event == QuoteEvent::Open) {
        append_normalized_quote(output, state, event);
        if (state == QuoteState::Outside) {
            return QuoteState::InsidePrimary;
        }
        if (state == QuoteState::InsidePrimary) {
            return QuoteState::InsideSecondary;
        }
        return QuoteState::InsideSecondary;
    }

    if (state == QuoteState::InsideSecondary) {
        append_normalized_quote(output, state, event);
        return QuoteState::InsidePrimary;
    }
    if (state == QuoteState::InsidePrimary) {
        append_normalized_quote(output, state, event);
        return QuoteState::Outside;
    }

    // A closing-looking quote while outside any dialogue is usually a broken opener in source text.
    append_normalized_quote(output, QuoteState::Outside, QuoteEvent::Open);
    return QuoteState::InsidePrimary;
}

/**
 * @brief Normalizes all quote characters within a single paragraph.
 *
 * @param paragraph Raw UTF-8 paragraph text.
 * @return Normalized paragraph text.
 */
auto normalize_paragraph(std::string_view paragraph) -> std::string {
    QuoteState quote_state = QuoteState::Outside;
    icu::UnicodeString output;

    const auto input = icu::UnicodeString::fromUTF8(paragraph);
    const auto synthetic_open_index = find_synthetic_primary_open_index(input);

    for (int32_t index = 0; index < input.length(); index = input.moveIndex32(index, 1)) {
        const auto ch = input.char32At(index);

        // If the paragraph only kept a trailing primary quote from the source text, recover the
        // missing opener at the paragraph start or right after the most recent opening-context
        // punctuation.
        if (synthetic_open_index.has_value() && synthetic_open_index.value() == index &&
            quote_state == QuoteState::Outside) {
            output.append(kPrimaryOpen);
            quote_state = QuoteState::InsidePrimary;

            if (is_primary_quote_candidate(ch)) {
                continue;
            }
        }

        if (!is_any_quote_candidate(ch)) {
            output.append(ch);
            continue;
        }

        const auto event = classify_quote_event(input, index, ch, quote_state);
        quote_state = apply_quote_event(output, quote_state, event);
    }

    std::string text;
    output.toUTF8String(text);
    return text;
}

} // namespace

namespace novelfmt {

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
