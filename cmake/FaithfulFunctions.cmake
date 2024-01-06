
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


function(target_include_main_deps target)
    # TODO:
endfunction()

function(target_include_asset_downloader_deps target)
    target_include_directories(target curl-build)
endfunction()

function(target_include_asset_processor_deps target)
    # TODO:
endfunction()


function(target_link_main_deps target)
    target_link_libraries(target alsa)
    target_link_libraries(target astc-encoder) # TODO: can missing
    # TODO:
endfunction()

function(target_link_asset_downloader_deps target)
    # TODO:
endfunction()

function(target_link_asset_processor_deps target)
    # TODO:
endfunction()
