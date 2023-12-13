cmake_minimum_required(VERSION 3.26)

project(AssetDownloader)

cmake_policy(SET CMP0069 NEW)  # LTO support
cmake_policy(SET CMP0091 NEW)  # MSVC runtime support


# TODO: include(cmake/FindDepsAssetProcessor.cmake)
# TODO: add run options (not there but in general)
# TODO: other compile options
# TODO: integrate ${CMAKE_SOURCE_DIR}

set(CMAKE_CXX_STANDARD 17)

# TODO: THERE IS NO FAITHFUL_project (AssetDownloader built as an ExternalProject_Add())

set(ASSETS_AUDIO_PATH "${CMAKE_SOURCE_DIR}/assets/audio")
set(ASSETS_MODEL_PATH "${CMAKE_SOURCE_DIR}/assets/models")
set(ASSETS_TEXTURE_PATH "${CMAKE_SOURCE_DIR}/assets/textures")

set(ASSETS_CONFIGURE_PATH "${CMAKE_SOURCE_DIR}/config/config_paths.h.in")


configure_file(${ASSETS_CONFIGURE_PATH} ${CMAKE_SOURCE_DIR}/config/config_paths.h)

add_subdirectory(
        "/home/pavlo/CLionProjects/Faithful/external/curl"
        "/home/pavlo/CLionProjects/Faithful/cmake-build-debug/temp/external/curl"
)
add_subdirectory(
        "/home/pavlo/CLionProjects/Faithful/external/miniz"
        "/home/pavlo/CLionProjects/Faithful/cmake-build-debug/temp/external/miniz"
)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "AssetDownloader executable output directory" FORCE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "AssetDownloader library output directory" FORCE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "AssetDownloader archive output directory" FORCE)

add_executable(AssetDownloader main.cpp)

target_link_libraries(AssetDownloader PUBLIC curl)
target_link_libraries(AssetDownloader PUBLIC miniz)
target_include_directories(AssetDownloader
        PUBLIC /home/pavlo/CLionProjects/Faithful/curl/curl/include
        PUBLIC /home/pavlo/CLionProjects/Faithful/miniz/miniz)
