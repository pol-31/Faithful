cmake_minimum_required(VERSION 3.26)

# !! Should be built as ExternalProject
project(AssetDownloader)

cmake_policy(SET CMP0069 NEW)  # LTO support
cmake_policy(SET CMP0091 NEW)  # MSVC runtime support

set(CMAKE_CXX_STANDARD 17)

# does the file already downloaded (if no we going to use cURL
#    that was already downloaded by parent project)
option(FAITHFUL_ASSET_DOWNLOADER_DOWNLOADED OFF)

set(FAITHFUL_ASSET_DOWNLOADER_MINIZ_LIB_PATH "")
set(FAITHFUL_ASSET_DOWNLOADER_MINIZ_INCLUDE_PATH "")
set(FAITHFUL_ASSET_DOWNLOADER_CURL_LIB_PATH "")
set(FAITHFUL_ASSET_DOWNLOADER_CURL_INCLUDE_PATH "")

add_executable(AssetDownloader main.cpp)

if (FAITHFUL_ASSET_DOWNLOADER_DOWNLOADED)
    target_compile_definitions(AssetDownloader PUBLIC FILE_ALREADY_DOWNLOADED)
endif()

target_link_libraries(AssetDownloader
        PUBLIC FAITHFUL_ASSET_DOWNLOADER_MINIZ_LIB_PATH
        PUBLIC FAITHFUL_ASSET_DOWNLOADER_CURL_LIB_PATH
)

target_include_directories(AssetDownloader
        PUBLIC FAITHFUL_ASSET_DOWNLOADER_MINIZ_INCLUDE_PATH
        PUBLIC FAITHFUL_ASSET_DOWNLOADER_CURL_INCLUDE_PATH
)
