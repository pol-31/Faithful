cmake_minimum_required(VERSION 3.26)
project(AssetProcessor)

set(CMAKE_CXX_STANDARD 17)

include(ExternalProject)

cmake_policy(SET CMP0069 NEW)  # LTO support
cmake_policy(SET CMP0091 NEW)  # MSVC runtime support

#[[
add_subdirectory(
        "/home/pavlo/CLionProjects/Faithful/external/curl"
        "/home/pavlo/CLionProjects/Faithful/cmake-build-debug/temp/external/curl"
)]]
add_subdirectory(assimp)
add_subdirectory(dr_libs)
add_subdirectory(libogg)
add_subdirectory(vorbis)
add_dependencies(vorbis libogg)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} CACHE INTERNAL "AssetProcessor executable output directory" FORCE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} CACHE INTERNAL "AssetProcessor library output directory" FORCE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} CACHE INTERNAL "AssetProcessor archive output directory" FORCE)

add_executable(AssetProcessor main.cpp ModelProcessor.cpp)

target_link_libraries(AssetProcessor
        PUBLIC assimp
        PUBLIC ${CMAKE_BINARY_DIR}/src/astcencoder_external-build/Source/libastcenc-native-static.a
        PUBLIC dr_libs
        PUBLIC vorbis
        PUBLIC libogg
)
target_include_directories(AssetProcessor
        PUBLIC ${CMAKE_SOURCE_DIR}/assimp/assimp/Source
        PUBLIC ${CMAKE_SOURCE_DIR}/astcencoder/astcencoder/Source
        PUBLIC ${CMAKE_SOURCE_DIR}/dr_libs/dr_libs
        PUBLIC ${CMAKE_SOURCE_DIR}/libogg/libogg/include
        PUBLIC ${CMAKE_SOURCE_DIR}/vorbis/vorbis/include
)



# TODO: include(cmake/FindDepsAssetProcessor.cmake)
#  faithful_asset_processor_build_external()

#add_definitions(-DASSET_PROCESSOR)
#set(sounds_bin_path "${CMAKE_SOURCE_DIR}/assets/audio")
#set(models_bin_path "${CMAKE_SOURCE_DIR}/assets/models")
#set(shaders_path "${CMAKE_SOURCE_DIR}/assets/shaders")
#set(textures_bin_path "${CMAKE_SOURCE_DIR}/assets/textures")



