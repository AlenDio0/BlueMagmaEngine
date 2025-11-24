# BlueMagma

set(SOURCES
    Source/BlueMagma.hpp
    Source/bmpch.hpp
    Source/bmpch.cpp
    Source/BlueMagma/Application.hpp
    Source/BlueMagma/Application.cpp
    Source/BlueMagma/Window.hpp
    Source/BlueMagma/Window.cpp
    Source/BlueMagma/EventDispatcher.hpp
    Source/BlueMagma/Layer.hpp
    Source/BlueMagma/Layer.cpp
    Source/BlueMagma/LayerMachine.hpp
    Source/BlueMagma/LayerMachine.cpp
    Source/BlueMagma/Asset.hpp
    Source/BlueMagma/AssetManager.hpp
    Source/BlueMagma/AssetManager.cpp
    Source/BlueMagma/SoundManager.hpp
    Source/BlueMagma/SoundManager.cpp
    Source/BlueMagma/Vec2.hpp
)

# Library
add_library(BlueMagma STATIC)
target_sources(BlueMagma PRIVATE ${SOURCES})

# IDE Structure
set_target_properties(BlueMagma PROPERTIES FOLDER "BlueMagma/Core")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})

# Dependencies
# SFML
target_link_libraries(BlueMagma PUBLIC SFML::Graphics)
target_link_libraries(BlueMagma PUBLIC SFML::Audio)
# yaml-cpp
target_link_libraries(BlueMagma PUBLIC yaml-cpp)

# Include
target_include_directories(BlueMagma PUBLIC "Source")

# Pre-Compiled Headers
target_precompile_headers(BlueMagma PRIVATE "Source/bmpch.hpp")
