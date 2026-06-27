#!/usr/bin/env bash
set -euo pipefail

# novelfmt dev script: configure, build, and symlink compile_commands.json

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
cd "${PROJECT_ROOT}"

PRESET="${1:-debug}"

echo "==> Configuring (preset: ${PRESET})..."
cmake --preset "${PRESET}"

echo "==> Building (preset: ${PRESET})..."
cmake --build --preset "${PRESET}"

echo "==> Linking compile_commands.json..."
BUILD_DIR="build/${PRESET}"
COMPILE_DB="${BUILD_DIR}/compile_commands.json"

if [ -f "${COMPILE_DB}" ]; then
    ln -sf "${COMPILE_DB}" compile_commands.json
    echo "Done: compile_commands.json -> ${COMPILE_DB}"
else
    echo "Warning: ${COMPILE_DB} not found (CMAKE_EXPORT_COMPILE_COMMANDS may be OFF)"
fi
