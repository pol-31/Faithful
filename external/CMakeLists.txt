cmake_minimum_required(VERSION 3.26)

#    ====================================================================
#    FAITHFUL-specific

# glad should be built/compiled first to find out FAITHFUL_GL_SUPPORTS_ASTC

#    --------------------------------------------------------------------
#    glad (Faithful: OpenGL specification)
message(STATUS "---> glad building")
add_subdirectory(glad EXCLUDE_FROM_ALL)

# if FAITHFUL_GL_SUPPORTS_ASTC, OpenGL has embedded support of astc textures,
# so we don't need astc-decoder (astc-encoder depends on AssetProcessor)

include(CheckCXXSourceRuns)

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
if(FAITHFUL_BUILD_ASSET_PROCESSOR OR NOT FAITHFUL_GL_SUPPORTS_ASTC)
    set(FAITHFUL_BUILD_ASTC_ENCODER ON CACHE INTERNAL "")
    message(STATUS "---> astc-encoder building")
    if(NOT FAITHFUL_GL_SUPPORTS_ASTC AND NOT FAITHFUL_BUILD_ASSET_PROCESSOR)
        set(ASTCENC_DECOMPRESSOR ON)
    endif()
    set(ASTCENC_BLOCK_MAX_TEXELS 64)
    add_subdirectory(astc-encoder)
endif()

#    --------------------------------------------------------------------
#    dr_libs (AssetProcessor only: decompress mp3, flac, wav into PCM
#             for further compression into ogg+Vorbis)
message(STATUS "---> dr_libs building")
add_subdirectory(dr_libs EXCLUDE_FROM_ALL)

#    --------------------------------------------------------------------
#    folly (Faithful: asynchronous Futures&Promises)
message(STATUS "---> folly building")
add_subdirectory(folly EXCLUDE_FROM_ALL)

#    --------------------------------------------------------------------
#    glfw (Faithful: Window/context creation)
message(STATUS "---> glfw building")
add_subdirectory(glfw EXCLUDE_FROM_ALL)

#    --------------------------------------------------------------------
message(STATUS "---> glm building")
add_subdirectory(glm EXCLUDE_FROM_ALL)

#    --------------------------------------------------------------------
message(STATUS "---> googletest building")
add_subdirectory(googletest EXCLUDE_FROM_ALL)

#    --------------------------------------------------------------------
#    libogg (Faithful: audio streaming to OpenAL-Soft;
#            AssetProcessor: compression, transrating)
message(STATUS "---> libogg building")
add_subdirectory(ogg EXCLUDE_FROM_ALL)

#    --------------------------------------------------------------------
#    mimalloc (all: general-purpose allocator - overrides malloc/new/etc...)
message(STATUS "---> mimalloc building")
set(MI_BUILD_SHARED OFF)
set(MI_BUILD_STATIC ON)
set(MI_BUILD_OBJECT OFF)
set(MI_BUILD_TESTS OFF)
set(MI_USE_CXX ON)
add_subdirectory(mimalloc EXCLUDE_FROM_ALL)

#    --------------------------------------------------------------------
#    OpenAL-Soft (Faithful: spatial audio streaming)
message(STATUS "---> openal-soft building")
set(LIBTYPE "STATIC")
add_subdirectory(openal-soft EXCLUDE_FROM_ALL)

#    --------------------------------------------------------------------
#    rapidjson (Faithful: json-parser for tinygltf
#               AssetProcessor: safely extract embedded textures for further
#                               .astc copmression)
#    only one header, so included directly

#    --------------------------------------------------------------------
#    stb_image, stb_image_write (Faithful: texture loading for further
#                                          compression into .astc)
message(STATUS "---> stb_image, stb_image_write building")
add_subdirectory(stb EXCLUDE_FROM_ALL)

#    --------------------------------------------------------------------
#    tinygltf (Faithful: process/draw .gltf model)
message(STATUS "---> tinygltf building")
add_subdirectory(tinygltf EXCLUDE_FROM_ALL)

#    --------------------------------------------------------------------
#    vorbis (Faithful: audio streaming to OpenAL-Soft;
#            AssetProcessor: compression, transrating)
message(STATUS "---> vorbis building")
add_subdirectory(vorbis EXCLUDE_FROM_ALL)

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
    add_subdirectory(miniz EXCLUDE_FROM_ALL)

    #    --------------------------------------------------------------------
    #    PicoSHA2 (AssetDownloader, AssetPack: validate downloaded assets)
    message(STATUS "---> picosha2 building")
    add_subdirectory(picosha2 EXCLUDE_FROM_ALL)
endif()
