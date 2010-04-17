/////////////////////////////////////////////////////////////////////////////
// Name:        textdlgg.h
// Purpose:     wxTextEntryDialog class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TEXTDLGG_H_
#define _WX_TEXTDLGG_H_

#include "wx/defs.h"

#if wxUSE_TEXTDLG

#include "wx/dialog.h"

#if wxUSE_VALIDATORS
#include "wx/valtext.h"
#include "wx/textctrl.h"
#endif

class WXDLLIMPEXP_FWD_CORE wxTextCtrl;

// ----------------------------------------------------------------------------
// wxTextEntryDialog: a dialog with text control, [ok] and [cancel] buttons
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxTextEntryDialog : public wxDialog
{
public:
    wxTextEntryDialog(wxWindow *parent,
                      const wxString& message,
                      const wxString& caption = wxGetTextFromUserPromptStr,
                      const wxString& value = wxEmptyString,
                      long style = wxTextEntryDialogStyle,
                      const wxPoint& pos = wxDefaultPosition);

    void SetValue(const wxString& val);
    wxString GetValue() const { return m_value; }

#if wxUSE_VALIDATORS
    void SetTextValidator( const wxTextValidator& validator );
#if WXWIN_COMPATIBILITY_2_8
    wxDEPRECATED( void SetTextValidator( long style ) );
#endif
    void SetTextValidator( wxTextValidatorStyle style = wxFILTER_NONE );
    wxTextValidator* GetTextValidator() { return (wxTextValidator*)m_textctrl->GetValidator(); }
#endif
  // wxUSE_VALIDATORS

    // implementation only
    void OnOK(wxCommandEvent& event);

protected:
    wxTextCtrl *m_textctrl;
    wxString    m_value;
    long        m_dialogStyle;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxTextEntryDialog)
    wxDECLARE_NO_COPY_CLASS(wxTextEntryDialog);
};

// ----------------------------------------------------------------------------
// wxPasswordEntryDialog: dialog with password control, [ok] and [cancel]
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxPasswordEntryDialog : public wxTextEntryDialog
{
public:
    wxPasswordEntryDialog(wxWindow *parent,
                      const wxString& message,
                      const wxString& caption = wxGetPasswordFromUserPromptStr,
                      const wxString& value = wxEmptyString,
                      long style = wxTextEntryDialogStyle,
                      const wxPoint& pos = wxDefaultPosition);
private:
    DECLARE_DYNAMIC_CLASS(wxPasswordEntryDialog)
    wxDECLARE_NO_COPY_CLASS(wxPasswordEntryDialog);
};

#endif
    // wxUSE_TEXTDLG
#endif // _WX_TEXTDLGG_H_
