cmake_minimum_required(VERSION 3.26)

add_executable(faithful-test-external
        main.cpp
)

target_link_libraries(faithful-core
        PUBLIC mimalloc-static
        PUBLIC glad
        PUBLIC glfw
        PUBLIC glm
        PUBLIC vorbisenc
        PUBLIC vorbisfile
        PUBLIC vorbis
        PUBLIC ogg
        PUBLIC OpenAL
        PUBLIC RapidJSON
        PUBLIC tinygltf
)
if (NOT FAITHFUL_GL_SUPPORTS_ASTC)
    target_link_libraries(faithful-core PUBLIC astcenc-native-static)
    target_include_directories(faithful-core PUBLIC ${FAITHFUL_SOURCE_DIR}/external)
endif()

target_include_directories(faithful-core
        PUBLIC /home/pavlo/CLionProjects/Faithful/cmake-build-debug/external/openal-soft
        PUBLIC ${FAITHFUL_SOURCE_DIR}/external/glad/include
        PUBLIC ${FAITHFUL_SOURCE_DIR}/external/glfw/include
        PUBLIC ${FAITHFUL_SOURCE_DIR}/external/glm
        PUBLIC ${FAITHFUL_SOURCE_DIR}/external/ogg/include
        PUBLIC ${FAITHFUL_SOURCE_DIR}/external/mimalloc/include
        PUBLIC ${FAITHFUL_SOURCE_DIR}/external/openal-soft/include
        PUBLIC ${FAITHFUL_SOURCE_DIR}/external/rapidjson/include/rapidjson
        PUBLIC ${FAITHFUL_SOURCE_DIR}/external/tinygltf
        PUBLIC ${FAITHFUL_SOURCE_DIR}/external/vorbis/include
)
