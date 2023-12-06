cmake_minimum_required(VERSION 3.26)

message(STATUS "glad building")

add_library(glad STATIC glad/src/glad.c)
target_include_directories(glad PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/glad/include")
