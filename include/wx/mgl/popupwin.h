///////////////////////////////////////////////////////////////////////////////
// Name:        wx/mgl/popupwin.h
// Purpose:     wxPopupWindow class for wxMGL
// Author:      Vadim Zeitlin
// Created:     06.01.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_POPUPWIN_H_
#define _WX_POPUPWIN_H_

// ----------------------------------------------------------------------------
// wxPopupWindow
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxPopupWindow : public wxPopupWindowBase
{
public:
    wxPopupWindow() { }

    wxPopupWindow(wxWindow *parent) { (void)Create(parent); }

    bool Create(wxWindow *parent, int flags = wxBORDER_NONE)
    {
        return wxPopupWindowBase::Create(parent) &&
               wxWindow::Create(parent, -1,
                                wxDefaultPosition, wxDefaultSize,
                                (flags & wxBORDER_MASK) | wxPOPUP_WINDOW);
    }
};

#endif // _WX_POPUPWIN_H_

