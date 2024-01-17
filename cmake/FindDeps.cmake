include(${CMAKE_SOURCE_DIR}/cmake/FindExternalMacros.cmake)

find_glfw()
find_glm()
find_libogg()
find_mimalloc()
find_openal_soft()
find_opengl()
find_rapidjson()
find_vorbis()

if (FAITHFUL_BUILD_TESTS)
    find_gtest()
endif()
