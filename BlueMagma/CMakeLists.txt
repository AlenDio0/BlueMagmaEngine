# BlueMagma

set(SOURCES
    Source/BlueMagma.hpp
    Source/bmpch.hpp
    Source/bmpch.cpp

    # Core
    Source/BlueMagma/Core/Log.hpp
    Source/BlueMagma/Core/Log.cpp
    Source/BlueMagma/Core/Assert.hpp
    Source/BlueMagma/Core/Random.hpp
    Source/BlueMagma/Core/Random.cpp
    Source/BlueMagma/Core/Utils.hpp
    Source/BlueMagma/Core/Utils.cpp
    Source/BlueMagma/Core/Timer.hpp
    Source/BlueMagma/Core/Timer.cpp
    
    # Math
    Source/BlueMagma/Math/Vec2.hpp
    Source/BlueMagma/Math/Rect.hpp
    Source/BlueMagma/Math/Color.hpp
    Source/BlueMagma/Math/Transform2D.hpp
    
    # Event
    Source/BlueMagma/Event/Event.hpp
    Source/BlueMagma/Event/EventDispatcher.hpp

    # Graphics
    Source/BlueMagma/Graphics/Camera2D.hpp
    Source/BlueMagma/Graphics/Camera2D.cpp
    Source/BlueMagma/Graphics/Renderer.hpp
    Source/BlueMagma/Graphics/Renderer.cpp
    Source/BlueMagma/Graphics/WindowContext.hpp
    Source/BlueMagma/Graphics/WindowContext.cpp
    Source/BlueMagma/Graphics/Window.hpp
    Source/BlueMagma/Graphics/Window.cpp

    # Framework
    Source/BlueMagma/Framework/Application.hpp
    Source/BlueMagma/Framework/Application.cpp
    # Framework/Layer
    Source/BlueMagma/Framework/Layer/Layer.hpp
    Source/BlueMagma/Framework/Layer/AppLayer.hpp
    Source/BlueMagma/Framework/Layer/AppLayer.cpp
    Source/BlueMagma/Framework/Layer/LayerMachine.hpp
    Source/BlueMagma/Framework/Layer/LayerMachine.cpp

    # Asset
    Source/BlueMagma/Asset/DefaultFont.hpp
    Source/BlueMagma/Asset/Asset.hpp
    Source/BlueMagma/Asset/AssetManager.hpp
    Source/BlueMagma/Asset/AssetManager.cpp
    Source/BlueMagma/Asset/SoundManager.hpp
    Source/BlueMagma/Asset/SoundManager.cpp

    # Scene
    Source/BlueMagma/Scene/Scene.hpp
    Source/BlueMagma/Scene/Scene.cpp
    Source/BlueMagma/Scene/EntityHandle.hpp
    Source/BlueMagma/Scene/Entity.hpp
    Source/BlueMagma/Scene/Entity.cpp
    # Scene/Component
    Source/BlueMagma/Scene/Component/Base.hpp
    Source/BlueMagma/Scene/Component/UI.hpp
    Source/BlueMagma/Scene/Component/UIMaker.hpp
    Source/BlueMagma/Scene/Component/UIMaker.cpp
    # Scene/System
    Source/BlueMagma/Scene/System/ISystem.hpp
    Source/BlueMagma/Scene/System/TransformSystem.hpp
    Source/BlueMagma/Scene/System/TransformSystem.cpp
    Source/BlueMagma/Scene/System/RenderSystem.hpp
    Source/BlueMagma/Scene/System/RenderSystem.cpp
    Source/BlueMagma/Scene/System/UI/WidgetSystem.hpp
    Source/BlueMagma/Scene/System/UI/WidgetSystem.cpp
    Source/BlueMagma/Scene/System/UI/WidgetColorSystem.hpp
    Source/BlueMagma/Scene/System/UI/WidgetColorSystem.cpp
    Source/BlueMagma/Scene/System/UI/ClickableSystem.hpp
    Source/BlueMagma/Scene/System/UI/ClickableSystem.cpp
    Source/BlueMagma/Scene/System/UI/InputTextSystem.hpp
    Source/BlueMagma/Scene/System/UI/InputTextSystem.cpp
    Source/BlueMagma/Scene/System/UI/UISystem.hpp
    Source/BlueMagma/Scene/System/UI/UISystem.cpp
)

# Library
add_library(BlueMagma STATIC)
target_sources(BlueMagma PRIVATE ${SOURCES})

# IDE Structure
set_target_properties(BlueMagma PROPERTIES FOLDER "BlueMagma/Core")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})

# Dependencies
## SFML
target_link_libraries(BlueMagma PUBLIC SFML::Graphics)
target_link_libraries(BlueMagma PUBLIC SFML::Audio)
## yaml-cpp
target_link_libraries(BlueMagma PUBLIC yaml-cpp)
## spdlog
target_link_libraries(BlueMagma PRIVATE spdlog)
## EnTT
target_link_libraries(BlueMagma PUBLIC EnTT)

# Include
target_include_directories(BlueMagma PUBLIC "Source")
target_include_directories(BlueMagma PUBLIC "Source/BlueMagma")

# Pre-Compiled Headers
target_precompile_headers(BlueMagma PRIVATE "Source/bmpch.hpp")
