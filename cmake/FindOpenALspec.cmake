cmake_minimum_required(VERSION 3.0)

find_path(OPENAL_INCLUDE_DIR NAMES AL/al.h)
find_library(OPENAL_LIBRARY NAMES OpenAL al)

include(${CMAKE_SOURCE_DIR}/cmake/FaithfulFunctions.cmake)
lib_found_info(OPENAL OPENAL_INCLUDE_DIR OPENAL_LIBRARY)

mark_as_advanced(OPENAL_INCLUDE_DIR OPENAL_LIBRARY)
