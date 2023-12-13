
find_path(CURL_INCLUDE_DIRS NAMES curl/curl.h)
find_library(CURL_LIBRARIES NAMES curl)

include(${CMAKE_SOURCE_DIR}/cmake/FaithfulFunctions.cmake)
lib_found_info(CURL CURL_INCLUDE_DIRS CURL_LIBRARIES)

mark_as_advanced(CURL_INCLUDE_DIRS CURL_LIBRARIES)
