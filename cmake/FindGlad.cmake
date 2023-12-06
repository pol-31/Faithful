cmake_minimum_required(VERSION 3.0)

find_path(GLAD_INCLUDE_DIR glad.h)
find_library(GLAD_LIBRARY NAMES glad)

include(${CMAKE_SOURCE_DIR}/cmake/FaithfulFunctions.cmake)
lib_found_info(GLAD GLAD_INCLUDE_DIR GLAD_LIBRARY)

mark_as_advanced(GLAD_INCLUDE_DIR GLAD_LIBRARY)
