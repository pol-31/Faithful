cmake_minimum_required(VERSION 3.26)

# TODO: if FREEZE_BUILD - RELEASE mode

include(ExternalProject)
include(CheckCXXSourceRuns)

#    ====================================================================
#    FAITHFUL-specific

# glad should be built/compiled first to find out FAITHFUL_GL_SUPPORTS_ASTC

#    --------------------------------------------------------------------
#    glad (Faithful: OpenGL specification)
message(STATUS "glad building")
add_library(glad ${CMAKE_SOURCE_DIR}/external/glad/src/glad.c)
set(FAITHFUL_GLAD_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external/glad/include CACHE INTERNAL "")
target_include_directories(glad PRIVATE ${FAITHFUL_GLAD_INCLUDE_DIR})

# if FAITHFUL_GL_SUPPORTS_ASTC, OpenGL has embedded support of astc textures,
# so we don't need astc-decoder (astc-encoder depends on AssetProcessor)

set(CMAKE_REQUIRED_INCLUDES ${GLAD_INCLUDE_DIR})

check_cxx_source_runs("
    #include \"glad/glad.h\"
    #include <iostream>
    int main() {
      #ifdef glCompressedTexImage2D
      return 0;
      #else
      return 1;
      #endif
    }" FAITHFUL_GL_SUPPORTS_ASTC)

if (FAITHFUL_GL_SUPPORTS_ASTC)
    message(STATUS "FAITHFUL_GL_SUPPORTS_ASTC --- supported")
else ()
    message(STATUS "FAITHFUL_GL_SUPPORTS_ASTC --- not supported")
endif ()

unset(CMAKE_REQUIRED_INCLUDES)


#    --------------------------------------------------------------------
#    Astc-encoder: Faithful (runtime decoding only
#      - all textures already compressed to .astc)
#    AssetProcessor - texture conversion to .astc (+encode, +decode)

if (FAITHFUL_BUILD_EXTERNAL OR FAITHFUL_BUILD_EXTERNAL_ALL)
    if (FAITHFUL_BUILD_ASSET_PROCESSOR OR NOT FAITHFUL_GL_SUPPORTS_ASTC)
        set(FAITHFUL_BUILD_ASTC_ENCODER ON CACHE BOOL "" FORCE)
    endif ()
endif ()
if (FAITHFUL_FALSE AND FAITHFUL_BUILD_ASTC_ENCODER)
    if (NOT FAITHFUL_GL_SUPPORTS_ASTC AND NOT FAITHFUL_BUILD_ASSET_PROCESSOR)
        set(FAITHFUL_ASTC_ENCODER_DECOMPRESSOR ON)
    else ()
        set(FAITHFUL_ASTC_ENCODER_DECOMPRESSOR OFF)
    endif ()

    message(STATUS "astc-encoder building")
    set(ASTC_ENCODER_SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/astc-encoder CACHE INTERNAL "astc-encoder source directory" FORCE)
    set(ASTC_ENCODER_BINARY_DIR ${CMAKE_BINARY_DIR}/external/astc-encoder CACHE INTERNAL "astc-encoder binary directory" FORCE)

    ExternalProject_Add(
            astcencoder_external
            PREFIX ${ASTC_ENCODER_BINARY_DIR}
            SOURCE_DIR ${ASTC_ENCODER_SOURCE_DIR}
            CONFIGURE_COMMAND ${CMAKE_COMMAND}
            -DASTCENC_ISA_NATIVE=ON
            -DASTCENC_DECOMPRESSOR=${FAITHFUL_ASTC_ENCODER_DECOMPRESSOR}
            -DASTCENC_SHAREDLIB=OFF
            -DASTCENC_DIAGNOSTICS=OFF
            -DASTCENC_ASAN=OFF
            -DASTCENC_UNITTEST=OFF
            -DASTCENC_INVARIANCE=OFF
            -DASTCENC_CLI=OFF
            -DASTCENC_BLOCK_MAX_TEXELS=64 # max 8x8x1 compression)
            ${ASTC_ENCODER_SOURCE_DIR}
            BUILD_COMMAND ${CMAKE_COMMAND} --build .
            INSTALL_COMMAND ${CMAKE_COMMAND} -E echo "Skipping install step"
            BUILD_BYPRODUCTS ${ASTC_ENCODER_BINARY_DIR}/src/astcencoder_external-build/Source/Debug/astcenc-native-static${CMAKE_STATIC_LIBRARY_SUFFIX}
            #BUILD_BYPRODUCTS ${ASTC_ENCODER_BINARY_DIR}/src/astcencoder_external-build/Source/libastcenc-native-static${CMAKE_STATIC_LIBRARY_SUFFIX} <------ TODO: its for UNIX
            LOG_CONFIGURE ON
            LOG_BUILD ON
    )

    add_library(astc-encoder INTERFACE)
    add_dependencies(astc-encoder astcencoder_external)
    target_link_libraries(astc-encoder
            INTERFACE ${ASTC_ENCODER_BINARY_DIR}/src/astcencoder_external-build/Source/Debug/astcenc-native-static${CMAKE_STATIC_LIBRARY_SUFFIX}
    )
    target_include_directories(astc-encoder INTERFACE ${ASTC_ENCODER_SOURCE_DIR}/Source)
endif ()
if (FAITHFUL_BUILD_ASTC_ENCODER)
    if (NOT FAITHFUL_GL_SUPPORTS_ASTC AND NOT FAITHFUL_BUILD_ASSET_PROCESSOR)
        set(FAITHFUL_ASTC_ENCODER_DECOMPRESSOR ON)
    else ()
        set(FAITHFUL_ASTC_ENCODER_DECOMPRESSOR OFF)
    endif ()
    message(STATUS "astc-encoder building")
    set(ISA_NATIVE ON CACHE INTERNAL "")
    set(DECOMPRESSOR ${FAITHFUL_ASTC_ENCODER_DECOMPRESSOR} CACHE INTERNAL "")
    #set(ASTCENC_SHAREDLIB ON CACHE INTERNAL "")
    set(DIAGNOSTICS OFF CACHE INTERNAL "")
    set(ASTCENC_ASAN OFF CACHE INTERNAL "")
    set(UNITTEST OFF CACHE INTERNAL "")
    set(ASTCENC_INVARIANCE OFF CACHE INTERNAL "")
    set(CLI OFF CACHE INTERNAL "")
   # set(ASTCENC_BLOCK_MAX_TEXELS 64 CACHE INTERNAL "")

    add_subdirectory(astc-encoder EXCLUDE_FROM_ALL)
endif ()

#    --------------------------------------------------------------------
#    dr_libs (AssetProcessor only: decompress mp3, flac, wav into PCM
#             for further compression into ogg+Vorbis)
message(STATUS "dr_libs building")
configure_file(dr_libs/dr_wav.h dr_wav.cpp COPYONLY)
configure_file(dr_libs/dr_flac.h dr_flac.cpp COPYONLY)
configure_file(dr_libs/dr_mp3.h dr_mp3.cpp COPYONLY)
add_library(dr_libs dr_wav.cpp dr_flac.cpp dr_mp3.cpp)
target_compile_definitions(dr_libs PRIVATE DR_WAV_IMPLEMENTATION DR_FLAC_IMPLEMENTATION DR_MP3_IMPLEMENTATION)
target_include_directories(dr_libs PRIVATE ${CMAKE_SOURCE_DIR}/external/dr_libs)


#    --------------------------------------------------------------------
#    glfw (Faithful: Window/context creation)
if (FAITHFUL_BUILD_EXTERNAL_ALL OR
      (FAITHFUL_BUILD_EXTERNAL AND NOT GLFW_FOUND))
    message(STATUS "glfw building")
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build glfw shared libraries" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build glfw example programs" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build glfw tests" FORCE)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build glfw documentation" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "Install glfw" FORCE)
    add_subdirectory(glfw EXCLUDE_FROM_ALL)
