PROJECT_DIR=$(pwd)
BUILD_DIR="build_linux"

echo "Checking dependencies..."
sudo apt update -qq && sudo apt install -y -qq \
    build-essential cmake g++ libx11-dev libxrandr-dev \
    libxcursor-dev libxi-dev libudev-dev libgl1-mesa-dev \
    libfreetype6-dev libopenal-dev > /dev/null

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