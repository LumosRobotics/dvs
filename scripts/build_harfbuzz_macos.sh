#!/bin/bash
set -e

cd "$(git rev-parse --show-toplevel)"

HARFBUZZ_DIR="src/externals/harfbuzz"
CMAKE_BUILD_DIR="$HARFBUZZ_DIR/cmake_build"
INSTALL_DIR="$HARFBUZZ_DIR/build"

echo "--- Cleaning HarfBuzz build/install directories ---"
rm -rf "$CMAKE_BUILD_DIR" "$INSTALL_DIR"

echo "--- Configuring HarfBuzz for macOS ---"
FT_SRC="$(pwd)/src/externals/freetype"
FT_BUILD="$(pwd)/src/externals/freetype/build"

cmake -S "$HARFBUZZ_DIR" \
      -B "$CMAKE_BUILD_DIR" \
      -DCMAKE_BUILD_TYPE=Release \
      -DBUILD_SHARED_LIBS=OFF \
      -DHB_HAVE_FREETYPE=ON \
      -DFREETYPE_INCLUDE_DIR_ft2build="$FT_SRC/include" \
      -DFREETYPE_INCLUDE_DIR_freetype2="$FT_BUILD/include" \
      -DFREETYPE_LIBRARY="$FT_BUILD/libfreetype.a" \
      -DHB_BUILD_SUBSET=OFF \
      -DHB_BUILD_UTILS=OFF \
      -DHB_BUILD_RASTER=OFF \
      -DHB_BUILD_VECTOR=OFF \
      -DHB_BUILD_GPU=OFF \
      -DCMAKE_INSTALL_PREFIX="$(pwd)/$INSTALL_DIR"

echo "--- Building HarfBuzz ---"
cmake --build "$CMAKE_BUILD_DIR" -j"$(sysctl -n hw.logicalcpu)"

echo "--- Installing into $INSTALL_DIR ---"
cmake --install "$CMAKE_BUILD_DIR"

echo "--- Done ---"
file "$INSTALL_DIR/lib/libharfbuzz.a"
