/////////////////////////////////////////////////////////////////////////////
// Name:        colour.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "colour.h"
#endif

#include "wx/gdicmn.h"

#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <gdk/gdkprivate.h>

//-----------------------------------------------------------------------------
// wxColour
//-----------------------------------------------------------------------------

class wxColourRefData: public wxObjectRefData
{
public:
    wxColourRefData();
    ~wxColourRefData();

    void FreeColour();

public:
    GdkColor     m_color;
    GdkColormap *m_colormap;
    bool         m_hasPixel;

    friend class wxColour;
};

wxColourRefData::wxColourRefData()
{
    m_color.red = 0;
    m_color.green = 0;
    m_color.blue = 0;
    m_color.pixel = 0;
    m_colormap = (GdkColormap *) NULL;
    m_hasPixel = FALSE;
}

wxColourRefData::~wxColourRefData()
{
    FreeColour();
}

void wxColourRefData::FreeColour()
{
    if (m_colormap)
    {
#ifdef __WXGTK20__
        if ((m_colormap->visual->type == GDK_VISUAL_GRAYSCALE) ||
            (m_colormap->visual->type == GDK_VISUAL_PSEUDO_COLOR))
#else
        GdkColormapPrivate *private_colormap = (GdkColormapPrivate*) m_colormap;
        if ((private_colormap->visual->type == GDK_VISUAL_GRAYSCALE) ||
            (private_colormap->visual->type == GDK_VISUAL_PSEUDO_COLOR))
#endif
        {
            // What happens if the colour has not been allocated
            // anew but has been found? RR.
            gdk_colormap_free_colors( m_colormap, &m_color, 1 );
        }
    }
}

//-----------------------------------------------------------------------------

#define M_COLDATA ((wxColourRefData *)m_refData)

#define SHIFT (8*(sizeof(short int)-sizeof(char)))

IMPLEMENT_DYNAMIC_CLASS(wxColour,wxGDIObject)

wxColour::wxColour()
{
}

wxColour::wxColour( unsigned char red, unsigned char green, unsigned char blue )
{
    m_refData = new wxColourRefData();
    M_COLDATA->m_color.red = ((unsigned short)red) << SHIFT;
    M_COLDATA->m_color.green = ((unsigned short)green) << SHIFT;
    M_COLDATA->m_color.blue = ((unsigned short)blue) << SHIFT;
    M_COLDATA->m_color.pixel = 0;
}

void wxColour::InitFromName( const wxString &colourName )
{
    wxNode *node = (wxNode *) NULL;
    if ( (wxTheColourDatabase) && (node = wxTheColourDatabase->Find(colourName)) )
    {
        wxColour *col = (wxColour*)node->Data();
        UnRef();
        if (col) Ref( *col );
    }
    else
    {
        m_refData = new wxColourRefData();
        if (!gdk_color_parse( colourName.mb_str(), &M_COLDATA->m_color ))
        {
            // VZ: asserts are good in general but this one is triggered by
            //     calling wxColourDatabase::FindColour() with an
            //     unrecognized colour name and this can't be avoided from the
            //     user code, so don't give it here
            //
            //     a better solution would be to changed code in FindColour()

            //wxFAIL_MSG( wxT("wxColour: couldn't find colour") );

            delete m_refData;
            m_refData = (wxObjectRefData *) NULL;
        }
    }
}

wxColour::wxColour( const wxColour& col )
{
    Ref( col );
}

wxColour::~wxColour()
{
}

wxColour& wxColour::operator = ( const wxColour& col )
{
    if (*this == col) return (*this);
    Ref( col );
    return *this;
}

bool wxColour::operator == ( const wxColour& col ) const
{
    if (m_refData == col.m_refData) return TRUE;

    if (!m_refData) return FALSE;
    if (!col.m_refData) return FALSE;

    GdkColor *own = &(((wxColourRefData*)m_refData)->m_color);
    GdkColor *other = &(((wxColourRefData*)col.m_refData)->m_color);
    if (own->red != other->red) return FALSE;
    if (own->blue != other->blue) return FALSE;
    if (own->green != other->green) return FALSE;

    return TRUE;
}

bool wxColour::operator != ( const wxColour& col) const
{
    return !(*this == col);
}

void wxColour::Set( unsigned char red, unsigned char green, unsigned char blue )
{
    UnRef();
    m_refData = new wxColourRefData();
    M_COLDATA->m_color.red = ((unsigned short)red) << SHIFT;
    M_COLDATA->m_color.green = ((unsigned short)green) << SHIFT;
    M_COLDATA->m_color.blue = ((unsigned short)blue) << SHIFT;
    M_COLDATA->m_color.pixel = 0;
}

unsigned char wxColour::Red() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid colour") );

    return (unsigned char)(M_COLDATA->m_color.red >> SHIFT);
}

unsigned char wxColour::Green() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid colour") );

    return (unsigned char)(M_COLDATA->m_color.green >> SHIFT);
}

unsigned char wxColour::Blue() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid colour") );

    return (unsigned char)(M_COLDATA->m_color.blue >> SHIFT);
}

bool wxColour::Ok() const
{
    return (m_refData != NULL);
}

void wxColour::CalcPixel( GdkColormap *cmap )
{
    if (!Ok()) return;

    if ((M_COLDATA->m_hasPixel) && (M_COLDATA->m_colormap == cmap)) return;

    M_COLDATA->FreeColour();

#ifdef __WXGTK20__
    if ((cmap->visual->type == GDK_VISUAL_GRAYSCALE) ||
        (cmap->visual->type == GDK_VISUAL_PSEUDO_COLOR))
#else
    GdkColormapPrivate *private_colormap = (GdkColormapPrivate*) cmap;
    if ((private_colormap->visual->type == GDK_VISUAL_GRAYSCALE) ||
        (private_colormap->visual->type == GDK_VISUAL_PSEUDO_COLOR))
#endif
    {
        M_COLDATA->m_hasPixel = gdk_colormap_alloc_color( cmap, &M_COLDATA->m_color, FALSE, TRUE );
    }
    else
    {
        M_COLDATA->m_hasPixel = gdk_color_alloc( cmap, &M_COLDATA->m_color );
    }

    M_COLDATA->m_colormap = cmap;
}

int wxColour::GetPixel() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid colour") );

    return M_COLDATA->m_color.pixel;
}

GdkColor *wxColour::GetColor() const
{
    wxCHECK_MSG( Ok(), (GdkColor *) NULL, wxT("invalid colour") );

    return &M_COLDATA->m_color;
}


