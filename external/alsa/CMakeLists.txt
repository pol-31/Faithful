cmake_minimum_required(VERSION 3.26)

message(STATUS "alsa building")

include(ExternalProject)

set(ALSA_SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/alsa/alsa_source)

set(ALSA_URL "https://github.com/alsa-project/alsa-lib.git")
set(ALSA_REVISION master)

ExternalProject_Add(
        alsa_external
        SOURCE_DIR ${ALSA_SOURCE_DIR}
        GIT_REPOSITORY ${ALSA_URL}
        GIT_TAG ${ALSA_REVISION}
        CONFIGURE_COMMAND ${ALSA_SOURCE_DIR}/gitcompile --enable-shared=yes --enable-static=no
        BUILD_COMMAND make -j8
        INSTALL_COMMAND cmake -E echo "Skipping install step"
        BUILD_BYPRODUCTS "${ALSA_SOURCE_DIR}/src/.libs/libasound.so"
        UPDATE_DISCONNECTED 1
        BUILD_IN_SOURCE 1  # necessary for autotools projects
)
#        CONFIGURE_COMMAND ${ALSA_SOURCE_DIR}/gitcompile

add_library(alsa INTERFACE)
add_dependencies(alsa alsa_external)

set(ALSA_FOUND ON CACHE BOOL "" FORCE)
set(ALSA_LIBRARIES "/usr/lib/x86_64-linux-gnu/libasound.so" CACHE STRING "" FORCE)
#set(ALSA_LIBRARIES "${ALSA_SOURCE_DIR}/src/.libs/libasound.so" CACHE STRING "" FORCE)
set(ALSA_INCLUDE_DIRS "${ALSA_SOURCE_DIR}/include" CACHE STRING "" FORCE)
set(ALSA_INCLUDE_DIR "${ALSA_SOURCE_DIR}/include" CACHE STRING "" FORCE)
set(ALSA_LIBRARY "/usr/lib/x86_64-linux-gnu/libasound.so" CACHE STRING "" FORCE)
#set(ALSA_LIBRARY "${ALSA_SOURCE_DIR}/src/.libs/libasound.so" CACHE STRING "" FORCE)

target_link_libraries(alsa INTERFACE ${ALSA_LIBRARY})
target_include_directories(alsa INTERFACE ${ALSA_INCLUDE_DIR})
