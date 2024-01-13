cmake_minimum_required(VERSION 3.26)

message(STATUS "meshoptimizer building")

include(ExternalProject)

set(MESHOPTIMIZER_SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/meshoptimizer/meshoptimizer_source)
set(MESHOPTIMIZER_BINARY_DIR ${CMAKE_BINARY_DIR}/external/meshoptimizer
        CACHE INTERNAL "meshoptimizer binary directory" FORCE)

set(MESHOPTIMIZER_URL https://github.com/zeux/meshoptimizer.git)
set(MESHOPTIMIZER_REVISION master)

ExternalProject_Add(
        meshoptimizer_external
        BINARY_DIR ${MESHOPTIMIZER_BINARY_DIR}
        SOURCE_DIR ${MESHOPTIMIZER_SOURCE_DIR}
        GIT_REPOSITORY ${MESHOPTIMIZER_URL}
        GIT_TAG ${MESHOPTIMIZER_REVISION}
        CONFIGURE_COMMAND ${CMAKE_COMMAND}
        -DMESHOPT_BUILD_GLTFPACK=ON
        ${MESHOPTIMIZER_SOURCE_DIR}
        BUILD_COMMAND ${CMAKE_COMMAND} --build .
        INSTALL_COMMAND ${CMAKE_COMMAND} -E echo "Skipping install step"
        UPDATE_DISCONNECTED 1
        BUILD_BYPRODUCTS ${MESHOPTIMIZER_BINARY_DIR}/gltfpack
)

set(FAITHFUL_ASSET_DOWNLOADER_GLTFPACK_PATH ${MESHOPTIMIZER_BINARY_DIR}/gltfpack CACHE PATH "" FORCE)

add_library(meshoptimizer INTERFACE)
add_dependencies(meshoptimizer meshoptimizer_external)
