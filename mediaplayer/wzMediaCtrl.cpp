#include "wzMediaCtrl.h"
#include <iostream>
#include "OGLMediaBackend.h"

//---------------------------------------------------------------------------
// wxMediaCtrl::Create (file version)
// wxMediaCtrl::Create (URL version)
//
// Searches for a backend that is installed on the system (backends
// starting with lower characters in the alphabet are given priority),
// and creates the control from it
//
// This searches by searching the global RTTI hashtable, class by class,
// attempting to call CreateControl on each one found that is a derivative
// of wxMediaBackend - if it succeeded Create returns true, otherwise
// it keeps iterating through the hashmap.
//---------------------------------------------------------------------------
bool wzMediaCtrl::Create(wxWindow* parent, wxWindowID id,
                const wxString& fileName,
                const wxPoint& pos,
                const wxSize& size,
                long style,
                const wxString& szBackend,
                const wxValidator& validator,
                const wxString& name)
{
    if(!DoCreate(parent, id,
                                pos, size, style, validator, name))
    {
        m_imp = nullptr;
        return false;
    }

    if (!fileName.empty())
    {
        if (!Load(fileName))
        {
            wxDELETE(m_imp);
            return false;
        }
    }

    SetInitialSize(size);
    return true;    
}

bool wzMediaCtrl::Create(wxWindow* parent, wxWindowID id,
                         const wxURI& location,
                         const wxPoint& pos,
                         const wxSize& size,
                         long style,
                         const wxString& szBackend,
                         const wxValidator& validator,
                         const wxString& name)
{
    if(!DoCreate(parent, id,pos, size, style, validator, name))
    {
        m_imp = nullptr;
        return false;
    }

    if (!Load(location))
    {
        wxDELETE(m_imp);
        return false;
    }

    SetInitialSize(size);
    return true;    
}

//---------------------------------------------------------------------------
// wxMediaCtrl::DoCreate
//
// Attempts to create the control from a backend
//---------------------------------------------------------------------------
bool wzMediaCtrl::DoCreate(wxWindow* parent, wxWindowID id,
                            const wxPoint& pos,
                            const wxSize& size,
                            long style,
                            const wxValidator& validator,
                            const wxString& name)
{
    m_imp = new OGLMediaBackend();

    if( m_imp->CreateControl(this, parent, id, pos, size,
                             style, validator, name) )
    {
        return true;
    }

    delete m_imp;
    return false;
}

//---------------------------------------------------------------------------
// wxMediaCtrl Destructor
//
// Free up the backend if it exists
//---------------------------------------------------------------------------
wzMediaCtrl::~wzMediaCtrl()
{
    delete m_imp;
}

//---------------------------------------------------------------------------
// wxMediaCtrl::Load (file version)
// wxMediaCtrl::Load (URL version)
// wxMediaCtrl::Load (URL & Proxy version)
// wxMediaCtrl::Load (wxInputStream version)
//
// Here we call load of the backend - keeping
// track of whether it was successful or not - which
// will determine which later method calls work
//---------------------------------------------------------------------------
bool wzMediaCtrl::Load(const wxString& fileName)
{
    if(m_imp)
        return (m_bLoaded = m_imp->Load(fileName));
    return false;
}

bool wzMediaCtrl::Load(const wxURI& location)
{
    if(m_imp)
        return (m_bLoaded = m_imp->Load(location));
    return false;
}

bool wzMediaCtrl::Load(const wxURI& location, const wxURI& proxy)
{
    if(m_imp)
        return (m_bLoaded = m_imp->Load(location, proxy));
    return false;
}

