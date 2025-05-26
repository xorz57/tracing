#!/usr/bin/env bash

set -euxo pipefail

BUILD_TYPE=${1:-Release}
BUILD_DIR="cmake-build-${BUILD_TYPE,,}"

cmake -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -S .
cmake --build "$BUILD_DIR" --config "$BUILD_TYPE" -- -j"$(nproc)"
ctest --test-dir "$BUILD_DIR" --build-config "$BUILD_TYPE" --output-on-failure

