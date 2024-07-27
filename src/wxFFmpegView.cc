#include "wxFFmpegView.h"

#include <GL/glew.h>
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/glcanvas.h>

#include "glmanager.h"
#include "movie.h"
#include <atomic>

class wxFFmpegInnerView : public wxGLCanvas {
public:
#if wxCHECK_VERSION(3, 1, 5)
    wxFFmpegInnerView(wxWindow *parent,
                      const wxGLAttributes &dispAttrs,
                      wxWindowID winid = wxID_ANY,
                      const wxPoint &pos = wxDefaultPosition,
                      const wxSize &size = wxDefaultSize,
                      long style = 0,
                      const wxString &name = wxGLCanvasName,
                      const wxPalette &palette = wxNullPalette);
#else
    wxFFmpegInnerView(wxWindow *parent,
                      wxWindowID winid = wxID_ANY,
                      const int *attribList = NULL,
                      const wxPoint &pos = wxDefaultPosition,
                      const wxSize &size = wxDefaultSize,
                      long style = 0,
                      const wxString &name = wxGLCanvasName,
                      const wxPalette &palette = wxNullPalette);
#endif

    ~wxFFmpegInnerView();

    void open(std::string filename);
    void start();
    void close();

    void play();
    void pause();
    void stop();
    void seek(int pos);
    bool isPlaying();
    bool isPaused();
    bool isStopped();
    bool isOpen();
    int getPosition();
    int getDuration();
    wxSize getSize();
    void setSize(int width, int height);
    void setSize(wxSize size);
    bool isFullScreen();
    void setFullScreen(bool enable = true);
    bool setVolume(double volume);
    double getVolume();
    void setPosition(int pos);
    bool getState(int& state);
    double getPlaybackRate();
    bool setPlaybackRate(double rate);
    float getFPS();    
    void setFPS(float fps);
    
private:
    void OnPaint(wxPaintEvent &event);
    void OnSize(wxSizeEvent &event);
    void OnRenderTimer(wxTimerEvent &event);

    wxGLContext *glContext_;
    GLManager *glManager_{nullptr};
    wxTimer renderTimer_;
    Movie *movie_{nullptr};
    int64_t pts_;
    float fps_;

    wxDECLARE_EVENT_TABLE();
};

// clang-format off
wxBEGIN_EVENT_TABLE(wxFFmpegInnerView, wxGLCanvas)
    EVT_PAINT(wxFFmpegInnerView::OnPaint)
    EVT_SIZE(wxFFmpegInnerView::OnSize)
wxEND_EVENT_TABLE();
// clang-format on

namespace {} // namespace

#if wxCHECK_VERSION(3, 1, 5)
wxFFmpegInnerView::wxFFmpegInnerView(wxWindow *parent,
                                     const wxGLAttributes &dispAttrs,
                                     wxWindowID winid,
                                     const wxPoint &pos,
                                     const wxSize &size,
                                     long style,
                                     const wxString &name,
                                     const wxPalette &palette)
    : wxGLCanvas(parent, dispAttrs, winid, pos, size, style, name, palette)
{
    wxGLContextAttrs ctxAttrs;
    ctxAttrs.PlatformDefaults().CoreProfile().OGLVersion(3, 3).EndList();
    glContext_ = new wxGLContext(this, nullptr, &ctxAttrs);
    if (!glContext_->IsOK()) {
        delete glContext_;
        glContext_ = nullptr;
        return;
    }

    renderTimer_.Bind(wxEVT_TIMER, &wxFFmpegInnerView::OnRenderTimer, this);
    movie_ = new Movie;
}
#else
wxFFmpegInnerView::wxFFmpegInnerView(wxWindow *parent,
                                     wxWindowID winid,
                                     const int *attribList,
                                     const wxPoint &pos,
                                     const wxSize &size,
                                     long style,
                                     const wxString &name,
                                     const wxPalette &palette)
    : wxGLCanvas(parent, winid, attribList, pos, size, style, name, palette) {
    glContext_ = new wxGLContext(this);

    renderTimer_.Bind(wxEVT_TIMER, &wxFFmpegInnerView::OnRenderTimer, this);
    movie_ = new Movie;
    fps_ = 1000.0/24; //defaultFPS
}
#endif

wxFFmpegInnerView::~wxFFmpegInnerView() {
    if (glContext_) {
        SetCurrent(*glContext_);
    }

    if (movie_) {
        movie_->close();
        delete movie_;
    }

    if (glManager_) {
        delete glManager_;
    }

    if (glContext_) {
        delete glContext_;
    }
}

void wxFFmpegInnerView::open(std::string filename) {
    movie_->open(std::move(filename));
    pts_ = std::numeric_limits<int64_t>::min();
    renderTimer_.Start(fps_);
}

void wxFFmpegInnerView::start() {
}

void wxFFmpegInnerView::close() {
    movie_->close();
}

void wxFFmpegInnerView::OnPaint(wxPaintEvent &event) {
    wxPaintDC dc(this);

    if (!glManager_ || !glManager_->ok()) {
        return;
    }
}

void wxFFmpegInnerView::OnSize(wxSizeEvent &event) {
    event.Skip();

    if (!IsShownOnScreen()) {
        return;
    }

    if (!glContext_) {
        return;
    }

    SetCurrent(*glContext_);

    if (!glManager_) {
        glManager_ = new GLManager;
    }

    if (!glManager_->ok()) {
        return;
    }

    wxSize size = event.GetSize() * GetContentScaleFactor();
    glManager_->setViewport(0, 0, size.x, size.y);
}

