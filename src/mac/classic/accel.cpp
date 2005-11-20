/////////////////////////////////////////////////////////////////////////////
// Name:        accel.cpp
// Purpose:     wxAcceleratorTable
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/setup.h"
#include "wx/accel.h"
#include "wx/string.h"

IMPLEMENT_DYNAMIC_CLASS(wxAcceleratorTable, wxObject)

// ----------------------------------------------------------------------------
// wxAccelList: a list of wxAcceleratorEntries
// ----------------------------------------------------------------------------

WX_DECLARE_LIST(wxAcceleratorEntry, wxAccelList);
#include "wx/listimpl.cpp"
WX_DEFINE_LIST(wxAccelList)

// ----------------------------------------------------------------------------
// wxAccelRefData: the data used by wxAcceleratorTable
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxAcceleratorRefData: public wxObjectRefData
{
    friend class WXDLLEXPORT wxAcceleratorTable;
public:
    wxAcceleratorRefData();
    ~wxAcceleratorRefData();

    wxAccelList m_accels;
};

#define M_ACCELDATA ((wxAcceleratorRefData *)m_refData)

wxAcceleratorRefData::wxAcceleratorRefData()
    : m_accels()
{
}

wxAcceleratorRefData::~wxAcceleratorRefData()
{
    m_accels.DeleteContents( TRUE );
}

wxAcceleratorTable::wxAcceleratorTable()
{
    m_refData = NULL;
}

wxAcceleratorTable::~wxAcceleratorTable()
{
}

// Create from an array
wxAcceleratorTable::wxAcceleratorTable(int n, const wxAcceleratorEntry entries[])
{
    m_refData = new wxAcceleratorRefData;

    for (int i = 0; i < n; i++)
    {
        int flag    = entries[i].GetFlags();
        int keycode = entries[i].GetKeyCode();
        int command = entries[i].GetCommand();
        if ((keycode >= (int)'a') && (keycode <= (int)'z')) keycode = (int)toupper( (char)keycode );
        M_ACCELDATA->m_accels.Append( new wxAcceleratorEntry( flag, keycode, command ) );
    }
}

bool wxAcceleratorTable::Ok() const
{
    return (m_refData != NULL);
}

int wxAcceleratorTable::GetCommand( wxKeyEvent &event )
{
    if (!Ok()) return -1;

    wxAccelList::Node *node = M_ACCELDATA->m_accels.GetFirst();
    while (node)
    {
        wxAcceleratorEntry *entry = (wxAcceleratorEntry*)node->GetData();
        if ((event.m_keyCode == entry->GetKeyCode()) &&
           (((entry->GetFlags() & wxACCEL_CTRL) == 0) || event.ControlDown()) &&
           (((entry->GetFlags() & wxACCEL_SHIFT) == 0) || event.ShiftDown()) &&
           (((entry->GetFlags() & wxACCEL_ALT) == 0) || event.AltDown() || event.MetaDown()))
        {
            return entry->GetCommand();
        }
        node = node->GetNext();
    }

    return -1;
}


