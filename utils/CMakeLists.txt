cmake_minimum_required(VERSION 3.26)

project(faithful-utils)

file(GLOB HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(faithful-utils ${SOURCE_FILES} ${HEADER_FILES})

add_subdirectory(AssetProcessor)
add_subdirectory(AssetDownloader)