void wxFFmpegInnerView::OnRenderTimer(wxTimerEvent &event) {
    auto [frame, pts] = movie_->currentFrame();
    if (frame && pts_ != pts) {
        pts_ = pts;
        SetCurrent(*glContext_);
        glManager_->draw(frame->width,
                         frame->height,
                         frame->data,
                         frame->linesize);
        SwapBuffers();
    }
}

void wxFFmpegInnerView::play() {
    if (movie_->isPlaying()) {
        return;
    }
    movie_->play();
    renderTimer_.Start(fps_);
}

void wxFFmpegInnerView::pause() {
    //TODO: how to pause the decoding & rendering
}

void wxFFmpegInnerView::stop() {
    if (movie_->isStopped()) {
        return;
    }
    movie_->stop();
    renderTimer_.Stop();
}

void wxFFmpegInnerView::seek(int pos) {
    movie_->seek(pos);
}
bool wxFFmpegInnerView::isPlaying() {
    return movie_->isPlaying();
}
bool wxFFmpegInnerView::isPaused() {
    return movie_->isPaused();
}
bool wxFFmpegInnerView::isStopped() {
    return movie_->isStopped();
}
bool wxFFmpegInnerView::isOpen() {
    return movie_->isOpen();
}
int wxFFmpegInnerView::getPosition() {
    return movie_->getPosition();
}
int wxFFmpegInnerView::getDuration() {
    return movie_->getDuration();
}

wxSize wxFFmpegInnerView::getSize() {
    return this->GetSize();
}

void wxFFmpegInnerView::setSize(int width, int height) {
    this->SetSize(width, height);
}

void wxFFmpegInnerView::setSize(wxSize size) {
    this->SetSize(size);
}
bool wxFFmpegInnerView::isFullScreen() {
    //TODO:
    return false;
}

void wxFFmpegInnerView::setFullScreen(bool enable) {
    //TODO: set full screen
}

bool wxFFmpegInnerView::setVolume(double volume) {
    return movie_->setVolume(volume);
}

double wxFFmpegInnerView::getVolume() {
    return movie_->getVolume();
}

void wxFFmpegInnerView::setPosition(int pos) {
    movie_->setPosition(pos);
}

double wxFFmpegInnerView::getPlaybackRate() {
    //TODO
    return 1.0;
}

bool wxFFmpegInnerView::setPlaybackRate(double rate) {
    return movie_->setPlaybackRate(rate);
}

float wxFFmpegInnerView::getFPS() {
    return fps_;
}

void wxFFmpegInnerView::setFPS(float fps) {
    fps_ = fps;
}

bool wxFFmpegInnerView::getState(int& state) {
    //TODO
    return 0;
}



//wxFFmpegView
wxFFmpegView::wxFFmpegView(wxWindow *parent,
                           wxWindowID winid,
                           const wxPoint &pos,
                           const wxSize &size,
                           long style,
                           const wxString &name)
    : wxPanel(parent, winid, pos, size, style, name) {
#if wxCHECK_VERSION(3, 1, 5)
    wxGLAttributes attrs;
    attrs.PlatformDefaults().Defaults().EndList();
    innerView_ = new wxFFmpegInnerView(this, attrs);
#else
    int attribList[] = {
            WX_GL_CORE_PROFILE,
            WX_GL_MAJOR_VERSION,
            3,
            WX_GL_MINOR_VERSION,
            3,
            WX_GL_DOUBLEBUFFER,
            0,
    };
    innerView_ = new wxFFmpegInnerView(this, wxID_ANY, attribList);
#endif

    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(innerView_, 1, wxEXPAND);
    SetSizer(sizer);
}

wxFFmpegView::~wxFFmpegView() {
}

void wxFFmpegView::open(std::string filename) {
    innerView_->open(std::move(filename));
}

void wxFFmpegView::close() {
    innerView_->close();
}

void wxFFmpegView::play() {
    innerView_->play();
}

void wxFFmpegView::pause() {
    innerView_->pause();
}

void wxFFmpegView::stop() {
    innerView_->stop();
}
void wxFFmpegView::seek(int pos) {
    innerView_->seek(pos);
}

bool wxFFmpegView::isPlaying() {
    return innerView_->isPlaying();
}

bool wxFFmpegView::isPaused() {
    return innerView_->isPaused();
}

bool wxFFmpegView::isStopped() {
    return innerView_->isStopped();
}

bool wxFFmpegView::isOpen() {
    return innerView_->isOpen();
}

int wxFFmpegView::getPosition() {
    return innerView_->getPosition();
}

int wxFFmpegView::getDuration() {
    return innerView_->getDuration();
}

wxSize wxFFmpegView::getSize() {
    return innerView_->getSize();
}

void wxFFmpegView::setSize(int width, int height) {
    innerView_->setSize(wxSize{width, height});
}

void wxFFmpegView::setSize(wxSize size) {
    innerView_->setSize(size);
}

bool wxFFmpegView::isFullScreen() {
    return innerView_->isFullScreen();
}

void wxFFmpegView::setFullScreen(bool enable) {
    innerView_->setFullScreen(enable);
}

bool wxFFmpegView::setVolume(double volume) {
    return innerView_->setVolume(volume);
}

double wxFFmpegView::getVolume() {
    return innerView_->getVolume();
}

void wxFFmpegView::setPosition(int pos) {
    innerView_->setPosition(pos);
}

bool wxFFmpegView::getState(int& state) {
    return innerView_->getState(state);
}

double wxFFmpegView::getPlaybackRate() {
    return innerView_->getPlaybackRate();
}

bool wxFFmpegView::setPlaybackRate(double rate) {
    return innerView_->setPlaybackRate(rate);
}

void wxFFmpegView::setFPS(float fps) {
    innerView_->setFPS(fps);
}