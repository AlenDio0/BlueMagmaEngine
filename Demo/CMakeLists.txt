# Demo

set(SOURCES
    Source/Main.cpp
    Source/DemoLayer.hpp
    Source/DemoLayer.cpp
    Source/GameLayer.hpp
    Source/GameLayer.cpp
)

# Executable
add_executable(Demo ${SOURCES})
target_sources(Demo PRIVATE ${SOURCES})

# IDE Structure
set_target_properties(Demo PROPERTIES FOLDER "BlueMagma")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})

# Dependencies
target_link_libraries(Demo PRIVATE BlueMagma)

# Include
target_include_directories(Demo PRIVATE "Source")

# Working directory for VS Debug
set_target_properties(Demo PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/Demo"
)
