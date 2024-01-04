cmake_minimum_required(VERSION 3.26)

project(Faithful LANGUAGES CXX C)

# TODO: only stable external lib versions to submodule (imp!)

cmake_policy(SET CMP0069 NEW)  # LTO support
cmake_policy(SET CMP0091 NEW)  # MSVC runtime support

# There is two stages of building & compiling:
#   1) build and compile all external libs + delete their "raw code"
#      so now we have only "our native" source code and .a files (or .so)
#      this building and compilation should happen only once
#   2) for compiling of user code - building & compiling SOLELY user's code
#      this building can be run all you want

# TODO: for first step of compiling use forse RELEASE


message(STATUS "._._._._._._<Faithful>_._._._._._.")
message(STATUS "+> Compilation has been started")
message(STATUS "+> CMake Version: ${CMAKE_VERSION}")
message(STATUS "+> CMake Generator: ${CMAKE_GENERATOR}")
message(STATUS "+> System: ${CMAKE_SYSTEM}")
message(STATUS "+> Build type: ${CMAKE_BUILD_TYPE}")
message("")

option(FAITHFUL_INSTALL "Install Faithful ()" ON)
set(FAITHFUL_INSTALL_DIR "${CMAKE_SOURCE_DIR}/Faithful" CACHE STRING
        "The subdirectory where to install")

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

set(FAITHFUL_CONFIG_PATH "${CMAKE_SOURCE_DIR}/config")


set(FAITHFUL_ASSET_PATH "${CMAKE_SOURCE_DIR}/assets")
set(FAITHFUL_ASSET_AUDIO_PATH "${CMAKE_SOURCE_DIR}/assets/audio")
set(FAITHFUL_ASSET_MODEL_PATH "${CMAKE_SOURCE_DIR}/assets/models")
set(FAITHFUL_ASSET_SHADER_PATH "${CMAKE_SOURCE_DIR}/assets/shaders")
set(FAITHFUL_ASSET_TEXTURE_PATH "${CMAKE_SOURCE_DIR}/assets/textures")
set(FAITHFUL_ASSET_TEMP_TEXTURES_PATH "${CMAKE_SOURCE_DIR}/utils/AssetProcessor/temp")

set(FAITHFUL_ASSET_INFO_FILE "${FAITHFUL_ASSET_PATH}/assets_info.txt")
set(FAITHFUL_ASSET_AUDIO_INFO_FILE "${FAITHFUL_ASSET_AUDIO_PATH}/audio_info.txt")
set(FAITHFUL_ASSET_MODEL_INFO_FILE "${FAITHFUL_ASSET_MODEL_PATH}/models_info.txt")
set(FAITHFUL_ASSET_SHADER_INFO_FILE "${FAITHFUL_ASSET_SHADER_PATH}/shaders_info.txt")
set(FAITHFUL_ASSET_TEXTURE_INFO_FILE "${FAITHFUL_ASSET_TEXTURE_PATH}/textures_info.txt")


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

if(MSVC)
    include(${CMAKE_SOURCE_DIR}/cmake/CompilerMSVC.cmake)
else()
    include(${CMAKE_SOURCE_DIR}/cmake/CompilerUNIX.cmake)
endif()

option(FAITHFUL_BUILD_EXTERNAL "Build external libs if missing (they won't be installed)" ON)

option(BUILD_ASSET_PROCESSOR "include package AssetProcessor" OFF)
option(INIT_ASSETS "download assets from GoogleDrive" ON)

