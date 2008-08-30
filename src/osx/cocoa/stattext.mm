/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/stattext.mm
// Purpose:     wxStaticText
// Author:      Stefan Csomor
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id: stattext.cpp 54845 2008-07-30 14:52:41Z SC $
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_STATTEXT

#include "wx/stattext.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/utils.h"
    #include "wx/dc.h"
    #include "wx/dcclient.h"
    #include "wx/settings.h"
#endif // WX_PRECOMP

#include "wx/notebook.h"
#include "wx/tabctrl.h"

#include "wx/osx/private.h"

#include <stdio.h>

wxSize wxStaticText::DoGetBestSize() const
{
    Point bounds;
#if wxOSX_USE_CARBON
    Rect bestsize = { 0 , 0 , 0 , 0 } ;

    // try the built-in best size if available
    Boolean former = m_peer->GetData<Boolean>( kControlStaticTextIsMultilineTag);
    m_peer->SetData( kControlStaticTextIsMultilineTag, (Boolean)0 );
    m_peer->GetBestRect( &bestsize ) ;
    m_peer->SetData( kControlStaticTextIsMultilineTag, former );
    if ( !EmptyRect( &bestsize ) )
    {
        bounds.h = bestsize.right - bestsize.left ;
        bounds.v = bestsize.bottom - bestsize.top ;
    }
    else
#endif
    {
#if wxOSX_USE_CARBON
        ControlFontStyleRec controlFont;
        OSStatus err = m_peer->GetData<ControlFontStyleRec>( kControlEntireControl, kControlFontStyleTag, &controlFont );
        verify_noerr( err );

        wxCFStringRef str( m_label,  GetFont().GetEncoding() );

#if wxOSX_USE_ATSU_TEXT
        SInt16 baseline;
        if ( m_font.MacGetThemeFontID() != kThemeCurrentPortFont )
        {
            err = GetThemeTextDimensions(
                (!m_label.empty() ? (CFStringRef)str : CFSTR(" ")),
                m_font.MacGetThemeFontID(), kThemeStateActive, false, &bounds, &baseline );
            verify_noerr( err );
        }
        else
#endif
#endif
        {
            wxClientDC dc(const_cast<wxStaticText*>(this));
            wxCoord width, height ;
            dc.GetTextExtent( m_label , &width, &height);
            bounds.h = width;
            bounds.v = height;
        }

        if ( m_label.empty() )
            bounds.h = 0;
    }
    bounds.h += MacGetLeftBorderSize() + MacGetRightBorderSize();
    bounds.v += MacGetTopBorderSize() + MacGetBottomBorderSize();

    return wxSize( bounds.h, bounds.v );
}

// for wxST_ELLIPSIZE_* support:

/*
   FIXME: UpdateLabel() should be called on size events when wxST_ELLIPSIZE_START is set
          to allow correct dynamic ellipsizing of the label
*/

wxWidgetImplType* wxWidgetImpl::CreateStaticText( wxWindowMac* wxpeer, 
                                    wxWindowMac* parent, 
                                    wxWindowID id, 
                                    const wxString& label,
                                    const wxPoint& pos, 
                                    const wxSize& size,
                                    long style, 
                                    long extraStyle) 
{
    NSView* sv = (wxpeer->GetParent()->GetHandle() );
    
    NSRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    wxNSTextField* v = [[wxNSTextField alloc] initWithFrame:r];
    [sv addSubview:v];

    [v setBezeled:NO];
    [v setEditable:NO];
    [v setDrawsBackground:NO];
    
    wxWidgetCocoaImpl* c = new wxWidgetCocoaImpl( wxpeer, v );
    [v setImplementation:c];
    return c;
/*
    Rect bounds = wxMacGetBoundsForControl( wxpeer, pos, size );

    wxMacControl* peer = new wxMacControl( wxpeer );
    OSStatus err = CreateStaticTextControl(
        MAC_WXHWND(parent->MacGetTopLevelWindowRef()),
        &bounds, NULL, NULL, peer->GetControlRefAddr() );
    verify_noerr( err );

    if ( ( style & wxST_ELLIPSIZE_END ) || ( style & wxST_ELLIPSIZE_MIDDLE ) )
    {
        TruncCode tCode = truncEnd;
        if ( style & wxST_ELLIPSIZE_MIDDLE )
            tCode = truncMiddle;

        err = peer->SetData( kControlStaticTextTruncTag, tCode );
        err = peer->SetData( kControlStaticTextIsMultilineTag, (Boolean)0 );
    }
    return peer;
    */
}

#endif //if wxUSE_STATTEXT
