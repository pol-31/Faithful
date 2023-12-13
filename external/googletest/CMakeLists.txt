cmake_minimum_required(VERSION 3.26)

message(STATUS "googletest building")

set(BUILD_GMOCK OFF CACHE BOOL "Build GoogleTest GMock" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "Install GoogleTest" FORCE)

set(GTEST_BINARY_DIR ${CMAKE_BINARY_DIR}/googletest-build CACHE INTERNAL "googletest binary directory" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${GTEST_BINARY_DIR} CACHE INTERNAL "googletest executable output directory" FORCE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${GTEST_BINARY_DIR} CACHE INTERNAL "googletest library output directory" FORCE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${GTEST_BINARY_DIR} CACHE INTERNAL "googletest archive output directory" FORCE)

add_subdirectory(googletest)
