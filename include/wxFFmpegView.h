#pragma once

#include <wx/panel.h>

class wxFFmpegInnerView;
class wxFFmpegView : public wxPanel {
public:
    wxFFmpegView(wxWindow *parent,
                 wxWindowID winid = wxID_ANY,
                 const wxPoint &pos = wxDefaultPosition,
                 const wxSize &size = wxDefaultSize,
                 long style = 0,
                 const wxString &name = _T("wxFFmpegView"));
    ~wxFFmpegView();

    void open(std::string filename);
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
    void setFPS(float fps);

    void OnSize(wxSizeEvent& event);
private:
    wxFFmpegInnerView *innerView_;

    wxDECLARE_EVENT_TABLE();
};
