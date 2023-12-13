
find_path(VORBIS_INCLUDE_DIR NAMES vorbis/codec.h)
find_library(VORBIS_LIBRARY NAMES vorbis)

include(${CMAKE_SOURCE_DIR}/cmake/FaithfulFunctions.cmake)
lib_found_info(VORBIS VORBIS_INCLUDE_DIR VORBIS_LIBRARY)

mark_as_advanced(VORBIS_INCLUDE_DIR VORBIS_LIBRARY)
