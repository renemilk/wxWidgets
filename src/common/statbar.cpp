///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/statbar.cpp
// Purpose:     wxStatusBarBase implementation
// Author:      Vadim Zeitlin
// Modified by: Francesco Montorsi
// Created:     14.10.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// License:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_STATUSBAR

#include "wx/statusbr.h"

#ifndef WX_PRECOMP
    #include "wx/frame.h"
#endif //WX_PRECOMP

const char wxStatusBarNameStr[] = "statusBar";


// ============================================================================
// wxStatusBarBase implementation
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(wxStatusBar, wxWindow)

#include "wx/arrimpl.cpp" // This is a magic incantation which must be done!
WX_DEFINE_EXPORTED_OBJARRAY(wxStatusBarPaneArray)


// ----------------------------------------------------------------------------
// ctor/dtor
// ----------------------------------------------------------------------------

wxStatusBarBase::wxStatusBarBase()
{
    m_bSameWidthForAllPanes = true;
}

wxStatusBarBase::~wxStatusBarBase()
{
    // notify the frame that it doesn't have a status bar any longer to avoid
    // dangling pointers
    wxFrame *frame = wxDynamicCast(GetParent(), wxFrame);
    if ( frame && frame->GetStatusBar() == this )
        frame->SetStatusBar(NULL);
}

// ----------------------------------------------------------------------------
// field widths
// ----------------------------------------------------------------------------

void wxStatusBarBase::SetFieldsCount(int number, const int *widths)
{
    wxCHECK_RET( number > 0, wxT("invalid field number in SetFieldsCount") );

    if ( (size_t)number > m_panes.GetCount() )
    {
        wxStatusBarPane newPane;

        // add more entries with the default style and zero width
        // (this will be set later)
        for (size_t i = m_panes.GetCount(); i < (size_t)number; ++i)
            m_panes.Add(newPane);
    }
    else if ( (size_t)number < m_panes.GetCount() )
    {
        // remove entries in excess
        m_panes.RemoveAt(number, m_panes.GetCount()-number);
    }

    // SetStatusWidths will automatically refresh
    SetStatusWidths(number, widths);
}

void wxStatusBarBase::SetStatusWidths(int WXUNUSED_UNLESS_DEBUG(n),
                                    const int widths[])
{
    wxASSERT_MSG( (size_t)n == m_panes.GetCount(), wxT("field number mismatch") );

    if (widths == NULL)
    {
        // special value meaning: override explicit pane widths and make them all
        // of the same size
        m_bSameWidthForAllPanes = true;
    }
    else
    {
        for ( size_t i = 0; i < m_panes.GetCount(); i++ )
            m_panes[i].m_nWidth = widths[i];

        m_bSameWidthForAllPanes = false;
    }

    // update the display after the widths changed
    Refresh();
}

void wxStatusBarBase::SetStatusStyles(int WXUNUSED_UNLESS_DEBUG(n),
                                    const int styles[])
{
    wxCHECK_RET( styles, wxT("NULL pointer in SetStatusStyles") );

    wxASSERT_MSG( (size_t)n == m_panes.GetCount(), wxT("field number mismatch") );

    for ( size_t i = 0; i < m_panes.GetCount(); i++ )
        m_panes[i].m_nStyle = styles[i];

    // update the display after the widths changed
    Refresh();
}

wxArrayInt wxStatusBarBase::CalculateAbsWidths(wxCoord widthTotal) const
{
    wxArrayInt widths;

    if ( m_bSameWidthForAllPanes )
    {
        // Default: all fields have the same width. This is not always
        // possible to do exactly (if widthTotal is not divisible by
        // m_panes.GetCount()) - if that happens, we distribute the extra
        // pixels among all fields:
        int widthToUse = widthTotal;

        for ( size_t i = m_panes.GetCount(); i > 0; i-- )
        {
            // divide the unassigned width evently between the
            // not yet processed fields:
            int w = widthToUse / i;
            widths.Add(w);
            widthToUse -= w;
        }
    }
    else // do not override explicit pane widths
    {
        // calculate the total width of all the fixed width fields and the
        // total number of var field widths counting with multiplicity
        size_t nTotalWidth = 0,
            nVarCount = 0,
            i;

        for ( i = 0; i < m_panes.GetCount(); i++ )
        {
            if ( m_panes[i].GetWidth() >= 0 )
                nTotalWidth += m_panes[i].GetWidth();
            else
                nVarCount += -m_panes[i].GetWidth();
        }

        // the amount of extra width we have per each var width field
        int widthExtra = widthTotal - nTotalWidth;

        // do fill the array
        for ( i = 0; i < m_panes.GetCount(); i++ )
        {
            if ( m_panes[i].GetWidth() >= 0 )
                widths.Add(m_panes[i].GetWidth());
            else
            {
                int nVarWidth = widthExtra > 0 ? (widthExtra * (-m_panes[i].GetWidth())) / nVarCount : 0;
                nVarCount += m_panes[i].GetWidth();
                widthExtra -= nVarWidth;
                widths.Add(nVarWidth);
            }
        }
    }

    return widths;
}

// ----------------------------------------------------------------------------
// status text stacks
// ----------------------------------------------------------------------------

void wxStatusBarBase::PushStatusText(const wxString& text, int number)
{
    // save the new text (in non-ellipsized form) in the stack
    m_panes[number].m_arrStack.push_back(text);

    SetStatusText(text, number);
        // update current status text (which will possibly be ellipsized)
        // also in the native control

    // SetStatusText() typically has an optimization built-in to avoid flickering
    // which won't refresh the status bar pane if the current top of the stack
    // is identic to the text passed to that function.
    // Since this optimization however cannot detect push/pop operations on the stack
    // we need to explicitely refresh the status bar pane ourselves:
    wxRect rect;
    GetFieldRect(number, rect);
    Refresh(true, &rect);
    Update();
}

void wxStatusBarBase::PopStatusText(int number)
{
    wxASSERT_MSG(m_panes[number].m_arrStack.GetCount() > 1,
                 "can't pop any further string");

    // the top of the stack is the status text currently shown in the native control;
    // remove it
    m_panes[number].m_arrStack.pop_back();

    // restore the previous status text in the native control
    const wxString& text = m_panes[number].m_arrStack.back();
    SetStatusText(text, number);

    // see comment in wxStatusBarBase::PushStatusText about why we need to explicitely
    // refresh the status bar pane
    wxRect rect;
    GetFieldRect(number, rect);
    Refresh(true, &rect);
    Update();
}

#endif // wxUSE_STATUSBAR
