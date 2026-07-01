#include "format/normalize.hpp"

#include <string>
#include <string_view>

#include "common/error.hpp"
#include "common/result.hpp"

#include <unicode/normalizer2.h>
#include <unicode/unistr.h>
#include <unicode/urename.h>
#include <unicode/utypes.h>

novelfmt::Result<void> normalize_to_nfc(std::string_view utf8_text, std::string& normalized_text) {
    UErrorCode status = U_ZERO_ERROR;
    const icu::Normalizer2* normalizer = icu::Normalizer2::getNFCInstance(status);

    if (U_FAILURE(status) || normalizer == nullptr) {
        return novelfmt::MakeUnexpected(novelfmt::ErrorKind::Format,
                                        "Failed to create ICU normalizer: {}", u_errorName(status));
    }

    icu::UnicodeString input = icu::UnicodeString::fromUTF8(utf8_text);
    icu::UnicodeString normalized;
    normalizer->normalize(input, normalized, status);

    if (U_FAILURE(status)) {
        return novelfmt::MakeUnexpected(novelfmt::ErrorKind::Format,
                                        "Failed to normalize UTF-8 text: {}", u_errorName(status));
    }

    normalized.toUTF8String(normalized_text);

    return {};
}
