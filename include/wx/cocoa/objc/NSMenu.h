///////////////////////////////////////////////////////////////////////////////
// Name:        wx/cocoa/objc/NSMenu.h
// Purpose:     WXNSMenu class
// Author:      David Elliott
// Modified by:
// Created:     2007/04/20 (move from NSMenu.mm)
// RCS-ID:      $Id$
// Copyright:   (c) 2002 David Elliott
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef __WX_COCOA_OBJC_NSMENU_H__
#define __WX_COCOA_OBJC_NSMENU_H__

#import <AppKit/NSMenu.h>

// ============================================================================
// @class WXNSMenu
// ============================================================================
@interface WXNSMenu : NSMenu
{
}

- (void)dealloc;

@end // WXNSMenu

#endif //ndef __WX_COCOA_OBJC_NSMENU_H__
