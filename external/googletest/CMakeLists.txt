# Note: CMake support is community-based. The maintainers do not use CMake
# internally.

cmake_minimum_required(VERSION 3.13)

project(googletest-distribution)
set(GOOGLETEST_VERSION 1.14.0)

if(NOT CYGWIN AND NOT MSYS AND NOT ${CMAKE_SYSTEM_NAME} STREQUAL QNX)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()

enable_testing()

include(CMakeDependentOption)
include(GNUInstallDirs)

option(GTEST_HAS_ABSL "Use Abseil and RE2. Requires Abseil and RE2 to be separately added to the build." OFF)

add_subdirectory( googletest )
