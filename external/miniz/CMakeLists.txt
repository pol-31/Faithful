cmake_minimum_required(VERSION 3.26)

message(STATUS "miniz building")

# there is an issue with CMake building of miniz git submodule:
# https://github.com/richgel999/miniz/issues/152
# so we just installed it from miniz release page

add_library(miniz INTERFACE miniz.c)
