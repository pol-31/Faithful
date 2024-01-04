cmake_minimum_required(VERSION 3.26)

message(STATUS "vorbis building")

set(VORBIS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/vorbis/vorbis CACHE INTERNAL "vorbis source directory" FORCE)
set(VORBIS_BINARY_DIR ${CMAKE_BINARY_DIR}/vorbis-build CACHE INTERNAL "vorbis binary directory" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${VORBIS_BINARY_DIR} CACHE INTERNAL "vorbis executable output directory" FORCE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${VORBIS_BINARY_DIR} CACHE INTERNAL "vorbis library output directory" FORCE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${VORBIS_BINARY_DIR} CACHE INTERNAL "vorbis archive output directory" FORCE)

include(ExternalProject)
ExternalProject_Add(
        vorbis_external
        PREFIX ${CMAKE_BINARY_DIR}
        SOURCE_DIR ${VORBIS_SOURCE_DIR}
        CONFIGURE_COMMAND ${CMAKE_COMMAND}
        -DBUILD_SHARED_LIBS=OFF
        ${VORBIS_SOURCE_DIR}
        BUILD_COMMAND ${CMAKE_COMMAND} --build .
        INSTALL_COMMAND ${CMAKE_COMMAND} -E echo "Skipping install step"
        BUILD_ALWAYS 1
        BUILD_BYPRODUCTS
        ${CMAKE_BINARY_DIR}/src/vorbis_external-build/lib/libvorbisenc.a
        ${CMAKE_BINARY_DIR}/src/vorbis_external-build/lib/libvorbisfile.a
        ${CMAKE_BINARY_DIR}/src/vorbis_external-build/lib/libvorbis.a
)

add_library(vorbis INTERFACE)
add_dependencies(vorbis vorbis_external)

target_link_libraries(vorbis
        INTERFACE ${CMAKE_BINARY_DIR}/src/vorbis_external-build/lib/libvorbisenc.a
        INTERFACE ${CMAKE_BINARY_DIR}/src/vorbis_external-build/lib/libvorbisfile.a
        INTERFACE ${CMAKE_BINARY_DIR}/src/vorbis_external-build/lib/libvorbis.a
)
target_include_directories(vorbis INTERFACE ${VORBIS_SOURCE_DIR}/include)
