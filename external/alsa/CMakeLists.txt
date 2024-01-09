cmake_minimum_required(VERSION 3.26)

message(STATUS "alsa building")

include(ExternalProject)

set(ALSA_SOURCE_DIR ${CMAKE_BINARY_DIR}/alsa)
#set(ALSA_BINARY_DIR ${CMAKE_BINARY_DIR}/alsa-build CACHE INTERNAL "alsa binary directory" FORCE)

set(ALSA_URL https://github.com/alsa-project/alsa-lib.git)
set(ALSA_REVISION master)

ExternalProject_Add(
        alsa_external
        SOURCE_DIR ${ALSA_SOURCE_DIR}
        BINARY_DIR ${CMAKE_BINARY_DIR}/external/alsa
        STAMP_DIR ${ALSA_SOURCE_DIR}/tmp
        PREFIX ${ALSA_SOURCE_DIR}
        GIT_REPOSITORY ${ALSA_URL}
        GIT_TAG ${ALSA_REVISION}
        CONFIGURE_COMMAND gitcompile
        BUILD_COMMAND make -j8
        INSTALL_COMMAND cmake -E echo "Skipping install step"
        UPDATE_DISCONNECTED 1
)

add_library(alsa INTERFACE)
add_dependencies(alsa alsa_external)

target_link_libraries(alsa INTERFACE alsa_external)
target_include_directories(alsa INTERFACE ${ALSA_SOURCE_DIR}/include)
