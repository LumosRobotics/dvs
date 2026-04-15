#!/bin/bash
set -e

cd "$(git rev-parse --show-toplevel)"

IMAGE_NAME="dvs_ubuntu"
BUILD_DIR="/tmp/dvs-build"

# Build the Ubuntu image if it doesn't already exist
if ! docker image inspect "$IMAGE_NAME" &>/dev/null; then
    echo "Image '$IMAGE_NAME' not found — building from docker/Dockerfile.ubuntu..."
    docker build -f docker/Dockerfile.ubuntu -t "$IMAGE_NAME" .
else
    echo "Image '$IMAGE_NAME' already exists, skipping build."
fi

echo "Starting container and building targets..."

docker run --rm \
    -v "$(pwd):/workspace" \
    "$IMAGE_NAME" \
    bash -c "
        set -e

        echo '--- Building FreeType ---'
        rm -rf /workspace/src/externals/freetype/build
        cd /workspace/src/externals/freetype
        cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF \
              -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
              -DFT_DISABLE_BZIP2=ON -DFT_DISABLE_BROTLI=ON \
              -DFT_DISABLE_ZLIB=ON -DFT_DISABLE_PNG=ON \
              -Wno-dev
        cmake --build build -j\$(nproc)

        echo '--- Configuring ---'
        cmake -S /workspace/src \
              -B $BUILD_DIR \
              -DCMAKE_BUILD_TYPE=Debug \
              -DCMAKE_PREFIX_PATH='/usr/lib/x86_64-linux-gnu' \
              -G Ninja -Wno-dev

        echo '--- Building duoplot ---'
        cmake --build $BUILD_DIR --target duoplot -j\$(nproc)
        file /workspace/src/build/duoplot

        echo '--- Building system-test ---'
        cmake --build $BUILD_DIR --target system-test -j\$(nproc)
        file $BUILD_DIR/system_test/system-test

        echo '--- Build successful ---'
    "
