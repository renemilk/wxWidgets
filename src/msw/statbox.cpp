/////////////////////////////////////////////////////////////////////////////
// Name:        msw/statbox.cpp
// Purpose:     wxStaticBox
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
    #pragma implementation "statbox.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_STATBOX

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/dcclient.h"
#endif

#include "wx/statbox.h"

#include "wx/msw/private.h"

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

#if wxUSE_EXTENDED_RTTI
WX_DEFINE_FLAGS( wxStaticBoxStyle )

WX_BEGIN_FLAGS( wxStaticBoxStyle )
    // new style border flags, we put them first to
    // use them for streaming out
    WX_FLAGS_MEMBER(wxBORDER_SIMPLE)
    WX_FLAGS_MEMBER(wxBORDER_SUNKEN)
    WX_FLAGS_MEMBER(wxBORDER_DOUBLE)
    WX_FLAGS_MEMBER(wxBORDER_RAISED)
    WX_FLAGS_MEMBER(wxBORDER_STATIC)
    WX_FLAGS_MEMBER(wxBORDER_NONE)
    
    // old style border flags
    WX_FLAGS_MEMBER(wxSIMPLE_BORDER)
    WX_FLAGS_MEMBER(wxSUNKEN_BORDER)
    WX_FLAGS_MEMBER(wxDOUBLE_BORDER)
    WX_FLAGS_MEMBER(wxRAISED_BORDER)
    WX_FLAGS_MEMBER(wxSTATIC_BORDER)
    WX_FLAGS_MEMBER(wxNO_BORDER)

    // standard window styles
    WX_FLAGS_MEMBER(wxTAB_TRAVERSAL)
    WX_FLAGS_MEMBER(wxCLIP_CHILDREN)
    WX_FLAGS_MEMBER(wxTRANSPARENT_WINDOW)
    WX_FLAGS_MEMBER(wxWANTS_CHARS)
    WX_FLAGS_MEMBER(wxNO_FULL_REPAINT_ON_RESIZE)
    WX_FLAGS_MEMBER(wxALWAYS_SHOW_SB )
    WX_FLAGS_MEMBER(wxVSCROLL)
    WX_FLAGS_MEMBER(wxHSCROLL)

WX_END_FLAGS( wxStaticBoxStyle )

IMPLEMENT_DYNAMIC_CLASS_XTI(wxStaticBox, wxControl,"wx/statbox.h")

WX_BEGIN_PROPERTIES_TABLE(wxStaticBox)
	WX_PROPERTY( Label,wxString, SetLabel, GetLabel, wxString() , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    WX_PROPERTY_FLAGS( WindowStyle , wxStaticBoxStyle , long , SetWindowStyleFlag , GetWindowStyleFlag , , 0 /*flags*/ , wxT("Helpstring") , wxT("group")) // style
/*
	TODO PROPERTIES :
		label
*/
WX_END_PROPERTIES_TABLE()

WX_BEGIN_HANDLERS_TABLE(wxStaticBox)
WX_END_HANDLERS_TABLE()

WX_CONSTRUCTOR_6( wxStaticBox , wxWindow* , Parent , wxWindowID , Id , wxString , Label , wxPoint , Position , wxSize , Size , long , WindowStyle ) 
#else
IMPLEMENT_DYNAMIC_CLASS(wxStaticBox, wxControl)
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxStaticBox
// ----------------------------------------------------------------------------

bool wxStaticBox::Create(wxWindow *parent,
                         wxWindowID id,
                         const wxString& label,
                         const wxPoint& pos,
                         const wxSize& size,
                         long style,
                         const wxString& name)
{
    if ( !CreateControl(parent, id, pos, size, style, wxDefaultValidator, name) )
        return FALSE;

    // as wxStaticBox doesn't draw its own background, we make it transparent
    // to force redrawing its background which could have been overwritten by
    // the other controls inside it
    //
    // FIXME: I still think that it isn't the right solution because the static
    //        boxes shouldn't have to be transparent if the redrawing was done
    //        right elsewhere - who ever had to make them transparent in non
    //        wxWindows programs, after all? But for now it does fix a serious
    //        problem (try resizing the sizers test screen in the layout sample
    //        after removing WS_EX_TRANSPARENT bit) and so let's use it until
    //        we fix the real underlying problem
    if ( !MSWCreateControl(wxT("BUTTON"), BS_GROUPBOX, pos, size, label,
#ifdef __WXWINCE__
        0
#else
        WS_EX_TRANSPARENT
#endif
                           ) )
        return FALSE;

    // to be transparent we should have the same colour as the parent as well
    SetBackgroundColour(GetParent()->GetBackgroundColour());

    return TRUE;
}

wxSize wxStaticBox::DoGetBestSize() const
{
    int cx, cy;
    wxGetCharSize(GetHWND(), &cx, &cy, &GetFont());

    int wBox;
    GetTextExtent(wxGetWindowText(m_hWnd), &wBox, &cy);

    wBox += 3*cx;
    int hBox = EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy);

    return wxSize(wBox, hBox);
}

long wxStaticBox::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
    switch ( nMsg )
    {
#ifndef __WXWINCE__
        case WM_NCHITTEST:
            // FIXME: this hack is specific to dialog ed, shouldn't it be
            //        somehow disabled during normal operation?
            {
                int xPos = LOWORD(lParam);  // horizontal position of cursor
                int yPos = HIWORD(lParam);  // vertical position of cursor

                ScreenToClient(&xPos, &yPos);

                // Make sure you can drag by the top of the groupbox, but let
                // other (enclosed) controls get mouse events also
                if ( yPos < 10 )
                    return (long)HTCLIENT;
            }
            break;
#endif
        case WM_ERASEBKGND:
            // prevent wxControl from processing this message because it will
            // erase the background incorrectly and there is no way for us to
            // override this at wxWin event level (if we do process the event,
            // we don't know how to do it properly - paint the background
            // without painting over other controls - and if we don't,
            // wxControl still gets it)
            return MSWDefWindowProc(nMsg, wParam, lParam);
    }

    return wxControl::MSWWindowProc(nMsg, wParam, lParam);
}

#endif // wxUSE_STATBOX
