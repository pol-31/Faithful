cmake_minimum_required(VERSION 3.26)

message(STATUS "vorbis building")

set(BUILD_SHARED_LIBS OFF CACHE INTERNAL)

add_subdirectory(vorbis)

target_include_directories(vorbisenc INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/vorbis/include")
target_include_directories(vorbisfile INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/vorbis/include")