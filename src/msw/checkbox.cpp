/////////////////////////////////////////////////////////////////////////////
// Name:        msw/checkbox.cpp
// Purpose:     wxCheckBox
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
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

#ifndef BST_CHECKED
    #define BST_CHECKED 0x0001
#endif

// ============================================================================
// implementation
// ============================================================================

#if wxUSE_EXTENDED_RTTI
WX_DEFINE_FLAGS( wxCheckBoxStyle )

wxBEGIN_FLAGS( wxCheckBoxStyle )
    // new style border flags, we put them first to
    // use them for streaming out
    wxFLAGS_MEMBER(wxBORDER_SIMPLE)
    wxFLAGS_MEMBER(wxBORDER_SUNKEN)
    wxFLAGS_MEMBER(wxBORDER_DOUBLE)
    wxFLAGS_MEMBER(wxBORDER_RAISED)
    wxFLAGS_MEMBER(wxBORDER_STATIC)
    wxFLAGS_MEMBER(wxBORDER_NONE)
    
    // old style border flags
    wxFLAGS_MEMBER(wxSIMPLE_BORDER)
    wxFLAGS_MEMBER(wxSUNKEN_BORDER)
    wxFLAGS_MEMBER(wxDOUBLE_BORDER)
    wxFLAGS_MEMBER(wxRAISED_BORDER)
    wxFLAGS_MEMBER(wxSTATIC_BORDER)
    wxFLAGS_MEMBER(wxBORDER)

    // standard window styles
    wxFLAGS_MEMBER(wxTAB_TRAVERSAL)
    wxFLAGS_MEMBER(wxCLIP_CHILDREN)
    wxFLAGS_MEMBER(wxTRANSPARENT_WINDOW)
    wxFLAGS_MEMBER(wxWANTS_CHARS)
    wxFLAGS_MEMBER(wxFULL_REPAINT_ON_RESIZE)
    wxFLAGS_MEMBER(wxALWAYS_SHOW_SB )
    wxFLAGS_MEMBER(wxVSCROLL)
    wxFLAGS_MEMBER(wxHSCROLL)

wxEND_FLAGS( wxCheckBoxStyle )

IMPLEMENT_DYNAMIC_CLASS_XTI(wxCheckBox, wxControl,"wx/checkbox.h")

wxBEGIN_PROPERTIES_TABLE(wxCheckBox)
	wxEVENT_PROPERTY( Click , wxEVT_COMMAND_CHECKBOX_CLICKED , wxCommandEvent )

	wxPROPERTY( Font , wxFont , SetFont , GetFont  , , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
	wxPROPERTY( Label,wxString, SetLabel, GetLabel, wxString() , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
	wxPROPERTY( Value ,bool, SetValue, GetValue, , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxPROPERTY_FLAGS( WindowStyle , wxCheckBoxStyle , long , SetWindowStyleFlag , GetWindowStyleFlag , , 0 /*flags*/ , wxT("Helpstring") , wxT("group")) // style
wxEND_PROPERTIES_TABLE()

wxBEGIN_HANDLERS_TABLE(wxCheckBox)
wxEND_HANDLERS_TABLE()

wxCONSTRUCTOR_6( wxCheckBox , wxWindow* , Parent , wxWindowID , Id , wxString , Label , wxPoint , Position , wxSize , Size , long , WindowStyle ) 
#else
IMPLEMENT_DYNAMIC_CLASS(wxCheckBox, wxControl)
#endif


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
    if ( !CreateControl(parent, id, pos, size, style, validator, name) )
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

bool wxCheckBox::GetValue() const
{
    return (SendMessage(GetHwnd(), BM_GETCHECK, 0, 0) & BST_CHECKED) != 0;
}

void wxCheckBox::Command(wxCommandEvent& event)
{
    SetValue(event.GetInt() != 0);
    ProcessCommand(event);
}

#endif // wxUSE_CHECKBOX
