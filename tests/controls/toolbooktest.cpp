///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/toolbooktest.cpp
// Purpose:     wxToolbook unit test
// Author:      Steven Lamerton
// Created:     2010-07-02
// RCS-ID:      $Id$
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/panel.h"
#endif // WX_PRECOMP

#include "wx/artprov.h"
#include "wx/imaglist.h"
#include "wx/toolbook.h"
#include "wx/toolbar.h"
#include "bookctrlbasetest.h"

class ToolbookTestCase : public BookCtrlBaseTestCase
{
public:
    ToolbookTestCase() { }

    virtual void setUp();
    virtual void tearDown();

private:
    virtual wxBookCtrlBase *GetBase() const { return m_toolbook; }

    virtual wxEventType GetChangedEvent() const
    { return wxEVT_COMMAND_TOOLBOOK_PAGE_CHANGED; }

    virtual wxEventType GetChangingEvent() const
    { return wxEVT_COMMAND_TOOLBOOK_PAGE_CHANGING; }

    CPPUNIT_TEST_SUITE( ToolbookTestCase );
        wxBOOK_CTRL_BASE_TESTS();
    CPPUNIT_TEST_SUITE_END();

    wxToolbook *m_toolbook;

    DECLARE_NO_COPY_CLASS(ToolbookTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( ToolbookTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ToolbookTestCase, "ToolbookTestCase" );

void ToolbookTestCase::setUp()
{
    m_toolbook = new wxToolbook(wxTheApp->GetTopWindow(), wxID_ANY, wxDefaultPosition, wxSize(400, 200));

    wxImageList* list = new wxImageList(32, 32);
    list->Add(wxArtProvider::GetIcon(wxART_INFORMATION, wxART_OTHER, wxSize(32, 32)));

    m_toolbook->AssignImageList(list);

    AddPanels();

    m_toolbook->GetToolBar()->Realize();
}

void ToolbookTestCase::tearDown()
{
    wxDELETE(m_toolbook);
}
