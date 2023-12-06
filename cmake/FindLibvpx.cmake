cmake_minimum_required(VERSION 3.0)

find_path(VPX_INCLUDE_DIR NAMES vpx/vpx_codec.h)
find_library(VPX_LIBRARY NAMES vpx)

include(${CMAKE_SOURCE_DIR}/cmake/FaithfulFunctions.cmake)
lib_found_info(VPX VPX_INCLUDE_DIR VPX_LIBRARY)

mark_as_advanced(VPX_INCLUDE_DIR VPX_LIBRARY)