endif()


#    --------------------------------------------------------------------
#    glm (Faithful: math)
if (FAITHFUL_BUILD_EXTERNAL_ALL OR
      (FAITHFUL_BUILD_EXTERNAL AND NOT GLM_FOUND))
    message(STATUS "glm building")
    add_subdirectory(glm EXCLUDE_FROM_ALL)
endif()


#    --------------------------------------------------------------------
#    googletest (all: testing)
if (FAITHFUL_BUILD_TESTS)
    if (FAITHFUL_BUILD_EXTERNAL_ALL OR
          (FAITHFUL_BUILD_EXTERNAL AND NOT GTEST_FOUND))
        message(STATUS "googletest building")
        set(BUILD_GMOCK OFF CACHE BOOL "Build GoogleTest GMock" FORCE)
        set(INSTALL_GTEST OFF CACHE BOOL "Install GoogleTest" FORCE)
        set(GTEST_BINARY_DIR ${CMAKE_BINARY_DIR}/external/googletest CACHE INTERNAL "googletest binary directory" FORCE)
        add_subdirectory(googletest EXCLUDE_FROM_ALL)
    endif ()
endif ()


#    --------------------------------------------------------------------
#    libogg (Faithful: audio streaming to OpenAL-Soft;
#            AssetProcessor: compression, transrating)
if (FAITHFUL_BUILD_EXTERNAL_ALL OR
      (FAITHFUL_BUILD_EXTERNAL AND NOT LIBOGG_FOUND))
    message(STATUS "libogg building")

    set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build libogg shared library" FORCE)
    set(INSTALL_DOCS OFF CACHE BOOL "Install libogg documentation" FORCE)
    set(INSTALL_PKG_CONFIG_MODULE OFF CACHE BOOL "Install libogg ogg.pc file" FORCE)
    set(INSTALL_CMAKE_PACKAGE_MODULE OFF CACHE BOOL "Install libogg CMake package configuration module" FORCE)
    add_subdirectory(ogg EXCLUDE_FROM_ALL)
