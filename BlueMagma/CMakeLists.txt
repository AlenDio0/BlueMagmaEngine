# BlueMagma

set(SOURCES
    Source/BlueMagma.hpp
    Source/bmpch.hpp
    Source/bmpch.cpp
    Source/BlueMagma/Application.hpp
    Source/BlueMagma/Application.cpp
    Source/BlueMagma/Window.hpp
    Source/BlueMagma/Window.cpp
)

add_library(BlueMagma STATIC)

target_sources(BlueMagma PRIVATE ${SOURCES})

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})

# Dependencies
# SFML
target_link_libraries(BlueMagma SFML::Graphics)
target_link_libraries(BlueMagma SFML::Audio)

# Include
target_include_directories(BlueMagma PUBLIC "Source")

# Pre-Compiled Headers
target_precompile_headers(BlueMagma PRIVATE "Source/bmpch.hpp")
