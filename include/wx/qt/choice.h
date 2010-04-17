/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/choice.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_CHOICE_H_
#define _WX_QT_CHOICE_H_

class WXDLLIMPEXP_CORE wxChoice : public wxChoiceBase
{
public:
    wxChoice();
    
    wxChoice( wxWindow *parent, wxWindowID id,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            int n = 0, const wxString choices[] = (const wxString *) NULL,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxChoiceNameStr );
    
    wxChoice( wxWindow *parent, wxWindowID id,
            const wxPoint& pos,
            const wxSize& size,
            const wxArrayString& choices,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxChoiceNameStr );

    bool Create( wxWindow *parent, wxWindowID id,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            int n = 0, const wxString choices[] = NULL,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxChoiceNameStr );
    
    bool Create( wxWindow *parent, wxWindowID id,
            const wxPoint& pos,
            const wxSize& size,
            const wxArrayString& choices,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxChoiceNameStr );

    virtual unsigned int GetCount() const;
    virtual wxString GetString(unsigned int n) const;
    virtual void SetString(unsigned int n, const wxString& s);

    virtual void SetSelection(int n);
    virtual int GetSelection() const;

protected:
    virtual int DoInsertItems(const wxArrayStringsAdapter & items,
                              unsigned int pos,
                              void **clientData,
                              wxClientDataType type);
    
    virtual void DoSetItemClientData(unsigned int n, void *clientData);
    virtual void *DoGetItemClientData(unsigned int n) const;

    virtual void DoClear();
    virtual void DoDeleteOneItem(unsigned int pos);


private:
    DECLARE_DYNAMIC_CLASS(wxChoice)
};

#endif // _WX_QT_CHOICE_H_
