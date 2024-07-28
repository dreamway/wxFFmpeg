#pragma once

#include "wx/control.h"
#include "wx/uri.h"
#include "wx/mediactrl.h"

class wzMediaCtrl : public wxControl
{
public:
    wzMediaCtrl() : m_imp(NULL), m_bLoaded(false)
    {                                                                   }

    wzMediaCtrl(wxWindow* parent, wxWindowID winid,
                const wxString& fileName = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& szBackend = wxEmptyString,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxT("mediaCtrl"))
                : m_imp(NULL), m_bLoaded(false)
    {   Create(parent, winid, fileName, pos, size, style,
               szBackend, validator, name);                             }

    wzMediaCtrl(wxWindow* parent, wxWindowID winid,
                const wxURI& location,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& szBackend = wxEmptyString,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxT("mediaCtrl"))
                : m_imp(NULL), m_bLoaded(false)
    {   Create(parent, winid, location, pos, size, style,
               szBackend, validator, name);                             }

    virtual ~wzMediaCtrl();

    bool Create(wxWindow* parent, wxWindowID winid,
                const wxString& fileName = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& szBackend = wxEmptyString,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxT("mediaCtrl"));

    bool Create(wxWindow* parent, wxWindowID winid,
                const wxURI& location,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& szBackend = wxEmptyString,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxT("mediaCtrl"));

    bool DoCreate(wxWindow* parent, wxWindowID winid,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxT("mediaCtrl"));

    bool Play();
    bool Pause();
    bool Stop();

    bool Load(const wxString& fileName);

    wxMediaState GetState();

    wxFileOffset Seek(wxFileOffset where, wxSeekMode mode = wxFromStart);
    wxFileOffset Tell(); //FIXME: This should be const
    wxFileOffset Length(); //FIXME: This should be const

    double GetPlaybackRate();           //All but MCI & GStreamer
    bool SetPlaybackRate(double dRate); //All but MCI & GStreamer

    bool Load(const wxURI& location);
    bool Load(const wxURI& location, const wxURI& proxy);

    wxFileOffset GetDownloadProgress(); // DirectShow only
    wxFileOffset GetDownloadTotal();    // DirectShow only

    double GetVolume();
    bool   SetVolume(double dVolume);

    bool    ShowPlayerControls(
        wxMediaCtrlPlayerControls flags = wxMEDIACTRLPLAYERCONTROLS_DEFAULT);

    //helpers for the wxPython people
    bool LoadURI(const wxString& fileName)
    {   return Load(wxURI(fileName));       }
    bool LoadURIWithProxy(const wxString& fileName, const wxString& proxy)
    {   return Load(wxURI(fileName), wxURI(proxy));       }

protected:
    void OnMediaFinished(wxMediaEvent& evt);
    virtual void DoMoveWindow(int x, int y, int w, int h) override;
    wxSize DoGetBestSize() const override;
    void OnSize(wxSizeEvent& event);

    class OGLMediaBackend *m_imp;
    bool m_bLoaded;

    //wxDECLARE_EVENT_TABLE();
};
