#include "wxFFmpegView.h"

#include <GL/glew.h>
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/glcanvas.h>
#include <wx/uri.h>
#include <wx/window.h>

#include "glmanager.h"
#include "movie.h"
#include "wx/mediactrl.h"
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

    void Open(std::string filename);
    bool Load(wxURI path);
    void Start();
    void Close();

    bool Play();
    bool Pause();
    bool Stop();
    void Seek(int pos);
    bool IsPlaying();
    bool IsPaused();
    bool IsStopped();
    bool IsOpened();
    int GetPosition();
    int GetDuration();
    wxSize GetSize();
    void SetSize(int width, int height);
    void SetSize(wxSize size);
    bool IsFullScreen();
    void SetFullScreen(bool enable = true);
    bool SetVolume(double volume);
    double GetVolume();
    void SetPosition(int pos);
    bool GetState(int& state);
    wxMediaState GetState();
    double GetPlaybackRate();
    bool SetPlaybackRate(double rate);
    float GetFPS();    
    void SetFPS(float fps);
    
    void OnSize(wxSizeEvent &event);

private:
    void OnPaint(wxPaintEvent &event);    
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
    SetBackgroundColour(*wxBLUE);
}
#endif

wxFFmpegInnerView::~wxFFmpegInnerView() {
    if (glContext_) {
        SetCurrent(*glContext_);
    }

    if (movie_) {
        movie_->Close();
        delete movie_;
    }

    if (glManager_) {
        delete glManager_;
    }

    if (glContext_) {
        delete glContext_;
    }
}

void wxFFmpegInnerView::Open(std::string filename) {
    movie_->Open(std::move(filename));
    pts_ = std::numeric_limits<int64_t>::min();
    renderTimer_.Start(fps_);
}

bool wxFFmpegInnerView::Load(wxURI path) {
    Open(std::string(path.BuildURI().c_str()));
    return true;
}

void wxFFmpegInnerView::Start() {
}

void wxFFmpegInnerView::Close() {
    movie_->Close();
}

void wxFFmpegInnerView::OnPaint(wxPaintEvent &event) {
    wxPaintDC dc(this);

    if (!glManager_ || !glManager_->ok()) {
        return;
    }
}

void wxFFmpegInnerView::OnSize(wxSizeEvent &event) {
    //event.Skip();

    if (!IsShownOnScreen()) {
        std::cout<<"skip , Not shown on screen"<<std::endl;
        return;
    }

    if (!glContext_) {
        std::cerr<<"glContent is null"<<std::endl;
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

    std::cout<<"wxFFMpegInnerView::OnSize: "<<size.x<<" "<<size.y<<","<<"contentScaleFactor:"<<GetContentScaleFactor()<<std::endl;    
    int width, height;    
    GetClientSize(&width, &height);    
    std::cout<<"wxFFMpegInnerView::OnSize, innerViewSize: "<<width<<"x"<<height<<std::endl;
    SetClientSize(event.GetSize());
    GetClientSize(&width, &height);    
    std::cout<<"wxFFMpegInnerView::OnSize, update after SetSize: "<<width<<"x"<<height<<std::endl;
}

void wxFFmpegInnerView::OnRenderTimer(wxTimerEvent &event) {
    auto [frame, pts] = movie_->currentFrame();
    //std::cout<<"wxFFmpegInnerView::OnRenderTimer: pts:"<<pts<<", currentPts:"<<pts_<<",frame->width:"<<frame->width<<"x"<<frame->height<<std::endl;
    if (frame && pts_ != pts) {
        pts_ = pts;
        SetCurrent(*glContext_);
        glManager_->clear();        
        glManager_->draw(frame->width,
                         frame->height,
                         frame->data,
                         frame->linesize);

        SwapBuffers();
    } else {
        glManager_->clear();
        SwapBuffers();
    }
}

bool wxFFmpegInnerView::Play() {
    if (movie_->IsPlaying()) {
        return false;
    }
    movie_->Play();
    renderTimer_.Start(fps_);
    return true;
}

bool wxFFmpegInnerView::Pause() {
    //TODO: how to pause the decoding & rendering
    return false;
}

bool wxFFmpegInnerView::Stop() {
    if (movie_->IsStopped()) {
        return false;
    }
    movie_->Stop();
    renderTimer_.Stop();
    return true;
}

void wxFFmpegInnerView::Seek(int pos) {
    movie_->Seek(pos);
}
bool wxFFmpegInnerView::IsPlaying() {
    return movie_->IsPlaying();
}
bool wxFFmpegInnerView::IsPaused() {
    return movie_->IsPaused();
}
bool wxFFmpegInnerView::IsStopped() {
    return movie_->IsStopped();
}
bool wxFFmpegInnerView::IsOpened() {
    return movie_->IsOpened();
}
int wxFFmpegInnerView::GetPosition() {
    return movie_->GetPosition();
}
int wxFFmpegInnerView::GetDuration() {
    return movie_->GetDuration();
}


bool wxFFmpegInnerView::IsFullScreen() {
    //TODO:
    return false;
}

void wxFFmpegInnerView::SetFullScreen(bool enable) {
    //TODO: set full screen
}

bool wxFFmpegInnerView::SetVolume(double volume) {
    return movie_->SetVolume(volume);
}

double wxFFmpegInnerView::GetVolume() {
    return movie_->GetVolume();
}

void wxFFmpegInnerView::SetPosition(int pos) {
    movie_->SetPosition(pos);
}

double wxFFmpegInnerView::GetPlaybackRate() {
    //TODO
    return 1.0;
}

bool wxFFmpegInnerView::SetPlaybackRate(double rate) {
    return movie_->SetPlaybackRate(rate);
}

float wxFFmpegInnerView::GetFPS() {
    return fps_;
}

void wxFFmpegInnerView::SetFPS(float fps) {
    fps_ = fps;
}

bool wxFFmpegInnerView::GetState(int& state) {
    //TODO
    return 0;
}

wxMediaState wxFFmpegInnerView::GetState() {
    return wxMEDIASTATE_PLAYING;
}


//wxFFmpegView
wxBEGIN_EVENT_TABLE(wxFFmpegView, wxPanel)
    EVT_SIZE(wxFFmpegView::OnSize)
wxEND_EVENT_TABLE()
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
    innerView_ = new wxFFmpegInnerView(this, wxID_ANY, attribList, pos, size, style, name);
#endif

    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(innerView_, 1, wxEXPAND);
    SetSizer(sizer);
    SetBackgroundColour(*wxGREEN);
}

