cmake_minimum_required(VERSION 3.26)
project(AssetProcessor)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} CACHE INTERNAL "AssetProcessor executable output directory" FORCE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} CACHE INTERNAL "AssetProcessor library output directory" FORCE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} CACHE INTERNAL "AssetProcessor archive output directory" FORCE)

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
        PUBLIC /home/pavlo/CLionProjects/Faithful/cmake-build-debug/src/astcencoder_external-build/Source/libastcenc-native-static.a
        PUBLIC /home/pavlo/CLionProjects/Faithful/cmake-build-debug/astc-encoder-build/libdr_libsd.a
        PUBLIC /home/pavlo/CLionProjects/Faithful/cmake-build-debug/src/vorbis_external-build/lib/libvorbisenc.a
        PUBLIC /home/pavlo/CLionProjects/Faithful/cmake-build-debug/src/vorbis_external-build/lib/libvorbisfile.a
        PUBLIC /home/pavlo/CLionProjects/Faithful/cmake-build-debug/src/vorbis_external-build/lib/libvorbis.a
        PUBLIC /home/pavlo/CLionProjects/Faithful/cmake-build-debug/src/libogg_external-build/src/.libs/libogg.a
#        PUBLIC rapidjson
#        PUBLIC tinygltf
)
target_include_directories(AssetProcessor
        PUBLIC /home/pavlo/CLionProjects/Faithful/external/astc-encoder/astc-encoder/Source
        PUBLIC /home/pavlo/CLionProjects/Faithful/external/dr_libs/dr_libs
        PUBLIC /home/pavlo/CLionProjects/Faithful/cmake-build-debug/src/libogg_external-build/include/ogg
        PUBLIC /home/pavlo/CLionProjects/Faithful/external/vorbis/vorbis/include
        PUBLIC /home/pavlo/CLionProjects/Faithful/external/libogg/libogg/include
        PUBLIC /home/pavlo/CLionProjects/Faithful/external/rapidjson/rapidjson/include/rapidjson
        PUBLIC /home/pavlo/CLionProjects/Faithful/external/tinygltf/tinygltf
)


#target_link_libraries(AssetProcessor
#        PUBLIC assimp
#        PUBLIC astc-encoder
#        PUBLIC dr_libs
#        PUBLIC vorbis
#        PUBLIC libogg
#)
#target_include_directories(AssetProcessor
#        PUBLIC ${CMAKE_SOURCE_DIR}/assimp/assimp/include
#        PUBLIC ${CMAKE_SOURCE_DIR}/external/astc-encoder/astc-encoder/Source
#        PUBLIC ${CMAKE_SOURCE_DIR}/external/dr_libs/dr_libs
#        PUBLIC ${CMAKE_SOURCE_DIR}/external/vorbis/vorbis/include
#        PUBLIC ${CMAKE_SOURCE_DIR}/external/libogg/libogg/include
#)


# TODO: include(cmake/FindDepsAssetProcessor.cmake)
#  faithful_asset_processor_build_external()

#add_definitions(-DASSET_PROCESSOR)
#set(sounds_bin_path "${CMAKE_SOURCE_DIR}/assets/audio")
#set(models_bin_path "${CMAKE_SOURCE_DIR}/assets/models")
#set(shaders_path "${CMAKE_SOURCE_DIR}/assets/shaders")
#set(textures_bin_path "${CMAKE_SOURCE_DIR}/assets/textures")



