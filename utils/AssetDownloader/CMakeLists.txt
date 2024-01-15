cmake_minimum_required(VERSION 3.26)
project(AssetDownloader)

# TODO: if FREEZE_BUILD --> RELEASE

set(CMAKE_CXX_STANDARD 17)

add_executable(AssetDownloader main.cpp)


target_link_libraries(AssetDownloader
        PUBLIC mimalloc
        PUBLIC miniz
)

target_include_directories(AssetDownloader
        PUBLIC ${CMAKE_SOURCE_DIR}/external/mimalloc/include
        PUBLIC ${CMAKE_SOURCE_DIR}/external/miniz
        PUBLIC ${CMAKE_SOURCE_DIR}/external/picosha2
)




if (WIN32)
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
    find_program(CURL_EXECUTABLE NAMES curl)
    if(CURL_EXECUTABLE)
        set(FAITHFUL_ASSET_DOWNLOADER_DOWNLOAD_BACKEND
            "FAITHFUL_ASSET_PROCESSOR_CURL_CLI" CACHE STRING "" FORCE)
    endif()
endif ()

if (NOT FAITHFUL_ASSET_DOWNLOADER_DOWNLOAD_BACKEND)
    find_package(CURL)
    if (CURL_FOUND)
        set(FAITHFUL_ASSET_DOWNLOADER_DOWNLOAD_BACKEND
            "FAITHFUL_ASSET_PROCESSOR_CURL_LIB" CACHE STRING "" FORCE)
    endif()
endif ()


if (FAITHFUL_ASSET_DOWNLOADER_DOWNLOAD_BACKEND)
    message(STATUS "AssetDownloader: found download backend: "
        "${FAITHFUL_ASSET_DOWNLOADER_DOWNLOAD_BACKEND}")
else ()
    message(FATAL_ERROR "AssetDownloader requires at least one download backed, "
        "which has not been provided. Try update your OS or install cURL (SSL/TLS)")
endif ()

if (${FAITHFUL_ASSET_DOWNLOADER_DOWNLOAD_BACKEND} STREQUAL FAITHFUL_ASSET_PROCESSOR_CURL_LIB)
    target_include_directories(AssetDownloader PUBLIC ${CURL_INCLUDE_DIR})
    if (WIN32)
        target_link_libraries(AssetDownloader PUBLIC Ws2_32.lib)
        target_link_libraries(AssetDownloader PUBLIC Wldap32.lib)
    endif ()
    target_link_libraries(AssetDownloader PUBLIC curl)
endif()

target_compile_definitions(AssetDownloader
        PRIVATE ${FAITHFUL_ASSET_DOWNLOADER_DOWNLOAD_BACKEND}
)
