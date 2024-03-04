cmake_minimum_required(VERSION 3.26)

project(faithful-core)

# TODO: file(GLOB_RECURSE FAITHFUL_MAIN_SOURCES ${FAITHFUL_SOURCE_DIR}/src/*.cpp)

add_executable(faithful-core
        ../external/glad/src/glad.c
        main.cpp

        collision/Bvh.cpp
        collision/Collision.cpp
        collision/CollisionArea.cpp

        common/CollisionManager.cpp
        common/DrawManager.cpp
        common/GlobalStateInfo.cpp
        common/InputManager.cpp
        common/LoadingManager.cpp
        common/TransformableObject.cpp
        common/UpdateManager.cpp

        entities/Ambient.cpp
        entities/Boss.cpp
        entities/Enemy.cpp
        entities/Furniture.cpp
        entities/Loot.cpp
        entities/Npc.cpp
        entities/Obstacle.cpp
        entities/PhenomenonArea.cpp

        environment/Biome.cpp
        environment/Map.cpp

        executors/AudioThreadPool.cpp
        executors/DisplayInteractionThreadPool.cpp
        executors/ExecutionEnvironment.cpp
        executors/GameLogicThreadPool.cpp
        executors/SupportThreadPool.cpp

        io/Cursor.cpp
        io/Monitor.cpp
        io/MonitorInfo.cpp
        io/Window.cpp

        loader/assets_data/ModelData.cpp
        loader/assets_data/ShaderObjectData.h.cpp
        loader/assets_data/ShaderProgramData.cpp
        loader/assets_data/TextureData.cpp

        loader/model/AnimatableModel.cpp
        loader/model/NonAnimatableModel.cpp

        loader/ModelPool.cpp
        loader/MusicPool.cpp
        loader/ShaderObjectPool.cpp
        loader/ShaderProgram.cpp
        loader/Sound.cpp
        loader/SoundPool.cpp
        loader/Texture.cpp
        loader/TexturePool.cpp

        player/PlayerCharacter.cpp
        ${FAITHFUL_MAIN_SOURCES})

target_compile_options(faithful-core PRIVATE ${FAITHFUL_COMPILE_OPTIONS})
target_link_options(faithful-core PRIVATE ${FAITHFUL_EXE_LINK_OPTIONS})

if(FAITHFUL_RELEASE)
    target_compile_definitions(faithful-core PUBLIC FAITHFUL_RELEASE)
else()
    target_compile_definitions(faithful-core PUBLIC FAITHFUL_DEBUG)
endif()


target_link_libraries(faithful-core
#        PRIVATE mimalloc-static
#        PRIVATE glad
        PRIVATE glfw
#        PRIVATE glm
#        PRIVATE vorbisenc
#        PRIVATE vorbisfile
#        PRIVATE vorbis
#        PRIVATE ogg
#        PRIVATE OpenAL
#        PRIVATE tinygltf
)
if(NOT FAITHFUL_GL_SUPPORTS_ASTC)
#    target_link_libraries(faithful-core PRIVATE astcenc-native-static)
#    target_include_directories(faithful-core PRIVATE ${FAITHFUL_SOURCE_DIR}/external)
endif()

target_include_directories(faithful-core
        PRIVATE /home/pavlo/CLionProjects/Faithful/cmake-build-debug/external/openal-soft
        PRIVATE ${FAITHFUL_SOURCE_DIR}/external/glad/include
        PRIVATE ${FAITHFUL_SOURCE_DIR}/external/glfw/include
        PRIVATE ${FAITHFUL_SOURCE_DIR}/external/glm
        PRIVATE ${FAITHFUL_SOURCE_DIR}/external/ogg/include
#        PRIVATE ${FAITHFUL_SOURCE_DIR}/external/mimalloc/include
        PRIVATE ${FAITHFUL_SOURCE_DIR}/external/openal-soft/include
        PRIVATE ${FAITHFUL_SOURCE_DIR}/external/rapidjson
        PRIVATE ${FAITHFUL_SOURCE_DIR}/external/tinygltf
        PRIVATE ${FAITHFUL_SOURCE_DIR}/external/vorbis/include
        # TODO: not always:
        PRIVATE ${FAITHFUL_SOURCE_DIR}/external/astc-encoder
)
