# BlueMagmaEngine

[![](https://github.com/AlenDio0/BlueMagmaEngine/workflows/CMake%20Test/badge.svg)](https://github.com/AlenDio0/BlueMagmaEngine/actions)

C++23 cross-platform Engine based on SFML.

## How to build
1. Clone the repository
```
git clone https://github.com/AlenDio0/BlueMagmaEngine.git
```
2. Change the directory
```
cd BlueMagmaEngine
```
3. Generate with CMake a build system
```
cmake -B <path-to-build> -G <generator-name>
```
or
```
mkdir <dir>
cd <dir>
cmake .. -G <generator-name>
```
4. Build a CMake-generated project binary tree (optional)
```
cmake --build <dir>
```
