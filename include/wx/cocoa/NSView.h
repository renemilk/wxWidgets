///////////////////////////////////////////////////////////////////////////////
// Name:        wx/cocoa/NSView.h
// Purpose:     wxCocoaNSView class
// Author:      David Elliott
// Modified by:
// Created:     2003/02/15
// RCS-ID:      $Id: 
// Copyright:   (c) 2003 David Elliott
// Licence:     wxWidgets licence
///////////////////////////////////////////////////////////////////////////////

#ifndef __WX_COCOA_NSVIEW_H__
#define __WX_COCOA_NSVIEW_H__

#include "wx/hashmap.h"
#include "wx/cocoa/ObjcAssociate.h"

typedef struct _NSRect NSRect;

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
    virtual bool Cocoa_drawRect(const NSRect &rect)
    {   return false; }
    virtual bool Cocoa_mouseDown(WX_NSEvent theEvent)
    {   return false; }
    virtual bool Cocoa_mouseDragged(WX_NSEvent theEvent)
    {   return false; }
    virtual bool Cocoa_mouseUp(WX_NSEvent theEvent)
    {   return false; }
    virtual bool Cocoa_mouseMoved(WX_NSEvent theEvent)
    {   return false; }
    virtual bool Cocoa_mouseEntered(WX_NSEvent theEvent)
    {   return false; }
    virtual bool Cocoa_mouseExited(WX_NSEvent theEvent)
    {   return false; }
    virtual bool Cocoa_rightMouseDown(WX_NSEvent theEvent)
    {   return false; }
    virtual bool Cocoa_rightMouseDragged(WX_NSEvent theEvent)
    {   return false; }
    virtual bool Cocoa_rightMouseUp(WX_NSEvent theEvent)
    {   return false; }
    virtual bool Cocoa_otherMouseDown(WX_NSEvent theEvent)
    {   return false; }
    virtual bool Cocoa_otherMouseDragged(WX_NSEvent theEvent)
    {   return false; }
    virtual bool Cocoa_otherMouseUp(WX_NSEvent theEvent)
    {   return false; }
};

#endif // _WX_COCOA_NSVIEW_H_
