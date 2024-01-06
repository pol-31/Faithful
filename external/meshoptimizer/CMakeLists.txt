cmake_minimum_required(VERSION 3.26)

message(STATUS "meshoptimizer building")

include(FetchContent)
FetchContent_Declare(
        rapidjson
        GIT_REPOSITORY https://github.com/zeux/meshoptimizer.git
        GIT_TAG master
#        EXCLUDE_FROM_ALL # cmake 3.28
)

set(MESHOPT_BUILD_GLTFPACK ON CACHE BOOL "Enable meshoptimizer gltfpack" FORCE)

FetchContent_MakeAvailable(rapidjson)
