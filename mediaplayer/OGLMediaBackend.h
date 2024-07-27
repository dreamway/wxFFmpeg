#pragma once
#include "wx/mediactrl.h"
#include "wxFFmpegView.h"

class OGLMediaBackend : public wxMediaBackendCommonBase
{
public:
    OGLMediaBackend();
    virtual ~OGLMediaBackend();

    virtual bool CreateControl(wxControl* ctrl, wxWindow* parent,
                                     wxWindowID id,
                                     const wxPoint& pos,
                                     const wxSize& size,
                                     long style,
                                     const wxValidator& validator,
                                     const wxString& name) override;

    virtual bool Play() override;
    virtual bool Pause() override;
    virtual bool Stop() override;

    virtual bool Load(const wxString& fileName) override;
    virtual bool Load(const wxURI& location) override;
    virtual bool Load(const wxURI& location, const wxURI& proxy) override;

    bool DoLoad(const wxString& location);
    void FinishLoad();

    virtual wxMediaState GetState() override;

    virtual bool SetPosition(wxLongLong where) override;
    virtual wxLongLong GetPosition() override;
    virtual wxLongLong GetDuration() override;

    virtual void Move(int x, int y, int w, int h) override;
    wxSize GetVideoSize() const override;

    virtual double GetPlaybackRate() override;
    virtual bool SetPlaybackRate(double) override;

    virtual double GetVolume() override;
    virtual bool SetVolume(double) override;

    virtual bool ShowPlayerControls(wxMediaCtrlPlayerControls flags) override;

    void DoGetDownloadProgress(wxLongLong*, wxLongLong*);
    virtual wxLongLong GetDownloadProgress() override
    {
        wxLongLong progress, total;
        DoGetDownloadProgress(&progress, &total);
        return progress;
    }
    virtual wxLongLong GetDownloadTotal() override
    {
        wxLongLong progress, total;
        DoGetDownloadProgress(&progress, &total);
        return total;
    }

    wxFFmpegView *ffmpegView;    
    wxSize m_bestSize;  // Cached size

    wxEvtHandler* m_evthandler; 
};
