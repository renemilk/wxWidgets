/////////////////////////////////////////////////////////////////////////////
// Name:        palmos/settings.cpp
// Purpose:     wxSystemSettingsNative implementation for Palm OS
// Author:      William Osborne
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id:
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/gdicmn.h"
#endif

#include "wx/settings.h"
#include "wx/module.h"
#include "wx/fontutil.h"

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxSystemSettingsNative
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// colours
// ----------------------------------------------------------------------------

wxColour wxSystemSettingsNative::GetColour(wxSystemColour index)
{
    // point to unused entry to mark lack of assignment
    UIColorTableEntries which = UILastColorTableEntry;

    switch( index )
    {
        case wxSYS_COLOUR_BACKGROUND:
        case wxSYS_COLOUR_WINDOW:
            which = UIFormFill;
            break;
        case wxSYS_COLOUR_WINDOWFRAME:
        case wxSYS_COLOUR_ACTIVECAPTION:
            which = UIFormFrame;
            break;
        case wxSYS_COLOUR_SCROLLBAR:
            which = UIObjectFill;
            break;
/*
        case wxSYS_COLOUR_INACTIVECAPTION:
        case wxSYS_COLOUR_MENU:
        case wxSYS_COLOUR_MENUTEXT:
        case wxSYS_COLOUR_WINDOWTEXT:
        case wxSYS_COLOUR_CAPTIONTEXT:
        case wxSYS_COLOUR_ACTIVEBORDER:
        case wxSYS_COLOUR_INACTIVEBORDER:
        case wxSYS_COLOUR_APPWORKSPACE:
        case wxSYS_COLOUR_HIGHLIGHT:
        case wxSYS_COLOUR_HIGHLIGHTTEXT:
        case wxSYS_COLOUR_BTNFACE:
        case wxSYS_COLOUR_BTNSHADOW:
        case wxSYS_COLOUR_GRAYTEXT:
        case wxSYS_COLOUR_BTNTEXT:
        case wxSYS_COLOUR_INACTIVECAPTIONTEXT:
        case wxSYS_COLOUR_BTNHIGHLIGHT:
        case wxSYS_COLOUR_3DDKSHADOW:
        case wxSYS_COLOUR_3DLIGHT:
        case wxSYS_COLOUR_INFOTEXT:
        case wxSYS_COLOUR_INFOBK:
        case wxSYS_COLOUR_LISTBOX:
        case wxSYS_COLOUR_HOTLIGHT:
        case wxSYS_COLOUR_GRADIENTACTIVECAPTION:
        case wxSYS_COLOUR_GRADIENTINACTIVECAPTION:
        case wxSYS_COLOUR_MENUHILIGHT:
        case wxSYS_COLOUR_MENUBAR:
*/
    }
    if ( which == UILastColorTableEntry )
        return wxNullColour;

    RGBColorType rgbP;

    UIColorGetTableEntryRGB (which,&rgbP);

    return wxColour(rgbP.r,rgbP.g,rgbP.b);
}

// ----------------------------------------------------------------------------
// fonts
// ----------------------------------------------------------------------------

wxFont wxCreateFontFromStockObject(int index)
{
    wxFont font;
    return font;
}

wxFont wxSystemSettingsNative::GetFont(wxSystemFont index)
{
    wxFont font;
    return font;
}

// ----------------------------------------------------------------------------
// system metrics/features
// ----------------------------------------------------------------------------

// Get a system metric, e.g. scrollbar size
int wxSystemSettingsNative::GetMetric(wxSystemMetric index)
{
    return -1;
}

bool wxSystemSettingsNative::HasFeature(wxSystemFeature index)
{
    return false;
}
