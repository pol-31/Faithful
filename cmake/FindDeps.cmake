
# --- find GLFW ---

find_path(GLFW_INCLUDE_DIR NAMES GLFW/glfw3.h)
find_library(GLFW_LIBRARY NAMES glfw3 glfw libglfw)
include(${FAITHFUL_SOURCE_DIR}/cmake/FaithfulFunctions.cmake)
lib_found_info(GLFW GLFW_INCLUDE_DIR GLFW_LIBRARY)
mark_as_advanced(GLFW_INCLUDE_DIR GLFW_LIBRARY)

# --- find GLM ---

find_path(GLM_INCLUDE_DIR NAMES glm/glm.hpp)
include(${FAITHFUL_SOURCE_DIR}/cmake/FaithfulFunctions.cmake)
header_found_info(GLM GLM_INCLUDE_DIR)
mark_as_advanced(GLM_INCLUDE_DIR)

# --- find GTest---

if (FAITHFUL_BUILD_TESTS)
    find_package(GTest)
endif()

# --- find mimalloc ---

find_path(MIMALLOC_INCLUDE_DIR NAMES mimalloc.h)
find_library(MIMALLOC_LIBRARY NAMES mimalloc libmimalloc)
include(${FAITHFUL_SOURCE_DIR}/cmake/FaithfulFunctions.cmake)
lib_found_info(MIMALLOC MIMALLOC_INCLUDE_DIR MIMALLOC_LIBRARY)
mark_as_advanced(MIMALLOC_INCLUDE_DIR MIMALLOC_LIBRARY)

# --- find libogg ---

find_path(LIBOGG_INCLUDE_DIR NAMES ogg/ogg.h)
find_library(LIBOGG_LIBRARY NAMES ogg libogg)
include(${FAITHFUL_SOURCE_DIR}/cmake/FaithfulFunctions.cmake)
lib_found_info(LIBOGG LIBOGG_INCLUDE_DIR LIBOGG_LIBRARY)
mark_as_advanced(LIBOGG_INCLUDE_DIR LIBOGG_LIBRARY)

# --- find OpenAL-Soft ---

find_package(OpenAL)

# --- find OpenGL (specification) ---

# set policy CMP0072 to suppress warning about
# founding both a legacy GL library and GLVND libs
cmake_policy(SET CMP0072 NEW)
find_package(OpenGL REQUIRED)
if (OPENGL_FOUND)
    message(STATUS "OPENGL: ${OPENGL_LIBRARY}")
else()
    include(${FAITHFUL_SOURCE_DIR}/cmake/FaithfulFunctions.cmake)
    set(FAITHFUL_DEPS_FOUND false)
    message(FATAL_ERROR "OPENGL: __not_found__")
endif()

# --- find RapidJSON ---

find_path(RAPIDJSON_INCLUDE_DIR NAMES rapidjson/rapidjson.h)
include(${FAITHFUL_SOURCE_DIR}/cmake/FaithfulFunctions.cmake)
header_found_info(RAPIDJSON RAPIDJSON_INCLUDE_DIR)
mark_as_advanced(RAPIDJSON_INCLUDE_DIR)

# --- find libvorbis (vorbis, vorbisenc, vorbisfile) ---

find_path(VORBIS_INCLUDE_DIR NAMES vorbis/codec.h)
find_library(VORBIS_LIBRARY NAMES vorbis libvorbis)
include(${FAITHFUL_SOURCE_DIR}/cmake/FaithfulFunctions.cmake)
lib_found_info(VORBIS VORBIS_INCLUDE_DIR VORBIS_LIBRARY)
mark_as_advanced(VORBIS_INCLUDE_DIR VORBIS_LIBRARY)

find_path(VORBISENC_INCLUDE_DIR NAMES vorbis/codec.h)
find_library(VORBISENC_LIBRARY NAMES vorbisenc libvorbisenc)
include(${FAITHFUL_SOURCE_DIR}/cmake/FaithfulFunctions.cmake)
lib_found_info(VORBISENC VORBISENC_INCLUDE_DIR VORBISENC_LIBRARY)
mark_as_advanced(VORBISENC_INCLUDE_DIR VORBISENC_LIBRARY)

find_path(VORBISFILE_INCLUDE_DIR NAMES vorbis/codec.h)
find_library(VORBISFILE_LIBRARY NAMES vorbisfile libvorbisfile)
include(${FAITHFUL_SOURCE_DIR}/cmake/FaithfulFunctions.cmake)
lib_found_info(VORBISFILE VORBISFILE_INCLUDE_DIR VORBISFILE_LIBRARY)
mark_as_advanced(VORBISFILE_INCLUDE_DIR VORBISFILE_LIBRARY)