message(STATUS "+> C Compiler: ${CMAKE_C_COMPILER} ${CMAKE_C_COMPILER_VERSION}")
message(STATUS "+> C Config: ${CMAKE_C_STANDARD}")
message(STATUS "+> CMAKE_C_FLAGS: ${CMAKE_C_FLAGS}")
message(STATUS "+> CXX Compiler: ${CMAKE_CXX_COMPILER} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "+> CXX Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "+> CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message("")

message(STATUS "+> Source directory: ${CMAKE_SOURCE_DIR}")
message(STATUS "+> Binary directory: ${CMAKE_BINARY_DIR}")
message(STATUS "+> Install directory: ${FAITHFUL_INSTALL_DIR}")
message("")

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
        message(STATUS "+> ${optName}  - ON")
    else()
        message(STATUS "+> ${optName}  - OFF")
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
    message(STATUS "+> logger: all          - ON")
endif()

message("")

option(FAITHFUL_LOGGER_BUFFERING "include package AssetProcessor" OFF)
option(FAITHFUL_LOGGER_SEVERITY_ALL "include package AssetProcessor" ON)
option(FAITHFUL_LOGGER_SEVERITY_NO_INFO "include package AssetProcessor" OFF)
option(FAITHFUL_LOGGER_SEVERITY_ONLY_FATAL "include package AssetProcessor" OFF)
# TODO: more options to print


# IMPORTANT: all external libs are built with very specific
#   build options, so installation for each library is disabled

# todo: there may occur situation where some external lib was built but
#       without needed configurations (example: Assimp doesn't support needed format)




include(${CMAKE_SOURCE_DIR}/cmake/FaithfulFunctions.cmake)

if(FAITHFUL_BUILD_EXTERNAL)
    #faithful_build_external()

    if (BUILD_ASSET_PROCESSOR)
        #add_subdirectory(${CMAKE_SOURCE_DIR}/utils/AssetProcessor)
    endif()

    if (INIT_ASSETS) # TODO: rename (?idk)
        #add_subdirectory(${CMAKE_SOURCE_DIR}/utils/AssetDownloader)
    endif()
else()
    # in that case if we can't find external lib - building fails.

    if (BUILD_ASSET_PROCESSOR)
        include(${CMAKE_SOURCE_DIR}/cmake/FindDepsAssetProcessor.cmake)
    endif()

    if (INIT_ASSETS)
        include(${CMAKE_SOURCE_DIR}/cmake/FindDepsAssetDownloader.cmake)
    endif()
endif()


if(FAITHFUL_DEPS_FOUND)

    # TODO: find_path(ALSA_INCLUDE_DIR NAMES asoundlib.h HINTS /home/pavlo/Downloads/openal_another_version/alsa/alsa/include)
    # TODO: find_library(ALSA_LIBRARY NAMES libasound.so HINTS /home/pavlo/Downloads/openal_another_version/alsa/alsa/src/.libs)
    #message(${PULSEAUDIO_INCLUDE_DIR})
    #message(${PULSEAUDIO_LIBRARY})

    add_subdirectory(external/astc-encoder)
    add_subdirectory(external/dr_libs)
    add_subdirectory(external/libogg)
    add_subdirectory(external/vorbis)
    add_subdirectory(external/rapidjson)
    add_subdirectory(external/tinygltf)

    add_dependencies(vorbis libogg)

    add_subdirectory(utils/AssetProcessor)

    add_executable(Faithful main.cpp)


    target_include_directories(Faithful
            PUBLIC /home/pavlo/CLionProjects/Faithful/external/astc-encoder/astc-encoder/Source
            PUBLIC /home/pavlo/CLionProjects/Faithful/external/dr_libs/dr_libs
            PUBLIC /home/pavlo/CLionProjects/Faithful/cmake-build-debug/src/libogg_external-build/include/ogg
            PUBLIC /home/pavlo/CLionProjects/Faithful/external/vorbis/vorbis/include
            PUBLIC /home/pavlo/CLionProjects/Faithful/external/libogg/libogg/include
            PUBLIC /home/pavlo/CLionProjects/Faithful/external/rapidjson/rapidjson/include/rapidjson
            PUBLIC /home/pavlo/CLionProjects/Faithful/external/tinygltf/tinygltf
    )
    target_link_libraries(Faithful
            PUBLIC astc-encoder
            PUBLIC dr_libs
            PUBLIC libogg
            PUBLIC vorbis
            PUBLIC RapidJSON
            PUBLIC tinygltf
    )

#    add_subdirectory(src)

#    target_link_libraries(Faithful INTERFACE faithful-core)

#    TARGET Faithful POST_BUILD

#    add_custom_target(AssetProcessorRun
#            COMMAND $<TARGET_FILE:AssetProcessor> "/home/pavlo/Desktop/test"
#            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
#            COMMENT "Running AssetProcessor..."
#            DEPENDS AssetProcessor external/rapidjson tinygltf
#    )
#    add_dependencies(Faithful AssetProcessorRun)


    # ASSET_DOWNLOADER
    #####################
#
#    include(ExternalProject)
#    ExternalProject_Add(
#            AssetDownloader
#            PREFIX ${CMAKE_BINARY_DIR}
#            SOURCE_DIR ${CMAKE_SOURCE_DIR}/utils/AssetDownloader
#            BINARY_DIR ${CMAKE_BINARY_DIR}/temp/AssetDownloader
#            CONFIGURE_COMMAND ${CMAKE_COMMAND}
#            ${CMAKE_SOURCE_DIR}/utils/AssetDownloader
#            BUILD_COMMAND ${CMAKE_COMMAND} --build .
#            INSTALL_COMMAND ${CMAKE_COMMAND} -E echo "Skipping install step"
#            BUILD_BYPRODUCTS ${CMAKE_BINARY_DIR}/temp/AssetDownloader/AssetDownloader
#    )
#    add_custom_target(RunSubproject
#            COMMAND ${CMAKE_BINARY_DIR}/temp/AssetDownloader/AssetDownloader
#            "${FAITHFUL_ASSET_LOG_PATH}"
#            "${FAITHFUL_ASSETDOWNLOADER_SOURCE_URL}"
#            COMMENT "Running Subproject before building Main Project"
#            DEPENDS AssetDownloader
#    )
#    add_dependencies(Faithful RunSubproject)
    #####################



    # OpenAL
    #####################
    add_subdirectory(${CMAKE_SOURCE_DIR}/external/openal-soft)
    target_include_directories(Faithful INTERFACE external/openal-soft/openal-soft/include)
#    add_dependencies(OpenAL alsa)
    target_link_libraries(Faithful INTERFACE OpenAL)
    #####################


    # Now you can link libvpx to your main target

    #   target_include_main_deps(Faithful)
 #   target_link_main_deps(Faithful)

    # TODO: faithful-core,  faithful-util (all headers in Faithful/utils)

    message(STATUS "+> Building of Faithful is completed")
    message("")

    if (BUILD_TESTS)

        # TODO: external_find_or_compile(googletest GoogleTest)

        file(GLOB_RECURSE faithful-test-src-sources RELATIVE ${CMAKE_SOURCE_DIR}/test/test_src/*.cpp)
        add_executable(faithful-test-src ${faithful-test-src-sources})
        target_link_libraries(faithful-test-src faithful-core)
        # TODO: message(STATUS success/failure)

        if (BUILD_TESTS_ASSETS)
            file(GLOB_RECURSE faithful-test-assets RELATIVE ${CMAKE_SOURCE_DIR}/test/test_assets/*.cpp)
            add_executable(faithful-test-assets ${faithful-test-assets-sources})
            target_link_libraries(faithful-test-assets faithful-core)
            # TODO: message(STATUS success/failure)
        endif()

        if(BUILD_TESTS_UTILS)
            file(GLOB_RECURSE faithful-test-utils RELATIVE ${CMAKE_SOURCE_DIR}/test/test_utils/*.cpp)
            add_executable(faithful-test-utils ${faithful-test-utils-sources})
            target_link_libraries(faithful-test-utils faithful-util)
            # TODO: message(STATUS success/failure)
        endif()
    endif()

    if(FAITHFUL_INSTALL)
        # TODO: install() --> should looks attractive
        include(${CMAKE_SOURCE_DIR}/cmake/CopyLicenses.cmake)
        message(STATUS "+> Faithful has been installed")
    endif()
else()
    message(STATUS "+> Building of Faithful has failed")
    message("")
endif()
