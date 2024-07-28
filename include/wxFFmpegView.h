#pragma once

#include <wx/panel.h>
#include "wx/mediactrl.h"


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

    void Open(std::string filename);
    bool Load(wxURI path);
    void Close();
    bool Play();
    bool Pause();
    bool Stop();
    int Seek(int pos);
    bool IsPlaying();
    bool IsPaused();
    bool IsStopped();
    bool IsOpened();
    int GetPosition();
    int GetDuration();
    bool IsFullScreen();
    void SetFullScreen(bool enable = true);
    bool SetVolume(double volume);
    double GetVolume();
    void SetPosition(int pos);
    bool GetState(int& state);
    wxMediaState GetState();
    double GetPlaybackRate();
    bool SetPlaybackRate(double rate);
    void SetFPS(float fps);
    wxLongLong Length();
    wxLongLong Tell();
    int GetValue();

    void OnSize(wxSizeEvent& event);
    void OnPaint(wxPaintEvent &event);
private:
    wxFFmpegInnerView *innerView_;

    wxDECLARE_EVENT_TABLE();
};
