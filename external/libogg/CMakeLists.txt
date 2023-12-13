cmake_minimum_required(VERSION 3.26)

message(STATUS "libogg building")

include(ExternalProject)

set(LIBOGG_SOURCE_DIR ${CMAKE_SOURCE_DIR}/libogg/libogg)
set(LIBOGG_BINARY_DIR ${CMAKE_BINARY_DIR}/libogg-build CACHE INTERNAL "libogg binary directory" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${LIBOGG_BINARY_DIR} CACHE INTERNAL "libogg executable output directory" FORCE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIBOGG_BINARY_DIR} CACHE INTERNAL "libogg library output directory" FORCE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIBOGG_BINARY_DIR} CACHE INTERNAL "libogg archive output directory" FORCE)

ExternalProject_Add(
        libogg_external
        PREFIX ${CMAKE_BINARY_DIR}
        SOURCE_DIR ${LIBOGG_SOURCE_DIR}
        CONFIGURE_COMMAND ${LIBOGG_SOURCE_DIR}/autogen.sh
        BUILD_COMMAND make -j8
        INSTALL_COMMAND cmake -E echo "Skipping install step"
        BUILD_BYPRODUCTS
        ${CMAKE_BINARY_DIR}/src/libogg_external-build/src/.libs/libogg.a
)

add_library(libogg INTERFACE)
add_dependencies(libogg libogg_external)

target_link_libraries(libogg INTERFACE ${CMAKE_BINARY_DIR}/src/libogg_external-build/src/.libs/libogg.a)
target_include_directories(libogg INTERFACE ${LIBOGG_SOURCE_DIR}/include)

set(Ogg_INCLUDE_DIRS "${LIBOGG_SOURCE_DIR}/include" CACHE PATH "" FORCE)
set(Ogg_LIBRARIES "${CMAKE_BINARY_DIR}/src/libogg_external-build/src/.libs/libogg.a" CACHE PATH "" FORCE)
set(OGG_INCLUDE_DIR "${LIBOGG_SOURCE_DIR}/include" CACHE PATH "" FORCE)
set(OGG_LIBRARY "${CMAKE_BINARY_DIR}/src/libogg_external-build/src/.libs/libogg.a" CACHE PATH "" FORCE)

