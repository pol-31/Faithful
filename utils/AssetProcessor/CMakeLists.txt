cmake_minimum_required(VERSION 3.26)
project(AssetProcessor)

set(CMAKE_CXX_STANDARD 17)

add_executable(AssetProcessor main.cpp
        AssetCategory.cpp
        AssetLoadingThreadPool.cpp
        AssetProcessor.cpp
        AssetsAnalyzer.cpp
        TextureProcessor.cpp
        ModelProcessor.cpp
        AudioProcessor.cpp
)
target_compile_options(AssetProcessor PRIVATE ${FAITHFUL_COMPILE_OPTIONS})
target_link_options(AssetProcessor PRIVATE ${FAITHFUL_EXE_LINK_OPTIONS})

target_link_libraries(AssetProcessor
        PRIVATE mimalloc-static
        PRIVATE stb
        PRIVATE dr_libs
        PRIVATE RapidJSON
        PRIVATE vorbisenc
        PRIVATE vorbisfile
        PRIVATE vorbis
        PRIVATE ogg
        PRIVATE tinygltf
)

if(FAITHFUL_BUILD_ASTC_ENCODER)
    target_link_libraries(AssetProcessor PUBLIC astcenc-native-static)
    target_include_directories(AssetProcessor PRIVATE ${FAITHFUL_SOURCE_DIR}/external)
endif()

add_dependencies(AssetProcessor meshoptimizer)

target_include_directories(AssetProcessor
        PRIVATE ${FAITHFUL_SOURCE_DIR}/external/mimalloc/include
        PRIVATE ${FAITHFUL_SOURCE_DIR}/external/stb
        PRIVATE ${FAITHFUL_SOURCE_DIR}/external/dr_libs
        PRIVATE ${FAITHFUL_SOURCE_DIR}/external/rapidjson/include
        PRIVATE ${FAITHFUL_SOURCE_DIR}/external/rapidjson/include/rapidjson
        PRIVATE ${FAITHFUL_SOURCE_DIR}/external/tinygltf
        PRIVATE ${FAITHFUL_SOURCE_DIR}/external/vorbis/include
        PRIVATE ${FAITHFUL_SOURCE_DIR}/external/ogg/include
)

