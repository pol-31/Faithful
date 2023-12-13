cmake_minimum_required(VERSION 3.26)

message(STATUS "glad building")

set(GLAD_BINARY_DIR ${CMAKE_BINARY_DIR}/glad-build CACHE INTERNAL "glad binary directory" FORCE)

set(GLAD_OUT_DIR "${GLAD_BINARY_DIR}" CACHE STRING "Output directory" FORCE)
set(GLAD_INSTALL OFF CACHE BOOL "Generate installation target" FORCE)
set(GLAD_GENERATOR "c" CACHE STRING "Language to generate the binding for" FORCE)

file(READ "${CMAKE_SOURCE_DIR}/config/opengl_config.txt" CONFIG_CONTENT)
string(REGEX REPLACE "\n" ";" CONFIG_LINES "${CONFIG_CONTENT}")

foreach(line ${CONFIG_LINES})
    string(REGEX MATCH "^([^,]+),([^,]+),([^,]+)$" _match ${line})
    set(key ${CMAKE_MATCH_1})
    set(type ${CMAKE_MATCH_2})
    set(value ${CMAKE_MATCH_3})
    if(type STREQUAL "STRING")
        set(${key} "${value}" CACHE STRING "Glad/OpenGL properties (see Faithful/config/opengl_config.txt)" FORCE)
    elseif(type STREQUAL "BOOL")
        set(${key} ${value} CACHE BOOL "" FORCE)
    endif()
endforeach()



message("GLAD_API: ${GLAD_API}")
message("GLAD_PROFILE: ${GLAD_PROFILE}")
message("GLAD_ALL_EXTENSIONS: ${GLAD_ALL_EXTENSIONS}")


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${GLAD_BINARY_DIR} CACHE INTERNAL "glad executable output directory" FORCE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${GLAD_BINARY_DIR} CACHE INTERNAL "glad library output directory" FORCE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${GLAD_BINARY_DIR} CACHE INTERNAL "glad archive output directory" FORCE)

set(GLAD_OUT_DIR "${GLAD_BINARY_DIR}" CACHE STRING "Output directory" FORCE)


add_subdirectory(glad)
