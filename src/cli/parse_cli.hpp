#pragma once

#include "options.hpp"

namespace novelfmt {

/**
 * @brief Parses command-line arguments into formatter options.
 *
 * @param argc Argument count from `main`.
 * @param argv Argument vector from `main`.
 * @return Parsed CLI options for the current process invocation.
 */
Options parse_cli(int argc, char* argv[]);

} // namespace novelfmt
