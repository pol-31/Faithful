
find_path(FREETYPE_INCLUDE_DIR NAMES freetype/freetype.h)
find_library(FREETYPE_LIBRARY NAMES freetype freetype.lib)

include(${CMAKE_SOURCE_DIR}/cmake/FaithfulFunctions.cmake)
lib_found_info(FREETYPE FREETYPE_INCLUDE_DIR FREETYPE_LIBRARY)

mark_as_advanced(FREETYPE_INCLUDE_DIR FREETYPE_LIBRARY)
