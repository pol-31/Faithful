cmake_minimum_required(VERSION 3.0)

find_path(GLM_INCLUDE_DIR NAMES glm/glm.hpp)

include(${CMAKE_SOURCE_DIR}/cmake/FaithfulFunctions.cmake)
header_found_info(GLM GLM_INCLUDE_DIR)

mark_as_advanced(GLM_INCLUDE_DIR)
