///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/radiobut.h
// Purpose:     wxRadioButton declaration
// Author:      Vadim Zeitlin
// Modified by:
// Created:     10.09.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_RADIOBUT_H_
#define _WX_UNIV_RADIOBUT_H_

#ifdef __GNUG__
    #pragma interface "univradiobut.h"
#endif

// ----------------------------------------------------------------------------
// wxRadioButton
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxRadioButton : public wxCheckBox
{
public:
    // constructors
    wxRadioButton() { Init(); }

    wxRadioButton(wxWindow *parent,
                  wxWindowID id,
                  const wxString& label,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = 0,
                  const wxValidator& validator = wxDefaultValidator,
                  const wxString& name = wxCheckBoxNameStr)
    {
        Init();

        Create(parent, id, label, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxCheckBoxNameStr);

    // override some base class methods
    virtual void ChangeValue(bool value);
    virtual wxBitmap GetBitmap(State state, Status status) const;

protected:
    // the radio button can only be cleared using this method, not
    // ChangeValue() above - and it is protected as it can only be called by
    // another radiobutton
    void ClearValue();

    // send event about radio button selection
    void SendEvent();

private:
    DECLARE_DYNAMIC_CLASS(wxRadioButton)
};

#endif // _WX_UNIV_RADIOBUT_H_
