/////////////////////////////////////////////////////////////////////////////
// Name:        brush.cpp
// Purpose:     wxBrush
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "brush.h"
#endif

#include "wx/setup.h"
#include "wx/utils.h"
#include "wx/brush.h"

#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxBrush, wxGDIObject)
#endif

wxBrushRefData::wxBrushRefData()
{
    m_style = wxSOLID;
    m_isMacTheme = false ;
    m_isMacThemeBackground = false ;
}

wxBrushRefData::wxBrushRefData(const wxBrushRefData& data)
{
  m_style = data.m_style;
  m_stipple = data.m_stipple;
  m_colour = data.m_colour;
  m_isMacTheme = data.m_isMacTheme ;
  m_macThemeBrush = data.m_macThemeBrush ;
}

wxBrushRefData::~wxBrushRefData()
{
}

// Brushes
wxBrush::wxBrush()
{
}

wxBrush::~wxBrush()
{
}

wxBrush::wxBrush(const wxColour& col, int Style)
{
    m_refData = new wxBrushRefData;

    M_BRUSHDATA->m_colour = col;
    M_BRUSHDATA->m_style = Style;

    RealizeResource();
}

wxBrush::wxBrush(const wxBitmap& stipple)
{
    m_refData = new wxBrushRefData;

    M_BRUSHDATA->m_style = wxSTIPPLE;
    M_BRUSHDATA->m_stipple = stipple;

    RealizeResource();
}

wxBrush::wxBrush(ThemeBrush macThemeBrush ) 
{
    m_refData = new wxBrushRefData;

    M_BRUSHDATA->m_isMacTheme = true;
    M_BRUSHDATA->m_macThemeBrush = macThemeBrush;

    RealizeResource();
}
void wxBrush::Unshare()
{
	// Don't change shared data
	if (!m_refData)
    {
		m_refData = new wxBrushRefData();
	}
    else
    {
		wxBrushRefData* ref = new wxBrushRefData(*(wxBrushRefData*)m_refData);
		UnRef();
		m_refData = ref;
	}
}

void wxBrush::SetColour(const wxColour& col)
{
    Unshare();
    M_BRUSHDATA->m_isMacTheme = false;
    M_BRUSHDATA->m_isMacThemeBackground = false ;
    M_BRUSHDATA->m_colour = col;

    RealizeResource();
}

void wxBrush::SetColour(unsigned char r, unsigned char g, unsigned char b)
{
    Unshare();

    M_BRUSHDATA->m_isMacTheme = false;
    M_BRUSHDATA->m_isMacThemeBackground = false ;
    M_BRUSHDATA->m_colour.Set(r, g, b);

    RealizeResource();
}

void wxBrush::SetStyle(int Style)
{
    Unshare();

    M_BRUSHDATA->m_isMacTheme = false;
    M_BRUSHDATA->m_isMacThemeBackground = false ;
    M_BRUSHDATA->m_style = Style;

    RealizeResource();
}

void wxBrush::SetStipple(const wxBitmap& Stipple)
{
    Unshare();

    M_BRUSHDATA->m_stipple = Stipple;

    RealizeResource();
}

void wxBrush::SetMacTheme(ThemeBrush macThemeBrush)
{
    Unshare();

    M_BRUSHDATA->m_isMacTheme = true;
    M_BRUSHDATA->m_isMacThemeBackground = false ;
    M_BRUSHDATA->m_macThemeBrush = macThemeBrush;

    RealizeResource();
}

void wxBrush::SetMacThemeBackground(ThemeBackgroundKind macThemeBackground)
{
    Unshare();

    M_BRUSHDATA->m_isMacTheme = false;
    M_BRUSHDATA->m_isMacThemeBackground = true ;
    M_BRUSHDATA->m_macThemeBackground = macThemeBackground;

    RealizeResource();
}

bool wxBrush::RealizeResource()
{
    return TRUE;
}

