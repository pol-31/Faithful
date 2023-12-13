cmake_minimum_required(VERSION 3.26)

message(STATUS "openal-soft building")

set(ALSOFT_DLOPEN OFF CACHE BOOL "Check for the dlopen API for loading optional libs"  FORCE)
set(ALSOFT_UTILS OFF CACHE BOOL "Build utility programs"  FORCE)
set(ALSOFT_EXAMPLES OFF CACHE BOOL "Build example programs"  FORCE)
set(ALSOFT_TESTS OFF CACHE BOOL "Build test programs"  FORCE)

set(ALSOFT_INSTALL OFF CACHE BOOL "Install main library" FORCE)
set(ALSOFT_INSTALL_CONFIG OFF CACHE BOOL "Install alsoft.conf sample configuration file" FORCE)
set(ALSOFT_INSTALL_HRTF_DATA OFF CACHE BOOL "Install HRTF data files" FORCE)
set(ALSOFT_INSTALL_AMBDEC_PRESETS OFF CACHE BOOL "Install AmbDec preset files" FORCE)
set(ALSOFT_INSTALL_EXAMPLES OFF CACHE BOOL "Install example programs (alplay, alstream, ...)" FORCE)
set(ALSOFT_INSTALL_UTILS OFF CACHE BOOL "Install utility programs (openal-info, alsoft-config, ...)" FORCE)
set(ALSOFT_UPDATE_BUILD_VERSION OFF CACHE BOOL "Update git build version info" FORCE)


set(OPENAL_BINARY_DIR ${CMAKE_BINARY_DIR}/openal-build CACHE INTERNAL "openal binary directory" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OPENAL_BINARY_DIR} CACHE INTERNAL "openal executable output directory" FORCE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OPENAL_BINARY_DIR} CACHE INTERNAL "openal library output directory" FORCE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OPENAL_BINARY_DIR} CACHE INTERNAL "openal archive output directory" FORCE)

add_subdirectory(openal-soft)
