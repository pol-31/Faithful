cmake_minimum_required(VERSION 3.26)

project(faithful-core)

add_executable(faithful-core)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE SOURCE_FILES ${SOURCE_DIR}/*.cpp)

target_sources(faithful-core PRIVATE ${SOURCE_FILES})

target_link_libraries(faithful-core PUBLIC OpenAL)
target_include_directories(faithful-core
        PRIVATE ${SOURCE_DIR}
        PUBLIC ${CMAKE_SOURCE_DIR}/external/openal-soft/openal-soft/include
)



