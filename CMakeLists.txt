cmake_minimum_required(VERSION 3.24)

project(isaac_engine_base
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL2 CONFIG REQUIRED)

add_executable(isaac_engine_base
    src/main.cpp

    src/engine/core/Application.hpp
    src/engine/core/Application.cpp
    src/engine/core/IGame.hpp
    src/engine/core/Log.hpp
    src/engine/core/Log.cpp

    src/engine/input/Input.hpp
    src/engine/input/Input.cpp
    src/engine/input/Key.hpp

    src/engine/math/Rect.hpp
    src/engine/math/Vec2.hpp

    src/engine/platform/Window.hpp
    src/engine/platform/Window.cpp

    src/engine/renderer/Color.hpp
    src/engine/renderer/Renderer2D.hpp
    src/engine/renderer/Renderer2D.cpp

    src/game/Game.hpp
    src/game/Game.cpp
)

target_include_directories(isaac_engine_base
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_definitions(isaac_engine_base
    PRIVATE
        SDL_MAIN_HANDLED
)

target_link_libraries(isaac_engine_base
    PRIVATE
        SDL2::SDL2
)

if(MSVC)
    target_compile_options(isaac_engine_base PRIVATE /W4 /permissive-)
else()
    target_compile_options(isaac_engine_base PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(WIN32)
    add_custom_command(TARGET isaac_engine_base POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:isaac_engine_base>
            $<TARGET_FILE_DIR:isaac_engine_base>
        COMMAND_EXPAND_LISTS
    )
endif()
