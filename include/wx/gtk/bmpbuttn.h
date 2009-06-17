/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/bmpbutton.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_BMPBUTTON_H_
#define _WX_GTK_BMPBUTTON_H_

// ----------------------------------------------------------------------------
// wxBitmapButton
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxBitmapButton : public wxBitmapButtonBase
{
public:
    wxBitmapButton() { }

    wxBitmapButton(wxWindow *parent,
                   wxWindowID id,
                   const wxBitmap& bitmap,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxButtonNameStr)
    {
        Create(parent, id, bitmap, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxBitmap& bitmap,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxButtonNameStr)
    {
        if ( !wxBitmapButtonBase::Create(parent, id, "",
                                         pos, size,
                                         style | wxBU_NOTEXT,
                                         validator, name) )
            return false;

        SetBitmapLabel(bitmap);

        return true;
    }

    DECLARE_DYNAMIC_CLASS(wxBitmapButton)
};

#endif // _WX_GTK_BMPBUTTON_H_
