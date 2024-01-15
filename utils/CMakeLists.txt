cmake_minimum_required(VERSION 3.26)

project(faithful-utils)

#file(GLOB HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
#file(GLOB SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(faithful-utils INTERFACE ${SOURCE_FILES} ${HEADER_FILES})

if (FAITHFUL_BUILD_ASSET_DOWNLOADER)
    add_subdirectory(AssetDownloader)
endif()

if (FAITHFUL_BUILD_ASSET_PACK)
    add_subdirectory(AssetPack)
endif()

if (FAITHFUL_BUILD_ASSET_PROCESSOR)
    add_subdirectory(AssetProcessor)
endif()
