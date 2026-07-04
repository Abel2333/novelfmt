#include <string>
#include <string_view>

#include "cli/options.hpp"
#include "common/result.hpp"
#include "common/try.hpp"
#include "format/formatter.hpp"
#include "format/normalize.hpp"
#include "format/quote.hpp"
#include "format/sanitize.hpp"

namespace novelfmt {

Result<std::string> run_pipeline(std::string_view raw_text, const Options& options) {
    std::string work_text;
    work_text.reserve(raw_text.size());

    NOVELFMT_TRY(normalize_to_nfc(raw_text, work_text));
    if (options.normalize_quotes) {
        normalize_quotes(work_text);
    }
    sanitize_text(work_text);

    auto work_view = std::string_view(work_text);

    std::string result;
    result.reserve(work_text.size());
    formatter_text(work_view, result, options);

    return result;
}

} // namespace novelfmt
