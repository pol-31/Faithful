macro(find_deps_main)
    include(${CMAKE_SOURCE_DIR}/cmake/FindAstcenc.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/FindCgltf.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/FindFreetype.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/FindGlad.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/FindGLFW.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/FindGlm.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/FindLibvorbis.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/FindLibvpx.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/FindOpenALspec.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/FindOpenGLspec.cmake)
endmacro(find_deps_main)

macro(find_deps_asset_processor)
    include(${CMAKE_SOURCE_DIR}/cmake/FindAstcenc.cmake) # texture processing
    include(${CMAKE_SOURCE_DIR}/cmake/FindAssimp.cmake) # models import
    include(${CMAKE_SOURCE_DIR}/cmake/FindCgltf.cmake) # models export
    include(${CMAKE_SOURCE_DIR}/cmake/FindGlm.cmake)
endmacro(find_deps_asset_processor)

macro(find_deps_asset_downloader)
    include(${CMAKE_SOURCE_DIR}/cmake/FindCurl.cmake)
endmacro(find_deps_asset_downloader)