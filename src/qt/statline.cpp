/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/statline.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/statline.h"

wxStaticLine::wxStaticLine()
{
}

wxStaticLine::wxStaticLine( wxWindow *parent,
              wxWindowID id,
              const wxPoint& pos,
              const wxSize& size,
              long style,
              const wxString &name)
{
    Create( parent, id, pos, size, style, name );
}

bool wxStaticLine::Create( wxWindow *parent,
             wxWindowID id,
             const wxPoint& pos,
             const wxSize& size,
             long style,
             const wxString &name)
{
    m_qtFrame = new QFrame( parent->GetHandle() );
    if ( style & wxLI_HORIZONTAL )
        m_qtFrame->setFrameStyle( QFrame::HLine );
    else if ( style & wxLI_VERTICAL )
        m_qtFrame->setFrameStyle( QFrame::VLine );

    return wxStaticLineBase::Create( parent, id, pos, size, style, wxDefaultValidator, name );
}

QFrame *wxStaticLine::GetHandle() const
{
    return m_qtFrame;
}
