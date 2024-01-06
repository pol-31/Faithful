cmake_minimum_required(VERSION 3.26)

message(STATUS "curl building")

include(FetchContent)
FetchContent_Declare(
        curl
        GIT_REPOSITORY https://github.com/curl/curl.git
        GIT_TAG master
#        EXCLUDE_FROM_ALL # cmake 3.28
)

set(BUILD_CURL_EXE OFF CACHE BOOL "Set to ON to build curl executable." FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build curl shared libraries" FORCE)
set(BUILD_STATIC_LIBS ON CACHE BOOL "Build curl static libraries" FORCE)
set(HTTP_ONLY ON CACHE BOOL "Disables all curl protocols except HTTP" FORCE)

set(PICKY_COMPILER OFF CACHE BOOL "Enable curl picky compiler options" FORCE)
set(CURL_DISABLE_INSTALL ON CACHE BOOL "Set to ON to disable curl installation targets" FORCE)
set(CURL_LTO ON CACHE BOOL "Turn on curl compiler Link Time Optimizations" FORCE)
set(CURL_ENABLE_EXPORT_TARGET OFF CACHE BOOL "To enable curl cmake export target" FORCE)

FetchContent_MakeAvailable(curl)
