/////////////////////////////////////////////////////////////////////////////
// Name:        sound.h
// Purpose:     wxSound class (loads and plays short Windows .wav files).
//              Optional on non-Windows platforms.
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SOUND_H_
#define _WX_SOUND_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "sound.h"
#endif

#if wxUSE_SOUND

#include "wx/object.h"

class WXDLLEXPORT wxSound : public wxSoundBase
{
public:
  wxSound();
  wxSound(const wxString& fileName, bool isResource = FALSE);
  wxSound(int size, const wxByte* data);
  ~wxSound();

public:
  bool  Create(const wxString& fileName, bool isResource = FALSE);
  bool  IsOk() const { return !m_sndname.IsEmpty(); }

  void* GetHandle();
protected:
  // prevent collision with some BSD definitions of macro Free()
  bool  FreeData();
  
  bool  DoPlay(unsigned flags) const;

private:
    wxString m_sndname; //file path
    char* m_hSnd; //pointer to resource or memory location
    int m_waveLength; //size of file in memory mode
    void* m_pTimer; //timer

    enum wxSoundType
    {
        wxSound_MEMORY,
        wxSound_FILE,
        wxSound_RESOURCE,
        wxSound_NONE
    } m_type; //mode
};

#endif
#endif
    // _WX_SOUND_H_
