cmake_minimum_required(VERSION 3.26)
project(Faithful)

set(CMAKE_CXX_STANDARD 17)

include_directories("/usr/local/include")

# Set the default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

# Define a preprocessor macro based on build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG_BUILD)
endif()

set(sounds_bin_path "${CMAKE_SOURCE_DIR}/assets/audio")
set(models_bin_path "${CMAKE_SOURCE_DIR}/assets/models")
set(shaders_path "${CMAKE_SOURCE_DIR}/assets/shaders")
set(textures_bin_path "${CMAKE_SOURCE_DIR}/assets/textures")

configure_file(config/config_paths.h.in ${CMAKE_SOURCE_DIR}/config/config_paths.h)

add_executable(Faithful main.cpp
        external/glad/gl.c
        external/stb_image/stb_image.cpp
        src/collision/CollisionManager.cpp
        src/entities/common/AnimationNode.cpp
        src/entities/common/RunningAnimation.cpp
        src/entities/Mesh.cpp
        src/entities/Model.cpp
        src/entities/Object.cpp
        src/entities/Objects_2d.cpp
        src/entities/Objects_3d.cpp
        src/environment/CubeMap.cpp
        src/environment/Environment.cpp
        src/io/Camera.cpp
        src/io/Cursor.cpp
        src/io/InputHandler.cpp
        src/io/Monitor.cpp
        src/io/Window.cpp
        src/loader/Image.cpp
        src/loader/ShaderProgram.cpp
        src/loader/Sprite.cpp
        src/loader/StaticBinLoader.cpp
        src/loader/Texture.cpp
        utils/Executors/Executor.cpp
        utils/Executors/ThreadPoolCall.cpp
        utils/Executors/ThreadPools.cpp
        src/Engine.cpp
        src/Scene.cpp
)

target_include_directories(Faithful PRIVATE /usr/local/include)
target_link_libraries(Faithful PRIVATE /usr/local/lib/GLFW/libglfw3.a)
target_link_libraries(Faithful PRIVATE /usr/local/lib/libassimp.so)

# i have to find and include: FIND_PACKAGES (?)