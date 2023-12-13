cmake_minimum_required(VERSION 3.26)

message(STATUS "assimp building")

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build Assimp shared libraries" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "Install Assimp" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "Build Assimp tools" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "Build Assimp tests" FORCE)
set(ASSIMP_BUILD_ASSIMP_VIEW OFF CACHE BOOL "Build Assimp view tool" FORCE)
set(DASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "Assimp warnings as errors" FORCE)


# Enabled by default: COLLADA, OBJ, PLY, BLEND, FBX, GLTF
# We are going to decode each to GLTF, but Assimp doesn't support
# flexible building options for this, so Assimp only import (for export - Cgltf lib)
foreach(name BVH X3D 3DS IQM M3D X AMF AC ASE ASSBIN ASSXML B3D DXF CSM HMP IRRMESH IRR LWO LWS MD2 MD3 MD5 MDC MDL NFF NDO OFF OGRE OPENGEX MS3D COB IFC XGL Q3D Q3BSP RAW SIB SMD TERRAGEN 3D 3MF MMD STEP)
    option(ASSIMP_BUILD_${name}_IMPORTER "Build ${name} importer" OFF)
endforeach()

#Enabled by default: GLTF
foreach(name COLLADA OBJ OPENGEX PLY 3DS ASSBIN ASSXML M3D FBX STL X X3D 3MF PBRT ASSJSON STEP)
    option(ASSIMP_BUILD_${name}_EXPORTER "Build ${name} exporter" OFF)
endforeach()

set(ASSIMP_BINARY_DIR ${CMAKE_BINARY_DIR}/assimp-build CACHE INTERNAL "assimp binary directory" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ASSIMP_BINARY_DIR} CACHE INTERNAL "assimp executable output directory" FORCE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${ASSIMP_BINARY_DIR} CACHE INTERNAL "assimp library output directory" FORCE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${ASSIMP_BINARY_DIR} CACHE INTERNAL "assimp archive output directory" FORCE)

add_subdirectory(assimp)
