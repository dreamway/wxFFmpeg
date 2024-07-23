if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_library(wxFFmpeg STATIC
        wxFFmpegView.cc
        glmanager.cc
        shader.cc
        movie.cc
        audio.cc
        video.cc
        )

    include_directories(${GLEW_INCLUDE_DIRS})
    target_include_directories(wxFFmpeg
    PRIVATE
    ${GLEW_INC_DIR}
    PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    )
    link_directories(${GLEW_LIB_DIR})

    target_link_libraries(wxFFmpeg PUBLIC
    ${wxWidgets_LIBRARIES}
    ${FFMPEG_LIBRARIES}
    ${GLEW_LIBRARIES}
    OpenAL
    )
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
    add_library(wxFFmpeg SHARED
        wxFFmpegView.cc
        glmanager.cc
        shader.cc
        movie.cc
        audio.cc
        video.cc
        )

    target_include_directories(wxFFmpeg
        PRIVATE
        #${CMAKE_SOURCE_DIR}/dep/glew/include
        /usr/include
        PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        )
    link_directories(/usr/lib64)

    target_link_libraries(wxFFmpeg PUBLIC
        ${wxWidgets_LIBRARIES}
        FFmpeg::FFmpeg
        GLEW
        OpenAL
        X11
        )
endif()