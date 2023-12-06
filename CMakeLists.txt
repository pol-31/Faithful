
cmake_minimum_required(VERSION 3.26)
project(Faithful LANGUAGES CXX C)

cmake_policy(SET CMP0069 NEW)  # LTO support

message(STATUS "._._._._._._<Faithful>_._._._._._.")
message(STATUS "+> compilation started")
message(STATUS "CMake Version: ${CMAKE_VERSION}")
message(STATUS "CMake Generator: ${CMAKE_GENERATOR}")
message(STATUS "System: ${CMAKE_SYSTEM}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# TODO:
#   set(FAITHFUL_BINARY_DIR)

set(CMAKE_MODULE_PATH
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
        ${CMAKE_MODULE_PATH}
)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_definitions(-DFAITHFUL_DEBUG_BUILD)
endif()

add_definitions(-DFAITHFUL_MAIN_LOGGER)

# TODO begin
#  openGL version (for Glad)
#  paths
#  generate configs (?)
# TODO end

# There also could be such definitions:
#   supporting specific OpenGL extensions (like KHR_texture_compression_astc_ldr);
#   OpenGL OS-specific configs (CORE_PROFILE for MacOS, etc...);
#   x86_64 / AMD64;
#   SSE, AVX, PCLMUL.
# But there are not benefits of it in CMakeLists.txt.
# Such checks and accelerations implemented directly in source code

if(MSVC)
    include(${CMAKE_SOURCE_DIR}/cmake/CompilerMSVC.cmake)
else()
    include(${CMAKE_SOURCE_DIR}/cmake/CompilerUNIX.cmake)
endif()

option(BUILD_ASSET_PROCESSOR "include package AssetProcessor" OFF)

include(cmake/FaithfulFunctions.cmake)
# In this CMakeLists.txt we DO NOT building dependencies -
# - we only try to find. If we can't - building will fail.
# To build dependencies please follow README.md instruction

include(${CMAKE_SOURCE_DIR}/cmake/FindDepsMain.cmake)

if (BUILD_ASSET_PROCESSOR)
    include(cmake/FindDepsAssetProcessor.cmake)
    # TODO: add package AssetProcessor
endif()

option(INIT_ASSETS "download assets from GoogleDrive" ON)

if (INIT_ASSETS)
    # TODO: download assets (curl)
endif()


message(STATUS "C Compiler: ${CMAKE_C_COMPILER} ${CMAKE_C_COMPILER_VERSION}")
message(STATUS "C Config: ${CMAKE_C_STANDARD} ${CMAKE_C_FLAGS}")
message(STATUS "CXX Compiler: ${CMAKE_CXX_COMPILER} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "CXX Standard: ${CMAKE_CXX_STANDARD} ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")


option(FAITHFUL_LOGGER_BUFFERING "include package AssetProcessor" OFF)
# by default FAITHFUL_LOGGER_SEVERITY_ALL always ON
option(FAITHFUL_LOGGER_SEVERITY_NO_INFO "include package AssetProcessor" OFF)
option(FAITHFUL_LOGGER_SEVERITY_ONLY_FATAL "include package AssetProcessor" OFF)

option(BUILD_TESTS "If enabled, compile the tests." OFF)
option(BUILD_TESTS_UTILS "test utility packages" OFF)
option(BUILD_TESTS_ASSETS "" OFF)
option(BUILD_BENCHMARKS "If enabled, compile the benchmarks." OFF)

function(printopt optName optVal)
    if(${optVal})
        message(STATUS "  ${optName}  - ON")
    else()
        message(STATUS "  ${optName}  - OFF")
    endif()
endfunction()

printopt("AssetProcessor     " ${BUILD_ASSET_PROCESSOR})
printopt("Download assets    " ${INIT_ASSETS})
printopt("Build tests        " ${BUILD_TESTS})
printopt("Build util tests   " ${BUILD_TESTS_UTILS})
printopt("Build assets tests " ${BUILD_TESTS_ASSETS})
printopt("build benchmarks   " ${BUILD_BENCHMARKS})

printopt("log buffering      " ${FAITHFUL_LOGGER_BUFFERING})
if (FAITHFUL_LOGGER_SEVERITY_ONLY_FATAL)
    printopt("logger: only fatal " ${FAITHFUL_LOGGER_SEVERITY_ONLY_FATAL})
elseif(FAITHFUL_LOGGER_SEVERITY_NO_INFO)
    printopt("logger: no info    " ${FAITHFUL_LOGGER_SEVERITY_NO_INFO})
else()
    message(STATUS "  logger: all          - ON")
endif()

option(FAITHFUL_LOGGER_BUFFERING "include package AssetProcessor" OFF)
option(FAITHFUL_LOGGER_SEVERITY_ALL "include package AssetProcessor" ON)
option(FAITHFUL_LOGGER_SEVERITY_NO_INFO "include package AssetProcessor" OFF)
option(FAITHFUL_LOGGER_SEVERITY_ONLY_FATAL "include package AssetProcessor" OFF)


# TODO: add_subdirectory(external EXCLUDE_FROM_ALL)
add_subdirectory(external/assimp EXCLUDE_FROM_ALL)
add_subdirectory(external/yasm EXCLUDE_FROM_ALL)
add_subdirectory(external/libav EXCLUDE_FROM_ALL)

# TODO: add_subdirectory(src)

# TODO: faithful-core
# TODO: faithful-util

if (BUILD_TESTS)
    file(GLOB_RECURSE faithful-test-src-sources RELATIVE ${CMAKE_SOURCE_DIR}/test/test_src/*.cpp)
    add_executable(faithful-test-src ${faithful-test-src-sources})
    target_link_libraries(faithful-test-src faithful-core)

    if (BUILD_TESTS_ASSETS)
        file(GLOB_RECURSE faithful-test-assets RELATIVE ${CMAKE_SOURCE_DIR}/test/test_assets/*.cpp)
        add_executable(faithful-test-assets ${faithful-test-assets-sources})
        target_link_libraries(faithful-test-assets faithful-core)
    endif()

    if(BUILD_TESTS_UTILS)
        file(GLOB_RECURSE faithful-test-utils RELATIVE ${CMAKE_SOURCE_DIR}/test/test_utils/*.cpp)
        add_executable(faithful-test-utils ${faithful-test-utils-sources})
        target_link_libraries(faithful-test-utils faithful-util)
endif()
endif()


# TODO: install() --> should looks attractive
include(${CMAKE_SOURCE_DIR}/cmake/CopyLicenses.cmake)

add_executable(Faithful main.cpp)
target_link_libraries(Faithful PRIVATE assimp)
add_dependencies(libav yasm)
#add_yasm_to_target(Faithful)
target_link_libraries(Faithful INTERFACE yasm)
target_link_libraries(Faithful PRIVATE libav)

message(STATUS "Configuration complete:")
message(STATUS "   Include directories: ${MY_PROJECT_INCLUDE_DIRS}")
message(STATUS "   Source directory: ${MY_PROJECT_SOURCE_DIR}")
message(STATUS "   Binary directory: ${MY_PROJECT_BINARY_DIR}")
