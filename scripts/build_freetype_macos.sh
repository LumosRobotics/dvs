#!/bin/bash
set -e

cd "$(git rev-parse --show-toplevel)"

FREETYPE_DIR="src/externals/freetype"
BUILD_DIR="$FREETYPE_DIR/build"

echo "--- Cleaning FreeType build directory ---"
rm -rf "$BUILD_DIR"

echo "--- Configuring FreeType for macOS ---"
HARFBUZZ_PREFIX="$(pwd)/src/externals/harfbuzz/build"

cmake -S "$FREETYPE_DIR" \
      -B "$BUILD_DIR" \
      -DCMAKE_BUILD_TYPE=Release \
      -DBUILD_SHARED_LIBS=OFF \
      -DFT_DISABLE_BZIP2=TRUE \
      -DFT_DISABLE_BROTLI=TRUE \
      -DFT_REQUIRE_HARFBUZZ=TRUE \
      -DCMAKE_PREFIX_PATH="$HARFBUZZ_PREFIX"

echo "--- Building FreeType ---"
cmake --build "$BUILD_DIR" -j"$(sysctl -n hw.logicalcpu)"

echo "--- Done ---"
file "$BUILD_DIR/libfreetype.a"
