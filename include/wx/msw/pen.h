/////////////////////////////////////////////////////////////////////////////
// Name:        pen.h
// Purpose:     wxPen class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PEN_H_
#define _WX_PEN_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "pen.h"
#endif

#include "wx/gdiobj.h"
#include "wx/bitmap.h"
#include "wx/colour.h"

typedef WXDWORD wxMSWDash;

class WXDLLEXPORT wxPen;

class WXDLLEXPORT wxPenRefData: public wxGDIRefData
{
    friend class WXDLLEXPORT wxPen;
public:
    wxPenRefData();
    wxPenRefData(const wxPenRefData& data);
    ~wxPenRefData();

protected:
  int           m_width;
  int           m_style;
  int           m_join ;
  int           m_cap ;
  wxBitmap      m_stipple ;
  int           m_nbDash ;
  wxDash *      m_dash ;
  wxColour      m_colour;
  WXHPEN        m_hPen;

private:
// Cannot use
//  DECLARE_NO_COPY_CLASS(wxPenRefData)
// because copy constructor is explicitly declared above;
// but no copy assignment operator is defined, so declare
// it private to prevent the compiler from defining it:
    wxPenRefData& operator=(const wxPenRefData&);
};

#define M_PENDATA ((wxPenRefData *)m_refData)
#define wxPENDATA(x) ((wxPenRefData *)(x).m_refData)

// Pen
class WXDLLEXPORT wxPen: public wxGDIObject
{
  DECLARE_DYNAMIC_CLASS(wxPen)
public:
  wxPen();
  wxPen(const wxColour& col, int width = 1, int style = wxSOLID);
  wxPen(const wxBitmap& stipple, int width);
  inline wxPen(const wxPen& pen) { Ref(pen); }
  ~wxPen();

  inline wxPen& operator = (const wxPen& pen) { if (*this == pen) return (*this); Ref(pen); return *this; }
  inline bool operator == (const wxPen& pen) const 
  { 
      // It is impossible to know if the user dashes have changed, 
      // so we must assume that they have
      if ( m_refData && pen.m_refData )
      {
          if ( M_PENDATA->m_nbDash != 0 || wxPENDATA(pen)->m_nbDash != 0 )
              return false;
      }
      return m_refData == pen.m_refData;
  }
  inline bool operator != (const wxPen& pen) const 
  { 
      // It is impossible to know if the user dashes have changed, 
      // so we must assume that they have
      if ( m_refData && pen.m_refData )
      {
          if ( M_PENDATA->m_nbDash != 0 || wxPENDATA(pen)->m_nbDash != 0 )
              return true;
      }
      return m_refData != pen.m_refData; 
  }

  virtual bool Ok() const { return (m_refData != NULL) ; }

  // Override in order to recreate the pen
  void SetColour(const wxColour& col) ;
  void SetColour(unsigned char r, unsigned char g, unsigned char b);

  void SetWidth(int width)  ;
  void SetStyle(int style)  ;
  void SetStipple(const wxBitmap& stipple)  ;
  void SetDashes(int nb_dashes, const wxDash *dash)  ;
  void SetJoin(int join)  ;
  void SetCap(int cap)  ;

  inline wxColour& GetColour() const { return (M_PENDATA ? M_PENDATA->m_colour : wxNullColour); };
  inline int GetWidth() const { return (M_PENDATA ? M_PENDATA->m_width : 0); };
  inline int GetStyle() const { return (M_PENDATA ? M_PENDATA->m_style : 0); };
  inline int GetJoin() const { return (M_PENDATA ? M_PENDATA->m_join : 0); };
  inline int GetCap() const { return (M_PENDATA ? M_PENDATA->m_cap : 0); };
  inline int GetDashes(wxDash **ptr) const
  {
    *ptr = (M_PENDATA ? (wxDash*)M_PENDATA->m_dash : (wxDash*) NULL);
    return (M_PENDATA ? M_PENDATA->m_nbDash : 0);
  }
  wxDash* GetDash() const { return (M_PENDATA ? (wxDash*)M_PENDATA->m_dash : (wxDash*)NULL); };
  inline int GetDashCount() const { return (M_PENDATA ? M_PENDATA->m_nbDash : 0); };

  inline wxBitmap *GetStipple() const { return (M_PENDATA ? (& M_PENDATA->m_stipple) : (wxBitmap*) NULL); };

  // Internal
  bool RealizeResource();
  bool FreeResource(bool force = FALSE);
  WXHANDLE GetResourceHandle() const;
  bool IsFree() const;
  void Unshare();
};

int wx2msPenStyle(int wx_style);

#endif
    // _WX_PEN_H_
