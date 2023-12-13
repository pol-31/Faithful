
find_path(GTEST_INCLUDE_DIR NAMES gtest/gtest.h)
find_library(GTEST_LIBRARY NAMES gtest gtest_main)

include(${CMAKE_SOURCE_DIR}/cmake/FaithfulFunctions.cmake)
lib_found_info(GTEST GTEST_INCLUDE_DIR GTEST_LIBRARY)

mark_as_advanced(GTEST_INCLUDE_DIR GTEST_LIBRARY)
