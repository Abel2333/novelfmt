cmake_minimum_required(VERSION 3.25)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(novelfmt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(ICU REQUIRED COMPONENTS uc i18n)
find_package(re2 CONFIG REQUIRED)
find_package(CLI11 CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(tl-expected CONFIG REQUIRED)

add_executable(novelfmt
    src/cli/parse_cli.cpp
    src/format/heading.cpp
    src/format/normalize.cpp
    src/format/sanitize.cpp
    src/format/pipeline.cpp
    src/format/formatter.cpp
    src/io/read_file.cpp
    src/io/write_file.cpp
    src/main.cpp
)

target_include_directories(novelfmt PRIVATE src)

target_link_libraries(novelfmt
    PRIVATE
        ICU::uc
        ICU::i18n
        re2::re2
        CLI11::CLI11
        fmt::fmt
        tl::expected
)

if(MSVC)
    target_compile_options(novelfmt PRIVATE /utf-8)
endif()
