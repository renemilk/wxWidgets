/////////////////////////////////////////////////////////////////////////////
// Name:        htmltag.h
// Purpose:     wxHtmlTag class (represents single tag)
// Author:      Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////


#ifndef _WX_HTMLTAG_H_
#define _WX_HTMLTAG_H_

#ifdef __GNUG__
#pragma interface "htmltag.h"
#endif

#include "wx/defs.h"
#if wxUSE_HTML

class WXDLLEXPORT wxHtmlEntitiesParser;

//-----------------------------------------------------------------------------
// wxHtmlTagsCache
//          - internal wxHTML class, do not use!
//-----------------------------------------------------------------------------

struct wxHtmlCacheItem;

class WXDLLEXPORT wxHtmlTagsCache : public wxObject
{
    DECLARE_DYNAMIC_CLASS(wxHtmlTagsCache)

private:
    wxHtmlCacheItem *m_Cache;
    int m_CacheSize;
    int m_CachePos;

public:
    wxHtmlTagsCache() : wxObject() {m_CacheSize = 0; m_Cache = NULL;}
    wxHtmlTagsCache(const wxString& source);
    ~wxHtmlTagsCache() {free(m_Cache);}

    // Finds parameters for tag starting at at and fills the variables
    void QueryTag(int at, int* end1, int* end2);
};


//--------------------------------------------------------------------------------
// wxHtmlTag
//                  This represents single tag. It is used as internal structure
//                  by wxHtmlParser.
//--------------------------------------------------------------------------------

class WXDLLEXPORT wxHtmlTag : public wxObject
{
    DECLARE_CLASS(wxHtmlTag)

public:
    // constructs wxHtmlTag object based on HTML tag.
    // The tag begins (with '<' character) at position pos in source
    // end_pos is position where parsing ends (usually end of document)
    wxHtmlTag(const wxString& source, int pos, int end_pos, 
              wxHtmlTagsCache *cache,
              wxHtmlEntitiesParser *entParser = NULL);

    // Returns tag's name in uppercase.
    inline wxString GetName() const {return m_Name;}

    // Returns TRUE if the tag has given parameter. Parameter
    // should always be in uppercase.
    // Example : <IMG SRC="test.jpg"> HasParam("SRC") returns TRUE
    bool HasParam(const wxString& par) const;

    // Returns value of the param. Value is in uppercase unless it is
    // enclosed with "
    // Example : <P align=right> GetParam("ALIGN") returns (RIGHT)
    //           <P IMG SRC="WhaT.jpg"> GetParam("SRC") returns (WhaT.jpg)
    //                           (or ("WhaT.jpg") if with_commas == TRUE)
    wxString GetParam(const wxString& par, bool with_commas = FALSE) const;

    // Convenience functions:    
    bool GetParamAsColour(const wxString& par, wxColour *clr) const;
    bool GetParamAsInt(const wxString& par, int *clr) const;

    // Scans param like scanf() functions family does.
    // Example : ScanParam("COLOR", "\"#%X\"", &clr);
    // This is always with with_commas=FALSE
    // Returns number of scanned values
    // (like sscanf() does)
    // NOTE: unlike scanf family, this function only accepts
    //       *one* parameter !
    int ScanParam(const wxString& par, wxChar *format, void *param) const;

    // Returns string containing all params.
    wxString GetAllParams() const;

    // return TRUE if this is ending tag (</something>) or FALSE
    // if it isn't (<something>)
    inline bool IsEnding() const {return m_Ending;}

    // return TRUE if this is ending tag (</something>) or FALSE
    // if it isn't (<something>)
    inline bool HasEnding() const {return m_End1 >= 0;}

    // returns beginning position of _internal_ block of text
    // See explanation (returned value is marked with *):
    // bla bla bla <MYTAG>* bla bla intenal text</MYTAG> bla bla
    inline int GetBeginPos() const {return m_Begin;}
    // returns ending position of _internal_ block of text.
    // bla bla bla <MYTAG> bla bla intenal text*</MYTAG> bla bla
    inline int GetEndPos1() const {return m_End1;}
    // returns end position 2 :
    // bla bla bla <MYTAG> bla bla internal text</MYTAG>* bla bla
    inline int GetEndPos2() const {return m_End2;}

private:
    wxString m_Name;
    int m_Begin, m_End1, m_End2;
    bool m_Ending;
    wxArrayString m_ParamNames, m_ParamValues;
};





#endif

#endif // _WX_HTMLTAG_H_

