# novelfmt

Chinese novel formatting tool. Detects chapter headings and normalizes their Markdown representation, with support for NFC Unicode normalization and Chinese word segmentation.

## Features

- Chapter heading detection: `第X章`, `序章`, `终章`, `尾声`, `后记`, etc.
- Heading normalization to Markdown format (`##`)
- NFC Unicode normalization (via ICU)
- Atomic file writes (tmp + rename)

## Dependencies

| Library | Purpose |
|---------|---------|
| ICU     | Unicode normalization, Chinese word segmentation |
| RE2     | Chapter heading regex matching |
| CLI11   | Command-line argument parsing |
| fmt     | Formatted output and file I/O |

## Build

```sh
# Install vcpkg and set VCPKG_ROOT
git clone https://github.com/microsoft/vcpkg.git ~/vcpkg
export VCPKG_ROOT=~/vcpkg

# Configure and build (debug)
cmake --preset debug
cmake --build --preset debug

# Or use the dev script
./scripts/dev.sh debug
```

## Usage

```sh
./build/debug/novelfmt input.txt                   # output to stdout
./build/debug/novelfmt input.txt -o output.txt     # output to file
./build/debug/novelfmt input.txt --in-place        # overwrite input
```

## Project Structure

```
src/
├── cli/            # Command-line parsing (CLI11)
├── common/         # Shared utilities (error handling)
├── format/         # Formatting pipeline
│   ├── normalize   # NFC Unicode normalization
│   ├── heading     # Chapter heading detection and normalization
│   └── pipeline    # Pipeline orchestration
├── io/             # File read/write with atomic writes
└── main.cpp        # Entry point
```

## License

Apache-2.0
