/////////////////////////////////////////////////////////////////////////////
// Name:        xh_sttxt.cpp
// Purpose:     XML resource for wxStaticText
// Author:      Bob Mitchell
// Created:     2000/03/21
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Bob Mitchell and Verant Interactive
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
 
#ifdef __GNUG__
#pragma implementation "xh_sttxt.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/xrc/xh_sttxt.h"
#include "wx/stattext.h"

wxStaticTextXmlHandler::wxStaticTextXmlHandler() 
: wxXmlResourceHandler() 
{
    ADD_STYLE(wxST_NO_AUTORESIZE);
    ADD_STYLE(wxALIGN_LEFT);
    ADD_STYLE(wxALIGN_RIGHT);
    ADD_STYLE(wxALIGN_CENTRE);
    AddWindowStyles();
}

wxObject *wxStaticTextXmlHandler::DoCreateResource()
{ 
    wxStaticText *text = new wxStaticText(m_parentAsWindow,
                                    GetID(),
                                    GetText(wxT("label")),
                                    GetPosition(), GetSize(),
                                    GetStyle(),
                                    GetName()
                                    );
    SetupWindow(text);
    
    return text;
}



bool wxStaticTextXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("wxStaticText"));
}


