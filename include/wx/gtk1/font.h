/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk1/font.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKFONTH__
#define __GTKFONTH__

#include "wx/hash.h"

// ----------------------------------------------------------------------------
// classes
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_FWD_CORE wxDC;
class WXDLLIMPEXP_FWD_CORE wxPaintDC;
class WXDLLIMPEXP_FWD_CORE wxWindow;

class WXDLLIMPEXP_FWD_CORE wxFont;

// ----------------------------------------------------------------------------
// wxFont
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxFont : public wxFontBase
{
public:
    // ctors and such
    wxFont() { }

    // wxGTK-specific
    wxFont(const wxString& fontname)
    {
        Create(fontname);
    }

    wxFont(const wxNativeFontInfo& info);

#if FUTURE_WXWIN_COMPATIBILITY_3_0
    wxFont(int size,
           int family,
           int style,
           int weight,
           bool underlined = false,
           const wxString& face = wxEmptyString,
           wxFontEncoding encoding = wxFONTENCODING_DEFAULT)
    {
        (void)Create(size, (wxFontFamily)family, (wxFontStyle)style, (wxFontWeight)weight, underlined, face, encoding);
    }
#endif

    wxFont(int size,
           wxFontFamily family,
           wxFontStyle style,
           wxFontWeight weight,
           bool underlined = false,
           const wxString& face = wxEmptyString,
           wxFontEncoding encoding = wxFONTENCODING_DEFAULT)
    {
        Create(size, family, style, weight, underlined, face, encoding);
    }

    bool Create(int size,
                wxFontFamily family,
                wxFontStyle style,
                wxFontWeight weight,
                bool underlined = false,
                const wxString& face = wxEmptyString,
                wxFontEncoding encoding = wxFONTENCODING_DEFAULT);


    // wxGTK-specific
    bool Create(const wxString& fontname);

    virtual ~wxFont();

    // implement base class pure virtuals
    virtual int GetPointSize() const;
    virtual wxFontFamily GetFamily() const;
    virtual wxFontStyle GetStyle() const;
    virtual wxFontWeight GetWeight() const;
    virtual wxString GetFaceName() const;
    virtual bool GetUnderlined() const;
    virtual wxFontEncoding GetEncoding() const;
    virtual const wxNativeFontInfo *GetNativeFontInfo() const;
    virtual bool IsFixedWidth() const;

    virtual void SetPointSize( int pointSize );
    virtual void SetFamily(wxFontFamily family);
    virtual void SetStyle(wxFontStyle style);
    virtual void SetWeight(wxFontWeight weight);
    virtual bool SetFaceName( const wxString& faceName );
    virtual void SetUnderlined( bool underlined );
    virtual void SetEncoding(wxFontEncoding encoding);

    WXDECLARE_COMPAT_SETTERS

    virtual void SetNoAntiAliasing( bool no = true );
    virtual bool GetNoAntiAliasing() const ;

    // implementation from now on
    void Unshare();

    GdkFont* GetInternalFont(float scale = 1.0) const;

protected:
    virtual wxGDIRefData *CreateGDIRefData() const;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const;

    virtual void DoSetNativeFontInfo( const wxNativeFontInfo& info );

private:
    DECLARE_DYNAMIC_CLASS(wxFont)
};

#endif // __GTKFONTH__
