/////////////////////////////////////////////////////////////////////////////
// Name:        checkbox.cpp
// Purpose:     wxCheckBox
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "checkbox.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_CHECKBOX

#ifndef WX_PRECOMP
    #include "wx/checkbox.h"
    #include "wx/brush.h"
    #include "wx/dcscreen.h"
    #include "wx/settings.h"
#endif

#include "wx/msw/private.h"

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxCheckBox, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxBitmapCheckBox, wxCheckBox)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxCheckBox
// ----------------------------------------------------------------------------

bool wxCheckBox::MSWCommand(WXUINT WXUNUSED(param), WXWORD WXUNUSED(id))
{
    wxCommandEvent event(wxEVT_COMMAND_CHECKBOX_CLICKED, m_windowId);
    event.SetInt(GetValue());
    event.SetEventObject(this);
    ProcessCommand(event);
    return TRUE;
}

bool wxCheckBox::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxString& label,
                        const wxPoint& pos,
                        const wxSize& size, long style,
                        const wxValidator& validator,
                        const wxString& name)
{
    if ( !CreateControl(parent, id, pos, size, style, wxDefaultValidator, name) )
        return FALSE;

    long msStyle = BS_AUTOCHECKBOX | WS_TABSTOP;
    if ( style & wxALIGN_RIGHT )
        msStyle |= BS_LEFTTEXT;

    return MSWCreateControl(wxT("BUTTON"), msStyle, pos, size, label, 0);
}

void wxCheckBox::SetLabel(const wxString& label)
{
    SetWindowText(GetHwnd(), label);
}

wxSize wxCheckBox::DoGetBestSize() const
{
    static int s_checkSize = 0;

    if ( !s_checkSize )
    {
        wxScreenDC dc;
        dc.SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));

        s_checkSize = dc.GetCharHeight();
    }

    wxString str = wxGetWindowText(GetHWND());

    int wCheckbox, hCheckbox;
    if ( !str.IsEmpty() )
    {
        GetTextExtent(str, &wCheckbox, &hCheckbox);
        wCheckbox += s_checkSize + GetCharWidth();

        if ( hCheckbox < s_checkSize )
            hCheckbox = s_checkSize;
    }
    else
    {
        wCheckbox = s_checkSize;
        hCheckbox = s_checkSize;
    }

    return wxSize(wCheckbox, hCheckbox);
}

void wxCheckBox::SetValue(bool val)
{
    SendMessage(GetHwnd(), BM_SETCHECK, val, 0);
}

#ifndef BST_CHECKED
#define BST_CHECKED 0x0001
#endif

bool wxCheckBox::GetValue() const
{
#ifdef __WIN32__
  return (SendMessage(GetHwnd(), BM_GETCHECK, 0, 0) == BST_CHECKED);
#else
  return ((0x001 & SendMessage(GetHwnd(), BM_GETCHECK, 0, 0)) == 0x001);
#endif
}

void wxCheckBox::Command (wxCommandEvent & event)
{
  SetValue ((event.GetInt() != 0));
  ProcessCommand (event);
}

// ----------------------------------------------------------------------------
// wxBitmapCheckBox
// ----------------------------------------------------------------------------

bool wxBitmapCheckBox::Create(wxWindow *parent, wxWindowID id, const wxBitmap *WXUNUSED(label),
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
  SetName(name);
#if wxUSE_VALIDATORS
  SetValidator(validator);
#endif // wxUSE_VALIDATORS
  if (parent) parent->AddChild(this);

  SetBackgroundColour(parent->GetBackgroundColour()) ;
  SetForegroundColour(parent->GetForegroundColour()) ;
  m_windowStyle = style;

  if ( id == -1 )
      m_windowId = NewControlId();
  else
      m_windowId = id;

  int x = pos.x;
  int y = pos.y;
  int width = size.x;
  int height = size.y;

  checkWidth = -1 ;
  checkHeight = -1 ;
  long msStyle = CHECK_FLAGS;

  HWND wx_button = CreateWindowEx(MakeExtendedStyle(m_windowStyle), CHECK_CLASS, wxT("toggle"),
                    msStyle,
                    0, 0, 0, 0, (HWND) parent->GetHWND(), (HMENU)m_windowId,
                    wxGetInstance(), NULL);

#if wxUSE_CTL3D
  if (!(GetParent()->GetWindowStyleFlag() & wxUSER_COLOURS))
  {
    Ctl3dSubclassCtl(wx_button);
    m_useCtl3D = TRUE;
  }
#endif

  m_hWnd = (WXHWND)wx_button;

  // Subclass again for purposes of dialog editing mode
  SubclassWin((WXHWND)wx_button);

  SetSize(x, y, width, height);

  ShowWindow(wx_button, SW_SHOW);

  return TRUE;
}

void wxBitmapCheckBox::SetLabel(const wxBitmap& WXUNUSED(bitmap))
{
    wxFAIL_MSG(wxT("not implemented"));
}

#endif // wxUSE_CHECKBOX
