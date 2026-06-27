#include "format/normalize.hpp"

#include <string>
#include <string_view>

#include "common/error.hpp"

#include <unicode/normalizer2.h>
#include <unicode/unistr.h>
#include <unicode/urename.h>
#include <unicode/utypes.h>

std::string normalize_to_nfc(const std::string_view utf8_text) {
    UErrorCode status = U_ZERO_ERROR;
    const icu::Normalizer2* normalizer = icu::Normalizer2::getNFCInstance(status);

    if (U_FAILURE(status) || normalizer == nullptr) {
        novelfmt::Throw("Failed to create ICU normalizer: {}", u_errorName(status));
    }

    icu::UnicodeString input = icu::UnicodeString::fromUTF8(utf8_text);
    icu::UnicodeString normalized;
    normalizer->normalize(input, normalized, status);

    if (U_FAILURE(status)) {
        novelfmt::Throw("Failed to normalize UTF-8 text: {}", u_errorName(status));
    }

    std::string normalized_utf8;
    normalized.toUTF8String(normalized_utf8);

    return normalized_utf8;
}
