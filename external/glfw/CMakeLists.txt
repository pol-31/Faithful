cmake_minimum_required(VERSION 3.26)

message(STATUS "glfw building")

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build glfw shared libraries" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build glfw example programs" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build glfw tests" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build glfw documentation" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "Install glfw" FORCE)


set(GLFW_BINARY_DIR ${CMAKE_BINARY_DIR}/glfw-build CACHE INTERNAL "glfw binary directory" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${GLFW_BINARY_DIR} CACHE INTERNAL "glfw executable output directory" FORCE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${GLFW_BINARY_DIR} CACHE INTERNAL "glfw library output directory" FORCE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${GLFW_BINARY_DIR} CACHE INTERNAL "glfw archive output directory" FORCE)

add_subdirectory(glfw)
