/////////////////////////////////////////////////////////////////////////////
// Name:        pen.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKPENH__
#define __GTKPENH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/string.h"
#include "wx/gdiobj.h"
#include "wx/gdicmn.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxPen;

//-----------------------------------------------------------------------------
// wxPen
//-----------------------------------------------------------------------------

class wxPen: public wxGDIObject
{
  DECLARE_DYNAMIC_CLASS(wxPen)

  public:

    wxPen();
    wxPen( const wxColour &colour, int width, int style );
    wxPen( const wxPen& pen );
    ~wxPen();
    wxPen& operator = ( const wxPen& pen );
    bool operator == ( const wxPen& pen );
    bool operator != ( const wxPen& pen );

    void SetColour( const wxColour &colour );
    void SetColour( int red, int green, int blue );
    void SetCap( int capStyle );
    void SetJoin( int joinStyle );
    void SetStyle( int style );
    void SetWidth( int width );
    wxColour &GetColour() const;
    int GetCap() const;
    int GetJoin() const;
    int GetStyle() const;
    int GetWidth() const;
    bool Ok() const;

    void Unshare();

    // no data :-)
};

#endif // __GTKPENH__
