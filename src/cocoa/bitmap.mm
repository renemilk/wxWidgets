/////////////////////////////////////////////////////////////////////////////
// Name:        src/cocoa/bitmap.cpp
// Purpose:     wxBitmap
// Author:      David Elliott
// Modified by:
// Created:     2003/07/19
// RCS-ID:      $Id$
// Copyright:   (c) 2003 David Elliott
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/utils.h"
    #include "wx/palette.h"
    #include "wx/icon.h"
    #include "wx/colour.h"
#endif //WX_PRECOMP
#include "wx/bitmap.h"
#include "wx/image.h"
#include "wx/xpmdecod.h"
#include "wx/rawbmp.h"

#include "wx/cocoa/autorelease.h"
#include "wx/cocoa/string.h"

#import <AppKit/NSBitmapImageRep.h>
#import <AppKit/NSGraphics.h>

// ========================================================================
// wxBitmapRefData
// ========================================================================
class wxBitmapRefData: public wxGDIRefData
{
    friend class wxBitmap;
public:
    wxBitmapRefData();
    wxBitmapRefData( const wxBitmapRefData& data );
    virtual ~wxBitmapRefData();

protected:
    int                 m_width;
    int                 m_height;
    int                 m_depth;
    bool                m_ok;
    int                 m_numColors;
    wxPalette           m_bitmapPalette;
    int                 m_quality;
    WX_NSBitmapImageRep m_cocoaNSBitmapImageRep;
    wxMask             *m_bitmapMask; // Optional mask
};

#define M_BITMAPDATA ((wxBitmapRefData *)m_refData)

wxBitmapRefData::wxBitmapRefData()
{
    m_ok = FALSE;
    m_width = 0;
    m_height = 0;
    m_depth = 0;
    m_quality = 0;
    m_numColors = 0;
    m_cocoaNSBitmapImageRep = nil;
    m_bitmapMask = NULL;
}

wxBitmapRefData::wxBitmapRefData( const wxBitmapRefData& data)
{
    m_width = data.m_width;
    m_height = data.m_height;
    m_depth = data.m_depth;
    m_ok = data.m_ok;
    m_numColors = data.m_numColors;
    m_bitmapPalette = data.m_bitmapPalette;
    m_quality = data.m_quality;
    m_cocoaNSBitmapImageRep = [data.m_cocoaNSBitmapImageRep copyWithZone:nil];
    m_bitmapMask = data.m_bitmapMask?new wxMask(*data.m_bitmapMask):NULL;
}

wxBitmapRefData::~wxBitmapRefData()
{
    [m_cocoaNSBitmapImageRep release];
    m_cocoaNSBitmapImageRep = NULL;

    delete m_bitmapMask;
    m_bitmapMask = NULL;
}

// ========================================================================
// wxBitmap
// ========================================================================
IMPLEMENT_DYNAMIC_CLASS(wxBitmap, wxGDIObject)

wxBitmap::wxBitmap()
{
    m_refData = NULL;
}

wxBitmap::~wxBitmap()
{
}

wxBitmap::wxBitmap(const char bits[], int the_width, int the_height, int no_bits)
{
    m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_width = the_width ;
    M_BITMAPDATA->m_height = the_height ;
    M_BITMAPDATA->m_depth = no_bits ;
    M_BITMAPDATA->m_numColors = 0;

    /* TODO: create the bitmap from data */
}

wxBitmap::wxBitmap(int w, int h, int d)
{
    (void)Create(w, h, d);
}

wxBitmap::wxBitmap(void *data, wxBitmapType type, int width, int height, int depth)
{
    (void) Create(data, type, width, height, depth);
}

wxBitmap::wxBitmap(const wxString& filename, wxBitmapType type)
{
    LoadFile(filename, type);
}

wxObjectRefData *wxBitmap::CreateRefData() const
{
    return new wxBitmapRefData;
}

wxObjectRefData *wxBitmap::CloneRefData(const wxObjectRefData *data) const
{
    return new wxBitmapRefData(*(wxBitmapRefData*)data);
}

WX_NSBitmapImageRep wxBitmap::GetNSBitmapImageRep()
{
    if(!M_BITMAPDATA)
        return NULL;
    return M_BITMAPDATA->m_cocoaNSBitmapImageRep;
}

void wxBitmap::SetNSBitmapImageRep(WX_NSBitmapImageRep bitmapImageRep)
{
    if(!M_BITMAPDATA)
        return;
    // NOTE: No checking is done to make sure width/height agree
    [bitmapImageRep retain];
    [M_BITMAPDATA->m_cocoaNSBitmapImageRep release];
    M_BITMAPDATA->m_cocoaNSBitmapImageRep = bitmapImageRep;
}

