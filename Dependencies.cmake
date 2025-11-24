# Dependencies

include(FetchContent)

# SFML
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)

FetchContent_MakeAvailable(SFML)

# yaml-cpp
FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG a83cd31
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
)
set(YAML_CPP_BUILD_TOOLS    OFF)
set(YAML_CPP_BUILD_CONTRIB  OFF)

set(CMAKE_FOLDER "yaml-cpp" CACHE INTERNAL "IDE Folder for yaml-cpp")

FetchContent_MakeAvailable(yaml-cpp)

# Unload Custom IDE Folder
unset(CMAKE_FOLDER CACHE)
