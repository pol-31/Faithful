cmake_minimum_required(VERSION 3.26)

message(STATUS "curl building")

set(BUILD_CURL_EXE OFF CACHE BOOL "Set to ON to build curl executable." FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build curl shared libraries" FORCE)
set(BUILD_STATIC_LIBS ON CACHE BOOL "Build curl static libraries" FORCE)
set(HTTP_ONLY ON CACHE BOOL "Disables all curl protocols except HTTP" FORCE)

set(PICKY_COMPILER OFF CACHE BOOL "Enable curl picky compiler options" FORCE)
set(CURL_DISABLE_INSTALL ON CACHE BOOL "Set to ON to disable curl installation targets" FORCE)
set(CURL_LTO ON CACHE BOOL "Turn on curl compiler Link Time Optimizations" FORCE)
set(CURL_ENABLE_EXPORT_TARGET OFF CACHE BOOL "To enable curl cmake export target" FORCE)


set(CURL_BINARY_DIR ${CMAKE_BINARY_DIR}/curl-build CACHE INTERNAL "curl binary directory" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CURL_BINARY_DIR} CACHE INTERNAL "curl executable output directory" FORCE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CURL_BINARY_DIR} CACHE INTERNAL "curl library output directory" FORCE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CURL_BINARY_DIR} CACHE INTERNAL "curl archive output directory" FORCE)

add_subdirectory(curl)