endif ()

#    --------------------------------------------------------------------
#    mimalloc (all: general-purpose allocator - overrides malloc/new/etc...)
if (FAITHFUL_BUILD_EXTERNAL_ALL OR
      (FAITHFUL_BUILD_EXTERNAL AND NOT MIMALLOC_FOUND))
    message(STATUS "mimalloc building")
    set(MIMALLOC_SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/mimalloc)
    set(MIMALLOC_BINARY_DIR ${CMAKE_BINARY_DIR}/external/mimalloc CACHE INTERNAL "mimalloc binary directory" FORCE)

    set(FAITHFUL_MIMALLOC_FLAGS "-DMI_BUILD_SHARED=OFF" "-DMI_BUILD_STATIC=ON" "-DMI_BUILD_OBJECT=OFF" "-DMI_BUILD_TESTS=OFF" "-DMI_USE_CXX=ON" CACHE STRING "" FORCE)
    if (WIN32)
        set(FAITHFUL_MIMALLOC_FLAGS "${FAITHFUL_MIMALLOC_FLAGS} -DMI_BUILD_SHARED=OFF" CACHE STRING "" FORCE)
    endif()

    ExternalProject_Add(
            mimalloc_external
            PREFIX ${MIMALLOC_BINARY_DIR}
            SOURCE_DIR ${MIMALLOC_SOURCE_DIR}
            CONFIGURE_COMMAND ${CMAKE_COMMAND}
            ${FAITHFUL_MIMALLOC_FLAGS}
            ${MIMALLOC_SOURCE_DIR}
            BUILD_COMMAND ${CMAKE_COMMAND} --build .
            INSTALL_COMMAND ${CMAKE_COMMAND} -E echo "Skipping install step"
            BUILD_BYPRODUCTS ${MIMALLOC_BINARY_DIR}/src/mimalloc_external-build/Debug/mimalloc-static${CMAKE_STATIC_LIBRARY_SUFFIX} # TODO: + /Debug dir on WIN32
            LOG_CONFIGURE ON
            LOG_BUILD ON
    )
    add_library(mimalloc INTERFACE)
    add_dependencies(mimalloc mimalloc_external)
    target_link_libraries(mimalloc
            INTERFACE ${MIMALLOC_BINARY_DIR}/src/mimalloc_external-build/Debug/mimalloc-static${CMAKE_STATIC_LIBRARY_SUFFIX}
    )
