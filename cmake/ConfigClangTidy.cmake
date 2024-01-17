find_program(CLANG_TIDY_EXE NAMES "clang-tidy" DOC "Path to clang-tidy executable")
if(CLANG_TIDY_EXE)
    message(STATUS "Enable clang-tidy for ${dirName}")
    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}" CACHE INTERNAL "")
else ()
    message(WARNING "clang-tidy for Faithful can't be run (no clang-tidy found)")
endif()

# TODO: customize .clang-tidy for CMake (separate CMake & IDE checking to speed up building)
