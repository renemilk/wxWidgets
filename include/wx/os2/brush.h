/////////////////////////////////////////////////////////////////////////////
// Name:        brush.h
// Purpose:     wxBrush class
// Author:      David Webster
// Modified by:
// Created:     10/13/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BRUSH_H_
#define _WX_BRUSH_H_

#include "wx/gdicmn.h"
#include "wx/gdiobj.h"
#include "wx/bitmap.h"

class WXDLLEXPORT wxBrush;

class WXDLLEXPORT wxBrushRefData: public wxGDIRefData
{
    friend class WXDLLEXPORT wxBrush;
public:
    wxBrushRefData();
    wxBrushRefData(const wxBrushRefData& data);
    ~wxBrushRefData();

protected:
    int           m_style;
    wxBitmap      m_stipple ;
    wxColour      m_colour;
    WXHBRUSH      m_hBrush;
};

#define M_BRUSHDATA ((wxBrushRefData *)m_refData)

// Brush
class WXDLLEXPORT wxBrush: public wxGDIObject
{
  DECLARE_DYNAMIC_CLASS(wxBrush)

public:
  wxBrush();
  wxBrush(const wxColour& col, int style);
  wxBrush(const wxBitmap& stipple);
  inline wxBrush(const wxBrush& brush) { Ref(brush); }
  ~wxBrush();

  virtual void SetColour(const wxColour& col)  ;
  virtual void SetColour(unsigned char r, unsigned char g, unsigned char b)  ;
  virtual void SetStyle(int style)  ;
  virtual void SetStipple(const wxBitmap& stipple)  ;

  inline wxBrush& operator = (const wxBrush& brush) { if (*this == brush) return (*this); Ref(brush); return *this; }
  inline bool operator == (const wxBrush& brush) { return m_refData == brush.m_refData; }
  inline bool operator != (const wxBrush& brush) { return m_refData != brush.m_refData; }

  inline wxColour& GetColour() const { return (M_BRUSHDATA ? M_BRUSHDATA->m_colour : wxNullColour); };
  inline int GetStyle() const { return (M_BRUSHDATA ? M_BRUSHDATA->m_style : 0); };
  inline wxBitmap *GetStipple() const { return (M_BRUSHDATA ? & M_BRUSHDATA->m_stipple : 0); };

  virtual bool Ok() const { return (m_refData != NULL) ; }

// Implementation

  // Useful helper: create the brush resource
  bool RealizeResource();

  WXHANDLE GetResourceHandle(void) ;
  bool FreeResource(bool force = FALSE);
  bool IsFree() const;

  // When setting properties, we must make sure we're not changing
  // another object
  void Unshare();
};

#endif
    // _WX_BRUSH_H_
