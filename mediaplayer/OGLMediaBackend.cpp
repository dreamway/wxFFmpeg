#include "OGLMediaBackend.h"

//---------------------------------------------------------------------------
// OGLMediaBackend Constructor
//---------------------------------------------------------------------------
OGLMediaBackend::OGLMediaBackend()
                 : m_bestSize(wxDefaultSize)
{
   m_evthandler = nullptr;
}

//---------------------------------------------------------------------------
// OGLMediaBackend Destructor
//---------------------------------------------------------------------------
OGLMediaBackend::~OGLMediaBackend()
{
    if(ffmpegView)
    {
        delete ffmpegView;
        ffmpegView = nullptr;
    }

    if (m_evthandler)
    {
        m_ctrl->RemoveEventHandler(m_evthandler);
        delete m_evthandler;
    }
}

//---------------------------------------------------------------------------
// OGLMediaBackend::CreateControl
//---------------------------------------------------------------------------
bool OGLMediaBackend::CreateControl(wxControl* ctrl, wxWindow* parent,
                                     wxWindowID id,
                                     const wxPoint& pos,
                                     const wxSize& size,
                                     long style,
                                     const wxValidator& validator,
                                     const wxString& name)
{
    // Create window
    // By default wxWindow(s) is created with a border -
    // so we need to get rid of those
    //
    // Since we don't have a child window like most other
    // backends, we don't need wxCLIP_CHILDREN
    //
    if ( !ctrl->wxControl::Create(parent, id, pos, size,
                            (style & ~wxBORDER_MASK) | wxBORDER_NONE,
                            validator, name) )
        return false;

    ffmpegView = new wxFFmpegView(ctrl, wxID_ANY);
    OGLMediaBackend::SetVolume(1.0);
    
    // don't erase the background of our control window so that resizing is a
    // bit smoother
    m_ctrl->SetBackgroundStyle(wxBG_STYLE_PAINT);

    // success
    return true;
}

//---------------------------------------------------------------------------
// OGLMediaBackend::Load (file version)
//---------------------------------------------------------------------------
bool OGLMediaBackend::Load(const wxString& fileName)
{
    return DoLoad(fileName);
}

//---------------------------------------------------------------------------
// OGLMediaBackend::Load (URL Version)
//---------------------------------------------------------------------------
bool OGLMediaBackend::Load(const wxURI& location)
{
    return DoLoad(location.BuildURI());
}

//---------------------------------------------------------------------------
// OGLMediaBackend::Load (URL Version with Proxy)
//---------------------------------------------------------------------------
bool OGLMediaBackend::Load(const wxURI& location, const wxURI& proxy)
{
    return DoLoad(location.BuildURI());
}

//---------------------------------------------------------------------------
// OGLMediaBackend::DoLoad
//
// Called by all functions - this actually renders
// the file and sets up the filter graph
//---------------------------------------------------------------------------
bool OGLMediaBackend::DoLoad(const wxString& location)
{
    ffmpegView->open(std::string(location.c_str()));
    m_bestSize = wxDefaultSize;
    return true;
}

//---------------------------------------------------------------------------
// OGLMediaBackend::FinishLoad
//
// Called when the media has finished loaded and is ready to play
//
// Here we get the original size of the video and
// send the loaded event to our watcher :).
//---------------------------------------------------------------------------
void OGLMediaBackend::FinishLoad()
{
    NotifyMovieLoaded();
}


//---------------------------------------------------------------------------
// OGLMediaBackend::Play
//
// Plays the stream.  If it is non-seekable, it will restart it (implicit).
//
// Note that we use SUCCEEDED here because run/pause/stop tend to be overly
// picky and return warnings on pretty much every call
//---------------------------------------------------------------------------
bool OGLMediaBackend::Play()
{
    // Actually try to play the movie (will fail if not loaded completely)
    ffmpegView->play();
    return true;
}

//---------------------------------------------------------------------------
// OGLMediaBackend::Pause
//
// Pauses the stream.
//---------------------------------------------------------------------------
bool OGLMediaBackend::Pause()
{
    ffmpegView->pause();
    return true;
}

