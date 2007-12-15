/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/carbon/brush.h
// Purpose:     wxBrush class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BRUSH_H_
#define _WX_BRUSH_H_

#include "wx/gdicmn.h"
#include "wx/gdiobj.h"
#include "wx/bitmap.h"

class WXDLLIMPEXP_FWD_CORE wxBrush;

// Brush
class WXDLLEXPORT wxBrush: public wxBrushBase
{
public:
    wxBrush();
    wxBrush(const wxColour& col, int style = wxSOLID);
    wxBrush(const wxBitmap& stipple);
    virtual ~wxBrush();

    virtual void SetColour(const wxColour& col) ;
    virtual void SetColour(unsigned char r, unsigned char g, unsigned char b) ;
    virtual void SetStyle(int style)  ;
    virtual void SetStipple(const wxBitmap& stipple)  ;

    bool operator==(const wxBrush& brush) const;
    bool operator!=(const wxBrush& brush) const { return !(*this == brush); }

    const wxColour& GetColour() const ;
    virtual int GetStyle() const ;
    wxBitmap *GetStipple() const ;

protected:
    virtual wxGDIRefData *CreateGDIRefData() const;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const;

private:
    DECLARE_DYNAMIC_CLASS(wxBrush)
};

#endif // _WX_BRUSH_H_
