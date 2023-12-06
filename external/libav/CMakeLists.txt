cmake_minimum_required(VERSION 3.26)

message(STATUS "libav building")

include(ExternalProject)

set(LIBAV_SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/libav/libav CACHE INTERNAL "libav binary directory" FORCE)
message(STATUS ${LIBAV_SOURCE_DIR})
set(LIBAV_BUILD_DIR ${CMAKE_BINARY_DIR}/libav-build CACHE INTERNAL "libav binary directory" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ASSIMP_BINARY_DIR} CACHE INTERNAL "libav executable output directory" FORCE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${ASSIMP_BINARY_DIR} CACHE INTERNAL "libav library output directory" FORCE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${ASSIMP_BINARY_DIR} CACHE INTERNAL "libav archive output directory" FORCE)

ExternalProject_Add(
        libav_project
        PREFIX ${LIBAV_BUILD_DIR}
        SOURCE_DIR ${LIBAV_SOURCE_DIR}
        CONFIGURE_COMMAND ${LIBAV_SOURCE_DIR}/configure --disable-all
        BUILD_COMMAND make -j8
        INSTALL_COMMAND ""
        #DEPENDS yasm
)


add_library(libav INTERFACE)
add_dependencies(libav libav_project)

target_include_directories(libav INTERFACE
        ${LIBAV_SOURCE_DIR}/libavcodec
        ${LIBAV_SOURCE_DIR}/libavformat
        ${LIBAV_SOURCE_DIR}/libavutil
)

target_link_libraries(libav INTERFACE libav_project)
