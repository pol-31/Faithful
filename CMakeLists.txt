cmake_minimum_required(VERSION 3.26)

project(Faithful LANGUAGES CXX C)

add_library(Faithful INTERFACE)

cmake_policy(SET CMP0069 NEW)  # LTO support
cmake_policy(SET CMP0091 NEW)  # MSVC runtime support
cmake_policy(SET CMP0075 NEW)  # honor to CMAKE_REQUIRED_LIBRARIES

set(FAITHFUL_FREEZE_DIR "${CMAKE_SOURCE_DIR}/Faithful" CACHE STRING
        "The subdirectory where to install")

# TODO: build/source/runtime dirs

set(CMAKE_MODULE_PATH
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
        ${CMAKE_MODULE_PATH}
)

# For FREEZE_BUILD there's 2 stages of compilation:
# 1) Freeze - all external libs, compiling of AssetProcessor, AssetDownloader
#      - only once (Release mode). It works like installing:
#      all static/dynamic libraries, licenses, assets
#      simply copied to some folder (FAITHFUL_FREEZE_DIR).
# 2) Then you don't have any ... but TODO: idk how to call it
# TODO: commits only to certain Git Work-tree

option(FAITHFUL_FREEZE_BUILD
        "Build & compile external libs, AssetProcessor, AssetDownloader only once (RELEASE)
        (further build/compiling depends solely on Faithful code)" ON
)

option(FAITHFUL_BUILD_ASSET_PROCESSOR "Build Faithful::AssetProcessor" ON)
option(FAITHFUL_BUILD_ASSET_DOWNLOADER "Faithful download/update assets" ON)

# all externals won't be installed
option(FAITHFUL_BUILD_EXTERNAL "Build Faithful external libs if missing" ON)
option(FAITHFUL_BUILD_EXTERNAL_ALL "Build all(!) Faithful external libs" OFF)
option(FAITHFUL_BUILD_TESTS "If enabled, compile Faithful tests" OFF)
option(FAITHFUL_ENABLE_CLANG_TIDY "Enable Faithful clang-tidy check" OFF)

# Only for assets designer, used for generating asset_info.txt with validate info
# including amount of assets and its hash for further
# uploading to (e.g.) Google Drive and replacing of asset_info.txt in
# official Faithful github repository.
option(FAITHFUL_BUILD_ASSET_PACK "Build Faithful::AssetPack" ON)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(${CMAKE_SOURCE_DIR}/cmake/PrintInfo.cmake)
print_system_and_cmake_info()

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_definitions(-DFAITHFUL_DEBUG_BUILD)
endif()

include(cmake/ConfigAssetsPaths.cmake)
config_asset_paths()

configure_file(${CMAKE_SOURCE_DIR}/config/paths.h.in ${CMAKE_SOURCE_DIR}/config/Paths.h)

set(FAITHFUL_CONFIG_PATH "${CMAKE_SOURCE_DIR}/config")
configure_file(${CMAKE_SOURCE_DIR}/config/paths.h.in ${CMAKE_SOURCE_DIR}/config/Paths.h)

# There also could be such definitions:
#   supporting specific OpenGL extensions (like KHR_texture_compression_astc_ldr);
#   OpenGL OS-specific configs (CORE_PROFILE for MacOS, etc...);
#   x86_64 / AMD64;
#   SSE, AVX, PCLMUL.
# But there are not benefits of it in CMakeLists.txt.
# Such checks and accelerations implemented directly in source code


# currently Clang is __REQUIRED__
include(${CMAKE_SOURCE_DIR}/cmake/CompilerClang.cmake)

#if(MSVC)
#    include(${CMAKE_SOURCE_DIR}/cmake/CompilerMSVC.cmake)
#else()
#    include(${CMAKE_SOURCE_DIR}/cmake/CompilerUNIX.cmake)
#endif()

print_compiler_info()

print_dir_info()

print_build_options()

include(${CMAKE_SOURCE_DIR}/cmake/FaithfulFunctions.cmake)
include(cmake/FindDeps.cmake)

set(FETCHCONTENT_FULLY_DISCONNECTED ON)

if(FAITHFUL_BUILD_EXTERNAL)
    add_subdirectory(external EXCLUDE_FROM_ALL)
# else()
#     in that case if we can't find external lib - building fails.
endif()

if(FAITHFUL_DEPS_FOUND)
    add_subdirectory(src)
    target_link_libraries(Faithful INTERFACE src)

    add_subdirectory(utils)
    target_link_libraries(Faithful INTERFACE utils)

    message(STATUS "+> Building of Faithful is completed")
    message("")

    if (FAITHFUL_BUILD_TESTS)
        add_subdirectory(test EXCLUDE_FROM_ALL)
    endif()

    if (FAITHFUL_ENABLE_CLANG_TIDY)
        include(${CMAKE_SOURCE_DIR}/cmake/ConfigClangTidy.cmake)
    else ()
        set(CMAKE_CXX_CLANG_TIDY "" CACHE INTERNAL "")
    endif ()

    if(FAITHFUL_FREEZE_BUILD)
        include (${CMAKE_SOURCE_DIR}/cmake/FreezeBuild.cmake)
        message(STATUS "+> Faithful has been frozen")
    endif()

    include(${CMAKE_SOURCE_DIR}/cmake/CopyLicenses.cmake)
    faithful_copy_licenses()

else()
    message(STATUS "+> Building of Faithful has failed: some externals missing") # TODO: need certain listing of them
    message("")
endif()
