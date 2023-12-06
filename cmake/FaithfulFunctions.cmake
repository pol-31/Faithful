cmake_minimum_required(VERSION 3.0)

# there should be useful utilities, functions, variables

set(FAITHFUL_DEPS_FOUND true) # PARENT_SCOPE

function(lib_found_info libName libraryIncludeDir Library)
    if (libraryIncludeDir AND Library)
        set(${libName}_FOUND true PARENT_SCOPE)
        message(STATUS "${libName}: ${Library}")
    else()
        set(${libName}_FOUND false PARENT_SCOPE)
        set(FAITHFUL_DEPS_FOUND false PARENT_SCOPE)
        message(STATUS "${libName}: __not_found__")
    endif()
endfunction()

function(header_found_info libName libraryIncludeDir)
    if (libraryIncludeDir AND LIBRARY)
        set(${libName}_FOUND true PARENT_SCOPE)
        message(STATUS "${libName}: ${libraryIncludeDir}")
    else()
        set(${libName}_FOUND false PARENT_SCOPE)
        set(FAITHFUL_DEPS_FOUND false PARENT_SCOPE)
        message(STATUS "${libName}: __not_found__")
    endif()
endfunction()
