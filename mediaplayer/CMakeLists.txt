cmake_minimum_required(VERSION 3.20)

project(mediaplayer)


find_package(wxWidgets 3.0 REQUIRED core base gl media)
include(${wxWidgets_USE_FILE})
if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    include(common.cmake)
    find_FFMpeg("D:\\DevTools\\FFmpeg")
    find_GLEW("D:\\DevTools\\glew")
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
    find_package(FFmpeg
        COMPONENTS AVCODEC AVFORMAT AVUTIL AVDEVICE AVFILTER SWSCALE SWRESAMPLE
        REQUIRED)
endif()
message(${wxWidgets_LIBRARIES})


include_directories(${wxWidgets_INCLUDE_DIRS})

add_executable(mediaplayer mediaplayer.cpp)

target_link_libraries(mediaplayer PRIVATE
    ${wxWidgets_LIBRARIES}
    wxFFmpeg
)

