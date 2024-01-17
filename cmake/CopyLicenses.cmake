
set(faithful_licenses_dir "${CMAKE_BINARY_DIR}/licenses" CACHE STRING "Licenses location")

# TODO: run as a custom target/command (need to wait until all other will be processed)

function(faithful_copy_license sourcePath targetDir targetName)
    configure_file("${sourcePath}" "${faithful_licenses_dir}/${targetDir}/${targetName}" COPYONLY)
endfunction(faithful_copy_license)

function(faithful_copy_licenses)

    faithful_copy_license("${CMAKE_SOURCE_DIR}/external/glad/LICENSE" "glad" "LICENSE")
    faithful_copy_license("${CMAKE_SOURCE_DIR}/external/glfw/LICENSE.md" "glfw" "LICENSE.md")
    faithful_copy_license("${CMAKE_SOURCE_DIR}/external/glm/manual.md" "glm" "manual.md")
    faithful_copy_license("${CMAKE_SOURCE_DIR}/external/libogg/COPYING" "libogg" "COPYING")
    faithful_copy_license("${CMAKE_SOURCE_DIR}/external/mimalloc/LICENSE" "mimalloc" "LICENSE")
    faithful_copy_license("${CMAKE_SOURCE_DIR}/external/openal-soft/BSD-3Clause" "openal-soft" "BSD-3Clause")
    faithful_copy_license("${CMAKE_SOURCE_DIR}/external/openal-soft/COPYING" "openal-soft" "COPYING")
    faithful_copy_license("${CMAKE_SOURCE_DIR}/external/openal-soft/LICENSE-pffft" "openal-soft" "LICENSE-pffft")
    faithful_copy_license("${CMAKE_SOURCE_DIR}/external/vorbis/COPYING" "vorbis" "COPYING")

    if (FAITHFUL_BUILD_ASTC_ENCODER)
        faithful_copy_license("${CMAKE_SOURCE_DIR}/external/astc-encoder/LICENSE.txt" "astc-encoder" "LICENSE.txt")
    endif ()

    if (FAITHFUL_BUILD_ASSET_PROCESSOR)
        faithful_copy_license("${CMAKE_SOURCE_DIR}/external/dr_libs/LICENSE" "dr_libs" "LICENSE")
        faithful_copy_license("${CMAKE_SOURCE_DIR}/external/stb/LICENSE" "stb" "LICENSE")
        faithful_copy_license("${CMAKE_SOURCE_DIR}/external/meshoptimizer/LICENSE.md" "meshoptimizer" "LICENSE.md")
    endif ()

    if (FAITHFUL_BUILD_ASSET_DOWNLOADER AND
    (${FAITHFUL_ASSET_DOWNLOADER_DOWNLOAD_BACKEND} STREQUAL "FAITHFUL_ASSET_PROCESSOR_CURL_LIB" OR
    ${FAITHFUL_ASSET_DOWNLOADER_DOWNLOAD_BACKEND} STREQUAL "FAITHFUL_ASSET_PROCESSOR_CURL_CLI"))
        faithful_copy_license("${CMAKE_SOURCE_DIR}/external/curl/LICENSES/BSD-3-Clause.txt" "curl" "BSD-3-Clause.txt")
        faithful_copy_license("${CMAKE_SOURCE_DIR}/external/curl/LICENSES/BSD-4-Clause-UC.txt" "curl" "BSD-4-Clause-UC")
        faithful_copy_license("${CMAKE_SOURCE_DIR}/external/curl/LICENSES/curl.txt" "curl" "curl.txt")
        faithful_copy_license("${CMAKE_SOURCE_DIR}/external/curl/LICENSES/ISC.txt" "curl" "ISC.txt")
    endif ()

    if (FAITHFUL_BUILD_ASSET_DOWNLOADER OR FAITHFUL_BUILD_ASSET_PACK)
        faithful_copy_license("${CMAKE_SOURCE_DIR}/external/miniz/LICENSE" "miniz" "LICENSE")
        faithful_copy_license("${CMAKE_SOURCE_DIR}/external/picosha2/LICENSE" "picosha2" "LICENSE")
    endif ()

    if (FAITHFUL_BUILD_TESTS)
        faithful_copy_license("${CMAKE_SOURCE_DIR}/external/googletest/LICENSE" "googletest" "LICENSE")
    endif ()

    # TODO: add Faithful MIT license
endfunction(faithful_copy_licenses)
