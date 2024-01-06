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


include(../../cmake/FaithfulFunctions.cmake)
target_link_asset_processor_deps(AssetProcessor)
target_include_asset_processor_deps(AssetProcessor)

#target_link_libraries(AssetProcessor
#        PUBLIC /home/pavlo/CLionProjects/Faithful/cmake-build-debug/src/astcencoder_external-build/Source/libastcenc-native-static.a
#        PUBLIC /home/pavlo/CLionProjects/Faithful/cmake-build-debug/astc-encoder-build/libdr_libsd.a
#        PUBLIC /home/pavlo/CLionProjects/Faithful/cmake-build-debug/src/vorbis_external-build/lib/libvorbisenc.a
#        PUBLIC /home/pavlo/CLionProjects/Faithful/cmake-build-debug/src/vorbis_external-build/lib/libvorbisfile.a
#        PUBLIC /home/pavlo/CLionProjects/Faithful/cmake-build-debug/src/vorbis_external-build/lib/libvorbis.a
#        PUBLIC /home/pavlo/CLionProjects/Faithful/cmake-build-debug/src/libogg_external-build/src/.libs/libogg.a
#)
#target_include_directories(AssetProcessor
#        PUBLIC /home/pavlo/CLionProjects/Faithful/external/astc-encoder/astc-encoder/Source
#        PUBLIC /home/pavlo/CLionProjects/Faithful/external/dr_libs/dr_libs
#        PUBLIC /home/pavlo/CLionProjects/Faithful/cmake-build-debug/src/libogg_external-build/include/ogg
#        PUBLIC /home/pavlo/CLionProjects/Faithful/external/vorbis/vorbis/include
#        PUBLIC /home/pavlo/CLionProjects/Faithful/external/libogg/libogg/include
#        PUBLIC /home/pavlo/CLionProjects/Faithful/external/rapidjson/rapidjson/include/rapidjson
#        PUBLIC /home/pavlo/CLionProjects/Faithful/external/tinygltf/tinygltf
#)

