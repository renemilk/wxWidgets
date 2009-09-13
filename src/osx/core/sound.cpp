/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/carbon/sound.cpp
// Purpose:     wxSound class implementation: optional
// Author:      Ryan Norton
// Modified by: Stefan Csomor
// Created:     1998-01-01
// RCS-ID:      $Id: sound.cpp 61475 2009-07-20 16:47:54Z VZ $
// Copyright:   (c) Ryan Norton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_SOUND

#if wxOSX_USE_AUDIOTOOLBOX

#include "wx/sound.h"

#ifndef WX_PRECOMP
    #include "wx/object.h"
    #include "wx/string.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/timer.h"
#endif

#include "wx/file.h"

class wxOSXAudioToolboxSoundData : public wxSoundData
{
public:
    wxOSXAudioToolboxSoundData(const wxString& fileName);

    ~wxOSXAudioToolboxSoundData();
    
    virtual bool Play(unsigned flags);

    virtual void DoStop();
protected:
    static void CompletionCallback(SystemSoundID  mySSID, void * soundRef);
    void SoundCompleted();
    
    SystemSoundID m_soundID;    
    wxString m_sndname; //file path
};

wxOSXAudioToolboxSoundData::wxOSXAudioToolboxSoundData(const wxString& fileName) :
    m_soundID(NULL)
{                                            
    m_sndname = fileName;
}

void wxOSXAudioToolboxSoundData::CompletionCallback(SystemSoundID  mySSID, void * soundRef)
{
    wxOSXAudioToolboxSoundData* data = (wxOSXAudioToolboxSoundData*) soundRef;
    
    data->SoundCompleted();
}

void wxOSXAudioToolboxSoundData::SoundCompleted()
{
    if ( m_flags & wxSOUND_ASYNC )
    {
        if (m_flags & wxSOUND_LOOP)
            AudioServicesPlaySystemSound(m_soundID);
        else 
            Stop();
    }
    else 
    {
        Stop();
        CFRunLoopStop(CFRunLoopGetCurrent());
    }

}

void wxOSXAudioToolboxSoundData::DoStop()
{
    if (m_soundID)
    {
        AudioServicesDisposeSystemSoundID (m_soundID);
        m_soundID = NULL;
        
        wxSound::SoundStopped(this);
    }
}

bool wxOSXAudioToolboxSoundData::DoPlay(unsigned flags) const
{
    Stop();

    m_flags = flags;
    
    wxCFRef<CFMutableStringRef> cfMutableString(CFStringCreateMutableCopy(NULL, 0, wxCFStringRef(m_sndname)));
    CFStringNormalize(cfMutableString,kCFStringNormalizationFormD);
    wxCFRef<CFURLRef> url(CFURLCreateWithFileSystemPath(kCFAllocatorDefault, cfMutableString , kCFURLPOSIXPathStyle, false));

    AudioServicesCreateSystemSoundID(url, &m_soundID);
    AudioServicesAddSystemSoundCompletion( m_soundID, NULL, NULL, wxOSXAudioToolboxSoundData::CompletionCallback, (void *) this );

    bool sync = !(flags & wxSOUND_ASYNC);

    AudioServicesPlaySystemSound(m_soundID);

    if ( sync )
    {
        while( m_soundID ) 
        {
            CFRunLoopRun();
        }
    }
            
    return true;
}

#endif // wxOSX_USE_AUDIOTOOLBOX

#endif //wxUSE_SOUND
