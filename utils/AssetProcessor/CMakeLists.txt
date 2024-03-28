cmake_minimum_required(VERSION 3.26)
project(AssetProcessor)

set(CMAKE_CXX_STANDARD 20)

add_executable(AssetProcessor
        main.cpp
        AssetLoadingThreadPool.cpp
        AssetProcessor.cpp
        AssetsAnalyzer.cpp
        AudioProcessor.cpp
        ModelProcessor.cpp
        TextureProcessor.cpp
)
target_compile_options(AssetProcessor PRIVATE ${FAITHFUL_COMPILE_OPTIONS})
target_link_options(AssetProcessor PRIVATE ${FAITHFUL_EXE_LINK_OPTIONS})

target_link_libraries(AssetProcessor
        PRIVATE astcenc-native-static
        PRIVATE stb
        PRIVATE tinygltf
        PRIVATE vorbisenc
        PRIVATE vorbisfile
        PRIVATE vorbis
        PRIVATE ogg
)

add_dependencies(AssetProcessor meshoptimizer)

target_include_directories(AssetProcessor
        PRIVATE ${FAITHFUL_SOURCE_DIR}/external
        PRIVATE ${FAITHFUL_SOURCE_DIR}/external/stb
        PRIVATE ${FAITHFUL_SOURCE_DIR}/external/tinygltf
        PRIVATE ${FAITHFUL_SOURCE_DIR}/external/rapidjson/include
        PRIVATE ${FAITHFUL_SOURCE_DIR}/external/vorbis/include
        PRIVATE ${FAITHFUL_SOURCE_DIR}/external/ogg/include
)

