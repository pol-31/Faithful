# GCC and Clang compilers

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(REQUIRED_COMPILER_NAME "Clang")
    set(REQUIRED_COMPILER_VERSION 13.0)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(REQUIRED_COMPILER_NAME "GCC")
    set(REQUIRED_COMPILER_VERSION 6.1)
endif()

message(STATUS "Faithful project requires ${REQUIRED_COMPILER_NAME} ${REQUIRED_COMPILER_VERSION}")

if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS REQUIRED_COMPILER_VERSION)
    message(FATAL_ERROR
            "Old version of ${REQUIRED_COMPILER_NAME} compiler: ${CMAKE_CXX_COMPILER_VERSION}, required ${REQUIRED_COMPILER_VERSION}."
    )
endif()

set(FAITHFUL_COMPILE_OPTIONS ${CMAKE_CXX_FLAGS}
        -Wall -Wextra
        -Wpedantic -fno-omit-frame-pointer # TODO: -flto
        CACHE STRING "Faithful compile options")
set(FAITHFUL_EXE_LINK_OPTIONS ${CMAKE_EXE_LINKER_FLAGS}
        CACHE STRING "Faithful link options")
if(NOT FAITHFUL_RELEASE)
    set(FAITHFUL_COMPILE_OPTIONS -g ${FAITHFUL_COMPILE_OPTIONS}
            CACHE STRING "Faithful debug compile options (additionally)")
endif()


set(FAITHFUL_ASAN_COMPILE_FLAGS -fsanitize=address,undefined -fno-sanitize-recover=all)
set(FAITHFUL_ASAN_LINK_FLAGS -fsanitize=address,undefined)

set(FAITHFUL_TSAN_COMPILE_FLAGS -fsanitize=thread -fno-sanitize-recover=all)
set(FAITHFUL_TSAN_LINK_FLAGS -fsanitize=thread)

if(FAITHFUL_ASAN)
    message(STATUS "Sanitize with Address Sanitizer")
    set(FAITHFUL_COMPILE_OPTIONS
            ${FAITHFUL_COMPILE_OPTIONS}
            ${FAITHFUL_ASAN_COMPILE_FLAGS})
    set(FAITHFUL_LINK_OPTIONS
            ${FAITHFUL_EXE_LINK_OPTIONS}
            ${FAITHFUL_ASAN_LINK_FLAGS})
endif()

if(FAITHFUL_TSAN)
    message(STATUS "Sanitize with Thread Sanitizer")
    set(FAITHFUL_COMPILE_OPTIONS
            ${FAITHFUL_COMPILE_OPTIONS}
            ${FAITHFUL_TSAN_COMPILE_FLAGS})
    set(FAITHFUL_LINK_OPTIONS
            ${FAITHFUL_EXE_LINK_OPTIONS}
            ${FAITHFUL_TSAN_LINK_FLAGS})
endif()
