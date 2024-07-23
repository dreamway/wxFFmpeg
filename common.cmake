# Brief: this function finds FFMpeg on Windows
#
# Params:
# FFMPEG_ROOT           (in)    path to directory with FFmpeg build
# FFMPEG_LIBRARIES      (out)   list of FFMpeg libraries
# FFMPEG_DLLS           (out)   list of FFMpeg DLLS. On platforms other then Windows it will be left untouched
# FFMPEG_INCLUDE_DIRS   (out)   list of paths to FFMpeg headers
function(find_FFMpeg FFMPEG_ROOT)

    # Find paths to headers
    set(FFMPEG_INC_DIR ${FFMPEG_ROOT}/include)

    find_path(AVFORMAT_INCLUDE_DIRS 
        libavformat/avformat.h 
        ${FFMPEG_INC_DIR})

    find_path(AVCODEC_INCLUDE_DIRS 
        libavcodec/avcodec.h 
        ${FFMPEG_INC_DIR})
    
    find_path(AVUTIL_INCLUDE_DIRS
        libavutil/avutil.h 
        ${FFMPEG_INC_DIR})

    find_path(SWRESAMPLE_INCLUDE_DIRS
        libswresample/swresample.h 
        ${FFMPEG_INC_DIR})

    find_path(SWSCALE_INCLUDE_DIRS
        libswscale/swscale.h 
        ${FFMPEG_INC_DIR})
    
    find_path(AVDEVICE_INCLUDE_DIRS
        libavdevice/avdevice.h
        ${FFMPEG_INC_DIR})

    find_path(AVFILTER_INCLUDE_DIRS
        libavfilter/avfilter.h
        ${FFMPEG_INC_DIR})

    set(FFMPEG_INCLUDE_DIRS "")
    list (APPEND
        FFMPEG_INCLUDE_DIRS
        ${AVFORMAT_INCLUDE_DIRS}
        ${AVCODEC_INCLUDE_DIRS}
        ${AVUTIL_INCLUDE_DIRS}
        ${SWRESAMPLE_INCLUDE_DIRS}
        ${SWSCALE_INCLUDE_DIRS}
        ${AVDEVICE_INCLUDE_DIRS})

    # Find libraries
    set(FFMPEG_LIB_DIR ${FFMPEG_ROOT}/lib)

    find_library(AVFORMAT_LIBRARIES
        avformat 
        ${FFMPEG_LIB_DIR})

    find_library(AVCODEC_LIBRARIES
        avcodec
        ${FFMPEG_LIB_DIR})

    find_library(AVUTIL_LIBRARIES
        avutil
        ${FFMPEG_LIB_DIR})

    find_library(SWRESAMPLE_LIBRARIES
        swresample
        ${FFMPEG_LIB_DIR})

    find_library(SWSCALE_LIBRARIES
        swscale
        ${FFMPEG_LIB_DIR})

    find_library(AVDEVICE_LIBRARIES
        avdevice
        ${FFMPEG_LIB_DIR})

    find_library(AVFILTER_LIBRARIES
        avfilter
        ${FFMPEG_LIB_DIR})

    set(FFMPEG_LIBRARIES "")
    list (APPEND
        FFMPEG_LIBRARIES
        ${AVFORMAT_LIBRARIES}
        ${AVCODEC_LIBRARIES}
        ${AVUTIL_LIBRARIES}
        ${SWRESAMPLE_LIBRARIES}
        ${SWSCALE_LIBRARIES}
        ${AVDEVICE_LIBRARIES}
        ${AVFILTER_LIBRARIES})

    if (WIN32)
        # Find FFMpeg DLLs
        set (FFMPEG_DLLS "")
        file(GLOB FFMPEG_DLLS "${FFMPEG_ROOT}/bin/*.dll")
    endif (WIN32)

    # Promote to parent scope
    set (FFMPEG_INCLUDE_DIRS    ${FFMPEG_INCLUDE_DIRS}  PARENT_SCOPE)
    set (FFMPEG_LIBRARIES       ${FFMPEG_LIBRARIES}     PARENT_SCOPE)
    set (FFMPEG_DLLS            ${FFMPEG_DLLS}          PARENT_SCOPE)
    message(INFO "FFMPEG_INCLUDE_DIRS ${FFMPEG_INCLUDE_DIRS}")
    message(INFO "FFMPEG_LIBRARIES ${FFMPEG_LIBRARIES}")
    message(INFO "FFMPEG_DLLS ${FFMPEG_DLLS}")
endfunction()


function(find_GLEW GLEW_ROOT)
    set(GLEW_INC_DIR ${GLEW_ROOT}/include)
    set(GLEW_INCLUDE_DIRS ${GLEW_ROOT}/include)
    set(GLEW_LIB_DIR ${GLEW_ROOT}/lib/Release/x64)
    find_library(GLEW_LIBRARIES glew32 ${GLEW_LIB_DIR})

    #promote to parent scope
    set(GLEW_LIBRARIES ${GLEW_LIBRARIES} PARENT_SCOPE)
    message(INFO "GLEW_INCLUDE_DIRS ${GLEW_INCLUDE_DIRS}")
    message(INFO "GLEW_LIBRARIES ${GLEW_LIBRARIES}")
endfunction()