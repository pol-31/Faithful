cmake_minimum_required(VERSION 3.0)

# set policy CMP0072 to suppress warning about
# founding both a legacy GL library and GLVND libs
cmake_policy(SET CMP0072 NEW)

find_package(OpenGL QUIET)

if (OPENGL_FOUND)
    message(STATUS "OPENGL: ${OPENGL_LIBRARY}")
else()
    include(${CMAKE_SOURCE_DIR}/cmake/FaithfulFunctions.cmake)
    set(FAITHFUL_DEPS_FOUND false)
    message(STATUS "OPENGL: __not_found__")
endif()
