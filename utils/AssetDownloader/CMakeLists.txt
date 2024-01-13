cmake_minimum_required(VERSION 3.26)
project(AssetDownloader)

# TODO: if FREEZE_BUILD --> RELEASE

set(CMAKE_CXX_STANDARD 17)

add_executable(AssetDownloader main.cpp)

if (FAITHFUL_BUILD_EXTERNAL_ALL)
    add_subdirectory(${CMAKE_SOURCE_DIR}/external/curl
            ${CMAKE_BINARY_DIR}/external/curl EXCLUDE_FROM_ALL)
    set(FAITHFUL_ASSET_DOWNLOADER_DOWNLOAD_BACKEND
            "FAITHFUL_ASSET_PROCESSOR_CURL_LIB" CACHE STRING "" FORCE)
endif ()

find_package(CURL)
if (CURL_FOUND)
    set(FAITHFUL_ASSET_DOWNLOADER_DOWNLOAD_BACKEND
            FAITHFUL_ASSET_PROCESSOR_CURL_LIB CACHE STRING "" FORCE)
endif()

if (WIN32 AND NOT FAITHFUL_ASSET_DOWNLOADER_DOWNLOAD_BACKEND)
    execute_process(COMMAND powershell "-Command" "$PSVersionTable.PSVersion.Major"
            OUTPUT_VARIABLE PS_MAJOR_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
    if (PS_MAJOR_VERSION GREATER_EQUAL 5)
        set(FAITHFUL_ASSET_DOWNLOADER_DOWNLOAD_BACKEND
                "FAITHFUL_ASSET_PROCESSOR_INVOKE_WEB_REQUEST" CACHE STRING "" FORCE)
    endif()
endif()

if (NOT FAITHFUL_ASSET_DOWNLOADER_DOWNLOAD_BACKEND)
    find_package(Wget)
    if(WGET_EXECUTABLE)
        set(FAITHFUL_ASSET_DOWNLOADER_DOWNLOAD_BACKEND
                "FAITHFUL_ASSET_PROCESSOR_WGET" CACHE STRING "" FORCE)
    endif()
endif ()

if (NOT FAITHFUL_ASSET_DOWNLOADER_DOWNLOAD_BACKEND)
    # find_package(CURL) does not search for executable, so need extra lookup
    find_program(CURL_EXECUTABLE NAMES curl)
    if(CURL_EXECUTABLE)
        set(FAITHFUL_ASSET_DOWNLOADER_DOWNLOAD_BACKEND
                "FAITHFUL_ASSET_PROCESSOR_CURL_CLI" CACHE STRING "" FORCE)
    endif()
endif ()


if (FAITHFUL_ASSET_DOWNLOADER_DOWNLOAD_BACKEND)
    message(STATUS "AssetDownloader: found download backend: "
            "${FAITHFUL_ASSET_DOWNLOADER_DOWNLOAD_BACKEND}")
else ()
    if (FAITHFUL_BUILD_EXTERNAL)
        message(STATUS "AssetDownloader: there is no download backend found, installing cURL")
        add_subdirectory(${CMAKE_SOURCE_DIR}/external/curl
                ${CMAKE_BINARY_DIR}/external/curl EXCLUDE_FROM_ALL)
        set(FAITHFUL_ASSET_DOWNLOADER_DOWNLOAD_BACKEND
                "FAITHFUL_ASSET_PROCESSOR_CURL_LIB" CACHE STRING "" FORCE)
    else ()
        message(FATAL_ERROR "AssetDownloader requires at least one download backed, "
                "which has not been provided. To build external "
                "set flag FAITHFUL_BUILD_EXTERNAL or FAITHFUL_BUILD_EXTERNAL_ALL")
    endif ()
endif ()

target_compile_definitions(AssetDownloader
        PRIVATE ${FAITHFUL_ASSET_DOWNLOADER_DOWNLOAD_BACKEND}
)

target_link_libraries(AssetDownloader
        PUBLIC mimalloc
        PUBLIC miniz
        PUBLIC curl
)

target_include_directories(AssetDownloader
        PUBLIC ${CMAKE_SOURCE_DIR}/external/mimalloc/include
        PUBLIC ${CMAKE_SOURCE_DIR}/external/miniz
        PUBLIC ${CMAKE_SOURCE_DIR}/external/picosha2
)
if (CURL_FOUND)
    target_include_directories(AssetDownloader
            PUBLIC ${CURL_INCLUDE_DIRS}
    )
else ()
    target_include_directories(AssetDownloader
            PUBLIC ${curl_SOURCE_DIR}/include
    )
endif()