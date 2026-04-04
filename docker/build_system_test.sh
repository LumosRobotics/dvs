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

echo "Starting container and building system-test target..."

docker run --rm \
    -v "$(pwd):/workspace" \
    "$IMAGE_NAME" \
    bash -c "
        set -e
        echo '--- Configuring ---'
        cmake -S /workspace/src \
              -B $BUILD_DIR \
              -DCMAKE_BUILD_TYPE=Debug \
              -DCMAKE_PREFIX_PATH='/usr/lib/x86_64-linux-gnu' \
              -G Ninja

        echo '--- Building system-test ---'
        cmake --build $BUILD_DIR --target system-test -j\$(nproc)

        echo '--- Build successful ---'
        file /workspace/src/build/system-test
    "
