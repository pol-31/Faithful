
cmake_minimum_required(VERSION 3.26)



# TODO: rewrite with GLOB_RECURSE *.cpp

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
        utils/executors/Executor.cpp
        utils/executors/ThreadPoolCall.cpp
        utils/executors/ThreadPools.cpp
        src/Engine.cpp
        src/Scene.cpp
)

