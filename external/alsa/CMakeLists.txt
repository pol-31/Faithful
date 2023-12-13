cmake_minimum_required(VERSION 3.26)

message(STATUS "alsa building")

include(ExternalProject)

set(ALSA_SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/alsa/alsa)
set(ALSA_BINARY_DIR ${CMAKE_BINARY_DIR}/alsa-build CACHE INTERNAL "alsa binary directory" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ALSA_BINARY_DIR} CACHE INTERNAL "alsa executable output directory" FORCE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${ALSA_BINARY_DIR} CACHE INTERNAL "alsa library output directory" FORCE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${ALSA_BINARY_DIR} CACHE INTERNAL "alsa archive output directory" FORCE)

ExternalProject_Add(
        alsa
        PREFIX ${ALSA_BINARY_DIR}
        SOURCE_DIR ${ALSA_SOURCE_DIR}
        CONFIGURE_COMMAND ${ALSA_SOURCE_DIR}/gitcompile
        BUILD_COMMAND make -j8
        INSTALL_COMMAND cmake -E echo "Skipping install step"
)



include_directories(${ALSA_SOURCE_DIR})
#target_link_libraries(libvpx ${VPX_BINARY_DIR}/libvpx.a)
