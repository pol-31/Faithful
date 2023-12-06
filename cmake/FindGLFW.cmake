cmake_minimum_required(VERSION 3.0)

find_path(GLFW_INCLUDE_DIR NAMES GLFW/glfw3.h)
find_library(GLFW_LIBRARY NAMES glfw3 glfw glfw3dll glfw3dll.lib glfw3.lib)

include(${CMAKE_SOURCE_DIR}/cmake/FaithfulFunctions.cmake)
lib_found_info(GLFW GLFW_INCLUDE_DIR GLFW_LIBRARY)

mark_as_advanced(GLFW_INCLUDE_DIR GLFW_LIBRARY)