//---------------------------------------------------------------------------
// wxMediaCtrl::Play
// wxMediaCtrl::Pause
// wxMediaCtrl::Stop
// wxMediaCtrl::GetPlaybackRate
// wxMediaCtrl::SetPlaybackRate
// wxMediaCtrl::Seek --> SetPosition
// wxMediaCtrl::Tell --> GetPosition
// wxMediaCtrl::Length --> GetDuration
// wxMediaCtrl::GetState
// wxMediaCtrl::DoGetBestSize
// wxMediaCtrl::SetVolume
// wxMediaCtrl::GetVolume
// wxMediaCtrl::ShowInterface
// wxMediaCtrl::GetDownloadProgress
// wxMediaCtrl::GetDownloadTotal
//
// 1) Check to see whether the backend exists and is loading
// 2) Call the backend's version of the method, returning success
//    if the backend's version succeeds
//---------------------------------------------------------------------------
bool wzMediaCtrl::Play()
{
    if(m_imp && m_bLoaded)
        return m_imp->Play();
    return 0;
}

bool wzMediaCtrl::Pause()
{
    if(m_imp && m_bLoaded)
        return m_imp->Pause();
    return 0;
}

bool wzMediaCtrl::Stop()
{
    if(m_imp && m_bLoaded)
        return m_imp->Stop();
    return 0;
}

double wzMediaCtrl::GetPlaybackRate()
{
    if(m_imp && m_bLoaded)
        return m_imp->GetPlaybackRate();
    return 0;
}

bool wzMediaCtrl::SetPlaybackRate(double dRate)
{
    if(m_imp && m_bLoaded)
        return m_imp->SetPlaybackRate(dRate);
    return false;
}

wxFileOffset wzMediaCtrl::Seek(wxFileOffset where, wxSeekMode mode)
{
    wxFileOffset offset;

    switch (mode)
    {
    case wxFromStart:
        offset = where;
        break;
    case wxFromEnd:
        offset = Length() - where;
        break;
    default:
        offset = Tell() + where;
        break;
    }

    if(m_imp && m_bLoaded && m_imp->SetPosition(offset))
        return offset;
    return wxInvalidOffset;
}

wxFileOffset wzMediaCtrl::Tell()
{
    if(m_imp && m_bLoaded)
        return (wxFileOffset) m_imp->GetPosition().ToLong();
    return wxInvalidOffset;
}

wxFileOffset wzMediaCtrl::Length()
{
    if(m_imp && m_bLoaded)
        return (wxFileOffset) m_imp->GetDuration().ToLong();
    return wxInvalidOffset;
}

wxMediaState wzMediaCtrl::GetState()
{
    if(m_imp && m_bLoaded)
        return m_imp->GetState();
    return wxMEDIASTATE_STOPPED;
}

wxSize wzMediaCtrl::DoGetBestSize() const
{
    if(m_imp)
        return m_imp->GetVideoSize();
    return wxSize(0,0);
}

double wzMediaCtrl::GetVolume()
{
    if(m_imp && m_bLoaded)
        return m_imp->GetVolume();
    return 0.0;
}

bool wzMediaCtrl::SetVolume(double dVolume)
{
    if(m_imp && m_bLoaded)
        return m_imp->SetVolume(dVolume);
    return false;
}

bool wzMediaCtrl::ShowPlayerControls(wxMediaCtrlPlayerControls flags)
{
    if(m_imp)
        return m_imp->ShowPlayerControls(flags);
    return false;
}

wxFileOffset wzMediaCtrl::GetDownloadProgress()
{
    //TODO: not support download
    return wxInvalidOffset;
}

wxFileOffset wzMediaCtrl::GetDownloadTotal()
{
    //TODO: not support download
    return wxInvalidOffset;
}

//---------------------------------------------------------------------------
// wxMediaCtrl::DoMoveWindow
//
// 1) Call parent's version so that our control's window moves where
//    it's supposed to
// 2) If the backend exists and is loaded, move the video
//    of the media to where our control's window is now located
//---------------------------------------------------------------------------
void wzMediaCtrl::DoMoveWindow(int x, int y, int w, int h)
{
    wxControl::DoMoveWindow(x,y,w,h);

    if(m_imp)
        m_imp->Move(x, y, w, h);
}