void wxBitmap::SetWidth(int w)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_width = w;
}

void wxBitmap::SetHeight(int h)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_height = h;
}

void wxBitmap::SetDepth(int d)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_depth = d;
}

void wxBitmap::SetQuality(int q)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_quality = q;
}

void wxBitmap::SetOk(bool isOk)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_ok = isOk;
}

void wxBitmap::SetPalette(const wxPalette& palette)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_bitmapPalette = palette ;
}

void wxBitmap::SetMask(wxMask *mask)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_bitmapMask = mask ;
}

bool wxBitmap::Ok() const
{
    return m_refData && M_BITMAPDATA->m_ok;
}

wxPalette* wxBitmap::GetPalette() const
{
    if(!m_refData)
        return NULL;
    return &M_BITMAPDATA->m_bitmapPalette;
}

wxMask* wxBitmap::GetMask() const
{
    if(!m_refData)
        return NULL;
    return M_BITMAPDATA->m_bitmapMask;
}

int wxBitmap::GetDepth() const
{
    if(!m_refData)
        return 0;
    return M_BITMAPDATA->m_depth;
}

int wxBitmap::GetWidth() const
{
    if(!m_refData)
        return 0;
    return M_BITMAPDATA->m_width;
}

int wxBitmap::GetHeight() const
{
    if(!m_refData)
        return 0;
    return M_BITMAPDATA->m_height;
}

bool wxBitmap::Create(int w, int h, int d)
{
    UnRef();

    m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_width = w;
    M_BITMAPDATA->m_height = h;
    M_BITMAPDATA->m_depth = d;

    /* TODO: create new bitmap */
    M_BITMAPDATA->m_cocoaNSBitmapImageRep = [[NSBitmapImageRep alloc]
            initWithBitmapDataPlanes: NULL
            pixelsWide: w
            pixelsHigh: h
            bitsPerSample: 8
            samplesPerPixel: 3
            hasAlpha: NO
            isPlanar: NO
            colorSpaceName: NSCalibratedRGBColorSpace
            bytesPerRow: 0
            bitsPerPixel: 0];

    wxLogDebug("M_BITMAPDATA=%p NSBitmapImageRep bitmapData=%p", M_BITMAPDATA, [M_BITMAPDATA->m_cocoaNSBitmapImageRep bitmapData]);
    M_BITMAPDATA->m_ok = true;
    M_BITMAPDATA->m_numColors = 0;
    M_BITMAPDATA->m_quality = 0;
    M_BITMAPDATA->m_bitmapMask = NULL;

    return M_BITMAPDATA->m_ok;
}

bool wxBitmap::LoadFile(const wxString& filename, wxBitmapType type)
{
    wxAutoNSAutoreleasePool pool;
    UnRef();

    m_refData = new wxBitmapRefData;

    NSBitmapImageRep *imageRep = [NSBitmapImageRep
        imageRepWithContentsOfFile:wxNSStringWithWxString(filename)];

    if(imageRep)
    {
        M_BITMAPDATA->m_width = [imageRep pixelsWide];
        M_BITMAPDATA->m_height = [imageRep pixelsHigh];
        M_BITMAPDATA->m_depth = 24; // FIXME
        M_BITMAPDATA->m_ok = true;
        M_BITMAPDATA->m_numColors = 0;
        M_BITMAPDATA->m_quality = 0;
        M_BITMAPDATA->m_cocoaNSBitmapImageRep = [imageRep retain];
        M_BITMAPDATA->m_bitmapMask = NULL;
        return true;
    }
    wxImage image;
    if(!image.LoadFile(filename,type))
        return false;
    if(!image.Ok())
        return false;
    *this = wxBitmap(image);
    return true;
}

bool wxBitmap::Create(void *data, wxBitmapType type, int width, int height, int depth)
{
    UnRef();

    m_refData = new wxBitmapRefData;

    return false;
}

bool wxBitmap::SaveFile(const wxString& filename, wxBitmapType type, const wxPalette *palette) const
{
    return false;
}

bool wxBitmap::CopyFromIcon(const wxIcon& icno)
{
    return false;
}

wxBitmap wxBitmap::GetSubBitmap(wxRect const&) const
{
    return wxNullBitmap;
}

wxImage wxBitmap::ConvertToImage() const
{
    if(!M_BITMAPDATA->m_ok)
        return wxImage(5,5)/*wxNullImage*/;
    return wxImage(M_BITMAPDATA->m_width,M_BITMAPDATA->m_height);
}

