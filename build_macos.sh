#!/bin/bash

BUILD_DIR="build_macos"

if [ -d "$BUILD_DIR" ]; then
    read -p "Do you want to cleanup last build? [y/N]: " answer

    if [[ "$answer" =~ ^[Yy]$ ]]; then
        echo "Cleaning last build..."
        rm -rf "$BUILD_DIR"
    else
        echo "Cleanup last build - skipped."
    fi
fi

mkdir "$BUILD_DIR" && cd "$BUILD_DIR"

echo "Starting CMake configuration in '"$BUILD_DIR"'..."
cmake ..

if [ $? -ne 0 ]; then
    echo "Error during CMake configuration!"
    exit 1
fi

echo "Starting build..."
cmake --build . --config Release

if [ $? -eq 0 ]; then
    echo "Build completed!"
    exit 0
else
    echo "Error during build!"
    exit 1
fi
