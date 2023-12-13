
find_path(ASSIMP_INCLUDE_DIR NAMES assimp/Importer.hpp)
find_library(ASSIMP_LIBRARY NAMES assimp)

include(${CMAKE_SOURCE_DIR}/cmake/FaithfulFunctions.cmake)
lib_found_info(ASSIMP ASSIMP_INCLUDE_DIR ASSIMP_LIBRARY)

mark_as_advanced(ASSIMP_INCLUDE_DIR ASSIMP_LIBRARY)
