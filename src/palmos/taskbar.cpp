/////////////////////////////////////////////////////////////////////////
// File:        taskbar.cpp
// Purpose:     Implements wxTaskBarIcon class for manipulating icons on
//              the task bar.
// Author:      Julian Smart
// Modified by: Vaclav Slavik
// Created:     24/3/98
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "taskbar.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/defs.h"
#include "wx/window.h"
#include "wx/frame.h"
#include "wx/utils.h"
#include "wx/menu.h"
#endif

#if defined(__WIN95__)

#include <string.h>
#include "wx/taskbar.h"

#if WXWIN_COMPATIBILITY_2_4
BEGIN_EVENT_TABLE(wxTaskBarIcon, wxTaskBarIconBase)
    EVT_TASKBAR_MOVE         (wxTaskBarIcon::_OnMouseMove)
    EVT_TASKBAR_LEFT_DOWN    (wxTaskBarIcon::_OnLButtonDown)
    EVT_TASKBAR_LEFT_UP      (wxTaskBarIcon::_OnLButtonUp)
    EVT_TASKBAR_RIGHT_DOWN   (wxTaskBarIcon::_OnRButtonDown)
    EVT_TASKBAR_RIGHT_UP     (wxTaskBarIcon::_OnRButtonUp)
    EVT_TASKBAR_LEFT_DCLICK  (wxTaskBarIcon::_OnLButtonDClick)
    EVT_TASKBAR_RIGHT_DCLICK (wxTaskBarIcon::_OnRButtonDClick)
END_EVENT_TABLE()
#endif


IMPLEMENT_DYNAMIC_CLASS(wxTaskBarIcon, wxEvtHandler)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxTaskBarIconWindow: helper window
// ----------------------------------------------------------------------------

// NB: this class serves two purposes:
//     1. win32 needs a HWND associated with taskbar icon, this provides it
//     2. we need wxTopLevelWindow so that the app doesn't exit when
//        last frame is closed but there still is a taskbar icon
class wxTaskBarIconWindow : public wxFrame
{
public:
    wxTaskBarIconWindow(wxTaskBarIcon *icon)
        : wxFrame(NULL, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0),
          m_icon(icon)
    {
    }

    WXLRESULT MSWWindowProc(WXUINT msg,
                            WXWPARAM wParam, WXLPARAM lParam)
    {
        return 0;
    }

private:
    wxTaskBarIcon *m_icon;
};

// ----------------------------------------------------------------------------
// wxTaskBarIcon
// ----------------------------------------------------------------------------

wxTaskBarIcon::wxTaskBarIcon()
{
}

wxTaskBarIcon::~wxTaskBarIcon()
{
}

// Operations
bool wxTaskBarIcon::SetIcon(const wxIcon& icon, const wxString& tooltip)
{
    return false;
}

bool wxTaskBarIcon::RemoveIcon()
{
	return false;
}

bool wxTaskBarIcon::PopupMenu(wxMenu *menu)
{
	return false;
}

#if WXWIN_COMPATIBILITY_2_4
// Overridables
void wxTaskBarIcon::OnMouseMove(wxEvent& e)         { e.Skip(); }
void wxTaskBarIcon::OnLButtonDown(wxEvent& e)       { e.Skip(); }
void wxTaskBarIcon::OnLButtonUp(wxEvent& e)         { e.Skip(); }
void wxTaskBarIcon::OnRButtonDown(wxEvent& e)       { e.Skip(); }
void wxTaskBarIcon::OnRButtonUp(wxEvent& e)         { e.Skip(); }
void wxTaskBarIcon::OnLButtonDClick(wxEvent& e)     { e.Skip(); }
void wxTaskBarIcon::OnRButtonDClick(wxEvent& e)     { e.Skip(); }

void wxTaskBarIcon::_OnMouseMove(wxTaskBarIconEvent& e)
    { OnMouseMove(e);     }
void wxTaskBarIcon::_OnLButtonDown(wxTaskBarIconEvent& e)
    { OnLButtonDown(e);   }
void wxTaskBarIcon::_OnLButtonUp(wxTaskBarIconEvent& e)
    { OnLButtonUp(e);     }
void wxTaskBarIcon::_OnRButtonDown(wxTaskBarIconEvent& e)
    { OnRButtonDown(e);   }
void wxTaskBarIcon::_OnRButtonUp(wxTaskBarIconEvent& e)
    { OnRButtonUp(e);     }
void wxTaskBarIcon::_OnLButtonDClick(wxTaskBarIconEvent& e)
    { OnLButtonDClick(e); }
void wxTaskBarIcon::_OnRButtonDClick(wxTaskBarIconEvent& e)
    { OnRButtonDClick(e); }
#endif

void wxTaskBarIcon::RegisterWindowMessages()
{
}

// ----------------------------------------------------------------------------
// wxTaskBarIcon window proc
// ----------------------------------------------------------------------------

long wxTaskBarIcon::WindowProc(unsigned int msg,
                               unsigned int WXUNUSED(wParam),
                               long lParam)
{
    return 0;
}

#endif // __WIN95__

