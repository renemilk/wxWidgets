/////////////////////////////////////////////////////////////////////////////
// Name:        cocoa/bmpbuttn.mm
// Purpose:     wxBitmapButton
// Author:      David Elliott
// Modified by:
// Created:     2003/03/16
// RCS-ID:      $Id:
// Copyright:   (c) 2003 David Elliott
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include "wx/defs.h"
    #include "wx/bmpbuttn.h"
    #include "wx/log.h"
#endif

#include "wx/cocoa/autorelease.h"

#import <AppKit/NSButton.h>
#import <Foundation/NSString.h>

IMPLEMENT_DYNAMIC_CLASS(wxBitmapButton, wxControl)
BEGIN_EVENT_TABLE(wxBitmapButton, wxBitmapButtonBase)
END_EVENT_TABLE()
WX_IMPLEMENT_COCOA_OWNER(wxBitmapButton,NSButton,NSControl,NSView)

bool wxBitmapButton::Create(wxWindow *parent, wxWindowID winid,
            const wxBitmap& bitmap, const wxPoint& pos,
            const wxSize& size, long style,
            const wxValidator& validator, const wxString& name)
{
    wxAutoNSAutoreleasePool pool;
    wxLogDebug(wxT("Creating control with id=%d"),winid);
    if(!CreateControl(parent,winid,pos,size,style,validator,name))
        return false;
    wxLogDebug(wxT("Created control with id=%d"),GetId());
    m_cocoaNSView = NULL;
    SetNSButton([[NSButton alloc] initWithFrame: MakeDefaultNSRect(size)]);
    // NOTE: YES we want to release this (to match the alloc).
    // DoAddChild(this) will retain us again since addSubView doesn't.
    [m_cocoaNSView release];

    [GetNSButton() setBezelStyle:NSRoundedBezelStyle];
    [GetNSButton() setTitle:@"Bitmap Button"];
    [GetNSControl() sizeToFit];

    if(m_parent)
        m_parent->CocoaAddChild(this);
    SetInitialFrameRect(pos,size);

    return true;
}

wxBitmapButton::~wxBitmapButton()
{
    DisassociateNSButton(GetNSButton());
}

void wxBitmapButton::Cocoa_wxNSButtonAction(void)
{
    wxLogDebug(wxT("YAY!"));
    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
    InitCommandEvent(event); //    event.SetEventObject(this);
    Command(event);
}