bool wxBitmap::CreateFromXpm(const char **xpm)
{
#if wxUSE_IMAGE && wxUSE_XPM
    UnRef();

    wxCHECK_MSG( xpm, false, wxT("invalid XPM data") )

    wxXPMDecoder decoder;
    wxImage img = decoder.ReadData(xpm);
    wxCHECK_MSG( img.Ok(), false, wxT("invalid XPM data") )

    *this = wxBitmap(img);
    return true;
#else
    return false;
#endif
}

bool wxBitmap::CreateFromImage(const wxImage& image, int depth)
{
    UnRef();

    wxCHECK_MSG(image.Ok(), false, wxT("invalid image"));
    wxCHECK_MSG(depth == -1 || depth == 1, false, wxT("invalid bitmap depth"));

    m_refData = new wxBitmapRefData();

    M_BITMAPDATA->m_width = image.GetWidth();
    M_BITMAPDATA->m_height = image.GetHeight();
    NSBitmapImageRep *bitmapImage = [[NSBitmapImageRep alloc]
            initWithBitmapDataPlanes: NULL
            pixelsWide: image.GetWidth()
            pixelsHigh: image.GetHeight()
            bitsPerSample: 8
            samplesPerPixel: 3
            hasAlpha: NO
            isPlanar: NO
            colorSpaceName: NSCalibratedRGBColorSpace
            bytesPerRow: 0
            bitsPerPixel: 0];

    const int numBytes = image.GetWidth()*image.GetHeight()*3;
    memcpy([bitmapImage bitmapData], image.GetData(), numBytes);
    // TODO: Alpha and convert to desired depth
    M_BITMAPDATA->m_depth = 24;
    M_BITMAPDATA->m_ok = true;
    M_BITMAPDATA->m_numColors = 0;
    M_BITMAPDATA->m_quality = 0;
    M_BITMAPDATA->m_cocoaNSBitmapImageRep = bitmapImage;
    M_BITMAPDATA->m_bitmapMask = new wxMask(*this,wxColour(image.GetMaskRed(),image.GetMaskGreen(),image.GetMaskBlue()));
    return true;
}

void *wxBitmap::GetRawData(wxPixelDataBase& data, int bpp)
{
    if(!Ok())
        return NULL;

    NSBitmapImageRep *bitmapRep = M_BITMAPDATA->m_cocoaNSBitmapImageRep;
    if(!bitmapRep)
        return NULL;

    if([bitmapRep bitsPerPixel]!=bpp)
    {
        wxFAIL_MSG( _T("incorrect bitmap type in wxBitmap::GetRawData()") );
        return NULL;
    }
    data.m_width = [bitmapRep pixelsWide];
    data.m_height = [bitmapRep pixelsHigh];
    data.m_stride = [bitmapRep bytesPerRow];
    return [bitmapRep bitmapData];

    // NOTE: It is up to the user to make sure they used the proper
    // pixel format class that details what is actually inside the pixels
    // We can only check to make sure that the total number of bits per
    // pixel are being iterated over properly
    // NSBitmapImageRep can contain grayscale or CMYK data and
    // wxPixelDataBase doesn't really define the color format
}

void wxBitmap::UngetRawData(wxPixelDataBase& data)
{   // TODO
}

// ========================================================================
// wxMask
// ========================================================================

IMPLEMENT_DYNAMIC_CLASS(wxMask, wxObject)

wxMask::wxMask()
{
    m_cocoaNSBitmapImageRep = nil;
}

// Construct a mask from a bitmap and a colour indicating
// the transparent area
wxMask::wxMask(const wxBitmap& bitmap, const wxColour& colour)
{
    m_cocoaNSBitmapImageRep = nil;
    Create(bitmap, colour);
}

// Construct a mask from a bitmap and a palette index indicating
// the transparent area
wxMask::wxMask(const wxBitmap& bitmap, int paletteIndex)
{
    m_cocoaNSBitmapImageRep = nil;

    Create(bitmap, paletteIndex);
}

// Construct a mask from a mono bitmap (copies the bitmap).
wxMask::wxMask(const wxBitmap& bitmap)
{
    m_cocoaNSBitmapImageRep = nil;

    Create(bitmap);
}

wxMask::~wxMask()
{
    [m_cocoaNSBitmapImageRep release];
}

// Create a mask from a mono bitmap (copies the bitmap).
bool wxMask::Create(const wxBitmap& bitmap)
{
// TODO
    return FALSE;
}

// Create a mask from a bitmap and a palette index indicating
// the transparent area
bool wxMask::Create(const wxBitmap& bitmap, int paletteIndex)
{
// TODO
    return FALSE;
}

