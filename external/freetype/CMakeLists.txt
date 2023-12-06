cmake_minimum_required(VERSION 3.26)

message(STATUS "freetype building")

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build FreeType shared libraries" FORCE)
set(SKIP_INSTALL_ALL ON CACHE BOOL "Install FreeType" FORCE)

set(FREETYPE_BINARY_DIR ${CMAKE_BINARY_DIR}/freetype-build CACHE INTERNAL "freetype binary directory" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${FREETYPE_BINARY_DIR} CACHE INTERNAL "freetype executable output directory" FORCE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${FREETYPE_BINARY_DIR} CACHE INTERNAL "freetype library output directory" FORCE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${FREETYPE_BINARY_DIR} CACHE INTERNAL "freetype archive output directory" FORCE)

add_subdirectory(freetype)

target_include_directories(freetype INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/freetype/include")
