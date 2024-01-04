cmake_minimum_required(VERSION 3.26)

message(STATUS "rapidjson building")

set(RAPIDJSON_BUILD_DOC OFF CACHE BOOL "Build rapidjson documentation." FORCE)
set(RAPIDJSON_BUILD_EXAMPLES OFF CACHE BOOL "Build rapidjson examples." FORCE)
set(RAPIDJSON_BUILD_TESTS OFF CACHE BOOL "Build rapidjson perftests and unittests." FORCE)
set(RAPIDJSON_BUILD_THIRDPARTY_GTEST OFF CACHE BOOL
        "Use rapidjson gtest installation in `thirdparty/gtest` by default if available" FORCE)

set(RAPIDJSON_BUILD_CXX11 OFF CACHE BOOL "Build rapidjson with C++11" FORCE)
set(RAPIDJSON_BUILD_CXX17 ON CACHE BOOL "Build rapidjson with C++17" FORCE)

set(RAPIDJSON_ENABLE_INSTRUMENTATION_OPT ON CACHE BOOL "Build rapidjson with -march or -mcpu options" FORCE)

set(RAPIDJSON_BINARY_DIR ${CMAKE_BINARY_DIR}/assimp-build CACHE INTERNAL "rapidjson binary directory" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${RAPIDJSON_BINARY_DIR} CACHE INTERNAL "rapidjson executable output directory" FORCE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${RAPIDJSON_BINARY_DIR} CACHE INTERNAL "rapidjson library output directory" FORCE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${RAPIDJSON_BINARY_DIR} CACHE INTERNAL "rapidjson archive output directory" FORCE)

add_subdirectory(rapidjson EXCLUDE_FROM_ALL)
