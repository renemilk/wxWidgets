/////////////////////////////////////////////////////////////////////////////
// Name:        unix/fontutil.cpp
// Purpose:     Font helper functions for X11 (GDK/X)
// Author:      Vadim Zeitlin
// Modified by:
// Created:     05.11.99
// RCS-ID:      $Id$
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "fontutil.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif // PCH

#ifdef __X__
    #ifdef __VMS__
        #pragma message disable nosimpint
    #endif

    #include <X11/Xlib.h>

    #ifdef __VMS__
        #pragma message enable nosimpint
    #endif

    #include "wx/utils.h"       // for wxGetDisplay()
#elif defined(__WXGTK__)
    // we have to declare struct tm to avoid problems with first forward
    // declaring it in C code (glib.h included from gdk.h does it) and then
    // defining it when time.h is included from the headers below - this is
    // known not to work at least with Sun CC 6.01
    #include <time.h>

    #include <gdk/gdk.h>
#endif

#include "wx/fontutil.h"
#include "wx/fontmap.h"
#include "wx/tokenzr.h"
#include "wx/hash.h"
#include "wx/module.h"

// ----------------------------------------------------------------------------
// private data
// ----------------------------------------------------------------------------

static wxHashTable *g_fontHash = (wxHashTable*) NULL;

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

// define the functions to create and destroy native fonts for this toolkit
#ifdef __X__
    static inline wxNativeFont wxLoadFont(const wxString& fontSpec)
    {
        return XLoadQueryFont((Display *)wxGetDisplay(), fontSpec);
    }

    static inline void wxFreeFont(wxNativeFont font)
    {
        XFreeFont((Display *)wxGetDisplay(), (XFontStruct *)font);
    }
#elif defined(__WXGTK__)
    static inline wxNativeFont wxLoadFont(const wxString& fontSpec)
    {
       return gdk_font_load( wxConvertWX2MB(fontSpec) );
    }

    static inline void wxFreeFont(wxNativeFont font)
    {
        gdk_font_unref(font);
    }
#else
    #error "Unknown GUI toolkit"
#endif

static bool wxTestFontSpec(const wxString& fontspec);

static wxNativeFont wxLoadQueryFont(int pointSize,
                                    int family,
                                    int style,
                                    int weight,
                                    bool underlined,
                                    const wxString& facename,
                                    const wxString& xregistry,
                                    const wxString& xencoding,
                                    wxString* xFontName);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxNativeEncodingInfo
// ----------------------------------------------------------------------------

// convert to/from the string representation: format is
//      encodingid;registry;encoding[;facename]
bool wxNativeEncodingInfo::FromString(const wxString& s)
{
    // use ";", not "-" because it may be part of encoding name
    wxStringTokenizer tokenizer(s, _T(";"));

    wxString encid = tokenizer.GetNextToken();
    long enc;
    if ( !encid.ToLong(&enc) )
        return FALSE;
    encoding = (wxFontEncoding)enc;

    xregistry = tokenizer.GetNextToken();
    if ( !xregistry )
        return FALSE;

    xencoding = tokenizer.GetNextToken();
    if ( !xencoding )
        return FALSE;

    // ok even if empty
    facename = tokenizer.GetNextToken();

    return TRUE;
}

wxString wxNativeEncodingInfo::ToString() const
{
    wxString s;
    s << (long)encoding << _T(';') << xregistry << _T(';') << xencoding;
    if ( !!facename )
    {
        s << _T(';') << facename;
    }

    return s;
}

// ----------------------------------------------------------------------------
// wxNativeFontInfo
// ----------------------------------------------------------------------------

void wxNativeFontInfo::Init()
{
    xFontName.clear();
}

bool wxNativeFontInfo::FromString(const wxString& s)
{
    wxStringTokenizer tokenizer(s, _T(";"));

    // check the version
    wxString token = tokenizer.GetNextToken();
    if ( token != _T('0') )
        return FALSE;

    xFontName = tokenizer.GetNextToken();

    // this should be the end
    if ( tokenizer.HasMoreTokens() )
        return FALSE;

    return FromXFontName(xFontName);
}

wxString wxNativeFontInfo::ToString() const
{
    // 0 is the version
    return wxString::Format(_T("%d;%s"), 0, GetXFontName().c_str());
}

bool wxNativeFontInfo::FromUserString(const wxString& s)
{
    return FromXFontName(s);
}

