/////////////////////////////////////////////////////////////////////////////
// Name:        htmltag.cpp
// Purpose:     wxHtmlTag class (represents single tag)
// Author:      Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation
#endif

#include "wx/wxprec.h"

#include "wx/defs.h"
#if wxUSE_HTML

#ifdef __BORDLANDC__
#pragma hdrstop
#endif

#ifndef WXPRECOMP
#endif

#include "wx/html/htmltag.h"
#include "wx/html/htmlpars.h"
#include <stdio.h> // for vsscanf
#include <stdarg.h>


//-----------------------------------------------------------------------------
// wxHtmlTagsCache
//-----------------------------------------------------------------------------

struct wxHtmlCacheItem
{
    // this is "pos" value passed to wxHtmlTag's constructor.
    // it is position of '<' character of the tag
    int Key;

    // end positions for the tag:
    // end1 is '<' of ending tag,
    // end2 is '>' or both are
    // -1 if there is no ending tag for this one...
    // or -2 if this is ending tag  </...>
    int End1, End2;

    // name of this tag
    wxChar *Name;
};


IMPLEMENT_CLASS(wxHtmlTagsCache,wxObject)

#define CACHE_INCREMENT  64

wxHtmlTagsCache::wxHtmlTagsCache(const wxString& source)
{
    const wxChar *src = source.c_str();
    int i, tg, pos, stpos;
    int lng = source.Length();
    wxChar dummy[256];

    m_Cache = NULL;
    m_CacheSize = 0;
    m_CachePos = 0;

    pos = 0;
    while (pos < lng) 
    {
        if (src[pos] == wxT('<'))   // tag found:
        {
            if (m_CacheSize % CACHE_INCREMENT == 0)
                m_Cache = (wxHtmlCacheItem*) realloc(m_Cache, (m_CacheSize + CACHE_INCREMENT) * sizeof(wxHtmlCacheItem));
            tg = m_CacheSize++;
            m_Cache[tg].Key = stpos = pos++;
            dummy[0] = 0; i = 0;
            while (pos < lng && 
                   src[pos] != wxT('>') &&
                   src[pos] != wxT(' ') && src[pos] != wxT('\r') && 
                   src[pos] != wxT('\n') && src[pos] != wxT('\t')) 
            {
                dummy[i] = src[pos++];
                if ((dummy[i] >= wxT('a')) && (dummy[i] <= wxT('z'))) dummy[i] -= (wxT('a') - wxT('A'));
                i++;
            }
            dummy[i] = 0;
            m_Cache[tg].Name = new wxChar[i+1];
            memcpy(m_Cache[tg].Name, dummy, (i+1)*sizeof(wxChar));

            while (pos < lng && src[pos] != wxT('>')) pos++;

            if (src[stpos+1] == wxT('/')) // ending tag:
            {
                m_Cache[tg].End1 = m_Cache[tg].End2 = -2;
                // find matching begin tag:
                for (i = tg; i >= 0; i--)
                    if ((m_Cache[i].End1 == -1) && (wxStrcmp(m_Cache[i].Name, dummy+1) == 0)) 
                    {
                        m_Cache[i].End1 = stpos;
                        m_Cache[i].End2 = pos + 1;
                        break;
                    }
            }
            else 
            {
                m_Cache[tg].End1 = m_Cache[tg].End2 = -1;
            }
        }

        pos++;
    }

    // ok, we're done, now we'll free .Name members of cache - we don't need it anymore:
    for (i = 0; i < m_CacheSize; i++) 
    {
        delete[] m_Cache[i].Name;
        m_Cache[i].Name = NULL;
    }
}

void wxHtmlTagsCache::QueryTag(int at, int* end1, int* end2)
{
    if (m_Cache == NULL) return;
    if (m_Cache[m_CachePos].Key != at) 
    {
        int delta = (at < m_Cache[m_CachePos].Key) ? -1 : 1;
        do 
        { 
            m_CachePos += delta; 
        }
        while (m_Cache[m_CachePos].Key != at);
    }
    *end1 = m_Cache[m_CachePos].End1;
    *end2 = m_Cache[m_CachePos].End2;
}




