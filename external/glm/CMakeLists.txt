cmake_minimum_required(VERSION 3.26)

message(STATUS "glm")

add_library(glm INTERFACE)
target_include_directories(glm INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/glm")

create_module(glm $<TARGET_LINKER_FILE:glm> "")