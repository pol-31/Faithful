macro(find_alsa)
    find_path(ALSA_INCLUDE_DIR NAMES alsa/asoundlib.h)
    find_library(ALSA_LIBRARY NAMES asound)
    include(${CMAKE_SOURCE_DIR}/cmake/FaithfulFunctions.cmake)
    lib_found_info(ALSA ALSA_INCLUDE_DIR ALSA_LIBRARY)
    mark_as_advanced(ALSA_INCLUDE_DIR ALSA_LIBRARY)
endmacro(find_alsa)

macro(find_curl)
    find_path(CURL_INCLUDE_DIRS NAMES curl/curl.h)
    find_library(CURL_LIBRARIES NAMES curl)
    include(${CMAKE_SOURCE_DIR}/cmake/FaithfulFunctions.cmake)
    lib_found_info(CURL CURL_INCLUDE_DIRS CURL_LIBRARIES)
    mark_as_advanced(CURL_INCLUDE_DIRS CURL_LIBRARIES)
endmacro(find_curl)

macro(find_glfw)
    find_path(GLFW_INCLUDE_DIR NAMES GLFW/glfw3.h)
    find_library(GLFW_LIBRARY NAMES glfw3 glfw glfw3dll glfw3dll.lib glfw3.lib)
    include(${CMAKE_SOURCE_DIR}/cmake/FaithfulFunctions.cmake)
    lib_found_info(GLFW GLFW_INCLUDE_DIR GLFW_LIBRARY)
    mark_as_advanced(GLFW_INCLUDE_DIR GLFW_LIBRARY)
endmacro(find_glfw)

macro(find_glm)
    find_path(GLM_INCLUDE_DIR NAMES glm/glm.hpp)
    include(${CMAKE_SOURCE_DIR}/cmake/FaithfulFunctions.cmake)
    header_found_info(GLM GLM_INCLUDE_DIR)
    mark_as_advanced(GLM_INCLUDE_DIR)
endmacro(find_glm)

macro(find_gtest)
    find_path(GTEST_INCLUDE_DIR NAMES gtest/gtest.h)
    find_library(GTEST_LIBRARY NAMES gtest gtest_main)
    include(${CMAKE_SOURCE_DIR}/cmake/FaithfulFunctions.cmake)
    lib_found_info(GTEST GTEST_INCLUDE_DIR GTEST_LIBRARY)
    mark_as_advanced(GTEST_INCLUDE_DIR GTEST_LIBRARY)
endmacro(find_gtest)

macro(find_libogg)
    find_path(LIBOGG_INCLUDE_DIR NAMES ogg/ogg.h)
    find_library(LIBOGG_LIBRARY NAMES ogg)
    include(${CMAKE_SOURCE_DIR}/cmake/FaithfulFunctions.cmake)
    lib_found_info(LIBOGG LIBOGG_INCLUDE_DIR LIBOGG_LIBRARY)
    mark_as_advanced(LIBOGG_INCLUDE_DIR LIBOGG_LIBRARY)
endmacro(find_libogg)

macro(find_mimalloc)
    find_path(MIMALLOC_INCLUDE_DIR NAMES mimalloc.h)
    find_library(MIMALLOC_LIBRARY NAMES mimalloc)
    include(${CMAKE_SOURCE_DIR}/cmake/FaithfulFunctions.cmake)
    lib_found_info(MIMALLOC MIMALLOC_INCLUDE_DIR MIMALLOC_LIBRARY)
    mark_as_advanced(MIMALLOC_INCLUDE_DIR MIMALLOC_LIBRARY)
endmacro(find_mimalloc)

macro(find_openal_soft)
    find_path(OPENAL_INCLUDE_DIR NAMES AL/al.h)
    find_library(OPENAL_LIBRARY NAMES OpenAL)#al libopenal)
    include(${CMAKE_SOURCE_DIR}/cmake/FaithfulFunctions.cmake)
    lib_found_info(OPENAL OPENAL_INCLUDE_DIR OPENAL_LIBRARY)
    mark_as_advanced(OPENAL_INCLUDE_DIR OPENAL_LIBRARY)
endmacro(find_openal_soft)

macro(find_opengl)
    # set policy CMP0072 to suppress warning about
    # founding both a legacy GL library and GLVND libs
    cmake_policy(SET CMP0072 NEW)
    find_package(OpenGL REQUIRED)
    if (OPENGL_FOUND)
        message(STATUS "OPENGL: ${OPENGL_LIBRARY}")
    else()
        include(${CMAKE_SOURCE_DIR}/cmake/FaithfulFunctions.cmake)
        set(FAITHFUL_DEPS_FOUND false)
        message(FATAL_ERROR "OPENGL: __not_found__")
    endif()
endmacro(find_opengl)

macro(find_rapidjson)
    find_path(RAPIDJSON_INCLUDE_DIR NAMES rapidjson/rapidjson.h)
    include(${CMAKE_SOURCE_DIR}/cmake/FaithfulFunctions.cmake)
    header_found_info(RAPIDJSON RAPIDJSON_INCLUDE_DIR)
    mark_as_advanced(RAPIDJSON_INCLUDE_DIR)
endmacro(find_rapidjson)

macro(find_vorbis)
    find_path(VORBIS_INCLUDE_DIR NAMES vorbis/codec.h)
    find_library(VORBIS_LIBRARY NAMES vorbis)
    include(${CMAKE_SOURCE_DIR}/cmake/FaithfulFunctions.cmake)
    lib_found_info(VORBIS VORBIS_INCLUDE_DIR VORBIS_LIBRARY)
    mark_as_advanced(VORBIS_INCLUDE_DIR VORBIS_LIBRARY)
endmacro(find_vorbis)
