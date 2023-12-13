
find_path(CGLTF_INCLUDE_DIR cgltf.h)
find_library(CGLTF_LIBRARY cgltf)

include(${CMAKE_SOURCE_DIR}/cmake/FaithfulFunctions.cmake)
lib_found_info(CGLTF CGLTF_INCLUDE_DIR CGLTF_LIBRARY)

mark_as_advanced(CGLTF_INCLUDE_DIR CGLTF_LIBRARY)
