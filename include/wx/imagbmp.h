/////////////////////////////////////////////////////////////////////////////
// Name:        imagbmp.h
// Purpose:     wxImage BMP, ICO and CUR handlers
// Author:      Robert Roebling, Chris Elliott
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling, Chris Elliott
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_IMAGBMP_H_
#define _WX_IMAGBMP_H_

#ifdef __GNUG__
#pragma interface "imagbmp.h"
#endif

#include "wx/image.h"

// defines for saving the BMP file in different formats, Bits Per Pixel
// USE: wximage.SetOption( wxBMP_FORMAT, wxBMP_xBPP );
#define wxBMP_FORMAT wxString(_T("wxBMP_FORMAT"))

enum
{
    wxBMP_24BPP        = 24, // default, do not need to set
    //wxBMP_16BPP      = 16, // wxQuantize can only do 236 colors?
    wxBMP_8BPP         =  8, // 8bpp, quantized colors
    wxBMP_8BPP_GREY    =  9, // 8bpp, rgb averaged to greys
    wxBMP_8BPP_GRAY    =  wxBMP_8BPP_GREY,
    wxBMP_8BPP_RED     = 10, // 8bpp, red used as greyscale
    wxBMP_8BPP_PALETTE = 11, // 8bpp, use the wxImage's palette
    wxBMP_4BPP         =  4, // 4bpp, quantized colors
    wxBMP_1BPP         =  1, // 1bpp, quantized "colors"
    wxBMP_1BPP_BW      =  2  // 1bpp, black & white from red
};

// ----------------------------------------------------------------------------
// wxBMPHandler
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxBMPHandler : public wxImageHandler
{
public:
    wxBMPHandler()
    {
        m_name = _T("Windows bitmap file");
        m_extension = _T("bmp");
        m_type = wxBITMAP_TYPE_BMP;
        m_mime = _T("image/x-bmp");
    };

#if wxUSE_STREAMS
    virtual bool SaveFile( wxImage *image, wxOutputStream& stream, bool verbose=TRUE );
    virtual bool LoadFile( wxImage *image, wxInputStream& stream, bool verbose=TRUE, int index=0 );
    virtual bool DoCanRead( wxInputStream& stream );
                                
protected:
    bool SaveDib(wxImage *image, wxOutputStream& stream, bool verbose, 
                 bool IsBmp, bool IsMask);
    bool DoLoadDib(wxImage *image, int width, int height, int bpp, int ncolors, 
                   int comp, off_t bmpOffset, wxInputStream& stream,
                   bool verbose, bool IsBmp, bool hasPalette);
    bool LoadDib(wxImage *image, wxInputStream& stream, bool verbose, bool IsBmp);
#endif // wxUSE_STREAMS

private:
     DECLARE_DYNAMIC_CLASS(wxBMPHandler)
};


// ----------------------------------------------------------------------------
// wxICOHandler
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxICOHandler : public wxBMPHandler
{
public:
    wxICOHandler()
    {
        m_name = _T("Windows icon file");
        m_extension = _T("ico");
        m_type = wxBITMAP_TYPE_ICO;
        m_mime = _T("image/x-ico");
    };

#if wxUSE_STREAMS
    virtual bool SaveFile( wxImage *image, wxOutputStream& stream, bool verbose=TRUE );
    virtual bool LoadFile( wxImage *image, wxInputStream& stream, bool verbose=TRUE, int index=0 );
    virtual bool DoCanRead( wxInputStream& stream );
    virtual int GetImagesCount( wxInputStream& stream );
#endif // wxUSE_STREAMS

private:
    DECLARE_DYNAMIC_CLASS(wxICOHandler)
};


// ----------------------------------------------------------------------------
// wxCURHandler
// ----------------------------------------------------------------------------

// These two options are filled in upon reading CUR file and can (should) be
// specified when saving a CUR file - they define the hotspot of the cursor:
#define wxCUR_HOTSPOT_X  wxT("HotSpotX")
#define wxCUR_HOTSPOT_Y  wxT("HotSpotY")

class WXDLLEXPORT wxCURHandler : public wxICOHandler
{
public:
    wxCURHandler()
    {
        m_name = _T("Windows cursor file");
        m_extension = _T("cur");
        m_type = wxBITMAP_TYPE_CUR;
        m_mime = _T("image/x-cur");
    };
    
    // VS: This handler's meat is implemented inside wxICOHandler (the two
    //     formats are almost identical), but we hide this fact at
    //     the API level, since it is a mere implementation detail.

#if wxUSE_STREAMS
    virtual bool DoCanRead( wxInputStream& stream );
#endif // wxUSE_STREAMS

private:
    DECLARE_DYNAMIC_CLASS(wxCURHandler)
};

#endif // _WX_IMAGBMP_H_