//-----------------------------------------------------------------------------
// wxHtmlTag
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxHtmlTag,wxObject)

wxHtmlTag::wxHtmlTag(const wxString& source, int pos, int end_pos, 
                     wxHtmlTagsCache *cache,
                     wxHtmlEntitiesParser *entParser) : wxObject()
{
    int i;
    wxChar c;

    // fill-in name, params and begin pos:
    i = pos+1;
    if (source[i] == wxT('/')) 
        { m_Ending = TRUE; i++; }
    else 
        m_Ending = FALSE;

    // find tag's name and convert it to uppercase:
    while ((i < end_pos) && 
           ((c = source[i++]) != wxT(' ') && c != wxT('\r') && 
             c != wxT('\n') && c != wxT('\t') &&
             c != wxT('>'))) 
    {
        if ((c >= wxT('a')) && (c <= wxT('z'))) 
            c -= (wxT('a') - wxT('A'));
        m_Name << c;
    }

    // if the tag has parameters, read them and "normalize" them,
    // i.e. convert to uppercase, replace whitespaces by spaces and 
    // remove whitespaces around '=':
    if (source[i-1] != wxT('>'))
    {
        #define IS_WHITE(c) (c == wxT(' ') || c == wxT('\r') || \
                             c == wxT('\n') || c == wxT('\t'))
        wxString pname, pvalue;
        wxChar quote;
        enum 
        {
            ST_BEFORE_NAME = 1, 
            ST_NAME,
            ST_BEFORE_EQ,
            ST_BEFORE_VALUE,
            ST_VALUE
        } state;
    
        quote = 0;
        state = ST_BEFORE_NAME;
        while (i < end_pos)
        {
            c = source[i++];

            if (c == wxT('>') && !(state == ST_VALUE && quote != 0)) 
            {
                if (state == ST_BEFORE_EQ || state == ST_NAME)
                {
                    m_ParamNames.Add(pname);
                    m_ParamValues.Add(wxEmptyString);
                }
                else if (state == ST_VALUE && quote == 0)
                {
                    m_ParamNames.Add(pname);
                    if (entParser)
                        m_ParamValues.Add(entParser->Parse(pvalue));
                    else
                        m_ParamValues.Add(pvalue);
                }
                break;
            }
            switch (state)
            {
                case ST_BEFORE_NAME:
                    if (!IS_WHITE(c))
                    {
                        pname = c;
                        state = ST_NAME;
                    }
                    break;
                case ST_NAME:
                    if (IS_WHITE(c))
                        state = ST_BEFORE_EQ;
                    else if (c == wxT('='))
                        state = ST_BEFORE_VALUE;
                    else
                        pname << c;
                    break;
                case ST_BEFORE_EQ:
                    if (c == wxT('='))
                        state = ST_BEFORE_VALUE;
                    else if (!IS_WHITE(c))
                    {
                        m_ParamNames.Add(pname);
                        m_ParamValues.Add(wxEmptyString);
                        pname = c;
                        state = ST_NAME;
                    }
                    break;
                case ST_BEFORE_VALUE:
                    if (!IS_WHITE(c))
                    {
                        if (c == wxT('"') || c == wxT('\''))
                            quote = c, pvalue = wxEmptyString;
                        else
                            quote = 0, pvalue = c;
                        state = ST_VALUE;
                    }
                    break;
                case ST_VALUE:
                    if ((quote != 0 && c == quote) ||
                        (quote == 0 && IS_WHITE(c)))
                    {
                        m_ParamNames.Add(pname);
                        if (quote == 0)
                        {
                            // VS: backward compatibility, no real reason,
                            //     but wxHTML code relies on this... :(
                            pvalue.MakeUpper();
                        }
                        if (entParser)
                            m_ParamValues.Add(entParser->Parse(pvalue));
                        else
                            m_ParamValues.Add(pvalue);
                        state = ST_BEFORE_NAME;
                    }
                    else
                        pvalue << c;
                    break;
            }
        }
        
        #undef IS_WHITE
   }
   m_Begin = i;

   cache->QueryTag(pos, &m_End1, &m_End2);
   if (m_End1 > end_pos) m_End1 = end_pos;
   if (m_End2 > end_pos) m_End2 = end_pos;
}