template <class PixelData>
static bool wxMask_CreateFromBitmapData(PixelData srcData, const wxColour& colour, unsigned char *dstData)
{
    wxCHECK_MSG(dstData,false,"Couldn't access mask data");
    class PixelData::Iterator p(srcData);
    const int nRows = srcData.GetHeight();
    const int nCols = srcData.GetWidth();
    // Total number of bytes per destination column
    const int dstRowLength = (nCols+7)/8;
    // Number of source columns that fit into a byte in the destination
    const int width_aligned = nCols/8*8;
    for(int y=0; y<nRows; ++y)
    {
        class PixelData::Iterator rowStart(p);
        unsigned char *dstRow = dstData + y*dstRowLength;
        for(int x=0; x<width_aligned; x+=8)
        {
            unsigned char *dstByte = dstRow + x/8;
            *dstByte = 0;
            // Take source RGB, compare it with the wxColour
            for(int j=0; j<8; ++j, ++p)
            {
                *dstByte +=
                (   p.Red()!=colour.Red()
                ||  p.Green()!=colour.Green()
                ||  p.Blue()!=colour.Blue()
                )   << (7-j);
            }
        }
        // Handle the remaining 0-7 pixels in the row
        unsigned char *dstByte = dstRow + width_aligned/8;
        *dstByte = 0;
        for(int j=0; j<(nCols%8); ++j, ++p)
        {
            *dstByte +=
            (   p.Red()!=colour.Red()
            ||  p.Green()!=colour.Green()
            ||  p.Blue()!=colour.Blue()
            )   << (7-j);
        }
        p = rowStart;
        p.OffsetY(srcData,1);
    }
    return true;
}

// Create a mask from a bitmap and a colour indicating
// the transparent area
bool wxMask::Create(const wxBitmap& bitmap, const wxColour& colour)
{
    wxAutoNSAutoreleasePool pool;
    if(!bitmap.Ok())
        return false;
    int bmpWidth = bitmap.GetWidth();
    int bmpHeight = bitmap.GetHeight();
    int dstRowLength = (bmpWidth+7)/8;

    // Create a bitmap image rep with 1-bit per pixel data representing
    // the alpha channel padded such that rows end on byte boundaries
    // Since NSBitmapImageRep doesn't have any sort of NSNullColorSpace
    // we must have at least one channel of non-alpha data.  In order to
    // make our life easy, we use planar data which results in two
    // separate arrays.  We don't need to touch the first because it
    // should never be used.  The second is the 1-bit "alpha" data.
    NSBitmapImageRep *maskRep = [[[NSBitmapImageRep alloc]
        initWithBitmapDataPlanes:NULL pixelsWide:bmpWidth
        pixelsHigh:bmpHeight bitsPerSample:1
        samplesPerPixel:2 hasAlpha:YES isPlanar:YES
        colorSpaceName:NSCalibratedWhiteColorSpace
        bytesPerRow:dstRowLength bitsPerPixel:1] autorelease];
    wxCHECK(maskRep,false);

    // We need the source NSBitmapImageRep to detemine its pixel format
    NSBitmapImageRep *srcBitmapRep = const_cast<wxBitmap&>(bitmap).GetNSBitmapImageRep();
    wxCHECK_MSG(srcBitmapRep,false,"Can't create mask for an uninitialized bitmap");

    // Get a pointer to the destination data
    unsigned char *dstPlanes[5] = {NULL,NULL,NULL,NULL,NULL};
    [maskRep getBitmapDataPlanes:dstPlanes];
    unsigned char *dstData = dstPlanes[1];
    if([srcBitmapRep bitsPerPixel]==24 && [srcBitmapRep bitsPerSample]==8 && [srcBitmapRep samplesPerPixel]==3 && [srcBitmapRep hasAlpha]==NO)
    {
        wxPixelData<wxBitmap,wxNativePixelFormat> pixelData(const_cast<wxBitmap&>(bitmap));
        wxCHECK_MSG(wxMask_CreateFromBitmapData(pixelData, colour, dstData),
            false, "Unable to access raw data");
    }
    else if([srcBitmapRep bitsPerPixel]==32 && [srcBitmapRep bitsPerSample]==8 && [srcBitmapRep samplesPerPixel]==4 && [srcBitmapRep hasAlpha]==YES)
    {
        wxPixelData<wxBitmap,wxAlphaPixelFormat> pixelData(const_cast<wxBitmap&>(bitmap));
        wxCHECK_MSG(wxMask_CreateFromBitmapData(pixelData, colour, dstData),
            false, "Unable to access raw data");
    }
    else
    {   wxCHECK_MSG(false,false,"Unimplemented pixel format"); }

    // maskRep was autoreleased in case we had to exit quickly
    m_cocoaNSBitmapImageRep = [maskRep retain];
    return true;
}

