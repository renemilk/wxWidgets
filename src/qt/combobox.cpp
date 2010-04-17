/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/combobox.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/combobox.h"

IMPLEMENT_DYNAMIC_CLASS( wxComboBox, wxControl )

wxComboBox::wxComboBox()
{
}


wxComboBox::wxComboBox(wxWindow *parent,
           wxWindowID id,
           const wxString& value,
           const wxPoint& pos,
           const wxSize& size,
           int n, const wxString choices[],
           long style,
           const wxValidator& validator,
           const wxString& name )
{
}


wxComboBox::wxComboBox(wxWindow *parent, wxWindowID id,
           const wxString& value,
           const wxPoint& pos,
           const wxSize& size,
           const wxArrayString& choices,
           long style,
           const wxValidator& validator,
           const wxString& name )
{
}


bool wxComboBox::Create(wxWindow *parent, wxWindowID id,
            const wxString& value,
            const wxPoint& pos,
            const wxSize& size,
            int n, const wxString choices[],
            long style,
            const wxValidator& validator,
            const wxString& name )
{
    return false;
}

bool wxComboBox::Create(wxWindow *parent, wxWindowID id,
            const wxString& value,
            const wxPoint& pos,
            const wxSize& size,
            const wxArrayString& choices,
            long style,
            const wxValidator& validator,
            const wxString& name )
{
    return false;
}


void wxComboBox::SetSelection(int n)
{
}

void wxComboBox::SetSelection(long from, long to)
{
}

int wxComboBox::GetSelection() const
{
    return 0;
}

void wxComboBox::GetSelection(long *from, long *to) const
{
}

wxString wxComboBox::GetStringSelection() const
{
    return wxString();
}


unsigned wxComboBox::GetCount() const
{
    return 0;
}

wxString wxComboBox::GetString(unsigned int n) const
{
    return wxString();
}

void wxComboBox::SetString(unsigned int n, const wxString& s)
{
}


int wxComboBox::DoInsertItems(const wxArrayStringsAdapter & items,
                  unsigned int pos,
                  void **clientData,
                  wxClientDataType type)
{
    return 0;
}


void wxComboBox::DoSetItemClientData(unsigned int n, void *clientData)
{
}

void *wxComboBox::DoGetItemClientData(unsigned int n) const
{
    return NULL;
}


void wxComboBox::DoClear()
{
}

void wxComboBox::DoDeleteOneItem(unsigned int pos)
{
}
