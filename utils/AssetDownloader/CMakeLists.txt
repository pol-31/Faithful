cmake_minimum_required(VERSION 3.26)
project(AssetDownloader)

# TODO: if FREEZE_BUILD --> RELEASE

set(CMAKE_CXX_STANDARD 17)

add_executable(AssetDownloader main.cpp)

target_link_libraries(AssetDownloader
        PUBLIC mimalloc
        PUBLIC miniz
        PUBLIC curl
)

target_include_directories(AssetDownloader
#        PUBLIC ${CMAKE_SOURCE_DIR}/external/mimalloc/include
        PUBLIC ${CMAKE_SOURCE_DIR}/external/miniz
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