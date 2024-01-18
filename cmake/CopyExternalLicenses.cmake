
add_custom_target(copy_main_licenses
        COMMAND ${CMAKE_COMMAND} -E copy
        "${FAITHFUL_SOURCE_DIR}/external/glad/LICENSE"
        "${FAITHFUL_BINARY_DIR}/licenses/glad/LICENSE"
        COMMAND ${CMAKE_COMMAND} -E copy
        "${FAITHFUL_SOURCE_DIR}/external/glfw/LICENSE.md"
        "${FAITHFUL_BINARY_DIR}/licenses/glfw/LICENSE.md"
        COMMAND ${CMAKE_COMMAND} -E copy
        "${FAITHFUL_SOURCE_DIR}/external/glm/copying.txt"
        "${FAITHFUL_BINARY_DIR}/licenses/glm/copying.txt"
        COMMAND ${CMAKE_COMMAND} -E copy
        "${FAITHFUL_SOURCE_DIR}/external/mimalloc/LICENSE"
        "${FAITHFUL_BINARY_DIR}/licenses/mimalloc/LICENSE"
        COMMAND ${CMAKE_COMMAND} -E copy
        "${FAITHFUL_SOURCE_DIR}/external/ogg/COPYING"
        "${FAITHFUL_BINARY_DIR}/licenses/ogg/COPYING"
        COMMAND ${CMAKE_COMMAND} -E copy
        "${FAITHFUL_SOURCE_DIR}/external/openal-soft/BSD-3Clause"
        "${FAITHFUL_BINARY_DIR}/licenses/openal-soft/BSD-3Clause"
        COMMAND ${CMAKE_COMMAND} -E copy
        "${FAITHFUL_SOURCE_DIR}/external/openal-soft/COPYING"
        "${FAITHFUL_BINARY_DIR}/licenses/openal-soft/COPYING"
        COMMAND ${CMAKE_COMMAND} -E copy
        "${FAITHFUL_SOURCE_DIR}/external/openal-soft/LICENSE-pffft"
        "${FAITHFUL_BINARY_DIR}/licenses/openal-soft/LICENSE-pffft"
        COMMAND ${CMAKE_COMMAND} -E copy
        "${FAITHFUL_SOURCE_DIR}/external/rapidjson/license.txt"
        "${FAITHFUL_BINARY_DIR}/licenses/rapidjson/license.txt"
        COMMAND ${CMAKE_COMMAND} -E copy
        "${FAITHFUL_SOURCE_DIR}/external/tinygltf/LICENSE"
        "${FAITHFUL_BINARY_DIR}/licenses/tinygltf/LICENSE"
        COMMAND ${CMAKE_COMMAND} -E copy
        "${FAITHFUL_SOURCE_DIR}/external/vorbis/COPYING"
        "${FAITHFUL_BINARY_DIR}/licenses/vorbis/COPYING"
)

if (FAITHFUL_BUILD_ASTC_ENCODER)
    add_custom_target(copy_astcenc_license
            COMMAND ${CMAKE_COMMAND} -E copy
            "${FAITHFUL_SOURCE_DIR}/external/googletest/LICENSE"
            "${FAITHFUL_BINARY_DIR}/licenses/googletest/LICENSE"
    )
endif ()

if (FAITHFUL_BUILD_TESTS)
    add_custom_target(copy_googletest_license
            COMMAND ${CMAKE_COMMAND} -E copy
            "${FAITHFUL_SOURCE_DIR}/external/astc-encoder/LICENSE.txt"
            "${FAITHFUL_BINARY_DIR}/licenses/astc-encoder/LICENSE.txt"
    )
endif ()

if (FAITHFUL_BUILD_ASSET_PROCESSOR)
    add_custom_target(copy_asset_processor_licenses
            COMMAND ${CMAKE_COMMAND} -E copy
            "${FAITHFUL_SOURCE_DIR}/external/dr_libs/LICENSE"
            "${FAITHFUL_BINARY_DIR}/licenses/dr_libs/LICENSE"
            COMMAND ${CMAKE_COMMAND} -E copy
            "${FAITHFUL_SOURCE_DIR}/external/meshoptimizer/meshoptimizer/LICENSE.md"
            "${FAITHFUL_BINARY_DIR}/licenses/meshoptimizer/LICENSE.md"
            COMMAND ${CMAKE_COMMAND} -E copy
            "${FAITHFUL_SOURCE_DIR}/external/stb/LICENSE"
            "${FAITHFUL_BINARY_DIR}/licenses/stb/LICENSE"
    )
endif ()

if (FAITHFUL_BUILD_ASSET_DOWNLOADER OR FAITHFUL_BUILD_ASSET_PACK)
    add_custom_target(copy_faithful_pack_faithful_download_licenses
            COMMAND ${CMAKE_COMMAND} -E copy
            "${FAITHFUL_SOURCE_DIR}/external/miniz/LICENSE"
            "${FAITHFUL_BINARY_DIR}/licenses/miniz/LICENSE"
            COMMAND ${CMAKE_COMMAND} -E copy
            "${FAITHFUL_SOURCE_DIR}/external/picosha2/LICENSE"
            "${FAITHFUL_BINARY_DIR}/licenses/picosha2/LICENSE"
    )
endif ()
