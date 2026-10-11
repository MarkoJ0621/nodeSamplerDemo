#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
JUCE_VERSION="8.0.10"
JUCE_DIR="${ROOT_DIR}/juce"
UI_DIR="${ROOT_DIR}/SvelteUI"
BUILD_DIR="${ROOT_DIR}/build"

die() {
    printf 'Error: %s\n' "$1" >&2
    exit 1
}

require_command() {
    command -v "$1" >/dev/null 2>&1 || die "'$1' is required but was not found in PATH"
}

require_command git
require_command cmake
require_command node
require_command npm

if [[ ! -d "${JUCE_DIR}" ]]; then
    printf 'Downloading JUCE %s...\n' "${JUCE_VERSION}"
    git clone --depth 1 --branch "${JUCE_VERSION}" \
        https://github.com/juce-framework/JUCE.git "${JUCE_DIR}"
elif [[ ! -f "${JUCE_DIR}/CMakeLists.txt" ]]; then
    die "${JUCE_DIR} exists but does not contain a JUCE checkout"
fi

[[ -f "${UI_DIR}/package.json" ]] || die "UI package.json not found at ${UI_DIR}"

printf 'Installing UI dependencies...\n'
npm --prefix "${UI_DIR}" install

printf 'Building UI...\n'
npm --prefix "${UI_DIR}" run build

printf 'Configuring plugin...\n'
cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE=Release

printf 'Building plugin...\n'
cmake --build "${BUILD_DIR}" --config Release

printf '\nBuild complete. Artifacts are in:\n%s\n' \
    "${BUILD_DIR}/NodeSampler_artefacts"
