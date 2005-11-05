///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/wince/checklst.cpp
// Purpose:     implementation of wxCheckListBox class
// Author:      Wlodzimierz ABX Skiba
// Modified by:
// Created:     30.10.2005
// RCS-ID:      $Id$
// Copyright:   (c) Wlodzimierz Skiba
// Licence:     wxWindows licence
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

#if wxUSE_CHECKLISTBOX

#ifndef WX_PRECOMP
#endif

#include "wx/checklst.h"

// include <commctrl.h> "properly"
#include "wx/msw/wrapcctl.h"

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(wxCheckListBox, wxControl)

// ----------------------------------------------------------------------------
// implementation of wxCheckListBox class
// ----------------------------------------------------------------------------

// define event table
// ------------------
BEGIN_EVENT_TABLE(wxCheckListBox, wxControl)
    EVT_SIZE(wxCheckListBox::OnSize)
END_EVENT_TABLE()

// control creation
// ----------------

// def ctor: use Create() to really create the control
wxCheckListBox::wxCheckListBox()
{
}

// ctor which creates the associated control
wxCheckListBox::wxCheckListBox(wxWindow *parent, wxWindowID id,
                               const wxPoint& pos, const wxSize& size,
                               int nStrings, const wxString choices[],
                               long style, const wxValidator& val,
                               const wxString& name)
{
    Create(parent, id, pos, size, nStrings, choices, style, val, name);
}

wxCheckListBox::wxCheckListBox(wxWindow *parent, wxWindowID id,
                               const wxPoint& pos, const wxSize& size,
                               const wxArrayString& choices,
                               long style, const wxValidator& val,
                               const wxString& name)
{
    Create(parent, id, pos, size, choices, style, val, name);
}

wxCheckListBox::~wxCheckListBox()
{
    m_itemsClientData.Clear();
}

bool wxCheckListBox::Create(wxWindow *parent, wxWindowID id,
                            const wxPoint& pos, const wxSize& size,
                            int n, const wxString choices[],
                            long style,
                            const wxValidator& validator, const wxString& name)
{
    // initialize base class fields
    if ( !CreateControl(parent, id, pos, size, style, validator, name) )
        return false;

    // create the native control
    if ( !MSWCreateControl(WC_LISTVIEW, wxEmptyString, pos, size) )
    {
        // control creation failed
        return false;
    }

    ::SendMessage(GetHwnd(), LVM_SETEXTENDEDLISTVIEWSTYLE, 0,
                  LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT );

    // insert single column with checkboxes and labels
    LV_COLUMN col;
    wxZeroMemory(col);
    ListView_InsertColumn(GetHwnd(), 0, &col );

    // initialize the contents
    for ( int i = 0; i < n; i++ )
    {
        Append(choices[i]);
    }

    m_itemsClientData.SetCount(n);

    // now we can compute our best size correctly, so do it if necessary
    SetBestSize(size);

    return true;
}

bool wxCheckListBox::Create(wxWindow *parent, wxWindowID id,
                            const wxPoint& pos, const wxSize& size,
                            const wxArrayString& choices,
                            long style,
                            const wxValidator& validator, const wxString& name)
{
    wxCArrayString chs(choices);
    return Create(parent, id, pos, size, chs.GetCount(), chs.GetStrings(),
                  style, validator, name);
}

WXDWORD wxCheckListBox::MSWGetStyle(long style, WXDWORD *exstyle) const
{
    WXDWORD wstyle = wxControl::MSWGetStyle(style, exstyle);

    wstyle |= LVS_REPORT | LVS_NOCOLUMNHEADER | LVS_NOSORTHEADER;

    return wstyle;
}

void wxCheckListBox::OnSize(wxSizeEvent& event)
{
    // set width of the column we use to the width of list client area
    event.Skip();
    int w = GetClientSize().x;
    ListView_SetColumnWidth( GetHwnd(), 0, w );
}

// misc overloaded methods
// -----------------------

void wxCheckListBox::Delete(int n)
{
    wxCHECK_RET( n >= 0 && n < GetCount(),
                 _T("invalid index in wxCheckListBox::Delete") );

    if ( !ListView_DeleteItem(GetHwnd(), n) )
    {
        wxLogLastError(_T("ListView_DeleteItem"));
    }
    m_itemsClientData.RemoveAt(n);
}

// check items
// -----------