wxString wxNativeFontInfo::ToUserString() const
{
    return GetXFontName();
}

wxString wxNativeFontInfo::GetXFontComponent(wxXLFDField field) const
{
    wxCHECK_MSG( field < wxXLFD_MAX, _T(""), _T("invalid XLFD field") );

    if ( fontElements[0].empty() )
    {
        // const_cast
        if ( !((wxNativeFontInfo *)this)->FromXFontName(xFontName) )
            return _T("");
    }

    return fontElements[field];
}

bool wxNativeFontInfo::FromXFontName(const wxString& fontname)
{
    // TODO: we should be able to handle the font aliases here, but how?
    wxStringTokenizer tokenizer(fontname, _T("-"), wxTOKEN_STRTOK);

    for ( size_t n = 0; n < WXSIZEOF(fontElements); n++ )
    {
        if ( !tokenizer.HasMoreTokens() )
        {
            // not enough elements in the XLFD - or maybe an alias
            return FALSE;
        }

        fontElements[n] = tokenizer.GetNextToken();
    }

    // this should be all
    return !tokenizer.HasMoreTokens();
}

wxString wxNativeFontInfo::GetXFontName() const
{
    if ( xFontName.empty() )
    {
        for ( size_t n = 0; n < WXSIZEOF(fontElements); n++ )
        {
            // replace the non specified elements with '*' except for the
            // additional style which is usually just omitted
            wxString elt = fontElements[n];
            if ( elt.empty() && n != 5 )
            {
                elt = _T('*');
            }

            // const_cast
            ((wxNativeFontInfo *)this)->xFontName << _T('-') << elt;
        }
    }

    return xFontName;
}

// ----------------------------------------------------------------------------
// common functions
// ----------------------------------------------------------------------------

bool wxGetNativeFontEncoding(wxFontEncoding encoding,
                             wxNativeEncodingInfo *info)
{
    wxCHECK_MSG( info, FALSE, _T("bad pointer in wxGetNativeFontEncoding") );

    if ( encoding == wxFONTENCODING_DEFAULT )
    {
        encoding = wxFont::GetDefaultEncoding();
    }

    switch ( encoding )
    {
        case wxFONTENCODING_ISO8859_1:
        case wxFONTENCODING_ISO8859_2:
        case wxFONTENCODING_ISO8859_3:
        case wxFONTENCODING_ISO8859_4:
        case wxFONTENCODING_ISO8859_5:
        case wxFONTENCODING_ISO8859_6:
        case wxFONTENCODING_ISO8859_7:
        case wxFONTENCODING_ISO8859_8:
        case wxFONTENCODING_ISO8859_9:
        case wxFONTENCODING_ISO8859_10:
        case wxFONTENCODING_ISO8859_11:
        case wxFONTENCODING_ISO8859_12:
        case wxFONTENCODING_ISO8859_13:
        case wxFONTENCODING_ISO8859_14:
        case wxFONTENCODING_ISO8859_15:
            {
                int cp = encoding - wxFONTENCODING_ISO8859_1 + 1;
                info->xregistry = wxT("iso8859");
                info->xencoding.Printf(wxT("%d"), cp);
            }
            break;

        case wxFONTENCODING_UTF8:
            info->xregistry = wxT("iso10646");
            info->xencoding = wxT("*");
            break;

        case wxFONTENCODING_KOI8:
            info->xregistry = wxT("koi8");

            // we don't make distinction between koi8-r, koi8-u and koi8-ru (so far)
            info->xencoding = wxT("*");
            break;

        case wxFONTENCODING_CP1250:
        case wxFONTENCODING_CP1251:
        case wxFONTENCODING_CP1252:
        case wxFONTENCODING_CP1253:
        case wxFONTENCODING_CP1254:
        case wxFONTENCODING_CP1255:
        case wxFONTENCODING_CP1256:
        case wxFONTENCODING_CP1257:
            {
                int cp = encoding - wxFONTENCODING_CP1250 + 1250;
                info->xregistry = wxT("microsoft");
                info->xencoding.Printf(wxT("cp%d"), cp);
            }
            break;

        case wxFONTENCODING_SYSTEM:
            info->xregistry =
            info->xencoding = wxT("*");
            break;

        default:
            // don't know how to translate this encoding into X fontspec
            return FALSE;
    }

    info->encoding = encoding;

    return TRUE;
}