//---------------------------------------------------------------------------
// OGLMediaBackend::Stop
//
// Stops the stream.
//---------------------------------------------------------------------------
bool OGLMediaBackend::Stop()
{
    ffmpegView->stop();
    return true;
}

//---------------------------------------------------------------------------
// OGLMediaBackend::SetPosition
//
// 1) Translates the current position's time to directshow time,
//    which is in a scale of 1 second (in a double)
//---------------------------------------------------------------------------
bool OGLMediaBackend::SetPosition(wxLongLong where)
{
    //todo

    return false;
}

//---------------------------------------------------------------------------
// OGLMediaBackend::GetPosition
//
// 1) Obtains the current play and stop positions from IMediaSeeking
// 2) Returns the play position translated to our time base
//---------------------------------------------------------------------------
wxLongLong OGLMediaBackend::GetPosition()
{
    //TODO
    return 0;
}

//---------------------------------------------------------------------------
// OGLMediaBackend::GetVolume and SetVolume()
//
// Notice that for the IActiveMovie interface value ranges from 0 (MAX volume)
// to -10000 (minimum volume) and the scale is logarithmic in 0.01db per step.
//---------------------------------------------------------------------------

double OGLMediaBackend::GetVolume()
{
    return ffmpegView->getVolume();
}

bool OGLMediaBackend::SetVolume(double volume)
{
    return ffmpegView->setVolume(volume);
}

//---------------------------------------------------------------------------
// OGLMediaBackend::GetDuration
//
// 1) Obtains the duration of the media from IActiveMovie
// 2) Converts that value to our time base, and returns it
//
// NB: With VBR MP3 files the default DirectShow MP3 render does not
// read the Xing header correctly, resulting in skewed values for duration
// and seeking
//---------------------------------------------------------------------------
wxLongLong OGLMediaBackend::GetDuration()
{
    return ffmpegView->getDuration();
}

//---------------------------------------------------------------------------
// OGLMediaBackend::GetState
//
// Returns the cached state
//---------------------------------------------------------------------------
wxMediaState OGLMediaBackend::GetState()
{
/*
    StateConstants nState;
    ffmpegView->getState(&nState);

    return (wxMediaState)nState;
    */
   return wxMEDIASTATE_PLAYING;//TODO
}

//---------------------------------------------------------------------------
// OGLMediaBackend::GetPlaybackRate
//
// Pretty simple way of obtaining the playback rate from
// the IActiveMovie interface
//---------------------------------------------------------------------------
double OGLMediaBackend::GetPlaybackRate()
{
    return ffmpegView->getPlaybackRate();
}

//---------------------------------------------------------------------------
// OGLMediaBackend::SetPlaybackRate
//
// Sets the playback rate of the media - DirectShow is pretty good
// about this, actually
//---------------------------------------------------------------------------
bool OGLMediaBackend::SetPlaybackRate(double dRate)
{
    return ffmpegView->setPlaybackRate(dRate);
}

//---------------------------------------------------------------------------
// OGLMediaBackend::GetVideoSize
//
// Obtains the cached original video size
//---------------------------------------------------------------------------
wxSize OGLMediaBackend::GetVideoSize() const
{
    if (m_bestSize == wxDefaultSize)
    {
        OGLMediaBackend* self = wxConstCast(this, OGLMediaBackend);
        long w = 0;
        long h = 0;


        if (w != 0 && h != 0)
            self->m_bestSize.Set(w, h);
        else
            return wxSize(0,0);
    }

   return m_bestSize;
}


void OGLMediaBackend::DoGetDownloadProgress(wxLongLong*, wxLongLong*) {

}

//---------------------------------------------------------------------------
// Do nothing, We take care of this in our redrawing
//---------------------------------------------------------------------------
void OGLMediaBackend::Move(int WXUNUSED(x), int WXUNUSED(y),
                            int WXUNUSED(w), int WXUNUSED(h))
{
}

bool OGLMediaBackend::ShowPlayerControls(wxMediaCtrlPlayerControls flags)
{
    return false;
}