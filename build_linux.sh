PROJECT_DIR=$(pwd)
BUILD_DIR="build_linux"

read -p "Do you want to install dependencies? [y/N]: " answer

if [[ "$answer" =~ ^[Yy]$ ]]; then
    echo "Installing dependencies..."
    sudo apt-get update && sudo apt-get install -y \
        libxrandr-dev \
        libxcursor-dev \
        libxi-dev \
        libudev-dev \
        libflac-dev \
        libvorbis-dev \
        libgl1-mesa-dev \
        libegl1-mesa-dev \
        libfreetype-dev \
        libharfbuzz-dev \
        libmbedtls-dev \
        libssh2-1-dev > /dev/null
else
    echo "Dependencies installation skipped."
fi

if [ -d "$BUILD_DIR" ]; then
    echo "Cleaning last build..."
    rm -rf "$BUILD_DIR"
fi

mkdir "$BUILD_DIR" && cd "$BUILD_DIR"

echo "Starting CMake configuration..."
cmake ..

if [ $? -ne 0 ]; then
    echo "Error during CMake configuration!"
    exit 1
fi

echo "Starting build..."
make -j12

if [ $? -eq 0 ]; then
    echo "Build completed!"
    exit 0
else
    echo "Error during build!"
    exit 1
fi
