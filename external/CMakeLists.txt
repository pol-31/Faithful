cmake_minimum_required(VERSION 3.26)

# TODO: if FREEZE_BUILD - RELEASE mode

include(ExternalProject)
include(CheckCXXSourceRuns)

#    ====================================================================
#    FAITHFUL-specific

#    --------------------------------------------------------------------
#    ALSA (Linux only; Windows, MacOS doesn't need it
if (UNIX AND NOT APPLE)
    if (FAITHFUL_BUILD_EXTERNAL_ALL OR
          (FAITHFUL_BUILD_EXTERNAL AND NOT AUDIO_BACKEND_FOUND))
        set(FAITHFUL_ALSOFT_ALSA_ONLY ON CACHE BOOL "" FORCE)
        add_subdirectory(alsa EXCLUDE_FROM_ALL)
    endif ()
endif ()

# glad should be built/compiled first to find out FAITHFUL_GL_SUPPORTS_ASTC

#    --------------------------------------------------------------------
#    glad (Faithful: OpenGL specification)
if (FAITHFUL_BUILD_EXTERNAL_ALL OR FAITHFUL_BUILD_EXTERNAL)
    message(STATUS "glad building")
    set(GLAD_BINARY_DIR ${CMAKE_BINARY_DIR}/external/glad CACHE INTERNAL "glad binary directory" FORCE)
#    file(REMOVE_RECURSE ${CMAKE_BINARY_DIR}/external/glad_out)
    set(GLAD_OUT_DIR "${CMAKE_BINARY_DIR}/external/glad_out" CACHE STRING "Output directory" FORCE)
    set(GLAD_INSTALL OFF CACHE BOOL "Generate installation target" FORCE)
    set(GLAD_GENERATOR "c" CACHE STRING "Language to generate the binding for" FORCE)

    file(READ "${CMAKE_SOURCE_DIR}/config/opengl.txt" CONFIG_CONTENT)
    string(REGEX REPLACE "\n" ";" CONFIG_LINES "${CONFIG_CONTENT}")

    foreach(line ${CONFIG_LINES})
        string(REGEX MATCH "^([^,]+),([^,]+),([^,]+)$" _match ${line})
        set(key ${CMAKE_MATCH_1})
        set(type ${CMAKE_MATCH_2})
        set(value ${CMAKE_MATCH_3})
        if(type STREQUAL "STRING")
            set(${key} "${value}" CACHE STRING "Glad/OpenGL properties (see Faithful/config/opengl.txt)" FORCE)
        elseif(type STREQUAL "BOOL")
            set(${key} ${value} CACHE BOOL "" FORCE)
        endif()
    endforeach()

    message("GLAD_API: ${GLAD_API}")
    message("GLAD_PROFILE: ${GLAD_PROFILE}")
    message("GLAD_ALL_EXTENSIONS: ${GLAD_ALL_EXTENSIONS}")
    add_subdirectory(glad EXCLUDE_FROM_ALL)
endif ()

# if FAITHFUL_GL_SUPPORTS_ASTC, OpenGL has embedded support of astc textures,
# so we don't need astc-decoder (astc-encoder depends on AssetProcessor)

