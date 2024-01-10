cmake_minimum_required(VERSION 3.26)

file(GLOB_RECURSE faithful-test-src-sources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/test_src/*.cpp)
add_executable(faithful-test-src ${faithful-test-src-sources})
target_link_libraries(faithful-test-src faithful-core)

if (FAITHFUL_DOWNLOAD_ASSETS)
    file(GLOB_RECURSE faithful-test-assets-sources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/test_assets/*.cpp)
    add_executable(faithful-test-assets ${faithful-test-assets-sources})
    target_link_libraries(faithful-test-assets faithful-core)
endif()

if(FAITHFUL_BUILD_ASSET_PROCESSOR)
    file(GLOB_RECURSE faithful-test-utils-sources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/test_utils/*.cpp)
    add_executable(faithful-test-utils ${faithful-test-utils-sources})
    target_link_libraries(faithful-test-utils faithful-util)
endif()