set(FAITHFUL_MSVS_MIN_VERSION 14.0)

message(STATUS "Faithful project requires MSVS 14.0")
if(MSVC_VERSION LESS FAITHFUL_MSVS_MIN_VERSION)
    message(FATAL_ERROR
            "Old version of Microsoft Visual Studio: MSVC ${MSVC_VERSION}, "
            "required MSVS ${FAITHFUL_MSVS_MIN_VERSION} (MSVS 2015) or newer."
    )
endif()

set(FAITHFUL_COMPILE_OPTIONS ${CMAKE_CXX_FLAGS} /W3 /O2 /MP
        CACHE STRING "Faithful compile options")
set(FAITHFUL_EXE_LINK_OPTIONS ${CMAKE_EXE_LINKER_FLAGS} /STACK:8388608
        CACHE STRING "Faithful link options")

if(FAITHFUL_RELEASE)
    set(FAITHFUL_COMPILE_OPTIONS ${FAITHFUL_COMPILE_OPTIONS} /Ox /GL /MT
            CACHE STRING "Faithful release compile options (additionally)")
    set(FAITHFUL_EXE_LINK_OPTIONS
            ${FAITHFUL_EXE_LINK_OPTIONS}
            /LTCG CACHE STRING "Faithful release link options (additionally)")
else() # DEBUG
    set(FAITHFUL_COMPILE_OPTIONS ${FAITHFUL_COMPILE_OPTIONS} /Zi /MTd
            CACHE STRING "Faithful debug compile options (additionally)")
endif()

set(FAITHFUL_ASAN_COMPILE_FLAGS /fsanitize=address /Zi /DEBUG:FULL)
set(FAITHFUL_ASAN_LINK_FLAGS /fsanitize=address /Zi /DEBUG:FULL)

if(FAITHFUL_ASAN)
    message(STATUS "Sanitize with Address Sanitizer")
    set(FAITHFUL_COMPILE_OPTIONS
            ${FAITHFUL_ASAN_COMPILE_FLAGS}
            ${FAITHFUL_COMPILE_OPTIONS})
    set(FAITHFUL_LINK_OPTIONS
            ${FAITHFUL_ASAN_LINK_FLAGS}
            ${FAITHFUL_EXE_LINK_OPTIONS})
endif()

if(FAITHFUL_TSAN)
    message(WARNING "There is no TSAN support for MSVS by now")
endif()
