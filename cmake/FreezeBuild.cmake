# ---   ---
# TODO: git (worktree? attributes/config? - idk)

# TODO: run as a custom target/command (need to wait until all other will be processed)

set(SOURCE_DIR ${CMAKE_SOURCE_DIR})
set(DESTINATION_DIR ${CMAKE_SOURCE_DIR}/Faithful)

# --- Freezing of header files

file(GLOB HEADER_FILES ${SOURCE_DIR}/external/astc-encoder/Source/*.h)
install(FILES ${HEADER_FILES}
        DESTINATION ${DESTINATION_DIR}/external/astc-encoder
)
file(GLOB HEADER_FILES ${SOURCE_DIR}/external/dr_libs/*.h)
install(FILES ${HEADER_FILES}
        DESTINATION ${DESTINATION_DIR}/external/dr_libs
)
file(GLOB HEADER_FILES ${SOURCE_DIR}/external/glad/include/*.h)
install(FILES ${HEADER_FILES}
        DESTINATION ${DESTINATION_DIR}/external/glad
)

install(DIRECTORY ${SOURCE_DIR}/external/glm
        DESTINATION ${DESTINATION_DIR}/external/glm
        FILES_MATCHING PATTERN "*"
)
install(DIRECTORY ${SOURCE_DIR}/external/mimalloc
        DESTINATION ${DESTINATION_DIR}/external/mimalloc
        FILES_MATCHING PATTERN "*"
)
install(DIRECTORY ${SOURCE_DIR}/external/googletest/googletest
        DESTINATION ${DESTINATION_DIR}/external/googletest
        FILES_MATCHING PATTERN "*"
)

file(GLOB HEADER_FILES ${SOURCE_DIR}/external/ogg/include/*.h)
install(FILES ${HEADER_FILES}
        DESTINATION ${DESTINATION_DIR}/ogg
)
file(GLOB HEADER_FILES ${SOURCE_DIR}/external/vorbis/include/*.h)
install(FILES ${HEADER_FILES}
        DESTINATION ${DESTINATION_DIR}/vorbis
)
file(GLOB HEADER_FILES ${SOURCE_DIR}/external/miniz/*.h)
install(FILES ${HEADER_FILES}
        DESTINATION ${DESTINATION_DIR}/miniz
)
file(GLOB HEADER_FILES ${SOURCE_DIR}/external/openal-soft/include/*.h)
install(FILES ${HEADER_FILES}
        DESTINATION ${DESTINATION_DIR}/openal-soft
)
file(GLOB HEADER_FILES ${SOURCE_DIR}/external/picosha2/*.h)
install(FILES ${HEADER_FILES}
        DESTINATION ${DESTINATION_DIR}/picosha2
)
file(GLOB HEADER_FILES ${SOURCE_DIR}/external/stb/*.h)
install(FILES ${HEADER_FILES}
        DESTINATION ${DESTINATION_DIR}/stb
)
file(GLOB HEADER_FILES ${SOURCE_DIR}/external/tinygltf/*.h)
install(FILES ${HEADER_FILES}
        DESTINATION ${DESTINATION_DIR}/tinygltf
)


# --- Freezing of libraries

set(SOURCE_DIR ${CMAKE_BINARY_DIR}/external)
set(DESTINATION_DIR ${CMAKE_BINARY_DIR}/destination_directory)

file(GLOB_RECURSE LIB_FILES ${SOURCE_DIR}/*${CMAKE_STATIC_LIBRARY_SUFFIX})

install(FILES ${LIB_FILES}
        DESTINATION ${DESTINATION_DIR}/external/libs
)

# --- Freezing of executables


if (FAITHFUL_BUILD_ASSET_DOWNLOADER)
    set_target_properties(AssetDownloader PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${DESTINATION_DIR}/utils
    )
    install(TARGETS AssetDownloader
            DESTINATION ${DESTINATION_DIR}/utils
    )
endif ()

if (FAITHFUL_BUILD_ASSET_PACK)
    set_target_properties(AssetPack PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${DESTINATION_DIR}/utils
    )
    install(TARGETS AssetPack
            DESTINATION ${DESTINATION_DIR}/utils
    )
endif ()

if (FAITHFUL_BUILD_ASSET_PROCESSOR)
    set_target_properties(AssetProcessor PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${DESTINATION_DIR}/utils
    )
    install(TARGETS AssetProcessor
            DESTINATION ${DESTINATION_DIR}/utils
    )
endif ()

#if (FAITHFUL_BUILD_ASSET_PROCESSOR)
#    install(TARGETS ${MESHOPTIMIZER_BINARY_DIR}/gltfpack
#            DESTINATION ${DESTINATION_DIR}/utils
#    )
#endif ()


# --- Freezing of other files (main source)


install(DIRECTORY ${SOURCE_DIR}/assets
        DESTINATION ${DESTINATION_DIR}/src
        FILES_MATCHING PATTERN "*"
)
install(DIRECTORY ${SOURCE_DIR}/cmake
        DESTINATION ${DESTINATION_DIR}/src
        FILES_MATCHING PATTERN "*"
)
install(DIRECTORY ${SOURCE_DIR}/config
        DESTINATION ${DESTINATION_DIR}/src
        FILES_MATCHING PATTERN "*"
)
install(DIRECTORY ${SOURCE_DIR}/docs
        DESTINATION ${DESTINATION_DIR}/src
        FILES_MATCHING PATTERN "*"
)
install(DIRECTORY ${SOURCE_DIR}/licenses
        DESTINATION ${DESTINATION_DIR}/src
        FILES_MATCHING PATTERN "*"
)
install(DIRECTORY ${SOURCE_DIR}/src
        DESTINATION ${DESTINATION_DIR}/src
        FILES_MATCHING PATTERN "*"
)
install(DIRECTORY ${SOURCE_DIR}/src
        DESTINATION ${DESTINATION_DIR}/src
        FILES_MATCHING PATTERN "*"
)
install(FILES ${SOURCE_DIR}/.clang-format
        DESTINATION ${DESTINATION_DIR}/.clang-format
)