endif()


#    --------------------------------------------------------------------
#    OpenAL-Soft (Faithful: spatial audio streaming)
if (FAITHFUL_BUILD_EXTERNAL_ALL OR
      (FAITHFUL_BUILD_EXTERNAL AND NOT OPENAL_FOUND))
    message(STATUS "openal-soft building")
    set(OPENALSOFT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/openal-soft)
    set(OPENALSOFT_BINARY_DIR ${CMAKE_BINARY_DIR}/external/openal-soft CACHE INTERNAL "openal-soft binary directory" FORCE)

    ExternalProject_Add(
            openal-soft_external
            PREFIX ${OPENALSOFT_BINARY_DIR}
            SOURCE_DIR ${OPENALSOFT_SOURCE_DIR}
            CONFIGURE_COMMAND ${CMAKE_COMMAND}
            -DLIBTYPE=STATIC
            -DALSOFT_DLOPEN=OFF
            -DALSOFT_UTILS=OFF
            -DALSOFT_EXAMPLES=OFF
            -DALSOFT_TESTS=OFF
            -DALSOFT_INSTALL=OFF
            -DALSOFT_INSTALL_CONFIG=OFF
            -DALSOFT_INSTALL_HRTF_DATA=OFF
            -DALSOFT_INSTALL_AMBDEC_PRESETS=OFF
            -DALSOFT_INSTALL_EXAMPLES=OFF
            -DALSOFT_INSTALL_UTILS=OFF
            -DALSOFT_UPDATE_BUILD_VERSION=OFF
            ${OPENALSOFT_SOURCE_DIR}
            BUILD_COMMAND ${CMAKE_COMMAND} --build .
            INSTALL_COMMAND ${CMAKE_COMMAND} -E echo "Skipping install step"
            BUILD_BYPRODUCTS ${OPENALSOFT_BINARY_DIR}/src/openal-soft_external-build/Debug/OpenAL32${CMAKE_STATIC_LIBRARY_SUFFIX}
            LOG_CONFIGURE ON
            LOG_BUILD ON
            #BUILD_BYPRODUCTS ${OPENALSOFT_BINARY_DIR}/src/openal-soft_external-build/libopenal${CMAKE_STATIC_LIBRARY_SUFFIX} <---------------- TODO: it's for Unix
    )
    add_library(openal-soft INTERFACE)
    add_dependencies(openal-soft openal-soft_external)
    target_link_libraries(openal-soft INTERFACE ${OPENALSOFT_BINARY_DIR}/src/openal-soft_external-build/libopenal.a)
endif()


#    --------------------------------------------------------------------
#    rapidjson (Faithful: json-parser for tinygltf
#               AssetProcessor: safely extract embedded textures for further
#                               .astc copmression)

if (FAITHFUL_BUILD_EXTERNAL_ALL OR
      (FAITHFUL_BUILD_EXTERNAL AND NOT RAPIDJSON_FOUND))
    message(STATUS "rapidjson building")
    set(RAPIDJSON_BUILD_DOC OFF CACHE BOOL "Build rapidjson documentation." FORCE)
    set(RAPIDJSON_BUILD_EXAMPLES OFF CACHE BOOL "Build rapidjson examples." FORCE)
    set(RAPIDJSON_BUILD_TESTS OFF CACHE BOOL "Build rapidjson perftests and unittests." FORCE)
    set(RAPIDJSON_BUILD_THIRDPARTY_GTEST OFF CACHE BOOL
            "Use rapidjson gtest installation in `thirdparty/gtest` by default if available" FORCE)

    set(RAPIDJSON_BUILD_CXX11 OFF CACHE BOOL "Build rapidjson with C++11" FORCE)
    set(RAPIDJSON_BUILD_CXX17 ON CACHE BOOL "Build rapidjson with C++17" FORCE)
    set(RAPIDJSON_ENABLE_INSTRUMENTATION_OPT ON CACHE BOOL "Build rapidjson with -march or -mcpu options" FORCE)

    add_subdirectory(rapidjson EXCLUDE_FROM_ALL)
