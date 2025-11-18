# Source

cmake_minimum_required(VERSION 3.10)
project(BlueMagmaEngine LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_STANDARD 23)

if (MSVC)
    add_compile_options(/MP)
endif()

# Options
option(BLUEMAGMAENGINE_BUILD_EXAMPLE "Build examples and demos." ON)

# Dependencies
include(Dependencies.cmake)

# Projects
add_subdirectory(BlueMagma)

if (BLUEMAGMAENGINE_BUILD_EXAMPLE)
add_subdirectory(Demo)
endif()

# Alias
add_library(BlueMagmaEngine ALIAS BlueMagma)
