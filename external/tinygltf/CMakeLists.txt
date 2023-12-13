cmake_minimum_required(VERSION 3.26)

message(STATUS "tinygltf building")

set(TINYGLTF_BUILD_LOADER_EXAMPLE OFF CACHE BOOL "Build loader_example(load glTF and dump infos)" FORCE)
set(TINYGLTF_BUILD_GL_EXAMPLES OFF CACHE BOOL "Build GL exampels(requires glfw, OpenGL, etc)" FORCE)
set(TINYGLTF_BUILD_VALIDATOR_EXAMPLE OFF CACHE BOOL "Build validator exampe" FORCE)
set(TINYGLTF_BUILD_BUILDER_EXAMPLE OFF CACHE BOOL "Build glTF builder example" FORCE)

set(TINYGLTF_HEADER_ONLY ON CACHE BOOL "On: header-only mode. Off: create tinygltf library(No TINYGLTF_IMPLEMENTATION required in your project)" FORCE)
set(TINYGLTF_INSTALL OFF CACHE BOOL "Install tinygltf files during install step. Usually set to OFF if you include tinygltf through add_subdirectory()" FORCE)

set(TINYGLTF_BINARY_DIR ${CMAKE_BINARY_DIR}/assimp-build CACHE INTERNAL "tinygltf binary directory" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${TINYGLTF_BINARY_DIR} CACHE INTERNAL "tinygltf executable output directory" FORCE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${TINYGLTF_BINARY_DIR} CACHE INTERNAL "tinygltf library output directory" FORCE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${TINYGLTF_BINARY_DIR} CACHE INTERNAL "tinygltf archive output directory" FORCE)

add_subdirectory(tinygltf)
