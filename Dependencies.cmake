# Dependencies

include(FetchContent)

## SFML
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)

FetchContent_MakeAvailable(SFML)

## yaml-cpp
FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG bbf8bdb
)
set(YAML_CPP_BUILD_TOOLS    OFF)
set(YAML_CPP_BUILD_CONTRIB  OFF)

set(CMAKE_FOLDER "yaml-cpp" CACHE INTERNAL "IDE Folder for yaml-cpp")

FetchContent_MakeAvailable(yaml-cpp)

## spdlog
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.16.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
)

set(CMAKE_FOLDER "spdlog" CACHE INTERNAL "IDE Folder for spdlog")

FetchContent_MakeAvailable(spdlog)

## EnTT
FetchContent_Declare(
    EnTT
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG v3.16.0
    GIT_SHALLOW ON
)

set(CMAKE_FOLDER "EnTT" CACHE INTERNAL "IDE Folder for EnTT")

FetchContent_MakeAvailable(EnTT)

# Unload Custom IDE Folder
unset(CMAKE_FOLDER CACHE)
