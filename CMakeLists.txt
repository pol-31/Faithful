cmake_minimum_required(VERSION 3.26)

project(Faithful LANGUAGES CXX C)

add_library(Faithful INTERFACE)

cmake_policy(SET CMP0069 NEW)  # LTO support
cmake_policy(SET CMP0091 NEW)  # MSVC runtime support
cmake_policy(SET CMP0075 NEW)  # honor to CMAKE_REQUIRED_LIBRARIES

# to set() variables that are option() in subdirectories (Faithful/external/)
cmake_policy(SET CMP0077 NEW)  # honors normal variables

set(CMAKE_WARN_DEPRECATED OFF)

set(FAITHFUL_SOURCE_DIR "${CMAKE_SOURCE_DIR}")
set(FAITHFUL_BINARY_DIR "${CMAKE_BINARY_DIR}")

# Faithful/installed_libs must be empty - all libs should not be installed and
# variable CMAKE_INSTALL_PREFIX was set to ensure nothing will be installed
# to system dirs (we marked all as EXCLUDE_FROM_ALL but accordingly to CMake
# official documentation it doesn't provide a 100% guarantee)
set(CMAKE_INSTALL_PREFIX "${FAITHFUL_BINARY_DIR}/installed_libs")

set(CMAKE_MODULE_PATH "${FAITHFUL_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(FAITHFUL_RELEASE OFF)
else()
    set(FAITHFUL_RELEASE ON)
endif()

if(FAITHFUL_FREEZE_BUILD)
    set(FAITHFUL_RELEASE ON)
endif()

option(FAITHFUL_BUILD_ASSET_PROCESSOR "Build Faithful::AssetProcessor" ON)
option(FAITHFUL_BUILD_ASSET_DOWNLOADER "Faithful download/update assets" ON)

# all externals won't be installed to system lib/include paths
option(FAITHFUL_BUILD_EXTERNAL "Build Faithful external libs if missing" ON)

# To avoid compatibility issues
option(FAITHFUL_BUILD_EXTERNAL_ALL "Build all(!) Faithful external libs" ON)

option(FAITHFUL_BUILD_TESTS "If enabled, compile Faithful tests" OFF)
option(FAITHFUL_ENABLE_CLANG_TIDY "Enable Faithful clang-tidy check" OFF)

option(FAITHFUL_ASAN "Run Faithful address sanitizer" OFF)
option(FAITHFUL_TSAN "Run Faithful thread sanitizer" OFF)

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

include(${FAITHFUL_SOURCE_DIR}/cmake/PrintInfo.cmake)
print_system_and_cmake_info()

set(FAITHFUL_CONFIG_PATH "${FAITHFUL_SOURCE_DIR}/config")
set(FAITHFUL_ASSET_PATH "${FAITHFUL_SOURCE_DIR}/assets")
set(FAITHFUL_ASSET_INFO_FILE "${FAITHFUL_ASSET_PATH}/assets_info.txt")

configure_file(${FAITHFUL_SOURCE_DIR}/config/paths.h.in ${FAITHFUL_SOURCE_DIR}/config/Paths.h)

set(FAITHFUL_CONFIG_PATH "${FAITHFUL_SOURCE_DIR}/config")
configure_file(${FAITHFUL_SOURCE_DIR}/config/paths.h.in ${FAITHFUL_SOURCE_DIR}/config/Paths.h)

# we don't need to configure rpath of visibility, because its
# not a library and we don't use shared libraries by now

if(MSVC)
    include(${FAITHFUL_SOURCE_DIR}/cmake/CompilerMSVS.cmake)
elseif((CMAKE_CXX_COMPILER_ID STREQUAL "Clang") OR (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
    include(${FAITHFUL_SOURCE_DIR}/cmake/CompilerClangOrGCC.cmake)
else()
    message(FATAL_ERROR "Compiler ${CMAKE_CXX_COMPILER_ID} is no supported")
endif()

print_compiler_info()

print_dir_info()

print_build_options()

include(${FAITHFUL_SOURCE_DIR}/cmake/FaithfulFunctions.cmake)
include(cmake/FindDeps.cmake)

if(FAITHFUL_BUILD_EXTERNAL)
    add_subdirectory(external EXCLUDE_FROM_ALL)
# else()
#     in that case if we can't find external lib - building fails.
endif()

if(FAITHFUL_DEPS_FOUND OR FAITHFUL_BUILD_EXTERNAL OR FAITHFUL_BUILD_EXTERNAL_ALL)
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
        include(${FAITHFUL_SOURCE_DIR}/cmake/ConfigClangTidy.cmake)
    endif ()

    include(${FAITHFUL_SOURCE_DIR}/cmake/CopyExternalLicenses.cmake)
else()
    message(STATUS "+> Building of Faithful has failed: some externals missing")
    print_not_found_externals()
    message(STATUS "+> Set FAITHFUL_BUILD_EXTERNAL or FAITHFUL_BUILD_EXTERNAL_ALL "
            "to build them with Faithful project OR install them on your own."
    )
    message(STATUS "(in this case they won't be installed to system path but "
            "only build to <your_build_directory>/external)")
endif()
