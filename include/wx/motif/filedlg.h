/////////////////////////////////////////////////////////////////////////////
// Name:        filedlg.h
// Purpose:     wxFileDialog class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FILEDLG_H_
#define _WX_FILEDLG_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "filedlg.h"
#endif

#include "wx/dialog.h"

/*
* File selector
*/

class WXDLLEXPORT wxFileDialog: public wxDialog
{
    DECLARE_DYNAMIC_CLASS(wxFileDialog)
public:
    wxString    m_message;
    long        m_dialogStyle;
    wxWindow *  m_parent;
    wxString    m_dir;
    wxString    m_path; // Full path
    wxString    m_fileName;
    wxString    m_wildCard;
    int         m_filterIndex;
    
    // For Motif
    wxPoint     m_pos;
    static wxString m_fileSelectorAnswer;
    static bool m_fileSelectorReturned;
    
public:
    wxFileDialog(wxWindow *parent, const wxString& message = wxFileSelectorPromptStr,
        const wxString& defaultDir = "", const wxString& defaultFile = "", const wxString& wildCard = wxFileSelectorDefaultWildcardStr,
        long style = 0, const wxPoint& pos = wxDefaultPosition);
    
    inline void SetMessage(const wxString& message) { m_message = message; }
    inline void SetPath(const wxString& path) { m_path = path; }
    inline void SetDirectory(const wxString& dir) { m_dir = dir; }
    inline void SetFilename(const wxString& name) { m_fileName = name; }
    inline void SetWildcard(const wxString& wildCard) { m_wildCard = wildCard; }
    inline void SetStyle(long style) { m_dialogStyle = style; }
    inline void SetFilterIndex(int filterIndex) { m_filterIndex = filterIndex; }
    
    inline wxString GetMessage() const { return m_message; }
    inline wxString GetPath() const { return m_path; }
    inline void GetPaths(wxArrayString& a) { a.Empty(); a.Add(m_path); }
    inline wxString GetDirectory() const { return m_dir; }
    inline wxString GetFilename() const { return m_fileName; }
    inline void GetFilenames(wxArrayString& a) { a.Empty();
    a.Add( m_fileName); }
    inline wxString GetWildcard() const { return m_wildCard; }
    inline long GetStyle() const { return m_dialogStyle; }
    inline int GetFilterIndex() const { return m_filterIndex ; }
    
    int ShowModal();
};

#endif // _WX_FILEDLG_H_
