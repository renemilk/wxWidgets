/////////////////////////////////////////////////////////////////////////////
// Name:        htmlpars.cpp
// Purpose:     wxHtmlParser class (generic parser)
// Author:      Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation
#endif

#include <wx/wxprec.h>

#include "wx/defs.h"
#if wxUSE_HTML

#ifdef __BORDLANDC__
#pragma hdrstop
#endif

#ifndef WXPRECOMP
#include <wx/wx.h>
#endif

#include "wx/tokenzr.h"
#include "wx/wfstream.h"
#include "wx/url.h"
#include "wx/html/htmldefs.h"
#include "wx/html/htmlpars.h"



//-----------------------------------------------------------------------------
// wxHtmlParser
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxHtmlParser,wxObject)


wxObject* wxHtmlParser::Parse(const wxString& source)
{
    wxObject *result;

    InitParser(source);
    DoParsing();
    result = GetProduct();
    DoneParser();
    return result;
}



void wxHtmlParser::InitParser(const wxString& source)
{
    m_Source = source;
    m_Cache = new wxHtmlTagsCache(m_Source);
}
        
        
        
void wxHtmlParser::DoneParser()
{
    delete m_Cache;
    m_Cache = NULL;
}



#define wxHTML_MAX_BUFLEN 1024

void wxHtmlParser::DoParsing(int begin_pos, int end_pos)
{
    char temp[wxHTML_BUFLEN], c;
    int i;
    int templen;

    templen = 0;
    i = begin_pos;

    while (i < end_pos) {
        c = m_Source[i];

        // continue building word:
        if (c != '<') {
            temp[templen++] = c;
            if (templen == wxHTML_BUFLEN-1) {
                temp[templen] = 0;
                AddText(temp);
                templen = 0;
            }
            i++;
        }

        else if (c == '<')  {
            wxHtmlTag tag(m_Source, i, end_pos, m_Cache);

            if (templen) {
                temp[templen] = 0;
                AddText(temp);
                templen = 0;
            }
            AddTag(tag);
            if (tag.HasEnding()) i = tag.GetEndPos2();
            else i = tag.GetBeginPos();
        }
    }

    if (templen) { // last word of block :-(
        temp[templen] = 0;
        AddText(temp);
    }
}



void wxHtmlParser::AddTag(const wxHtmlTag& tag)
{
    wxHtmlTagHandler *h;
    bool inner = FALSE;

    h = (wxHtmlTagHandler*) m_HandlersHash.Get(tag.GetName());
    if (h)
        inner = h -> HandleTag(tag);
    if (!inner) {
        if (tag.HasEnding())
            DoParsing(tag.GetBeginPos(), tag.GetEndPos1());
    }
}



void wxHtmlParser::AddTagHandler(wxHtmlTagHandler *handler)
{
    wxString s(handler -> GetSupportedTags());
    wxStringTokenizer tokenizer(s, ", ");

    while (tokenizer.HasMoreTokens())
        m_HandlersHash.Put(tokenizer.NextToken(), handler);

    if (m_HandlersList.IndexOf(handler) == wxNOT_FOUND)
        m_HandlersList.Append(handler);

    handler -> SetParser(this);
}



wxHtmlParser::~wxHtmlParser()
{
    m_HandlersHash.Clear();
    m_HandlersList.DeleteContents(TRUE);
    m_HandlersList.Clear();
}



//-----------------------------------------------------------------------------
// wxHtmlTagHandler
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxHtmlTagHandler,wxObject)
#endif

