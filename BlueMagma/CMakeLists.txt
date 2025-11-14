# BlueMagma

set(SOURCES
    Source/BlueMagma.hpp
    Source/BlueMagma/Application.hpp
    Source/BlueMagma/Application.cpp
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
