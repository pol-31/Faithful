cmake_minimum_required(VERSION 3.26)

project(faithful-core)

#file(GLOB_RECURSE FAITHFUL_MAIN_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(faithful-core main.cpp io/Monitor.cpp io/Window.cpp loader/ShaderProgram.cpp ${FAITHFUL_MAIN_SOURCES})

#target_compile_options(faithful-core PRIVATE "/MTd")

if (FAITHFUL_ALSOFT_ALSA_ONLY)
#    target_link_libraries(faithful-core PUBLIC alsa)
elseif (WIN32) # TODO: XAudio2
    target_link_libraries(faithful-core PUBLIC winmm)
endif ()
target_link_libraries(faithful-core
        PUBLIC mimalloc
        PUBLIC stb
        PUBLIC glad
        PUBLIC glfw
        PUBLIC glm
        PUBLIC vorbis
        PUBLIC ogg
        PUBLIC libasound.so
        PUBLIC openal-soft
        PUBLIC RapidJSON
        PUBLIC tinygltf
        PUBLIC astcenc-native-static

)
if (NOT FAITHFUL_GL_SUPPORTS_ASTC)
    #target_link_libraries(faithful-core PUBLIC astc-encoder)
endif()

target_include_directories(faithful-core
        PUBLIC /home/pavlo/CLionProjects/Faithful/cmake-build-debug/external/openal-soft
        PUBLIC ${CMAKE_SOURCE_DIR}/external/stb
        PUBLIC ${CMAKE_SOURCE_DIR}/external/glad/include
        PUBLIC ${CMAKE_SOURCE_DIR}/external/glfw/include
        PUBLIC ${CMAKE_SOURCE_DIR}/external/glm
        PUBLIC ${CMAKE_SOURCE_DIR}/external/ogg/include
        PUBLIC ${CMAKE_SOURCE_DIR}/external/mimalloc/include
        PUBLIC ${CMAKE_SOURCE_DIR}/external/openal-soft/include
        PUBLIC ${CMAKE_SOURCE_DIR}/external/rapidjson/include/rapidjson
        PUBLIC ${CMAKE_SOURCE_DIR}/external/tinygltf
        PUBLIC ${CMAKE_SOURCE_DIR}/external/vorbis/include
        PUBLIC ${CMAKE_SOURCE_DIR}/external # TODO: astcenc
)

if (FAITHFUL_GL_SUPPORTS_ASTC)
    #target_include_directories(faithful-core PUBLIC ${CMAKE_SOURCE_DIR}/external)
endif()
