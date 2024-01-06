cmake_minimum_required(VERSION 3.26)

project(AssetDownloader)

cmake_policy(SET CMP0069 NEW)  # LTO support
cmake_policy(SET CMP0091 NEW)  # MSVC runtime support


# TODO: include(cmake/FindDepsAssetProcessor.cmake)
# TODO: add compile options
# TODO: integrate ${CMAKE_SOURCE_DIR}

set(CMAKE_CXX_STANDARD 17)


#include(../../cmake/FaithfulFunctions.cmake)
#target_link_asset_downloader_deps(AssetDownloader)
#target_include_asset_downloader_deps(AssetDownloader)

# TODO: THERE IS NO FAITHFUL_project (AssetDownloader built as an ExternalProject_Add())

#set(ASSETS_AUDIO_PATH "${CMAKE_SOURCE_DIR}/assets/audio")
#set(ASSETS_MODEL_PATH "${CMAKE_SOURCE_DIR}/assets/models")
#set(ASSETS_TEXTURE_PATH "${CMAKE_SOURCE_DIR}/assets/textures")
#set(ASSETS_CONFIGURE_PATH "${CMAKE_SOURCE_DIR}/config/config_paths.h.in")
#configure_file(${ASSETS_CONFIGURE_PATH} ${CMAKE_SOURCE_DIR}/config/config_paths.h)

#add_subdirectory(
#        "/home/pavlo/CLionProjects/Faithful/external/curl"
#        "/home/pavlo/CLionProjects/Faithful/cmake-build-debug/temp/external/curl"
#)
#add_subdirectory(
#        "/home/pavlo/CLionProjects/Faithful/external/miniz"
#        "/home/pavlo/CLionProjects/Faithful/cmake-build-debug/temp/external/miniz"
#)

#add_executable(AssetDownloader main.cpp)

#target_link_libraries(AssetDownloader PUBLIC curl)
#target_link_libraries(AssetDownloader PUBLIC miniz)
#target_include_directories(AssetDownloader
#        PUBLIC /home/pavlo/CLionProjects/Faithful/curl/curl/include
#        PUBLIC /home/pavlo/CLionProjects/Faithful/miniz/miniz)
