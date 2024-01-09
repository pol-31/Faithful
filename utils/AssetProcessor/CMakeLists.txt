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
        PUBLIC mimalloc
        PUBLIC stb
        PUBLIC dr_libs
        PUBLIC RapidJSON
        PUBLIC vorbis
        PUBLIC libogg
        PUBLIC tinygltf
)
if (FAITHFUL_BUILD_ASSET_PROCESSOR)
    target_link_libraries(AssetProcessor PUBLIC astc-encoder)
endif()

add_dependencies(AssetProcessor meshoptimizer)

target_include_directories(AssetProcessor
        PUBLIC ${CMAKE_SOURCE_DIR}/external/mimalloc/include
        PUBLIC ${CMAKE_SOURCE_DIR}/external/stb
        PUBLIC ${CMAKE_SOURCE_DIR}/external/dr_libs
        PUBLIC ${CMAKE_SOURCE_DIR}/external/rapidjson/include
        PUBLIC ${CMAKE_SOURCE_DIR}/external/rapidjson/include/rapidjson
        PUBLIC ${CMAKE_SOURCE_DIR}/external/tinygltf
        PUBLIC ${CMAKE_SOURCE_DIR}/external/vorbis/include
        PUBLIC ${CMAKE_SOURCE_DIR}/external/libogg/include
#        PUBLIC ${CMAKE_SOURCE_DIR}/external
)