endif()


#    --------------------------------------------------------------------
#    stb_image, stb_image_write (Faithful: texture loading for further
#                                          compression into .astc)
message(STATUS "stb_image, stb_image_write building")
configure_file(stb/stb_image.h stb_image.cpp COPYONLY)
configure_file(stb/stb_image_write.h stb_image_write.cpp COPYONLY)
add_library(stb stb_image.cpp stb_image_write.cpp)
target_compile_definitions(stb PRIVATE
        STB_IMAGE_IMPLEMENTATION
        STB_IMAGE_WRITE_IMPLEMENTATION
)
target_include_directories(stb PRIVATE ${CMAKE_SOURCE_DIR}/external/stb)


#    --------------------------------------------------------------------
#    tinygltf (Faithful: process/draw .gltf model)
message(STATUS "tinygltf building")
configure_file(tinygltf/tiny_gltf.h tiny_gltf.cpp COPYONLY)
add_library(tinygltf tiny_gltf.cpp)
target_compile_definitions(tinygltf PUBLIC TINYGLTF_IMPLEMENTATION)
target_compile_definitions(tinygltf PUBLIC
        TINYGLTF_NOEXCEPTION
        JSON_NOEXCEPTION
        TINYGLTF_NO_EXTERNAL_IMAGE
        TINYGLTF_NO_STB_IMAGE
        TINYGLTF_NO_STB_IMAGE_WRITE
        TINYGLTF_USE_RAPIDJSON
        TINYGLTF_USE_CPP14
)
target_include_directories(tinygltf
        PRIVATE ${CMAKE_SOURCE_DIR}/external/rapidjson/include/rapidjson
        tinygltf PRIVATE ${CMAKE_SOURCE_DIR}/external/tinygltf
)

#    --------------------------------------------------------------------
#    vorbis (Faithful: audio streaming to OpenAL-Soft;
#            AssetProcessor: compression, transrating)

if (FAITHFUL_BUILD_EXTERNAL_ALL OR
      (FAITHFUL_BUILD_EXTERNAL AND NOT VORBIS_FOUND))
    message(STATUS "vorbis building")
    set(BUILD_SHARED_LIB OFF CACHE BOOL "Build shared library" FORCE)
    set(INSTALL_CMAKE_PACKAGE_MODULE OFF CACHE BOOL "Install CMake package configuration module" FORCE)
    add_subdirectory(vorbis EXCLUDE_FROM_ALL)
endif ()


#    ====================================================================
#    FAITHFUL_ASSET_PROCESSOR-specific

if (FAITHFUL_BUILD_ASSET_PROCESSOR)
    #    --------------------------------------------------------------------
    #    meshoptimizer (AssetProcessor: gltfpack - gltf optimizatio
    add_subdirectory(meshoptimizer EXCLUDE_FROM_ALL)
endif ()


#    ====================================================================
#    FAITHFUL_ASSET_DOWNLOADER-specific

if (FAITHFUL_BUILD_ASSET_DOWNLOADER OR FAITHFUL_BUILD_ASSET_PACK)
    #    --------------------------------------------------------------------
    #    miniz (AssetDownloader, AssetPack: unzip downloaded by curl single file)
    message(STATUS "miniz building")
    add_library(miniz ${CMAKE_SOURCE_DIR}/external/miniz/miniz.c)
    target_compile_definitions(miniz PUBLIC
            MINIZ_NO_ZLIB_APIS
            MINIZ_NO_TIME
    )
    target_include_directories(miniz PRIVATE ${CMAKE_SOURCE_DIR}/external/miniz)

    #    --------------------------------------------------------------------
    #    CURL (AssetDownloader only: single http request to download assets)
    #    see utils/AssetDownloader/CMakeLists.txt

    #    --------------------------------------------------------------------
    #    PicoSHA2 (AssetDownloader, AssetPack: validate assets downloaded by curl)
    #    only one header, so included directly
endif ()
