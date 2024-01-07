cmake_minimum_required(VERSION 3.26)

project(faithful-utils)

file(GLOB HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(faithful-utils ${SOURCE_FILES} ${HEADER_FILES})


# ASSET_PROCESSOR
#####################
#    add_subdirectory(utils/AssetProcessor)
#    TARGET Faithful POST_BUILD

#    add_custom_target(AssetProcessorRun
#            COMMAND $<TARGET_FILE:AssetProcessor> "/home/pavlo/Desktop/test"
#            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
#            COMMENT "Running AssetProcessor..."
#            DEPENDS AssetProcessor external/rapidjson tinygltf
#    )
#    add_dependencies(Faithful AssetProcessorRun)


option(FAITHFUL_ASSET_DOWNLOADER_DOWNLOADED OFF)
set(FAITHFUL_ASSET_DOWNLOADER_MINIZ_LIB_PATH MINIZ_LIBRARIES)
set(FAITHFUL_ASSET_DOWNLOADER_MINIZ_INCLUDE_PATH MINIZ_INCLUDE_DIRS)
set(FAITHFUL_ASSET_DOWNLOADER_CURL_LIB_PATH ${CURL_LIBRARIES})
set(FAITHFUL_ASSET_DOWNLOADER_CURL_INCLUDE_PATH ${CURL_INCLUDE_DIRS})

# ASSET_DOWNLOADER
#####################
#
#    include(ExternalProject)
#    ExternalProject_Add(
#            AssetDownloader
#            PREFIX ${CMAKE_BINARY_DIR}
#            SOURCE_DIR ${CMAKE_SOURCE_DIR}/utils/AssetDownloader
#            BINARY_DIR ${CMAKE_BINARY_DIR}/temp/AssetDownloader
#            CONFIGURE_COMMAND ${CMAKE_COMMAND}
#            ${CMAKE_SOURCE_DIR}/utils/AssetDownloader
#            BUILD_COMMAND ${CMAKE_COMMAND} --build .
#            INSTALL_COMMAND ${CMAKE_COMMAND} -E echo "Skipping install step"
#            BUILD_BYPRODUCTS ${CMAKE_BINARY_DIR}/temp/AssetDownloader/AssetDownloader
#    )
#    add_custom_target(RunSubproject
#            COMMAND ${CMAKE_BINARY_DIR}/temp/AssetDownloader/AssetDownloader
#            "${FAITHFUL_ASSET_LOG_PATH}"
#            "${FAITHFUL_ASSETDOWNLOADER_SOURCE_URL}"
#            COMMENT "Running Subproject before building Main Project"
#            DEPENDS AssetDownloader
#    )
#    add_dependencies(Faithful RunSubproject)
#####################


# todo; clean up curl(if was installed during build)
