cmake_minimum_required(VERSION 3.26)

message(STATUS "meshoptimizer building")

include(ExternalProject)

set(MESHOPTIMIZER_SOURCE_DIR ${FAITHFUL_SOURCE_DIR}/external/meshoptimizer/meshoptimizer)
set(MESHOPTIMIZER_BINARY_DIR ${FAITHFUL_BINARY_DIR}/external/meshoptimizer)

ExternalProject_Add(
        meshoptimizer_external
        BINARY_DIR ${MESHOPTIMIZER_BINARY_DIR}
        SOURCE_DIR ${MESHOPTIMIZER_SOURCE_DIR}
        CONFIGURE_COMMAND ${CMAKE_COMMAND}
        -DMESHOPT_BUILD_GLTFPACK=ON
        ${MESHOPTIMIZER_SOURCE_DIR}
        BUILD_COMMAND ${CMAKE_COMMAND} --build .
        INSTALL_COMMAND ${CMAKE_COMMAND} -E echo ""
        BUILD_BYPRODUCTS ${MESHOPTIMIZER_BINARY_DIR}/gltfpack
)

set(FAITHFUL_GLTFPACK_PATH ${MESHOPTIMIZER_BINARY_DIR}/gltfpack PARENT_SCOPE)

add_library(meshoptimizer INTERFACE)
add_dependencies(meshoptimizer meshoptimizer_external)
