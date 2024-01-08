cmake_minimum_required(VERSION 3.26)

project(faithful-core)

file(GLOB_RECURSE FAITHFUL_MAIN_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(faithful-core ${FAITHFUL_MAIN_SOURCES})

#add_executable(Faithful src/main.cpp
#        src/io/Window.cpp
#        src/io/Monitor.cpp
#        src/loader/ShaderProgram.cpp
#)


target_link_libraries(faithful-core
        PUBLIC mimalloc
        PUBLIC stb
        PUBLIC alsa
        PUBLIC glad
        PUBLIC glfw
        PUBLIC glm
        PUBLIC libogg
        PUBLIC openal-soft
        PUBLIC rapidjson
        PUBLIC tinygltf
        PUBLIC vorbis
)
if (NOT FAITHFUL_GL_SUPPORTS_ASTC)
    target_link_libraries(faithful-core PUBLIC astc-encoder)
endif()

if (UNIX AND NOT APPLE)
    target_include_directories(faithful-core PUBLIC ALSA_INCLUDE_DIR)
endif ()

target_include_directories(faithful-core
        PUBLIC ${CMAKE_SOURCE_DIR}/external/stb
        PUBLIC ${GLAD_BINARY_DIR}/include
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
