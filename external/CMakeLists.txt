cmake_minimum_required(VERSION 3.26)

include(CheckCXXSourceRuns)

#    ====================================================================
#    FAITHFUL-specific

# glad should be built/compiled first to find out FAITHFUL_GL_SUPPORTS_ASTC

#    --------------------------------------------------------------------
#    glad (Faithful: OpenGL specification)
message(STATUS "---> glad building")
add_library(glad STATIC ${FAITHFUL_SOURCE_DIR}/external/glad/src/glad.c)
set(FAITHFUL_GLAD_INCLUDE_DIR ${FAITHFUL_SOURCE_DIR}/external/glad/include)
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

if(FAITHFUL_GL_SUPPORTS_ASTC)
    message(STATUS "FAITHFUL_GL_SUPPORTS_ASTC --- supported")
else()
    message(STATUS "FAITHFUL_GL_SUPPORTS_ASTC --- not supported")
endif()

unset(CMAKE_REQUIRED_INCLUDES)


#    --------------------------------------------------------------------
#    Astc-encoder: Faithful (runtime decoding only
#      - all textures already compressed to .astc)
#    AssetProcessor - texture conversion to .astc (+encode, +decode)

if(FAITHFUL_BUILD_EXTERNAL OR FAITHFUL_BUILD_EXTERNAL_ALL)
    if(FAITHFUL_BUILD_ASSET_PROCESSOR OR NOT FAITHFUL_GL_SUPPORTS_ASTC)
        set(FAITHFUL_BUILD_ASTC_ENCODER ON CACHE INTERNAL "")
    endif()
endif()
if(FAITHFUL_BUILD_ASTC_ENCODER)
    message(STATUS "---> astc-encoder building")
    set(ASTCENC_ISA_NATIVE ON)
    if(NOT FAITHFUL_GL_SUPPORTS_ASTC AND NOT FAITHFUL_BUILD_ASSET_PROCESSOR)
        set(ASTCENC_DECOMPRESSOR ON)
    endif()
    set(ASTCENC_SHAREDLIB OFF)
    set(ASTCENC_DIAGNOSTICS OFF)
    set(ASTCENC_ASAN OFF)
    set(ASTCENC_UNITTEST OFF)
    set(ASTCENC_INVARIANCE OFF)
    set(ASTCENC_CLI OFF)
    set(ASTCENC_BLOCK_MAX_TEXELS 64)
    add_subdirectory(astc-encoder)
endif()

#    --------------------------------------------------------------------
#    dr_libs (AssetProcessor only: decompress mp3, flac, wav into PCM
#             for further compression into ogg+Vorbis)
message(STATUS "---> dr_libs building")
configure_file(dr_libs/dr_wav.h dr_wav.cpp COPYONLY)
configure_file(dr_libs/dr_flac.h dr_flac.cpp COPYONLY)
configure_file(dr_libs/dr_mp3.h dr_mp3.cpp COPYONLY)
add_library(dr_libs STATIC dr_wav.cpp dr_flac.cpp dr_mp3.cpp)
target_compile_definitions(dr_libs PRIVATE DR_WAV_IMPLEMENTATION DR_FLAC_IMPLEMENTATION DR_MP3_IMPLEMENTATION)
target_include_directories(dr_libs PRIVATE ${FAITHFUL_SOURCE_DIR}/external/dr_libs)


#    --------------------------------------------------------------------
#    glfw (Faithful: Window/context creation)
if(FAITHFUL_BUILD_EXTERNAL_ALL OR
      (FAITHFUL_BUILD_EXTERNAL AND NOT GLFW_FOUND))
    message(STATUS "---> glfw building")
    set(BUILD_SHARED_LIBS OFF)
    set(GLFW_BUILD_EXAMPLES OFF)
    set(GLFW_BUILD_TESTS OFF)
    set(GLFW_BUILD_DOCS OFF)
    set(GLFW_INSTALL OFF)
    add_subdirectory(glfw EXCLUDE_FROM_ALL)
endif()


#    --------------------------------------------------------------------
#    glm (Faithful: math)
if(FAITHFUL_BUILD_EXTERNAL_ALL OR
      (FAITHFUL_BUILD_EXTERNAL AND NOT GLM_FOUND))
    message(STATUS "---> glm building")
    add_subdirectory(glm EXCLUDE_FROM_ALL)
endif()


#    --------------------------------------------------------------------
#    googletest (all: testing)
if(FAITHFUL_BUILD_TESTS)
    if(FAITHFUL_BUILD_EXTERNAL_ALL OR
          (FAITHFUL_BUILD_EXTERNAL AND NOT GTEST_FOUND))
        message(STATUS "---> googletest building")
        set(BUILD_GMOCK OFF)
        set(INSTALL_GTEST OFF)
        add_subdirectory(googletest EXCLUDE_FROM_ALL)
    endif()
endif()


#    --------------------------------------------------------------------
#    libogg (Faithful: audio streaming to OpenAL-Soft;
#            AssetProcessor: compression, transrating)
if(FAITHFUL_BUILD_EXTERNAL_ALL OR
      (FAITHFUL_BUILD_EXTERNAL AND NOT LIBOGG_FOUND))
    message(STATUS "---> libogg building")
    set(BUILD_SHARED_LIBS OFF)
    set(INSTALL_DOCS OFF)
    set(INSTALL_PKG_CONFIG_MODULE OFF)
    set(INSTALL_CMAKE_PACKAGE_MODULE OFF)
    add_subdirectory(ogg EXCLUDE_FROM_ALL)
endif()

