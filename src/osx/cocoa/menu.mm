/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/menu.mm
// Purpose:     wxMenu, wxMenuBar, wxMenuItem
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id: menu.cpp 54129 2008-06-11 19:30:52Z SC $
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// headers & declarations
// ============================================================================

// wxWidgets headers
// -----------------

#include "wx/wxprec.h"

#include "wx/menu.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/app.h"
    #include "wx/utils.h"
    #include "wx/frame.h"
    #include "wx/menuitem.h"
#endif

#include "wx/osx/private.h"

// other standard headers
// ----------------------
#include <string.h>

@implementation wxNSMenu

- (id) init
{
    [super init];
    return self;
}

- (void)setImplementation: (wxMenuImpl *) theImplementation
{
    impl = theImplementation;
}

- (wxMenuImpl*) implementation
{
    return impl;
}

@end

@interface wxNSMenuController : NSObject
{
}

- (void)menuWillOpen:(NSMenu *)menu;
- (void)menuDidClose:(NSMenu *)menu;
- (void)menu:(NSMenu *)menu willHighlightItem:(NSMenuItem *)item;

@end

@implementation wxNSMenuController

- (id) init
{
    [super init];
    return self;
}

- (void)menuWillOpen:(NSMenu *)smenu
{
    wxNSMenu* menu = (wxNSMenu*) smenu;
    wxMenuImpl* menuimpl = [menu implementation];
    if ( menuimpl )
    {
        wxMenu* wxpeer = (wxMenu*) menuimpl->GetWXPeer();
        if ( wxpeer )
            wxpeer->HandleMenuOpened();
    }
}

- (void)menuDidClose:(NSMenu *)smenu
{
    wxNSMenu* menu = (wxNSMenu*) smenu;
    wxMenuImpl* menuimpl = [menu implementation];
    if ( menuimpl )
    {
        wxMenu* wxpeer = (wxMenu*) menuimpl->GetWXPeer();
        if ( wxpeer )
            wxpeer->HandleMenuClosed();
    }
}

- (void)menu:(NSMenu *)smenu willHighlightItem:(NSMenuItem *)item
{
    wxNSMenu* menu = (wxNSMenu*) smenu;
    wxMenuImpl* menuimpl = [menu implementation];
    if ( menuimpl )
    {
        wxMenu* wxpeer = (wxMenu*) menuimpl->GetWXPeer();
        if ( [ item isKindOfClass:[wxNSMenuItem class] ] )
        {
            wxMenuItemImpl* menuitemimpl = (wxMenuItemImpl*) [ (wxNSMenuItem*) item implementation ];
            if ( wxpeer && menuitemimpl )
            {
                wxpeer->HandleMenuItemHighlighted( menuitemimpl->GetWXPeer() );
            }
        }
    }
}

@end

@interface NSApplication(MissingAppleMenuCall) 
- (void)setAppleMenu:(NSMenu *)menu; 
@end 

class wxMenuCocoaImpl : public wxMenuImpl 
{
public :
    wxMenuCocoaImpl( wxMenu* peer , NSMenu* menu) : wxMenuImpl(peer), m_osxMenu(menu)
    {
    }
    
    virtual ~wxMenuCocoaImpl();
        
    virtual void InsertOrAppend(wxMenuItem *pItem, size_t pos) 
    {
        if ( pos == (size_t) -1 )
            [m_osxMenu addItem:(NSMenuItem*) pItem->GetPeer()->GetHMenuItem() ];
        else
            [m_osxMenu insertItem:(NSMenuItem*) pItem->GetPeer()->GetHMenuItem() atIndex:pos];
    }
        
    virtual void Remove( wxMenuItem *pItem ) 
    {
        [m_osxMenu removeItem:(NSMenuItem*) pItem->GetPeer()->GetHMenuItem()];
    }
    
    virtual void MakeRoot()
    {
        [NSApp setMainMenu:m_osxMenu];
        [NSApp setAppleMenu:[[m_osxMenu itemAtIndex:0] submenu]];
    }

    virtual void Enable( bool enable )
    {
    }
    
    virtual void SetTitle( const wxString& text )
    {
        wxCFStringRef cfText(text);
        [m_osxMenu setTitle:cfText.AsNSString()];
    }

    virtual void PopUp( wxWindow *win, int x, int y )
    {
        win->ScreenToClient( &x , &y ) ;
        NSView *view = win->GetPeer()->GetWXWidget();
        NSRect frame = [view frame];
        frame.origin.x = x;
        frame.origin.y = y;
        frame.size.width = 1;
        frame.size.height = 1;
        NSPopUpButtonCell *popUpButtonCell = [[NSPopUpButtonCell alloc] initTextCell:@"" pullsDown:NO];
        [popUpButtonCell setAutoenablesItems:NO];
        [popUpButtonCell setAltersStateOfSelectedItem:NO];
        [popUpButtonCell setMenu:m_osxMenu];
        [popUpButtonCell selectItem:nil];
        [popUpButtonCell performClickWithFrame:frame inView:view];
        [popUpButtonCell release];
    }

    WXHMENU GetHMenu() { return m_osxMenu; }

    static wxMenuImpl* Create( wxMenu* peer, const wxString& title );
    static wxMenuImpl* CreateRootMenu( wxMenu* peer );
protected :
    NSMenu* m_osxMenu;
} ;

wxMenuCocoaImpl::~wxMenuCocoaImpl()
{
    [m_osxMenu release];
}

wxMenuImpl* wxMenuImpl::Create( wxMenu* peer, const wxString& title )
{
    static wxNSMenuController* controller = NULL;
    if ( controller == NULL )
    {
        controller = [[wxNSMenuController alloc] init];
    }
    wxCFStringRef cfText( title );
    wxNSMenu* menu = [[wxNSMenu alloc] initWithTitle:cfText.AsNSString()];
    wxMenuImpl* c = new wxMenuCocoaImpl( peer, menu );
    [menu setDelegate:controller];
    [menu setImplementation:c];
    return c;
}
