
set(faithful_licenses_dir "${CMAKE_CURRENT_LIST_DIR}")
set(faithful_licenses_path "${CMAKE_SOURCE_DIR}/licenses" CACHE STRING "Licenses location")

function(faithful_copy_license sourcePath targetName)
    configure_file("${sourcePath}" "${faithful_licenses_path}/${targetName}" COPYONLY)
endfunction(faithful_copy_license)

function(faithful_copy_licenses)
    faithful_copy_license("${ALSA_SOURCE_DIR}/COPYING" "alsa")
    faithful_copy_license("${CMAKE_SOURCE_DIR}/external/astc-encoder/LICENSE.txt" "astc-encoder")
#    faithful_copy_license("${CURL_SOURCE_DIR}/../external/curl/curl/LICENSES/BSD-3-Clause.txt" "curl") # TODO: cond, more licenses
#    faithful_copy_license("${CMAKE_SOURCE_DIR}/external/dr_libs/LICENSE" "dr_libs") # TODO: cond
    faithful_copy_license("${CMAKE_SOURCE_DIR}/external/glad/LICENSE" "glad")
    faithful_copy_license("${CMAKE_SOURCE_DIR}/external/glfw/LICENSE.md" "glfw")
    faithful_copy_license("${CMAKE_SOURCE_DIR}/external/glm/manual.md" "glm")
#    faithful_copy_license("${CMAKE_SOURCE_DIR}/external/googletest/googletest/LICENSE" "googletest") # TODO: cond
    faithful_copy_license("${CMAKE_SOURCE_DIR}/external/libogg/COPYING" "libogg")
    # TODO: meshoptimizer
    # TODO: mimalloc
    # TODO: miniz
    faithful_copy_license("${CMAKE_SOURCE_DIR}/external/openal-soft/LICENSE-pffft" "openal-soft")
    # TODO: picosha2
    # TODO: rapidjson
    # TODO: stb
    # TODO: tinygltf
    faithful_copy_license("${CMAKE_SOURCE_DIR}/external/vorbis/COPYING" "vorbis")
    # TODO: add "/../LICENSE" "faithful"
endfunction(faithful_copy_licenses)
