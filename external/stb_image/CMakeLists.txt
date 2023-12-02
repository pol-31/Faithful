cmake_minimum_required(VERSION 3.26)

message(STATUS "stb")

file(GLOB stb-sources "stb/*.h")
add_library(stb STATIC stb.c ${stb-sources})
target_include_directories(stb PUBLIC stb)