#    --------------------------------------------------------------------
#    mimalloc (all: general-purpose allocator - overrides malloc/new/etc...)
if(FAITHFUL_BUILD_EXTERNAL_ALL OR
      (FAITHFUL_BUILD_EXTERNAL AND NOT MIMALLOC_FOUND))
    message(STATUS "---> mimalloc building")
    set(MI_BUILD_SHARED OFF)
    set(MI_BUILD_STATIC ON)
    set(MI_BUILD_OBJECT OFF)
    set(MI_BUILD_TESTS OFF)
    set(MI_USE_CXX ON)
    add_subdirectory(mimalloc EXCLUDE_FROM_ALL)
endif()


#    --------------------------------------------------------------------
#    OpenAL-Soft (Faithful: spatial audio streaming)
if(FAITHFUL_BUILD_EXTERNAL_ALL OR
      (FAITHFUL_BUILD_EXTERNAL AND NOT OPENAL_FOUND))
    message(STATUS "---> openal-soft building")
    set(LIBTYPE "STATIC")
    set(ALSOFT_DLOPEN OFF)
    set(ALSOFT_UTILS OFF)
    set(ALSOFT_EXAMPLES OFF)
    set(ALSOFT_TESTS OFF)
    set(ALSOFT_INSTALL OFF)
    set(ALSOFT_INSTALL_CONFIG OFF)
    set(ALSOFT_INSTALL_HRTF_DATA OFF)
    set(ALSOFT_INSTALL_AMBDEC_PRESETS OFF)
    set(ALSOFT_INSTALL_EXAMPLES OFF)
    set(ALSOFT_INSTALL_UTILS OFF)
    set(ALSOFT_UPDATE_BUILD_VERSION OFF)
    add_subdirectory(openal-soft EXCLUDE_FROM_ALL)
endif()


#    --------------------------------------------------------------------
#    rapidjson (Faithful: json-parser for tinygltf
#               AssetProcessor: safely extract embedded textures for further
#                               .astc copmression)
#    only one header, so included directly

#    --------------------------------------------------------------------
#    stb_image, stb_image_write (Faithful: texture loading for further
#                                          compression into .astc)
message(STATUS "---> stb_image, stb_image_write building")
configure_file(stb/stb_image.h stb_image.cpp COPYONLY)
configure_file(stb/stb_image_write.h stb_image_write.cpp COPYONLY)
add_library(stb STATIC stb_image.cpp stb_image_write.cpp)
target_compile_definitions(stb PUBLIC
        STB_IMAGE_IMPLEMENTATION
        STB_IMAGE_WRITE_IMPLEMENTATION
)
target_include_directories(stb PRIVATE ${FAITHFUL_SOURCE_DIR}/external/stb)


#    --------------------------------------------------------------------
#    tinygltf (Faithful: process/draw .gltf model)
message(STATUS "---> tinygltf building")
configure_file(tinygltf/tiny_gltf.h tiny_gltf.cpp COPYONLY)
add_library(tinygltf STATIC tiny_gltf.cpp)
#[[target_compile_definitions(tinygltf PUBLIC
        TINYGLTF_IMPLEMENTATION
        TINYGLTF_NOEXCEPTION
        JSON_NOEXCEPTION
        TINYGLTF_NO_EXTERNAL_IMAGE
        TINYGLTF_NO_STB_IMAGE
        TINYGLTF_NO_STB_IMAGE_WRITE
        TINYGLTF_USE_RAPIDJSON
        TINYGLTF_USE_CPP14
)]]
target_include_directories(tinygltf
        PRIVATE ${FAITHFUL_SOURCE_DIR}/external/rapidjson
        PRIVATE ${FAITHFUL_SOURCE_DIR}/external/tinygltf
)

#    --------------------------------------------------------------------
#    vorbis (Faithful: audio streaming to OpenAL-Soft;
#            AssetProcessor: compression, transrating)

if(FAITHFUL_BUILD_EXTERNAL_ALL OR
      (FAITHFUL_BUILD_EXTERNAL AND NOT VORBIS_FOUND))
    message(STATUS "---> vorbis building")
    set(BUILD_SHARED_LIB OFF)
    set(INSTALL_CMAKE_PACKAGE_MODULE OFF)
    add_subdirectory(vorbis EXCLUDE_FROM_ALL)
endif()


#    ====================================================================
#    FAITHFUL_ASSET_PROCESSOR-specific

if(FAITHFUL_BUILD_ASSET_PROCESSOR)
    #    --------------------------------------------------------------------
    #    meshoptimizer (AssetProcessor: gltfpack - gltf optimization
    add_subdirectory(meshoptimizer EXCLUDE_FROM_ALL)
endif()


#    ====================================================================
#    {FAITHFUL_ASSET_DOWNLOADER / FAITHFUL_ASSET_PACK} -specific

if(FAITHFUL_BUILD_ASSET_DOWNLOADER OR FAITHFUL_BUILD_ASSET_PACK)
    #    --------------------------------------------------------------------
    #    miniz (AssetDownloader, AssetPack: unzip downloaded assets archive)
    message(STATUS "---> miniz building")
    add_library(miniz STATIC ${FAITHFUL_SOURCE_DIR}/external/miniz/miniz.c)
    target_compile_definitions(miniz PUBLIC
            MINIZ_NO_ZLIB_APIS
            MINIZ_NO_TIME
    )
    target_include_directories(miniz PRIVATE ${FAITHFUL_SOURCE_DIR}/external/miniz)

    #    --------------------------------------------------------------------
    #    PicoSHA2 (AssetDownloader, AssetPack: validate downloaded assets)
    #    only one header, so included directly
endif()
