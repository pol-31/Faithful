cmake_minimum_required(VERSION 3.26)

project(faithful-core)

#file(GLOB_RECURSE FAITHFUL_MAIN_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(faithful-core main.cpp io/Monitor.cpp io/Window.cpp loader/ShaderProgram.cpp ${FAITHFUL_MAIN_SOURCES})

#add_executable(Faithful src/main.cpp
#        src/io/Window.cpp
#        src/io/Monitor.cpp
#        src/loader/ShaderProgram.cpp
#)


if (FAITHFUL_ALSOFT_ALSA_ONLY)
    target_link_libraries(faithful-core
#            PUBLIC /usr/lib/x86_64-linux-gnu/alsa-lib/libasound_module_conf_pulse.so
#            PUBLIC /home/pavlo/CLionProjects/Faithful/external/alsa/alsa_source/src/.libs/libasound.so
#            PUBLIC alsa
            PUBLIC /usr/lib/x86_64-linux-gnu/libasound.so
#            PUBLIC /usr/lib/x86_64-linux-gnu/libdbus-1.so
    )
endif ()
target_link_libraries(faithful-core
        PUBLIC mimalloc
        PUBLIC stb
        PUBLIC glad
        PUBLIC glfw
        PUBLIC glm
        PUBLIC vorbis
        PUBLIC libogg
        PUBLIC openal-soft
        PUBLIC /usr/lib/x86_64-linux-gnu/libsndfile.so
#        PUBLIC ${CMAKE_BINARY_DIR}/external/openal-soft/libcommon.a # do i need it?
        PUBLIC RapidJSON
        PUBLIC tinygltf
)
if (NOT FAITHFUL_GL_SUPPORTS_ASTC)
    target_link_libraries(faithful-core PUBLIC astc-encoder)
endif()

if (UNIX AND NOT APPLE)
    if (NOT AUDIO_BACKEND_FOUND)
        target_include_directories(faithful-core PUBLIC ALSA_INCLUDE_DIR)
    endif ()
endif ()

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
