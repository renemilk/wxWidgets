///////////////////////////////////////////////////////////////////////////////
// Name:        wx/cocoa/NSView.h
// Purpose:     wxCocoaNSView class
// Author:      David Elliott
// Modified by:
// Created:     2003/02/15
// RCS-ID:      $Id: 
// Copyright:   (c) 2003 David Elliott
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef __WX_COCOA_NSVIEW_H__
#define __WX_COCOA_NSVIEW_H__

#include "wx/hashmap.h"
#include "wx/cocoa/ObjcPose.h"

WX_DECLARE_OBJC_HASHMAP(NSView);
class wxCocoaNSView
{
/* NSView is a rather special case and requires some extra attention */
    WX_DECLARE_OBJC_INTERFACE_HASHMAP(NSView)
public:
    void AssociateNSView(WX_NSView cocoaNSView);
    void DisassociateNSView(WX_NSView cocoaNSView);
protected:
    static void *sm_cocoaObserver;
public:
    virtual void Cocoa_FrameChanged(void) = 0;
};

#endif // _WX_COCOA_NSVIEW_H_
