cmake_minimum_required(VERSION 3.26)

message(STATUS "glm building")

set(GLM_BINARY_DIR ${CMAKE_BINARY_DIR}/glm-build CACHE INTERNAL "glm binary directory" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${GLM_BINARY_DIR} CACHE INTERNAL "glm executable output directory" FORCE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${GLM_BINARY_DIR} CACHE INTERNAL "glm library output directory" FORCE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${GLM_BINARY_DIR} CACHE INTERNAL "glm archive output directory" FORCE)

add_subdirectory(glm)
