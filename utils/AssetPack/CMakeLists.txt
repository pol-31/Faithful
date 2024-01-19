cmake_minimum_required(VERSION 3.26)
project(AssetPack)

set(CMAKE_CXX_STANDARD 17)

add_executable(AssetPack main.cpp)
target_compile_options(AssetPack PRIVATE ${FAITHFUL_COMPILE_OPTIONS})
target_link_options(AssetPack PRIVATE ${FAITHFUL_EXE_LINK_OPTIONS})

target_link_libraries(AssetPack
        PRIVATE mimalloc-static
        PRIVATE miniz
)

target_include_directories(AssetPack
        PRIVATE ${FAITHFUL_SOURCE_DIR}/external/mimalloc/include
        PRIVATE ${FAITHFUL_SOURCE_DIR}/external/miniz
        PRIVATE ${FAITHFUL_SOURCE_DIR}/external/picosha2
)
