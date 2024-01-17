# GCC and Clang compilers

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(REQUIRED_COMPILER_NAME "Clang")
    set(REQUIRED_COMPILER_VERSION 13.0)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(REQUIRED_COMPILER_NAME "GCC")
    set(REQUIRED_COMPILER_VERSION 6.1)
else()
    message(FATAL_ERROR
            "Unsupported compiler: ${CMAKE_CXX_COMPILER_ID}. Use Clang or GCC."
    )
endif()

message(STATUS "Faithful project requires ${REQUIRED_COMPILER_NAME} ${REQUIRED_COMPILER_VERSION}")

if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS REQUIRED_COMPILER_VERSION)
    message(FATAL_ERROR
            "Old version of ${REQUIRED_COMPILER_NAME} compiler: ${CMAKE_CXX_COMPILER_VERSION}, required ${REQUIRED_COMPILER_VERSION}."
    )
endif()



if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g)
endif()

add_compile_options(-Werror -Wall -Wextra -Wpedantic -fno-omit-frame-pointer -flto)

set(ASAN_COMPILE_FLAGS -fsanitize=address,undefined -fno-sanitize-recover=all)
set(ASAN_LINK_FLAGS -fsanitize=address,undefined)

set(TSAN_COMPILE_FLAGS -fsanitize=thread -fno-sanitize-recover=all)
set(TSAN_LINK_FLAGS -fsanitize=thread)

if(ASAN)
    message(STATUS "Sanitize with Address Sanitizer")
    add_compile_options(${ASAN_COMPILE_FLAGS})
    add_link_options(${ASAN_LINK_FLAGS})
endif()

if(TSAN)
    message(STATUS "Sanitize with Thread Sanitizer")
    add_compile_options(${TSAN_COMPILE_FLAGS})
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${TSAN_LINK_FLAGS}")
endif()
