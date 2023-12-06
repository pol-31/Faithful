cmake_minimum_required(VERSION 3.26)

message(STATUS "googletest building")

set(BUILD_GMOCK OFF CACHE INTERNAL)
set(INSTALL_GTEST OFF CACHE INTERNAL)

add_subdirectory(googletest)
