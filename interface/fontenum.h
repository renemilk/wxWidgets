/////////////////////////////////////////////////////////////////////////////
// Name:        fontenum.h
// Purpose:     documentation for wxFontEnumerator class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxFontEnumerator
    @wxheader{fontenum.h}

    wxFontEnumerator enumerates either all available fonts on the system or only
    the ones with given attributes - either only fixed-width (suited for use in
    programs such as terminal emulators and the like) or the fonts available in
    the given encoding.

    To do this, you just have to call one of EnumerateXXX() functions - either
    wxFontEnumerator::EnumerateFacenames or
    wxFontEnumerator::EnumerateEncodings and the
    corresponding callback (wxFontEnumerator::OnFacename or
    wxFontEnumerator::OnFontEncoding) will be called
    repeatedly until either all fonts satisfying the specified criteria are
    exhausted or the callback returns @false.

    @library{wxcore}
    @category{FIXME}

    @seealso
    @ref overview_wxfontencodingoverview "Font encoding overview", @ref
    overview_samplefont "Font sample", wxFont, wxFontMapper
*/
class wxFontEnumerator
{
public:
    /**
        Call OnFontEncoding() for each
        encoding supported by the given font - or for each encoding supported by at
        least some font if @e font is not specified.
    */
    virtual bool EnumerateEncodings(const wxString& font = "");

    /**
        Call OnFacename() for each font which
        supports given encoding (only if it is not wxFONTENCODING_SYSTEM) and is of
        fixed width (if @e fixedWidthOnly is @true).
        
        Calling this function with default arguments will result in enumerating all
        fonts available on the system.
    */
    virtual bool EnumerateFacenames(wxFontEncoding encoding = wxFONTENCODING_SYSTEM,
                                    bool fixedWidthOnly = @false);

    /**
        Return array of strings containing all encodings found by
        EnumerateEncodings().
    */
    static wxArrayString GetEncodings(const wxString& facename = "");

    /**
        Return array of strings containing all facenames found by
        EnumerateFacenames().
    */
    static wxArrayString GetFacenames(wxFontEncoding encoding = wxFONTENCODING_SYSTEM,
                                      bool fixedWidthOnly = @false);

    /**
        Returns @true if the given string is valid face name, i.e. it's the face name
        of an installed
        font and it can safely be used with wxFont::SetFaceName.
    */
    static bool IsValidFacename(const wxString & facename);

    /**
        Called by EnumerateFacenames() for
        each match. Return @true to continue enumeration or @false to stop it.
    */
    virtual bool OnFacename(const wxString& font);

    /**
        Called by EnumerateEncodings() for
        each match. Return @true to continue enumeration or @false to stop it.
    */
    virtual bool OnFontEncoding(const wxString& font,
                                const wxString& encoding);
};
