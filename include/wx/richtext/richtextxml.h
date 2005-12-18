/////////////////////////////////////////////////////////////////////////////
// Name:        wx/richtext/richeditxml.h
// Purpose:     XML and HTML I/O for wxRichTextCtrl
// Author:      Julian Smart
// Modified by:
// Created:     2005-09-30
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RICHTEXTXML_H_
#define _WX_RICHTEXTXML_H_

/*!
 * Includes
 */

#include "wx/richtext/richtextbuffer.h"

#if wxUSE_RICHTEXT && wxUSE_XML

/*!
 * wxRichTextXMLHandler
 */

class WXDLLIMPEXP_XML wxXmlNode;

class WXDLLIMPEXP_ADV wxRichTextXMLHandler: public wxRichTextFileHandler
{
    DECLARE_CLASS(wxRichTextXMLHandler)
public:
    wxRichTextXMLHandler(const wxString& name = wxT("XML"), const wxString& ext = wxT("xml"), int type = wxRICHTEXT_TYPE_XML)
        : wxRichTextFileHandler(name, ext, type)
        { }

#if wxUSE_STREAMS
    virtual bool DoLoadFile(wxRichTextBuffer *buffer, wxInputStream& stream);
    virtual bool DoSaveFile(wxRichTextBuffer *buffer, wxOutputStream& stream);

    /// Recursively export an object
    bool ExportXML(wxOutputStream& stream, wxMBConv* convMem, wxMBConv* convFile, wxRichTextObject& obj, int level);

    /// Recursively import an object
    bool ImportXML(wxRichTextBuffer* buffer, wxXmlNode* node);

    /// Create style parameters
    wxString CreateStyle(const wxTextAttrEx& attr, bool isPara = false);

    /// Get style parameters
    bool GetStyle(wxTextAttrEx& attr, wxXmlNode* node, bool isPara = false);
#endif

    /// Can we save using this handler?
    virtual bool CanSave() const { return true; }

    /// Can we load using this handler?
    virtual bool CanLoad() const { return true; }

// Implementation

    bool HasParam(wxXmlNode* node, const wxString& param);
    wxXmlNode *GetParamNode(wxXmlNode* node, const wxString& param);
    wxString GetNodeContent(wxXmlNode *node);
    wxString GetParamValue(wxXmlNode *node, const wxString& param);
    wxString GetText(wxXmlNode *node, const wxString& param = wxEmptyString, bool translate = false);

protected:

};

#endif
    // wxUSE_RICHTEXT && wxUSE_XML

#endif
    // _WX_RICHTEXTXML_H_