set(CMAKE_REQUIRED_INCLUDES "${GLAD_OUT_DIR}/include")

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
if (FAITHFUL_BUILD_ASTC_ENCODER)
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
            BUILD_BYPRODUCTS ${ASTC_ENCODER_BINARY_DIR}/src/astcencoder_external-build/Source/libastcenc-native-static.a
    )

    add_library(astc-encoder INTERFACE)
    add_dependencies(astc-encoder astcencoder_external)
    target_link_libraries(astc-encoder
            INTERFACE ${ASTC_ENCODER_BINARY_DIR}/src/astcencoder_external-build/Source/libastcenc-native-static.a
    )
    target_include_directories(astc-encoder INTERFACE ${ASTC_ENCODER_SOURCE_DIR}/Source)
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
    set(LIBOGG_SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/libogg CACHE INTERNAL "libogg binary directory" FORCE)
    set(LIBOGG_BINARY_DIR ${CMAKE_BINARY_DIR}/external/libogg CACHE INTERNAL "libogg binary directory" FORCE)

    ExternalProject_Add(
            libogg_external
            PREFIX ${LIBOGG_BINARY_DIR}
            SOURCE_DIR ${LIBOGG_SOURCE_DIR}
            CONFIGURE_COMMAND ${LIBOGG_SOURCE_DIR}/autogen.sh
            BUILD_COMMAND make -j8
            INSTALL_COMMAND cmake -E echo "Skipping install step"
            BUILD_BYPRODUCTS
            ${LIBOGG_BINARY_DIR}/src/libogg_external-build/src/.libs/libogg.a
    )

    add_library(libogg INTERFACE)
    add_dependencies(libogg libogg_external)

    target_link_libraries(libogg INTERFACE ${LIBOGG_BINARY_DIR}/src/libogg_external-build/src/.libs/libogg.a)
    target_include_directories(libogg INTERFACE ${LIBOGG_SOURCE_DIR}/include)

    set(Ogg_INCLUDE_DIRS "${LIBOGG_SOURCE_DIR}/include" CACHE PATH "" FORCE)
    set(Ogg_LIBRARIES "${LIBOGG_BINARY_DIR}/src/libogg_external-build/src/.libs/libogg.a" CACHE PATH "" FORCE)
    set(OGG_INCLUDE_DIR "${LIBOGG_SOURCE_DIR}/include" CACHE PATH "" FORCE)
    set(OGG_LIBRARY "${LIBOGG_BINARY_DIR}/src/libogg_external-build/src/.libs/libogg.a" CACHE PATH "" FORCE)
    set(LIBOGG_INCLUDE_DIR "${LIBOGG_SOURCE_DIR}/include" CACHE PATH "" FORCE)
    set(LIBOGG_LIBRARY "${LIBOGG_BINARY_DIR}/src/libogg_external-build/src/.libs/libogg.a" CACHE PATH "" FORCE)
endif()


#    --------------------------------------------------------------------
#    mimalloc (all: general-purpose allocator - overrides malloc/new/etc...)
if (FAITHFUL_BUILD_EXTERNAL_ALL OR
      (FAITHFUL_BUILD_EXTERNAL AND NOT MIMALLOC_FOUND))
    message(STATUS "mimalloc building")
    set(MIMALLOC_SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/mimalloc)
    set(MIMALLOC_BINARY_DIR ${CMAKE_BINARY_DIR}/external/mimalloc CACHE INTERNAL "mimalloc binary directory" FORCE)

    ExternalProject_Add(
            mimalloc_external
            PREFIX ${MIMALLOC_BINARY_DIR}
            SOURCE_DIR ${MIMALLOC_SOURCE_DIR}
            CONFIGURE_COMMAND ${CMAKE_COMMAND}
            -DMI_BUILD_SHARED=OFF
            -DMI_BUILD_STATIC=ON
            -DMI_BUILD_OBJECT=OFF
            -DMI_BUILD_TESTS=OFF
            -DMI_USE_CXX=ON
            ${MIMALLOC_SOURCE_DIR}
            BUILD_COMMAND ${CMAKE_COMMAND} --build .
            INSTALL_COMMAND ${CMAKE_COMMAND} -E echo "Skipping install step"
            BUILD_BYPRODUCTS ${MIMALLOC_BINARY_DIR}/src/mimalloc_external-build/libmimalloc.a
    )
    add_library(mimalloc INTERFACE)
    add_dependencies(mimalloc mimalloc_external)
    target_link_libraries(mimalloc
            INTERFACE ${MIMALLOC_BINARY_DIR}/src/mimalloc_external-build/libmimalloc.a
    )
endif()


