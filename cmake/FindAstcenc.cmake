cmake_minimum_required(VERSION 3.0)

find_path(ASTC_ENCODER_INCLUDE_DIR NAMES astcenc.h)
find_library(ASTC_ENCODER_LIBRARY NAMES astcenc)

include(${CMAKE_SOURCE_DIR}/cmake/FaithfulFunctions.cmake)
lib_found_info(ASTC_ENCODER ASTC_ENCODER_INCLUDE_DIR ASTC_ENCODER_LIBRARY)

mark_as_advanced(ASTC_ENCODER_INCLUDE_DIR ASTC_ENCODER_LIBRARY)
