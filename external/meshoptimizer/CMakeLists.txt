cmake_minimum_required(VERSION 3.26)

message(STATUS "meshoptimizer building")

set(MESHOPTIMIZER_BINARY_DIR ${CMAKE_BINARY_DIR}/meshoptimizer-build CACHE INTERNAL "meshoptimizer binary directory" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MESHOPTIMIZER_BINARY_DIR} CACHE INTERNAL "meshoptimizer executable output directory" FORCE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${MESHOPTIMIZER_BINARY_DIR} CACHE INTERNAL "meshoptimizer library output directory" FORCE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${MESHOPTIMIZER_BINARY_DIR} CACHE INTERNAL "meshoptimizer archive output directory" FORCE)

add_subdirectory(meshoptimizer EXCLUDE_FROM_ALL)
