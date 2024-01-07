cmake_minimum_required(VERSION 3.26)

message(STATUS "curl building")

include(ExternalProject)

ExternalProject_Add(
        curl_project
        PREFIX "${CMAKE_BINARY_DIR}/external/curl"
        GIT_REPOSITORY https://github.com/curl/curl.git
        GIT_TAG master
        UPDATE_COMMAND ""
        CONFIGURE_COMMAND ${CMAKE_COMMAND}
        -DBUILD_CURL_EXE=OFF
        -DBUILD_SHARED_LIBS=OFF
        -DBUILD_STATIC_LIBS=ON
        -DHTTP_ONLY=ON
        -DPICKY_COMPILER=OFF
        -DCURL_DISABLE_INSTALL=ON
        -DCURL_LTO=ON
        -DCURL_ENABLE_EXPORT_TARGET=OFF
        .
        BUILD_COMMAND ${CMAKE_COMMAND} --build .
        INSTALL_COMMAND ${CMAKE_COMMAND} -E echo "Skipping install step"
)

ExternalProject_Get_Property(curl_project SOURCE_DIR)
set(CURL_INCLUDE_DIRS "${SOURCE_DIR}/include" CACHE INTERNAL "Path to cURL include directory")

ExternalProject_Get_Property(curl_project BINARY_DIR)
set(CURL_LIBRARIES "${BINARY_DIR}/lib/libcurl.a" CACHE INTERNAL "Path to cURL library")