bool wxTestFontEncoding(const wxNativeEncodingInfo& info)
{
    wxString fontspec;
    fontspec.Printf(_T("-*-%s-*-*-*-*-*-*-*-*-*-*-%s-%s"),
                    !info.facename ? _T("*") : info.facename.c_str(),
                    info.xregistry.c_str(),
                    info.xencoding.c_str());

    return wxTestFontSpec(fontspec);
}

// ----------------------------------------------------------------------------
// X-specific functions
// ----------------------------------------------------------------------------

wxNativeFont wxLoadQueryNearestFont(int pointSize,
                                    int family,
                                    int style,
                                    int weight,
                                    bool underlined,
                                    const wxString &facename,
                                    wxFontEncoding encoding,
                                    wxString* xFontName)
{
    if ( encoding == wxFONTENCODING_DEFAULT )
    {
        encoding = wxFont::GetDefaultEncoding();
    }

    // first determine the encoding - if the font doesn't exist at all in this
    // encoding, it's useless to do all other approximations (i.e. size,
    // family &c don't matter much)
    wxNativeEncodingInfo info;
    if ( encoding == wxFONTENCODING_SYSTEM )
    {
        // This will always work so we don't test to save time
        wxGetNativeFontEncoding(wxFONTENCODING_SYSTEM, &info);
    }
    else
    {
        if ( !wxGetNativeFontEncoding(encoding, &info) ||
             !wxTestFontEncoding(info) )
        {
#if wxUSE_FONTMAP
            if ( !wxTheFontMapper->GetAltForEncoding(encoding, &info) )
#endif // wxUSE_FONTMAP
            {
                // unspported encoding - replace it with the default
                //
                // NB: we can't just return 0 from here because wxGTK code doesn't
                //     check for it (i.e. it supposes that we'll always succeed),
                //     so it would provoke a crash
                wxGetNativeFontEncoding(wxFONTENCODING_SYSTEM, &info);
            }
        }
    }

    // OK, we have the correct xregistry/xencoding in info structure
    wxNativeFont font = 0;

    // if we already have the X font name, try to use it
    if( xFontName && !xFontName->IsEmpty() )
    {
        //
        //  Make sure point size is correct for scale factor.
        //
        wxStringTokenizer tokenizer(*xFontName, _T("-"), wxTOKEN_RET_DELIMS);
        wxString newFontName;

        for(int i = 0; i < 8; i++)
          newFontName += tokenizer.NextToken();

        (void) tokenizer.NextToken();

        newFontName += wxString::Format("%d-", pointSize);

        while(tokenizer.HasMoreTokens())
          newFontName += tokenizer.GetNextToken();

        font = wxLoadFont(newFontName);

        if(font)
          *xFontName = newFontName;
    }

    // try to load exactly the font requested first
    if( !font )
    {
        font = wxLoadQueryFont( pointSize, family, style, weight,
                                underlined, facename,
                                info.xregistry, info.xencoding,
                                xFontName );
    }

    if ( !font )
    {
        // search up and down by stepsize 10
        int max_size = pointSize + 20 * (1 + (pointSize/180));
        int min_size = pointSize - 20 * (1 + (pointSize/180));

        int i;

        // Search for smaller size (approx.)
        for ( i = pointSize - 10; !font && i >= 10 && i >= min_size; i -= 10 )
        {
            font = wxLoadQueryFont(i, family, style, weight, underlined,
                                   facename, info.xregistry, info.xencoding,
                                   xFontName);
        }

        // Search for larger size (approx.)
        for ( i = pointSize + 10; !font && i <= max_size; i += 10 )
        {
            font = wxLoadQueryFont(i, family, style, weight, underlined,
                                   facename, info.xregistry, info.xencoding,
                                   xFontName);
        }

        // Try default family
        if ( !font && family != wxDEFAULT )
        {
            font = wxLoadQueryFont(pointSize, wxDEFAULT, style, weight,
                                   underlined, facename,
                                   info.xregistry, info.xencoding,
                                   xFontName );
        }

        // ignore size, family, style and weight but try to find font with the
        // given facename and encoding
        if ( !font )
        {
            font = wxLoadQueryFont(120, wxDEFAULT, wxNORMAL, wxNORMAL,
                                   underlined, facename,
                                   info.xregistry, info.xencoding,
                                   xFontName);

            // ignore family as well
            if ( !font )
            {
                font = wxLoadQueryFont(120, wxDEFAULT, wxNORMAL, wxNORMAL,
                                       underlined, wxEmptyString,
                                       info.xregistry, info.xencoding,
                                       xFontName);

                // if it still failed, try to get the font of any size but
                // with the requested encoding: this can happen if the
                // encoding is only available in one size which happens to be
                // different from 120
                if ( !font )
                {
                    font = wxLoadQueryFont(-1, wxDEFAULT, wxNORMAL, wxNORMAL,
                                           FALSE, wxEmptyString,
                                           info.xregistry, info.xencoding,
                                           xFontName);

                    // this should never happen as we had tested for it in the
                    // very beginning, but if it does, do return something non
                    // NULL or we'd crash in wxFont code
                    if ( !font )
                    {
                        wxFAIL_MSG( _T("this encoding should be available!") );

                        font = wxLoadQueryFont(-1,
                                               wxDEFAULT, wxNORMAL, wxNORMAL,
                                               FALSE, wxEmptyString,
                                               _T("*"), _T("*"),
                                               xFontName);
                    }
                }
            }
        }
    }

    return font;
}

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