bool wxCheckListBox::IsChecked(size_t uiIndex) const
{
    wxCHECK_MSG( uiIndex < (size_t)GetCount(), false,
                 _T("invalid index in wxCheckListBox::IsChecked") );

    return (ListView_GetCheckState(((HWND)GetHWND()), uiIndex) != 0);
}

void wxCheckListBox::Check(size_t uiIndex, bool bCheck)
{
    wxCHECK_RET( uiIndex < (size_t)GetCount(),
                 _T("invalid index in wxCheckListBox::Check") );

    ListView_SetCheckState(((HWND)GetHWND()), uiIndex, bCheck)
}

// interface derived from wxListBox and lower classes
// --------------------------------------------------

void wxCheckListBox::Clear()
{
    int n = GetCount();

    while ( n > 0 )
    {
        n--;
        Delete(n);
    }

    m_itemsClientData.Clear();

    wxCHECK_RET( n == GetCount(),
                 _T("broken wxCheckListBox::Clear()") );
}

int wxCheckListBox::GetCount() const
{
    return ListView_GetItemCount( (HWND)GetHWND() );
}

int wxCheckListBox::GetSelection() const
{
    return wxNOT_FOUND;
}

int wxCheckListBox::GetSelections(wxArrayInt& aSelections) const
{
    int n = GetCount();
    while ( n > 0 )
    {
        n--;
        if(IsChecked(n)) aSelections.Insert(n,0);
    }

    return aSelections.GetCount();
}

wxString wxCheckListBox::GetString(int n) const
{
    const int bufSize = 513;
    wxChar buf[bufSize];
    ListView_GetItemText( (HWND)GetHWND(), n, 0, buf, bufSize - 1 );
    buf[bufSize-1] = _T('\0');
    wxString str(buf);
    return str;
}

bool wxCheckListBox::IsSelected(int n) const
{
    return IsChecked(n);
}

void wxCheckListBox::SetString(int n, const wxString& s)
{
    wxCHECK_RET( n < GetCount(),
                 _T("invalid index in wxCheckListBox::SetString") );
    wxChar *buf = new wxChar[s.length()+1];
    wxStrcpy(buf, s.c_str());
    ListView_SetItemText( (HWND)GetHWND(), n, 0, buf );
    delete [] buf;
}

int wxCheckListBox::DoAppend(const wxString& item)
{
    int n = GetCount();
    LVITEM newItem;
    wxZeroMemory(newItem);
    newItem.iItem = n;
    int ret = ListView_InsertItem( (HWND)GetHWND(), & newItem );
    wxCHECK_MSG( n == ret , -1, _T("Item not added") );
    SetString( ret , item );
    m_itemsClientData.Insert(NULL, ret);
    return ret;
}

void* wxCheckListBox::DoGetItemClientData(int n) const
{
    return m_itemsClientData.Item(n);
}

wxClientData* wxCheckListBox::DoGetItemClientObject(int n) const
{
    return (wxClientData *)DoGetItemClientData(n);
}

void wxCheckListBox::DoInsertItems(const wxArrayString& items, int pos)
{
    for( size_t i = 0; i < items.GetCount(); i++ )
    {
        Insert(items[i],pos+i);
    }
}

void wxCheckListBox::DoSetFirstItem(int n)
{
    int pos = ListView_GetTopIndex( (HWND)GetHWND() );
    if(pos == n) return;
    POINT ppt;
    BOOL ret = ListView_GetItemPosition( (HWND)GetHWND(), n, &ppt );
    wxCHECK_RET( ret == TRUE, _T("Broken DoSetFirstItem") );
    ListView_Scroll( (HWND)GetHWND(), 0, 0 );
    ListView_Scroll( (HWND)GetHWND(), 0, ppt.y );
}

void wxCheckListBox::DoSetItemClientData(int n, void* clientData)
{
    m_itemsClientData.Item(n) = clientData;
}

void wxCheckListBox::DoSetItemClientObject(int n, wxClientData* clientData)
{
    DoSetItemClientData(n, clientData);
}

void wxCheckListBox::DoSetItems(const wxArrayString& items, void **clientData)
{
    for( size_t i = 0; i < items.GetCount(); i++ )
    {
        int pos = Append(items[i]);
        if( pos >= 0 && clientData )
            DoSetItemClientData(pos, clientData[i]);
    }
}

void wxCheckListBox::DoSetSelection(int n, bool select)
{
    Check(n,select);
}

#endif
