
# todo: dep-names to lists (maybe) (now is too inconvenient to add/remove deps)

set(FAITHFUL_DEPS_FOUND ON CACHE BOOL PARENT_SCOPE FORCE)

function(lib_found_info libName libraryIncludeDir Library)
    message(STATUS "1 - ${FAITHFUL_DEPS_FOUND}")
    if (${libraryIncludeDir} AND ${Library})
        set(${libName}_FOUND true PARENT_SCOPE)
        message(STATUS "${libName}: ${${Library}}")
    else()
        set(${libName}_FOUND false PARENT_SCOPE)
        set(FAITHFUL_DEPS_FOUND OFF CACHE BOOL PARENT_SCOPE FORCE)
        message(STATUS "${libName}: __not_found__")
    endif()
    message(STATUS "2 - ${FAITHFUL_DEPS_FOUND}")
endfunction()

function(header_found_info libName libraryIncludeDir)
    message(STATUS "1 - ${FAITHFUL_DEPS_FOUND}")
    if (${libraryIncludeDir})
        set(${libName}_FOUND true PARENT_SCOPE)
        message(STATUS "${libName}: ${${libraryIncludeDir}}")
    else()
        set(${libName}_FOUND false PARENT_SCOPE)
        set(FAITHFUL_DEPS_FOUND OFF CACHE BOOL PARENT_SCOPE FORCE)
        message(STATUS "${libName}: __not_found__")
    endif()
    message(STATUS "2 - ${FAITHFUL_DEPS_FOUND}")
endfunction()


function(external_find_or_compile name findName)
    if(FAITHFUL_DEPS_FOUND)
        include(${CMAKE_SOURCE_DIR}/cmake/Find${findName}.cmake)
        if(FAITHFUL_DEPS_FOUND)
            # after missing - by default it set to false,
            #  so we saying "now all is nice"
            set(FAITHFUL_DEPS_FOUND ON CACHE BOOL PARENT_SCOPE FORCE)
        else()
            add_subdirectory(${CMAKE_SOURCE_DIR}/external/${name})
        endif()
    else()
        include(${CMAKE_SOURCE_DIR}/cmake/Find${findName}.cmake)
        if(NOT FAITHFUL_DEPS_FOUND)
            add_subdirectory(${CMAKE_SOURCE_DIR}/external/${name})
        endif()
    endif()
endfunction()

function(faithful_build_external)
    external_find_or_compile(astc-encoder Astcenc)
    external_find_or_compile(cgltf Cgltf)
    external_find_or_compile(freetype Freetype)
    external_find_or_compile(glad Glad)
    external_find_or_compile(glfw GLFW)
    external_find_or_compile(glm Glm)
    external_find_or_compile(vorbis Libvorbis)
    external_find_or_compile(openal-soft OpenALspec)
    include(${CMAKE_SOURCE_DIR}/cmake/FindOpenGLspec.cmake)
endfunction()

function(faithful_asset_downloader_build_external)
    external_find_or_compile(curl Curl)
endfunction()

function(faithful_asset_processor_build_external)
    external_find_or_compile(astc-encoder Astcenc)
    external_find_or_compile(assimp Assimp)
    external_find_or_compile(cgltf Cgltf)
    external_find_or_compile(glm Glm)
endfunction()


function(target_include_main_deps target)
    target_include_directories(target ${CMAKE_BINARY_DIR}/astc-encoder-build)
    target_include_directories(target ${CMAKE_BINARY_DIR}/cgltf-build)
    target_include_directories(target ${CMAKE_BINARY_DIR}/freetype-build)
    target_include_directories(target ${CMAKE_BINARY_DIR}/glad-build)
    target_include_directories(target ${CMAKE_BINARY_DIR}/glfw-build)
    target_include_directories(target ${CMAKE_BINARY_DIR}/glm-build)
    target_include_directories(target ${CMAKE_BINARY_DIR}/vorbis-build)
    target_include_directories(target ${CMAKE_BINARY_DIR}/openal-soft-build)
    target_include_directories(target ${CMAKE_BINARY_DIR}/opengl-build)
endfunction()

function(target_include_asset_downloader_deps target)
    target_include_directories(target curl-build)
endfunction()

function(target_include_asset_processor_deps target)
    target_include_directories(target astc-encoder-build)
    target_include_directories(target assimp-build)
    target_include_directories(target cgltf-build)
    target_include_directories(target glm-build)
endfunction()


function(target_link_main_deps target)
    target_link_libraries(target astc-encoder)
    target_link_libraries(target cgltf)
    target_link_libraries(target freetype)
    target_link_libraries(target glad)
    target_link_libraries(target glfw)
    target_link_libraries(target glm)
    target_link_libraries(target vorbis)
    target_link_libraries(target openal-soft)
    target_link_libraries(target opengl)
endfunction()

function(target_link_asset_downloader_deps target)
    target_link_libraries(target curl)
endfunction()

function(target_link_asset_processor_deps target)
    target_link_libraries(target astc-encoder)
    target_link_libraries(target assimp)
    target_link_libraries(target cgltf)
    target_link_libraries(target glm)
endfunction()