// returns TRUE if there are any fonts matching this font spec
static bool wxTestFontSpec(const wxString& fontspec)
{
    // some X servers will fail to load this font because there are too many
    // matches so we must test explicitly for this
    if ( fontspec == _T("-*-*-*-*-*-*-*-*-*-*-*-*-*-*") )
    {
        return TRUE;
    }

    wxNativeFont test = (wxNativeFont) g_fontHash->Get( fontspec );
    if (test)
    {
        return TRUE;
    }

    test = wxLoadFont(fontspec);
    g_fontHash->Put( fontspec, (wxObject*) test );

    if ( test )
    {
        wxFreeFont(test);

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static wxNativeFont wxLoadQueryFont(int pointSize,
                                    int family,
                                    int style,
                                    int weight,
                                    bool WXUNUSED(underlined),
                                    const wxString& facename,
                                    const wxString& xregistry,
                                    const wxString& xencoding,
                                    wxString* xFontName)
{
    wxString xfamily;
    switch (family)
    {
        case wxDECORATIVE: xfamily = wxT("lucida"); break;
        case wxROMAN:      xfamily = wxT("times");  break;
        case wxMODERN:     xfamily = wxT("courier"); break;
        case wxSWISS:      xfamily = wxT("helvetica"); break;
        case wxTELETYPE:   xfamily = wxT("lucidatypewriter"); break;
        case wxSCRIPT:     xfamily = wxT("utopia"); break;
        default:           xfamily = wxT("*");
    }

    wxString fontSpec;
    if (!facename.IsEmpty())
    {
        fontSpec.Printf(wxT("-*-%s-*-*-normal-*-*-*-*-*-*-*-*-*"),
                        facename.c_str());

        if ( wxTestFontSpec(fontSpec) )
        {
            xfamily = facename;
        }
        //else: no such family, use default one instead
    }

    wxString xstyle;
    switch (style)
    {
        case wxSLANT:
            fontSpec.Printf(wxT("-*-%s-*-o-*-*-*-*-*-*-*-*-*-*"),
                    xfamily.c_str());
            if ( wxTestFontSpec(fontSpec) )
            {
                xstyle = wxT("o");
                break;
            }
            // fall through - try wxITALIC now

        case wxITALIC:
            fontSpec.Printf(wxT("-*-%s-*-i-*-*-*-*-*-*-*-*-*-*"),
                    xfamily.c_str());
            if ( wxTestFontSpec(fontSpec) )
            {
                xstyle = wxT("i");
            }
            else if ( style == wxITALIC ) // and not wxSLANT
            {
                // try wxSLANT
                fontSpec.Printf(wxT("-*-%s-*-o-*-*-*-*-*-*-*-*-*-*"),
                        xfamily.c_str());
                if ( wxTestFontSpec(fontSpec) )
                {
                    xstyle = wxT("o");
                }
                else
                {
                    // no italic, no slant - leave default
                    xstyle = wxT("*");
                }
            }
            break;

        default:
            wxFAIL_MSG(_T("unknown font style"));
            // fall back to normal

        case wxNORMAL:
            xstyle = wxT("r");
            break;
    }

    wxString xweight;
    switch (weight)
    {
         case wxBOLD:
             {
                  fontSpec.Printf(wxT("-*-%s-bold-*-*-*-*-*-*-*-*-*-*-*"),
                         xfamily.c_str());
                  if ( wxTestFontSpec(fontSpec) )
                  {
                       xweight = wxT("bold");
                       break;
                  }
                  fontSpec.Printf(wxT("-*-%s-heavy-*-*-*-*-*-*-*-*-*-*-*"),
                         xfamily.c_str());
                  if ( wxTestFontSpec(fontSpec) )
                  {
                       xweight = wxT("heavy");
                       break;
                  }
                  fontSpec.Printf(wxT("-*-%s-extrabold-*-*-*-*-*-*-*-*-*-*-*"),
                         xfamily.c_str());
                  if ( wxTestFontSpec(fontSpec) )
                  {
                      xweight = wxT("extrabold");
                      break;
                  }
                  fontSpec.Printf(wxT("-*-%s-demibold-*-*-*-*-*-*-*-*-*-*-*"),
                         xfamily.c_str());
                  if ( wxTestFontSpec(fontSpec) )
                  {
                      xweight = wxT("demibold");
                      break;
                  }
                  fontSpec.Printf(wxT("-*-%s-black-*-*-*-*-*-*-*-*-*-*-*"),
                         xfamily.c_str());
                  if ( wxTestFontSpec(fontSpec) )
                  {
                      xweight = wxT("black");
                      break;
                  }
                  fontSpec.Printf(wxT("-*-%s-ultrablack-*-*-*-*-*-*-*-*-*-*-*"),
                         xfamily.c_str());
                  if ( wxTestFontSpec(fontSpec) )
                  {
                      xweight = wxT("ultrablack");
                      break;
                  }
              }
              break;
        case wxLIGHT:
             {
                  fontSpec.Printf(wxT("-*-%s-light-*-*-*-*-*-*-*-*-*-*-*"),
                         xfamily.c_str());
                  if ( wxTestFontSpec(fontSpec) )
                  {
                       xweight = wxT("light");
                       break;
                  }
                  fontSpec.Printf(wxT("-*-%s-thin-*-*-*-*-*-*-*-*-*-*-*"),
                         xfamily.c_str());
                  if ( wxTestFontSpec(fontSpec) )
                  {
                       xweight = wxT("thin");
                       break;
                  }
             }
             break;
         case wxNORMAL:
             {
                  fontSpec.Printf(wxT("-*-%s-medium-*-*-*-*-*-*-*-*-*-*-*"),
                         xfamily.c_str());
                  if ( wxTestFontSpec(fontSpec) )
                  {
                       xweight = wxT("medium");
                       break;
                  }
                  fontSpec.Printf(wxT("-*-%s-normal-*-*-*-*-*-*-*-*-*-*-*"),
                         xfamily.c_str());
                  if ( wxTestFontSpec(fontSpec) )
                  {
                       xweight = wxT("normal");
                       break;
                  }
                  fontSpec.Printf(wxT("-*-%s-regular-*-*-*-*-*-*-*-*-*-*-*"),
                         xfamily.c_str());
                  if ( wxTestFontSpec(fontSpec) )
                  {
                      xweight = wxT("regular");
                      break;
                  }
                  xweight = wxT("*");
              }
              break;
        default:           xweight = wxT("*"); break;
    }

    // if pointSize is -1, don't specify any
    wxString sizeSpec;
    if ( fontSpec == -1 )
    {
        sizeSpec = _T('*');
    }
    else
    {
        sizeSpec.Printf(_T("%d"), pointSize);
    }

    // construct the X font spec from our data
    fontSpec.Printf(wxT("-*-%s-%s-%s-normal-*-*-%s-*-*-*-*-%s-%s"),
                    xfamily.c_str(), xweight.c_str(), xstyle.c_str(),
                    sizeSpec.c_str(), xregistry.c_str(), xencoding.c_str());

    if( xFontName )
        *xFontName = fontSpec;

    return wxLoadFont(fontSpec);
}

// ----------------------------------------------------------------------------
// wxFontModule
// ----------------------------------------------------------------------------

class wxFontModule : public wxModule
{
public:
    bool OnInit();
    void OnExit();

private:
    DECLARE_DYNAMIC_CLASS(wxFontModule)
};

IMPLEMENT_DYNAMIC_CLASS(wxFontModule, wxModule)

bool wxFontModule::OnInit()
{
    g_fontHash = new wxHashTable( wxKEY_STRING );

    return TRUE;
}

void wxFontModule::OnExit()
{
    delete g_fontHash;

    g_fontHash = (wxHashTable *)NULL;
}
