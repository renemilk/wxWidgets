///////////////////////////////////////////////////////////////////////////////
// Name:        wx/filectrl.h
// Purpose:     Header for wxFileCtrlBase and other common functions used by
//              platform-specific wxFileCtrl's
// Author:      Diaa M. Sami
// Modified by:
// Created:     Jul-07-2007
// RCS-ID:      $Id$
// Copyright:   (c) Diaa M. Sami
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FILECTRL_H_BASE_
#define _WX_FILECTRL_H_BASE_

#include "wx/defs.h"

#if wxUSE_FILECTRL

#include "wx/string.h"
#include "wx/event.h"

enum
{
    wxFC_OPEN              = 0x0001,
    wxFC_SAVE              = 0x0002,
    wxFC_MULTIPLE          = 0x0004,
    wxFC_NOSHOWHIDDEN      = 0x0008,
};

#define wxFC_DEFAULT_STYLE wxFC_OPEN
extern WXDLLEXPORT_DATA( const wxChar ) wxFileCtrlNameStr[];

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EXPORTED_EVENT_TYPE( WXDLLIMPEXP_CORE, wxEVT_FILECTRL_SELECTIONCHANGED, 1150 )
    DECLARE_EXPORTED_EVENT_TYPE( WXDLLIMPEXP_CORE, wxEVT_FILECTRL_FILEACTIVATED, 1151 )
    DECLARE_EXPORTED_EVENT_TYPE( WXDLLIMPEXP_CORE, wxEVT_FILECTRL_FOLDERCHANGED, 1152 )
END_DECLARE_EVENT_TYPES()

class WXDLLIMPEXP_CORE wxFileCtrlBase
{
public:
    virtual ~wxFileCtrlBase() {}

    virtual void SetWildcard( const wxString& wildCard ) = 0;
    virtual void SetFilterIndex( int filterindex ) = 0;
    virtual bool SetDirectory( const wxString& dir ) = 0;

    // Selects a certain file.
    // In case the filename specified isn't found/couldn't be shown with
    // currently selected filter, false is returned and nothing happens
    virtual bool SetFilename( const wxString& name ) = 0;

    // chdirs to a certain directory and selects a certain file.
    // In case the filename specified isn't found/couldn't be shown with
    // currently selected filter, false is returned and if directory exists
    // it's chdir'ed to
    virtual bool SetPath( const wxString& path ) = 0;

    virtual wxString GetFilename() const = 0;
    virtual wxString GetDirectory() const = 0;
    virtual wxString GetWildcard() const = 0;
    virtual wxString GetPath() const = 0;
    virtual void GetPaths( wxArrayString& paths ) const = 0;
    virtual void GetFilenames( wxArrayString& files ) const = 0;
    virtual int GetFilterIndex() const = 0;

    virtual bool HasMultipleFileSelection() const = 0;
    virtual void ShowHidden(const bool show) = 0;
};

void GenerateFolderChangedEvent( wxFileCtrlBase *fileCtrl, wxWindow *wnd );
void GenerateSelectionChangedEvent( wxFileCtrlBase *fileCtrl, wxWindow *wnd );
void GenerateFileActivatedEvent( wxFileCtrlBase *fileCtrl, wxWindow *wnd, const wxString filename = wxEmptyString );

#if defined(__WXGTK24__) && !defined(__WXUNIVERSAL__)
    #define wxFileCtrl wxGtkFileCtrl
    #include "wx/gtk/filectrl.h"
#else
    #define wxFileCtrl wxGenericFileCtrl
    #include "wx/generic/filectrlg.h"
#endif

// Some documentation
// On wxEVT_FILECTRL_FOLDERCHANGED, only the value returned by GetDirectory is
// valid and it represents the (new) current directory for the wxFileCtrl.
// On wxEVT_FILECTRL_FILEACTIVATED, GetDirectory returns the current directory
// for the wxFileCtrl and GetFiles returns the names of the file(s) activated.
// On wxEVT_FILECTRL_SELECTIONCHANGED, GetDirectory returns the current directory
// for the wxFileCtrl and GetFiles returns the names of the currently selected
// file(s).
// In wxGTK, after each wxEVT_FILECTRL_FOLDERCHANGED, wxEVT_FILECTRL_SELECTIONCHANGED
// is fired automatically once or more with 0 files.
class WXDLLIMPEXP_CORE wxFileCtrlEvent : public wxCommandEvent
{
public:
    wxFileCtrlEvent() {}
    wxFileCtrlEvent( wxEventType type, wxObject *evtObject, int id )
            : wxCommandEvent( type, id )
    {
        SetEventObject( evtObject );
    }

    // no need for the copy constructor as the default one will be fine.
    virtual wxEvent *Clone() const { return new wxFileCtrlEvent( *this ); }

    void SetFiles( const wxArrayString &files ) { this->files = files; }
    void SetDirectory( const wxString &directory ) { this->directory = directory; }

    wxArrayString GetFiles() const { return files; }
    wxString GetDirectory() const { return directory; }

    wxString GetFile() const;

protected:
    wxString  directory;
    wxArrayString files;

    DECLARE_DYNAMIC_CLASS_NO_ASSIGN( wxFileCtrlEvent )
};

typedef void ( wxEvtHandler::*wxFileCtrlEventFunction )( wxFileCtrlEvent& );

#define wxFileCtrlEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxFileCtrlEventFunction, &func)

#define EVT_FILECTRL_FILEACTIVATED(id, fn) \
    wx__DECLARE_EVT1(wxEVT_FILECTRL_FILEACTIVATED, id, wxFileCtrlEventHandler(fn))

#define EVT_FILECTRL_SELECTIONCHANGED(id, fn) \
    wx__DECLARE_EVT1(wxEVT_FILECTRL_SELECTIONCHANGED, id, wxFileCtrlEventHandler(fn))

#define EVT_FILECTRL_FOLDERCHANGED(id, fn) \
    wx__DECLARE_EVT1(wxEVT_FILECTRL_FOLDERCHANGED, id, wxFileCtrlEventHandler(fn))

#endif // wxUSE_FILECTRL

#endif // _WX_FILECTRL_H_BASE_
