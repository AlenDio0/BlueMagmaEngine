[![](https://github.com/AlenDio0/BlueMagmaEngine/workflows/CI-Core/badge.svg)](https://github.com/AlenDio0/BlueMagmaEngine/actions)
[![](https://github.com/AlenDio0/BlueMagmaEngine/workflows/CI-Example/badge.svg)](https://github.com/AlenDio0/BlueMagmaEngine/actions)

C++23 cross-platform Engine based on SFML.

## Supported Platforms
- Windows
- Linux (Clang)
- MacOS

## How to build
1. Clone the repository
```
git clone https://github.com/AlenDio0/BlueMagmaEngine.git
```
2. Change the directory
```
cd BlueMagmaEngine
```

#### CMake
3. Generate with CMake a build system
```
cmake -B <path-to-build> [-G <generator-name>]
```
4. Build a CMake-generated project binary tree (optional)
```
cmake --build <dir>
```

#### Windows
3. Generate with CMake and build (automated)
```
./build_windows.bat
```

#### Linux Clang
3. Generate with CMake and build (automated)
```
chmod +x ./build_linux.sh
```
```
./build_linux.sh
```

#### MacOS
3. Generate with CMake and build (automated)
```
chmod +x ./build_macos.sh
```
```
./build_macos.sh
```

## Examples Screenshots
<img width="960" height="540" alt="Paddle Example Screenshot" src="https://github.com/user-attachments/assets/add6fbc7-a05d-4a2a-8a94-25ca087a3f7f" />
<img width="960" height="540" alt="Screenshot 2026-09-14 171151" src="https://github.com/user-attachments/assets/5ae009c7-ac5b-4671-a9a0-0b0b77b63048" />

## External libraries
- ### [SFML](https://github.com/SFML/SFML.git)
- ### [yaml-cpp](https://github.com/jbeder/yaml-cpp.git)
- ### [spdlog](https://github.com/gabime/spdlog.git)
- ### [EnTT](https://github.com/skypjack/entt.git)
