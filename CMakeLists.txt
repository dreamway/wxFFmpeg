cmake_minimum_required(VERSION 3.20)

project(money)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED yes)


find_package(wxWidgets 3.0 REQUIRED core base gl)
include(${wxWidgets_USE_FILE})

#FFmpeg
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set(FFMPEG_PKG_CONFIG_SUFFIX "" CACHE STRING
    "This suffix uses for FFmpeg component names searches by pkg-config"
    )

#ffmpeg compile
if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    include(common.cmake)
    find_FFMpeg("D:\\DevTools\\FFmpeg")
    find_GLEW("D:\\DevTools\\glew")
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
    find_package(FFmpeg
        COMPONENTS AVCODEC AVFORMAT AVUTIL AVDEVICE AVFILTER SWSCALE SWRESAMPLE
        REQUIRED)
endif()

add_subdirectory(src)
add_subdirectory(example)

#execute_process(COMMAND make
#    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/dep/glew/auto
#    )
#add_subdirectory(dep/glew/build/cmake)

add_subdirectory(dep/openal-soft)
