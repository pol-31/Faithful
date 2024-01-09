include(${CMAKE_SOURCE_DIR}/cmake/FindExternalMacros.cmake)

if (UNIX AND NOT APPLE)
    find_audio_backend()
endif ()

find_glfw()
find_glm()
find_libogg()
find_mimalloc()
find_openal_soft()
find_opengl()
find_rapidjson()
find_vorbis()

if (FAITHFUL_DOWNLOAD_ASSETS)
    find_curl()
endif ()
