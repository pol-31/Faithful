cmake_minimum_required(VERSION 3.26)

project(Faithful LANGUAGES CXX C)

cmake_policy(SET CMP0069 NEW)  # LTO support
cmake_policy(SET CMP0091 NEW)  # MSVC runtime support

set(FAITHFUL_FREEZE_DIR "${CMAKE_SOURCE_DIR}/Faithful" CACHE STRING
        "The subdirectory where to install")

set(CMAKE_MODULE_PATH
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
        ${CMAKE_MODULE_PATH}
)

# For FREEZE_BUILD there's 2 stages of compilation:
# 1) Freeze - all external libs, assets downloading by AssetDownloader,
#      compiling of AssetProcessor - only once (Release mode). It works
#      like installing - all static/dynamic libraries, licenses, assets
#      simply copied to some folder (FAITHFUL_FREEZE_DIR).
# 2) Then you don't have any ... but TODO: idk how to call it
# TODO: commits only to certain Git Work-tree

option(FAITHFUL_FREEZE_BUILD
        "Build & compile external libs and download assets only once (RELEASE)
        (further build/compiling depends solely on Faithful code)" ON
)

option(FAITHFUL_BUILD_ASSET_PROCESSOR "Build Faithful::AssetProcessor" ON)
option(FAITHFUL_DOWNLOAD_ASSETS "Faithful download assets from GoogleDrive" ON)
option(FAITHFUL_BUILD_EXTERNAL "Build Faithful external libs if missing (they won't be installed)" ON)
option(FAITHFUL_EMBED_EXTERNAL "Build all(!) Faithful external libs in-place" OFF)
option(FAITHFUL_BUILD_TESTS "If enabled, compile Faithful tests" OFF)

option(FAITHFUL_LOGGER_BUFFERING "Faithful logger buffering" OFF)
# by default FAITHFUL_LOGGER_SEVERITY_ALL always ON
option(FAITHFUL_LOGGER_SEVERITY_NO_INFO "Faithful logger severity: no info (only warnings & errors)" OFF)
option(FAITHFUL_LOGGER_SEVERITY_ONLY_FATAL "Faithful logger severity: only fatal errors" OFF)


set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(cmake/PrintInfo.cmake)
print_system_and_cmake_info()

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_definitions(-DFAITHFUL_DEBUG_BUILD)
endif()

include(cmake/ConfigPaths.cmake)
config_asset_paths()

set(FAITHFUL_ASSETDOWNLOADER_SOURCE_URL
        "https://drive.google.com/uc?export=download&id=1Uuu2nKZG17t-u94f1TpcUo8IWxyHvpFo")
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

if(FAITHFUL_BUILD_EXTERNAL)
    add_subdirectory(external EXCLUDE_FROM_ALL)
else()
    # in that case if we can't find external lib - building fails.
endif()

if(FAITHFUL_DEPS_FOUND)
    add_subdirectory(src)
    target_link_libraries(Faithful INTERFACE faithful-core)

    add_subdirectory(utils)
    target_link_libraries(Faithful INTERFACE faithful-utils)

    message(STATUS "+> Building of Faithful is completed")
    message("")

    if (FAITHFUL_BUILD_TESTS)
        add_subdirectory(test EXCLUDE_FROM_ALL)
    endif()

    # TODO: not installing, but FREEZING
    if(FAITHFUL_FREEZE_BUILD)
        # TODO: (...)
        # include(${CMAKE_SOURCE_DIR}/cmake/CopyLicenses.cmake)
        message(STATUS "+> Faithful has been installed")
    endif()
else()
    message(STATUS "+> Building of Faithful has failed: some externals missing") # TODO: need certain listing of them
    message("")
endif()
