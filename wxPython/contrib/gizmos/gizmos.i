/////////////////////////////////////////////////////////////////////////////
// Name:        gizmos.i
// Purpose:     Wrappers for the "gizmo" classes in wx/contrib
//
// Author:      Robin Dunn
//
// Created:     23-Nov-2001
// RCS-ID:      $Id$
// Copyright:   (c) 2001 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%module gizmos


%{
#include "export.h"
#include "wx/gizmos/dynamicsash.h"
%}

//---------------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

%extern wx.i
%extern windows.i
%extern _defs.i
%extern events.i
%extern controls.i


//----------------------------------------------------------------------

enum {
    wxEVT_DYNAMIC_SASH_SPLIT,
    wxEVT_DYNAMIC_SASH_UNIFY,

    wxDS_MANAGE_SCROLLBARS,
    wxDS_DRAG_CORNER,
};


/*
    wxDynamicSashSplitEvents are sent to your view by wxDynamicSashWindow
    whenever your view is being split by the user.  It is your
    responsibility to handle this event by creating a new view window as
    a child of the wxDynamicSashWindow.  wxDynamicSashWindow will
    automatically reparent it to the proper place in its window hierarchy.
*/
class wxDynamicSashSplitEvent : public wxCommandEvent {
public:
    wxDynamicSashSplitEvent(wxObject *target);
};


/*
    wxDynamicSashUnifyEvents are sent to your view by wxDynamicSashWindow
    whenever the sash which splits your view and its sibling is being
    reunified such that your view is expanding to replace its sibling.
    You needn't do anything with this event if you are allowing
    wxDynamicSashWindow to manage your view's scrollbars, but it is useful
    if you are managing the scrollbars yourself so that you can keep
    the scrollbars' event handlers connected to your view's event handler
    class.
*/
class wxDynamicSashUnifyEvent : public wxCommandEvent {
public:
    wxDynamicSashUnifyEvent(wxObject *target);
};



/*

    wxDynamicSashWindow

    wxDynamicSashWindow widgets manages the way other widgets are viewed.
    When a wxDynamicSashWindow is first shown, it will contain one child
    view, a viewport for that child, and a pair of scrollbars to allow the
    user to navigate the child view area.  Next to each scrollbar is a small
    tab.  By clicking on either tab and dragging to the appropriate spot, a
    user can split the view area into two smaller views separated by a
    draggable sash.  Later, when the user wishes to reunify the two subviews,
    the user simply drags the sash to the side of the window.
    wxDynamicSashWindow will automatically reparent the appropriate child
    view back up the window hierarchy, and the wxDynamicSashWindow will have
    only one child view once again.

    As an application developer, you will simply create a wxDynamicSashWindow
    using either the Create() function or the more complex constructor
    provided below, and then create a viewfrom wxPython.wx import * window whose parent is the
    wxDynamicSashWindow.  The child should respond to
    wxDynamicSashSplitEvents -- perhaps with an OnSplit() event handler -- by
    constructing a new view window whose parent is also the
    wxDynamicSashWindow.  That's it!  Now your users can dynamically split
    and reunify the view you provided.

    If you wish to handle the scrollbar events for your view, rather than
    allowing wxDynamicSashWindow to do it for you, things are a bit more
    complex.  (You might want to handle scrollbar events yourself, if,
    for instance, you wish to scroll a subwindow of the view you add to
    your wxDynamicSashWindow object, rather than scrolling the whole view.)
    In this case, you will need to construfrom wxPython.wx import *ct your wxDynamicSashWindow without
    the wxDS_MANAGE_SCROLLBARS style and  you will need to use the
    GetHScrollBar() and GetVScrollBar() methods to retrieve the scrollbar
    controls and call SetEventHanler() on them to redirect the scrolling
    events whenever your window is reparented by wxDyanmicSashWindow.
    You will need to set the scrollbars' event handler at three times:

        *  When your view is created
        *  When your view receives a wxDynamicSashSplitEvent
        *  When your view receives a wxDynamicSashUnifyEvent

    See the dynsash_switch sample application for an example which does this.

*/

class wxDynamicSashWindow : public wxWindow {
public:
    wxDynamicSashWindow(wxWindow *parent, wxWindowID id,
                        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                        long style = wxCLIP_CHILDREN | wxDS_MANAGE_SCROLLBARS | wxDS_DRAG_CORNER,
                        const char* name = "dynamicSashWindow");
    %name(wxPreDynamicSashWindow)wxDynamicSashWindow();

    bool Create(wxWindow *parent, wxWindowID id,
                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                long style = wxCLIP_CHILDREN | wxDS_MANAGE_SCROLLBARS | wxDS_DRAG_CORNER,
                const char* name = "dynamicSashWindow");

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreDynamicSashWindow:val._setOORInfo(val)"

    wxScrollBar *GetHScrollBar(const wxWindow *child) const;
    wxScrollBar *GetVScrollBar(const wxWindow *child) const;
};



//----------------------------------------------------------------------
// Python functions to act like the event macros

%pragma(python) code = "
def EVT_DYNAMIC_SASH_SPLIT(win, id, func):
    win.Connect(id, -1, wxEVT_DYNAMIC_SASH_SPLIT, func)

def EVT_DYNAMIC_SASH_UNIFY(win, id, func):
    win.Connect(id, -1, wxEVT_DYNAMIC_SASH_UNIFY, func)
"

//----------------------------------------------------------------------

%init %{

    wxClassInfo::CleanUpClasses();
    wxClassInfo::InitializeClasses();

%}


//----------------------------------------------------------------------

%pragma(python) include="_gizmoextras.py";

//----------------------------------------------------------------------
//----------------------------------------------------------------------