#    --------------------------------------------------------------------
#    OpenAL-Soft (Faithful: spatial audio streaming)
if (FAITHFUL_BUILD_EXTERNAL_ALL OR
      (FAITHFUL_BUILD_EXTERNAL AND NOT OPENAL_FOUND))
    message(STATUS "openal-soft building")
    set(OPENALSOFT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/openal-soft)
    set(OPENALSOFT_BINARY_DIR ${CMAKE_BINARY_DIR}/external/openal-soft CACHE INTERNAL "openal-soft binary directory" FORCE)

    if (FAITHFUL_ALSOFT_ALSA_ONLY)
        set(ALSOFT_OPTIONS
                "-DALSOFT_REQUIRE_ALSA=ON "
                "-DALSOFT_BACKEND_ALSA=ON "
                "-DALSOFT_BACKEND_OSS=OFF "
                "-DALSOFT_BACKEND_SOLARIS=OFF "
                "-DALSOFT_BACKEND_SNDIO=OFF "
                "-DALSOFT_BACKEND_QSA=OFF "
                "-DALSOFT_BACKEND_PORTAUDIO=OFF "
                "-DALSOFT_BACKEND_PULSEAUDIO=OFF "
                "-DALSOFT_BACKEND_JACK=OFF "
                "-DALSOFT_BACKEND_COREAUDIO=OFF "
                "-DALSOFT_BACKEND_WAVE=OFF "
                "-DALSOFT_RTKIT=OFF"
                CACHE INTERNAL "ALSOFT options"
        )
    else ()
        set(ALSOFT_OPTIONS "" CACHE INTERNAL "ALSOFT options")
    endif ()

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
            ${ALSOFT_OPTIONS}

            ${OPENALSOFT_SOURCE_DIR}
            BUILD_COMMAND ${CMAKE_COMMAND} --build .
            INSTALL_COMMAND ${CMAKE_COMMAND} -E echo "Skipping install step"
            BUILD_BYPRODUCTS ${OPENALSOFT_BINARY_DIR}/src/openal-soft_external-build/libopenal.a
#            BUILD_BYPRODUCTS ${OPENALSOFT_BINARY_DIR}/src/mimalloc_external-build/libmimalloc.a
    )
    add_library(openal-soft INTERFACE)
    add_dependencies(openal-soft openal-soft_external)
    target_link_libraries(openal-soft INTERFACE ${OPENALSOFT_BINARY_DIR}/src/openal-soft_external-build/libopenal.a)

    if (FAITHFUL_ALSOFT_ALSA_ONLY)
        add_dependencies(openal-soft alsa)
    endif ()
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
    set(VORBIS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/vorbis CACHE INTERNAL "vorbis source directory" FORCE)
    set(VORBIS_BINARY_DIR ${CMAKE_BINARY_DIR}/external/vorbis CACHE INTERNAL "vorbis binary directory" FORCE)

    ExternalProject_Add(
            vorbis_external
            PREFIX ${VORBIS_BINARY_DIR}
            SOURCE_DIR ${VORBIS_SOURCE_DIR}
            CONFIGURE_COMMAND ${CMAKE_COMMAND}
            -DBUILD_SHARED_LIBS=OFF
            ${VORBIS_SOURCE_DIR}
            BUILD_COMMAND ${CMAKE_COMMAND} --build .
            INSTALL_COMMAND ${CMAKE_COMMAND} -E echo "Skipping install step"
            BUILD_BYPRODUCTS
            ${VORBIS_BINARY_DIR}/src/vorbis_external-build/lib/libvorbisenc.a
            ${VORBIS_BINARY_DIR}/src/vorbis_external-build/lib/libvorbisfile.a
            ${VORBIS_BINARY_DIR}/src/vorbis_external-build/lib/libvorbis.a
            DEPENDS libogg
    )

    add_library(vorbis INTERFACE)
    add_dependencies(vorbis vorbis_external)

    target_link_libraries(vorbis
            INTERFACE ${VORBIS_BINARY_DIR}/src/vorbis_external-build/lib/libvorbisenc.a
            INTERFACE ${VORBIS_BINARY_DIR}/src/vorbis_external-build/lib/libvorbisfile.a
            INTERFACE ${VORBIS_BINARY_DIR}/src/vorbis_external-build/lib/libvorbis.a
    )
    target_include_directories(vorbis INTERFACE ${VORBIS_SOURCE_DIR}/include)
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
