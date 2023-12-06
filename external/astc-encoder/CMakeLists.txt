cmake_minimum_required(VERSION 3.26)

message(STATUS "astc-encoder building")

set(ASTCENC_SHAREDLIB OFF CACHE BOOL "Build astc-encoder shared libraries" FORCE)
set(ASTCENC_BLOCK_MAX_TEXELS 64 CACHE STRING "Build astc-encoder with constrained block size" FORCE)
set(ASTCENC_INVARIANCE OFF CACHE BOOL "Build astc-encoder non-invariant libraries" FORCE)
set(ASTCENC_UNITTEST OFF CACHE BOOL "Build astc-enc tests" FORCE)

set(ASTC_ENCODER_BINARY_DIR ${CMAKE_BINARY_DIR}/astc-encoder-build CACHE INTERNAL "astc-encoder binary directory" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ASTC_ENCODER_BINARY_DIR} CACHE INTERNAL "astc-encoder executable output directory" FORCE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${ASTC_ENCODER_BINARY_DIR} CACHE INTERNAL "astc-encoder library output directory" FORCE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${ASTC_ENCODER_BINARY_DIR} CACHE INTERNAL "astc-encoder archive output directory" FORCE)

add_subdirectory(astc-encoder)
