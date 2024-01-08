set(REQUIRED_CXX_COMPILER "Clang")
set(CXX_COMPILER_MIN_VERSION 13.0)

message(STATUS "-- requires Clang 13.0 --")

# the most severe flags... (see clang-tidy for details)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL REQUIRED_CXX_COMPILER)
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS CXX_COMPILER_MIN_VERSION)
        message(FATAL_ERROR
                "Old version of ${REQUIRED_CXX_COMPILER} compiler: ${CMAKE_CXX_COMPILER_VERSION}, required ${CXX_COMPILER_MIN_VERSION}."
        )
    endif()
else()
    message(FATAL_ERROR
            "Unsupported compiler: ${CMAKE_CXX_COMPILER_ID}. Use ${REQUIRED_CXX_COMPILER}, version >= ${CXX_COMPILER_MIN_VERSION}."
    )
endif()

# TODO: -g, -flto, _______________-
#add_compile_options(-Werror -Wall -Wextra -Wpedantic -g -fno-omit-frame-pointer)

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
