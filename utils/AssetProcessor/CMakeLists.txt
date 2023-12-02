cmake_minimum_required(VERSION 3.26)
project(AssetProcessor)

set(CMAKE_CXX_STANDARD 17)

# TODO: add default paths for output folder, external headers/libs

add_definitions(-DASSET_PROCESSOR)


include_directories("/usr/local/include")
include_directories("/usr/include")


add_executable(AssetProcessor main.cpp
        ImageProcessor.cpp
        ImageProcessor.h)

find_package(OpenAL REQUIRED)
target_link_libraries(AssetProcessor PRIVATE OpenAL::OpenAL)

#find_package(FFmpeg REQUIRED COMPONENTS AVCODEC AVFORMAT)

#target_include_directories(your_target PRIVATE ${FFMPEG_INCLUDE_DIRS})
#target_link_libraries(your_target PRIVATE ${FFMPEG_LIBRARIES})


target_include_directories(AssetProcessor PRIVATE ${FFMPEG_INCLUDE_DIRS})
target_include_directories(AssetProcessor PRIVATE /usr/local/include)
target_include_directories(AssetProcessor PRIVATE /usr/include)

target_link_libraries(AssetProcessor PRIVATE ${FFMPEG_LIBRARIES})
target_link_libraries(AssetProcessor PRIVATE /usr/local/lib/GLFW/libglfw3.a)
target_link_libraries(AssetProcessor PRIVATE /usr/lib/x86_64-linux-gnu/libavformat.so)
target_link_libraries(AssetProcessor PRIVATE /usr/lib/x86_64-linux-gnu/libavcodec.so)
target_link_libraries(AssetProcessor PRIVATE /usr/lib/x86_64-linux-gnu/libavutil.so)
target_link_libraries(AssetProcessor PRIVATE /usr/lib/x86_64-linux-gnu/libaudio.so)
target_link_libraries(AssetProcessor PRIVATE /usr/local/lib/libassimp.so)

# i have to find and include: FIND_PACKAGES (?)