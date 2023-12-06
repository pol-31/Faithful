cmake_minimum_required(VERSION 3.26)

message(STATUS "yasm building")

set(YASM_BUILD_SHARED_LIBS OFF CACHE BOOL "Build yasm shared libraries" FORCE)
set(YASM_BUILD_TESTS OFF CACHE BOOL "Build yasm tests" FORCE)

set(YASM_SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/yasm/yasm CACHE INTERNAL "yasm source directory" FORCE)
set(YASM_BINARY_DIR ${CMAKE_BINARY_DIR}/yasm-build CACHE INTERNAL "yasm binary directory" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${YASM_BINARY_DIR} CACHE INTERNAL "yasm executable output directory" FORCE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${YASM_BINARY_DIR} CACHE INTERNAL "yasm library output directory" FORCE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${YASM_BINARY_DIR} CACHE INTERNAL "yasm archive output directory" FORCE)

include(ExternalProject)

ExternalProject_Add(
        yasm
        PREFIX ${YASM_BINARY_DIR}
        SOURCE_DIR ${YASM_SOURCE_DIR}
        BINARY_DIR ${YASM_BINARY_DIR}
        CMAKE_ARGS
        -DBUILD_SHARED_LIBS:BOOL=${YASM_BUILD_SHARED_LIBS}
        -DYASM_BUILD_TESTS:BOOL=${YASM_BUILD_TESTS}
)

ExternalProject_Get_Property(yasm SOURCE_DIR)
include_directories(${SOURCE_DIR}/include)
link_directories(${YASM_BINARY_DIR}/lib)
