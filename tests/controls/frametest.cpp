///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/frametest.cpp
// Purpose:     wxFrame  unit test
// Author:      Steven Lamerton
// Created:     2010-07-10
// RCS-ID:      $Id$
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/frame.h"
#endif // WX_PRECOMP

#include "testableframe.h"

class FrameTestCase : public CppUnit::TestCase
{
public:
    FrameTestCase() { }

    void setUp();
    void tearDown();

private:
    CPPUNIT_TEST_SUITE( FrameTestCase );
        CPPUNIT_TEST( Iconize );
        CPPUNIT_TEST( Close );
    CPPUNIT_TEST_SUITE_END();

    void Iconize();
    void Close();

    wxFrame *m_frame;

    DECLARE_NO_COPY_CLASS(FrameTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( FrameTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( FrameTestCase, "FrameTestCase" );

void FrameTestCase::setUp()
{
    m_frame = new wxFrame(NULL, wxID_ANY, "test frame");
    m_frame->Show();
}

void FrameTestCase::tearDown()
{
    wxDELETE(m_frame);
}

void FrameTestCase::Iconize()
{
#if defined(__WXMSW__) || defined(__WXGTK__)
    wxTestableFrame* testframe = wxStaticCast(wxTheApp->GetTopWindow(),
                                              wxTestableFrame);

    EventCounter count(m_frame, wxEVT_ICONIZE);

    m_frame->Iconize();
    m_frame->Iconize(false);

    CPPUNIT_ASSERT_EQUAL(2, testframe->GetEventCount());
#endif
}

void FrameTestCase::Close()
{
    wxTestableFrame* testframe = wxStaticCast(wxTheApp->GetTopWindow(),
                                              wxTestableFrame);

    EventCounter count(m_frame, wxEVT_CLOSE_WINDOW);

    m_frame->Close();

    CPPUNIT_ASSERT_EQUAL(1, testframe->GetEventCount());
}

