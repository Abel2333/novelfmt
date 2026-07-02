#pragma once

#include <string_view>

#include "cli/options.hpp"
#include "common/result.hpp"

namespace novelfmt {

/**
 * @brief Runs the end-to-end formatting pipeline for a UTF-8 document.
 *
 * @param text Raw input text.
 * @param options Active formatter options.
 * @return Fully formatted text or a pipeline error.
 */
Result<std::string> run_pipeline(std::string_view text, const Options& options);

} // namespace novelfmt
