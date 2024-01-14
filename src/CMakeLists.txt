cmake_minimum_required(VERSION 3.26)

project(faithful-core)

#file(GLOB_RECURSE FAITHFUL_MAIN_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(faithful-core main.cpp)# io/Monitor.h io/Window.h io/Monitor.cpp io/Window.cpp ${FAITHFUL_MAIN_SOURCES}) # loader/ShaderProgram.h loader/ShaderProgram.cpp 

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
        PUBLIC openal-soft
        PUBLIC RapidJSON
        PUBLIC tinygltf
        #PUBLIC astc-encoder
)
if (NOT FAITHFUL_GL_SUPPORTS_ASTC)
    #target_link_libraries(faithful-core PUBLIC astc-encoder)
endif()

if (UNIX AND NOT APPLE)
    if (NOT AUDIO_BACKEND_FOUND)
        target_include_directories(faithful-core PUBLIC ALSA_INCLUDE_DIR)
    endif ()
endif ()

target_include_directories(faithful-core
        PUBLIC ${CMAKE_SOURCE_DIR}/src/loader
        PUBLIC ${CMAKE_SOURCE_DIR}/src/io
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
