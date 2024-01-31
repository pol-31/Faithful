cmake_minimum_required(VERSION 3.26)

project(faithful-core)

#file(GLOB_RECURSE FAITHFUL_MAIN_SOURCES ${FAITHFUL_SOURCE_DIR}/src/*.cpp)

add_executable(faithful-core
        main.cpp
        io/Monitor.cpp
        io/Window.cpp
        executors/queues/LifoBoundedMPSCBlockingQueue.cpp
        executors/AudioThreadPool.cpp
        ${FAITHFUL_MAIN_SOURCES})

target_compile_options(faithful-core PRIVATE ${FAITHFUL_COMPILE_OPTIONS})
target_link_options(faithful-core PRIVATE ${FAITHFUL_EXE_LINK_OPTIONS})

if(FAITHFUL_RELEASE)
    target_compile_definitions(faithful-core PUBLIC FAITHFUL_RELEASE)
else()
    target_compile_definitions(faithful-core PUBLIC FAITHFUL_DEBUG)
endif()


target_link_libraries(faithful-core
        PRIVATE mimalloc-static
        PRIVATE glad
        PRIVATE glfw
        PRIVATE glm
        PRIVATE vorbisenc
        PRIVATE vorbisfile
        PRIVATE vorbis
        PRIVATE ogg
        PRIVATE OpenAL
        PRIVATE RapidJSON
        PRIVATE tinygltf
)
if(NOT FAITHFUL_GL_SUPPORTS_ASTC)
    target_link_libraries(faithful-core PRIVATE astcenc-native-static)
    target_include_directories(faithful-core PRIVATE ${FAITHFUL_SOURCE_DIR}/external)
endif()

target_include_directories(faithful-core
        PRIVATE /home/pavlo/CLionProjects/Faithful/cmake-build-debug/external/openal-soft
        PRIVATE ${FAITHFUL_SOURCE_DIR}/external/glad/include
        PRIVATE ${FAITHFUL_SOURCE_DIR}/external/glfw/include
        PRIVATE ${FAITHFUL_SOURCE_DIR}/external/glm
        PRIVATE ${FAITHFUL_SOURCE_DIR}/external/ogg/include
        PRIVATE ${FAITHFUL_SOURCE_DIR}/external/mimalloc/include
        PRIVATE ${FAITHFUL_SOURCE_DIR}/external/openal-soft/include
        PRIVATE ${FAITHFUL_SOURCE_DIR}/external/rapidjson/include/rapidjson
        PRIVATE ${FAITHFUL_SOURCE_DIR}/external/tinygltf
        PRIVATE ${FAITHFUL_SOURCE_DIR}/external/vorbis/include
)
