# BlueMagma

set(SOURCES
    Source/BlueMagma.hpp
    Source/bmpch.hpp
    Source/bmpch.cpp
    Source/BlueMagma/Core/Log.hpp
    Source/BlueMagma/Core/Log.cpp
    Source/BlueMagma/Core/Assert.hpp
    Source/BlueMagma/Base/Application.hpp
    Source/BlueMagma/Base/Application.cpp
    Source/BlueMagma/Base/Window.hpp
    Source/BlueMagma/Base/Window.cpp
    Source/BlueMagma/Base/EventDispatcher.hpp
    Source/BlueMagma/Layer/Layer.hpp
    Source/BlueMagma/Layer/Layer.cpp
    Source/BlueMagma/Layer/LayerMachine.hpp
    Source/BlueMagma/Layer/LayerMachine.cpp
    Source/BlueMagma/Asset/Asset.hpp
    Source/BlueMagma/Asset/AssetManager.hpp
    Source/BlueMagma/Asset/AssetManager.cpp
    Source/BlueMagma/Asset/SoundManager.hpp
    Source/BlueMagma/Asset/SoundManager.cpp
    Source/BlueMagma/System/Vec2.hpp
    Source/BlueMagma/System/Rect.hpp
    Source/BlueMagma/System/Random.hpp
    Source/BlueMagma/System/Random.cpp
    Source/BlueMagma/System/Timer.hpp
    Source/BlueMagma/System/Timer.cpp
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
# spdlog
target_link_libraries(BlueMagma PRIVATE spdlog)

# Include
target_include_directories(BlueMagma PUBLIC "Source")
target_include_directories(BlueMagma PUBLIC "Source/BlueMagma")

# Pre-Compiled Headers
target_precompile_headers(BlueMagma PRIVATE "Source/bmpch.hpp")
