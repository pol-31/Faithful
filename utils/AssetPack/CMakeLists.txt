cmake_minimum_required(VERSION 3.26)
project(AssetPack)

# TODO: if FREEZE_BUILD --> RELEASE

set(CMAKE_CXX_STANDARD 17)

add_executable(AssetPack main.cpp)

target_link_libraries(AssetPack
        PUBLIC mimalloc-static
        PUBLIC miniz
)

target_include_directories(AssetPack
        PUBLIC ${FAITHFUL_SOURCE_DIR}/external/mimalloc/include
        PUBLIC ${FAITHFUL_SOURCE_DIR}/external/miniz
        PUBLIC ${FAITHFUL_SOURCE_DIR}/external/picosha2
)
