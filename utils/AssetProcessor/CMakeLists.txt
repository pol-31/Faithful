cmake_minimum_required(VERSION 3.26)
project(AssetProcessor)

# TODO: if FREEZE_BUILD --> RELEASE

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

target_link_libraries(AssetProcessor
        PUBLIC mimalloc-static
        PUBLIC stb
        PUBLIC dr_libs
        PUBLIC RapidJSON
        PUBLIC vorbisenc
        PUBLIC vorbisfile
        PUBLIC vorbis
        PUBLIC ogg
        PUBLIC tinygltf
)

if (FAITHFUL_BUILD_ASTC_ENCODER)
    target_link_libraries(AssetProcessor PUBLIC astcenc-native-static)
    target_include_directories(AssetProcessor PUBLIC ${FAITHFUL_SOURCE_DIR}/external)
endif ()


add_dependencies(AssetProcessor meshoptimizer)

target_include_directories(AssetProcessor
        PUBLIC ${FAITHFUL_SOURCE_DIR}/external/mimalloc/include
        PUBLIC ${FAITHFUL_SOURCE_DIR}/external/stb
        PUBLIC ${FAITHFUL_SOURCE_DIR}/external/dr_libs
        PUBLIC ${FAITHFUL_SOURCE_DIR}/external/rapidjson/include
        PUBLIC ${FAITHFUL_SOURCE_DIR}/external/rapidjson/include/rapidjson
        PUBLIC ${FAITHFUL_SOURCE_DIR}/external/tinygltf
        PUBLIC ${FAITHFUL_SOURCE_DIR}/external/vorbis/include
        PUBLIC ${FAITHFUL_SOURCE_DIR}/external/ogg/include
)

