cmake_minimum_required(VERSION 3.26)

message(STATUS "cgltf building")

add_library(CGLTF INTERFACE)

set(CGLTF_BINARY_DIR ${CMAKE_BINARY_DIR}/cgltf-build CACHE INTERNAL "cgltf binary directory" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CGLTF_BINARY_DIR} CACHE INTERNAL "cgltf executable output directory" FORCE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CGLTF_BINARY_DIR} CACHE INTERNAL "cgltf library output directory" FORCE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CGLTF_BINARY_DIR} CACHE INTERNAL "cgltf archive output directory" FORCE)

target_include_directories(CGLTF INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/cgltf")
