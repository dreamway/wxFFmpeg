add_library(wxFFmpeg SHARED
    wxFFmpegView.cc
    #GLManager.cc
    glmanager.cc
    shader.cc
    #Movie.cc
    movie.cc
    #Audio.cc
    audio.cc
    #Video.cc
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
