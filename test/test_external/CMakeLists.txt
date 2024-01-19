cmake_minimum_required(VERSION 3.26)

add_executable(faithful-test-external
        test_glad.cpp
        test_glfw.cpp
        test_glm.cpp
        test_mimalloc.cpp
        test_ogg.cpp
        test_openal-soft.cpp
        test_rapidjson.cpp
        test_tinygltf.cpp
        test_vorbis.cpp
)

target_link_libraries(faithful-test-assets
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

target_include_directories(faithful-test-assets
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

if(FAITHFUL_BUILD_ASTC_ENCODER)
    target_sources(faithful-test-assets test_astc-encoder.cpp)
    target_link_libraries(faithful-test-assets PRIVATE astcenc-native-static)
    target_include_directories(faithful-test-assets PRIVATE ${FAITHFUL_SOURCE_DIR}/external)
endif()

if(NOT FAITHFUL_GL_SUPPORTS_ASTC AND NOT FAITHFUL_BUILD_ASSET_PROCESSOR)
    target_compile_definitions(faithful-test-assets FAITHFUL_ASTCENC_DECOMP_ONLY)
endif()

if(FAITHFUL_BUILD_ASSET_DOWNLOADER OR FAITHFUL_BUILD_ASSET_PACK)
    target_sources(faithful-test-assets
            test_picosha2.cpp
            test_miniz.cpp
    )
    target_link_libraries(faithful-test-assets
            PRIVATE miniz
    )
    target_include_directories(faithful-test-assets
            PRIVATE ${FAITHFUL_SOURCE_DIR}/external/miniz
            PRIVATE ${FAITHFUL_SOURCE_DIR}/external/picosha2
    )
endif()


if(FAITHFUL_BUILD_ASSET_PROCESSOR)
    target_sources(faithful-test-assets
            test_stb.cpp
            test_dr_libs.cpp
    )
    target_link_libraries(faithful-test-assets
            PRIVATE stb
            PRIVATE dr_libs
    )
    target_include_directories(faithful-test-assets
            PRIVATE ${FAITHFUL_SOURCE_DIR}/external/stb
            PRIVATE ${FAITHFUL_SOURCE_DIR}/external/dr_libs
    )
endif()


