# ---   ---
# TODO: git (worktree? attributes/config? - idk)

# TODO: configure_file(Faithful/config/paths.h.in)

# TODO: Faithful/utils/*.(h|cpp)

set(SOURCE_DIR ${FAITHFUL_SOURCE_DIR})
set(DESTINATION_DIR ${FAITHFUL_SOURCE_DIR}/Faithful)


# TODO:_________---__---__---____---_---__--_--__--__--__--_---_---_----
# TODO:_________---__---__---____---_---__--_--__--__--__--_---_---_----
# TODO:_________---__---__---____---_---__--_--__--__--__--_---_---_----
#add_custom_target(copy_astcenc_license
#        COMMAND ${CMAKE_COMMAND} -E copy
#        "${FAITHFUL_SOURCE_DIR}/external/googletest/LICENSE"
#        "${FAITHFUL_BINARY_DIR}/licenses/googletest/LICENSE"
#        DEPENDS
#)

#file(GLOB ASTCENC_HEADER_FILES ${SOURCE_DIR}/external/dr_libs/*.h)
#file(GLOB DR_LIBS_HEADER_FILES ${SOURCE_DIR}/external/dr_libs/*.h)

# Copy the header files to the destination directory
execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy ${HEADER_FILES} ${DESTINATION_DIR}/external/dr_libs
)

# --- Freezing of header files
add_custom_target(install_headers
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${SOURCE_DIR}/external/astc-encoder/Source
        ${DESTINATION_DIR}/external/astc-encoder
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${SOURCE_DIR}/external/dr_libs
        ${DESTINATION_DIR}/external/dr_libs
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${SOURCE_DIR}/external/glad/include
        ${DESTINATION_DIR}/external/glad
        DEPENDS
        ${FAITHFUL_BINARY_DIR}/external/astc-encoder/libastc.a
        ${FAITHFUL_BINARY_DIR}/external/dr_libs/libdr_libs.a
        ${FAITHFUL_BINARY_DIR}/external/glad/libglad.a
)


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

set(SOURCE_DIR ${FAITHFUL_BINARY_DIR}/external)
set(DESTINATION_DIR ${FAITHFUL_BINARY_DIR}/destination_directory)

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