wxFFmpegView::~wxFFmpegView() {
}

void wxFFmpegView::Open(std::string filename) {
    innerView_->Open(std::move(filename));
}

bool wxFFmpegView::Load(wxURI path) {
    return innerView_->Load(std::move(path));
}

void wxFFmpegView::Close() {
    innerView_->Close();
}

bool wxFFmpegView::Play() {
    return innerView_->Play();
}

bool wxFFmpegView::Pause() {
    return innerView_->Pause();
}

bool wxFFmpegView::Stop() {
    return innerView_->Stop();
}
int wxFFmpegView::Seek(int pos) {
    innerView_->Seek(pos);
    return pos;
}

bool wxFFmpegView::IsPlaying() {
    return innerView_->IsPlaying();
}

bool wxFFmpegView::IsPaused() {
    return innerView_->IsPaused();
}

bool wxFFmpegView::IsStopped() {
    return innerView_->IsStopped();
}

bool wxFFmpegView::IsOpened() {
    return innerView_->IsOpened();
}

int wxFFmpegView::GetPosition() {
    return innerView_->GetPosition();
}

int wxFFmpegView::GetDuration() {
    return innerView_->GetDuration();
}


bool wxFFmpegView::IsFullScreen() {
    return innerView_->IsFullScreen();
}

void wxFFmpegView::SetFullScreen(bool enable) {
    innerView_->SetFullScreen(enable);
}

bool wxFFmpegView::SetVolume(double volume) {
    return innerView_->SetVolume(volume);
}

double wxFFmpegView::GetVolume() {
    return innerView_->GetVolume();
}

void wxFFmpegView::SetPosition(int pos) {
    innerView_->SetPosition(pos);
}

bool wxFFmpegView::GetState(int& state) {
    return innerView_->GetState(state);
}

double wxFFmpegView::GetPlaybackRate() {
    return innerView_->GetPlaybackRate();
}

bool wxFFmpegView::SetPlaybackRate(double rate) {
    return innerView_->SetPlaybackRate(rate);
}

void wxFFmpegView::SetFPS(float fps) {
    innerView_->SetFPS(fps);
}


void wxFFmpegView::OnSize(wxSizeEvent& event) {
    // wxSize size = event.GetSize();
    // SetSize(size);
    innerView_->OnSize(event);
}

wxMediaState wxFFmpegView::GetState() {
    return wxMEDIASTATE_PLAYING;
}

wxLongLong wxFFmpegView::Length() {
    //TODO
    return 10L;
}

wxLongLong wxFFmpegView::Tell() {
    //TODO
    return 10L;
}

int wxFFmpegView::GetValue() {
    //TODO
    return 1;
}
                   