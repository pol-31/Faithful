cmake_minimum_required(VERSION 3.26)

message(STATUS "astc-encoder building")

set(ASTC_ENCODER_SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/astc-encoder/astc-encoder CACHE INTERNAL "astc-encoder source directory" FORCE)
set(ASTC_ENCODER_BINARY_DIR ${CMAKE_BINARY_DIR}/astc-encoder-build CACHE INTERNAL "astc-encoder binary directory" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ASTC_ENCODER_BINARY_DIR} CACHE INTERNAL "astc-encoder executable output directory" FORCE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${ASTC_ENCODER_BINARY_DIR} CACHE INTERNAL "astc-encoder library output directory" FORCE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${ASTC_ENCODER_BINARY_DIR} CACHE INTERNAL "astc-encoder archive output directory" FORCE)

include(ExternalProject)
ExternalProject_Add(
        astcencoder_external
        PREFIX ${CMAKE_BINARY_DIR}
        SOURCE_DIR ${ASTC_ENCODER_SOURCE_DIR}
        CONFIGURE_COMMAND ${CMAKE_COMMAND}
            -DASTCENC_ISA_NATIVE=ON
            -DASTCENC_DECOMPRESSOR=OFF
            -DASTCENC_SHAREDLIB=OFF
            -DASTCENC_DIAGNOSTICS=OFF
            -DASTCENC_ASAN=OFF
            -DASTCENC_UNITTEST=OFF
            -DASTCENC_INVARIANCE=OFF
            -DASTCENC_CLI=OFF
            -DASTCENC_BLOCK_MAX_TEXELS=64 # max 8x8x1 compression)
            ${ASTC_ENCODER_SOURCE_DIR}
        BUILD_COMMAND ${CMAKE_COMMAND} --build .
        INSTALL_COMMAND ${CMAKE_COMMAND} -E echo "Skipping install step"
        BUILD_BYPRODUCTS ${CMAKE_BINARY_DIR}/src/astcencoder_external-build/Source/libastcenc-native-static.a
)


add_library(astc-encoder INTERFACE)
add_dependencies(astc-encoder astcencoder_external)

target_link_libraries(astc-encoder
        INTERFACE ${CMAKE_BINARY_DIR}/src/astcencoder_external-build/Source/libastcenc-native-static.a
)
target_include_directories(astc-encoder INTERFACE ${VORBIS_SOURCE_DIR}/include)