bool wxHtmlTag::HasParam(const wxString& par) const
{
    return (m_ParamNames.Index(par, FALSE) != wxNOT_FOUND);
}

wxString wxHtmlTag::GetParam(const wxString& par, bool with_commas) const
{
    int index = m_ParamNames.Index(par, FALSE);
    if (index == wxNOT_FOUND)
        return wxEmptyString;
    if (with_commas)
    {
        // VS: backward compatibility, seems to be never used by wxHTML...
        wxString s;
        s << wxT('"') << m_ParamValues[index] << wxT('"');
        return s;
    }
    else
        return m_ParamValues[index];
}

int wxHtmlTag::ScanParam(const wxString& par, wxChar *format, void *param) const
{
    wxString parval = GetParam(par);
    return wxSscanf(parval, format, param);
}

bool wxHtmlTag::GetParamAsColour(const wxString& par, wxColour *clr) const
{
    wxString str = GetParam(par);
    
    if (str.IsEmpty()) return FALSE;
    if (str.GetChar(0) == wxT('#'))
    {
        unsigned long tmp;
        if (ScanParam(par, wxT("#%lX"), &tmp) != 1)
            return FALSE;
        *clr = wxColour((unsigned char)((tmp & 0xFF0000) >> 16),
					    (unsigned char)((tmp & 0x00FF00) >> 8),
					    (unsigned char)(tmp & 0x0000FF));
        return TRUE;
    }
    else
    {
        // Handle colours defined in HTML 4.0:
        #define HTML_COLOUR(name,r,g,b)                 \
            if (str.IsSameAs(wxT(name), FALSE))         \
                { *clr = wxColour(r,g,b); return TRUE; }
        HTML_COLOUR("black",   0x00,0x00,0x00)
        HTML_COLOUR("silver",  0xC0,0xC0,0xC0)
        HTML_COLOUR("gray",    0x80,0x80,0x80)
        HTML_COLOUR("white",   0xFF,0xFF,0xFF)
        HTML_COLOUR("maroon",  0x80,0x00,0x00)
        HTML_COLOUR("red",     0xFF,0x00,0x00)
        HTML_COLOUR("purple",  0x80,0x00,0x80)
        HTML_COLOUR("fuchsia", 0xFF,0x00,0xFF)
        HTML_COLOUR("green",   0x00,0x80,0x00)
        HTML_COLOUR("lime",    0x00,0xFF,0x00)
        HTML_COLOUR("olive",   0x80,0x80,0x00)
        HTML_COLOUR("yellow",  0xFF,0xFF,0x00)
        HTML_COLOUR("navy",    0x00,0x00,0x80)
        HTML_COLOUR("blue",    0x00,0x00,0xFF)
        HTML_COLOUR("teal",    0x00,0x80,0x80)
        HTML_COLOUR("aqua",    0x00,0xFF,0xFF)
        #undef HTML_COLOUR
        return FALSE;
    }
}

bool wxHtmlTag::GetParamAsInt(const wxString& par, int *clr) const
{
    if (!HasParam(par)) return FALSE;
    long i;
    bool succ = GetParam(par).ToLong(&i);
    *clr = (int)i;
    return succ;
}

wxString wxHtmlTag::GetAllParams() const
{
    // VS: this function is for backward compatiblity only, 
    //     never used by wxHTML
    wxString s;
    size_t cnt = m_ParamNames.GetCount();
    for (size_t i = 0; i < cnt; i++)
    {
        s << m_ParamNames[i];
        s << wxT('=');
        if (m_ParamValues[i].Find(wxT('"')) != wxNOT_FOUND)
            s << wxT('\'') << m_ParamValues[i] << wxT('\'');
        else
            s << wxT('"') << m_ParamValues[i] << wxT('"');
    }
    return s;
}

#endif
