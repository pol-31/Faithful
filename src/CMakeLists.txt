cmake_minimum_required(VERSION 3.26)

project(faithful-core)

#file(GLOB_RECURSE FAITHFUL_MAIN_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(faithful-core main.cpp io/Monitor.cpp io/Window.cpp loader/ShaderProgram.cpp ${FAITHFUL_MAIN_SOURCES})

target_link_libraries(faithful-core
        PUBLIC mimalloc
        PUBLIC stb
        PUBLIC glad
        PUBLIC glfw
        PUBLIC glm
        PUBLIC vorbis
        PUBLIC libogg
        PUBLIC libasound.so
        PUBLIC openal-soft
        PUBLIC /usr/lib/x86_64-linux-gnu/libsndfile.so
        PUBLIC RapidJSON
        PUBLIC tinygltf
)
if (NOT FAITHFUL_GL_SUPPORTS_ASTC)
    target_link_libraries(faithful-core PUBLIC astc-encoder)
endif()

target_include_directories(faithful-core
        PUBLIC /home/pavlo/CLionProjects/Faithful/cmake-build-debug/external/openal-soft
        PUBLIC ${CMAKE_SOURCE_DIR}/external/stb
        PUBLIC ${GLAD_OUT_DIR}/include
        PUBLIC ${CMAKE_SOURCE_DIR}/external/glfw/include
        PUBLIC ${CMAKE_SOURCE_DIR}/external/glm
        PUBLIC ${CMAKE_SOURCE_DIR}/external/libogg/include
        PUBLIC ${CMAKE_SOURCE_DIR}/external/mimalloc/include
        PUBLIC ${CMAKE_SOURCE_DIR}/external/openal-soft/include
        PUBLIC ${CMAKE_SOURCE_DIR}/external/rapidjson/include/rapidjson
        PUBLIC ${CMAKE_SOURCE_DIR}/external/tinygltf
        PUBLIC ${CMAKE_SOURCE_DIR}/external/vorbis/include
)

if (FAITHFUL_GL_SUPPORTS_ASTC)
    target_include_directories(faithful-core PUBLIC ${CMAKE_SOURCE_DIR}/external)
endif()

message(STATUS ${OPENAL_INCLUDE_DIR})
message(STATUS ${OPENAL_LIBRARY})
