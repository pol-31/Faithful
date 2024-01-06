cmake_minimum_required(VERSION 3.26)

project(faithful-core)

file(GLOB_RECURSE FAITHFUL_MAIN_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(faithful-core ${FAITHFUL_MAIN_SOURCES})

include(../cmake/FaithfulFunctions.cmake)
target_link_main_deps(faithful-core)
target_include_main_deps(faithful-core)

#add_executable(Faithful src/main.cpp
#        src/io/Window.cpp
#        src/io/Monitor.cpp
#        src/loader/ShaderProgram.cpp
#)
