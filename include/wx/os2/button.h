/////////////////////////////////////////////////////////////////////////////
// Name:        button.h
// Purpose:     wxButton class
// Author:      David Webster
// Modified by:
// Created:     10/13/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BUTTON_H_
#define _WX_BUTTON_H_

#include "wx/control.h"

WXDLLEXPORT_DATA(extern const char*) wxButtonNameStr;

// Pushbutton
class WXDLLEXPORT wxButton: public wxButtonBase
{
 public:
    inline wxButton() {}
    inline wxButton( wxWindow*          pParent
                    ,wxWindowID         vId
                    ,const wxString&    rsLabel
                    ,const wxPoint&     rPos = wxDefaultPosition
                    ,const wxSize&      rSize = wxDefaultSize
                    ,long               lStyle = 0
#if wxUSE_VALIDATORS
                    ,const wxValidator& rValidator = wxDefaultValidator
#endif
                    ,const wxString&    rsName = wxButtonNameStr
                   )
    {
        Create( pParent
               ,vId
               ,rsLabel
               ,rPos
               ,rSize
               ,lStyle
#if wxUSE_VALIDATORS
               ,rValidator
#endif
               ,rsName
              );
    }

    bool Create( wxWindow*          pParent
                ,wxWindowID         vId
                ,const wxString&    rsLabel
                ,const wxPoint&     rPos = wxDefaultPosition
                ,const wxSize&      rSize = wxDefaultSize
                ,long               lStyle = 0
#if wxUSE_VALIDATORS
                ,const wxValidator& rValidator = wxDefaultValidator
#endif
                ,const wxString&    rsName = wxButtonNameStr
               );

    virtual ~wxButton();

    virtual void     SetDefault(void);
    static wxSize    GetDefaultSize(void);
    virtual void     Command(wxCommandEvent& rEvent);
    virtual bool     OS2Command( WXUINT uParam
                                ,WXWORD vId
                               );
    virtual WXHBRUSH OnCtlColor( WXHDC    hDC
                                ,WXHWND   hWnd
                                ,WXUINT   uCtlColor
                                ,WXUINT   uMessage
                                ,WXWPARAM wParam
                                ,WXLPARAM lParam
                               );
    void          MakeOwnerDrawn(void);

    virtual MRESULT WindowProc( WXUINT   uMsg
                               ,WXWPARAM wParam
                               ,WXLPARAM lParam
                              );

protected:

    bool           SendClickEvent(void);
    virtual wxSize DoGetBestSize(void) const;
private:
  DECLARE_DYNAMIC_CLASS(wxButton)
}; // end of CLASS wxButton

#endif
    // _WX_BUTTON_H_

