/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/msgdlg.mm
// Purpose:     wxMessageDialog
// Author:      Stefan Csomor
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id: msgdlg.cpp 54129 2008-06-11 19:30:52Z SC $
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/msgdlg.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/app.h"
#endif

#include "wx/thread.h"
#include "wx/osx/private.h"


IMPLEMENT_CLASS(wxMessageDialog, wxDialog)


wxMessageDialog::wxMessageDialog(wxWindow *parent,
                                 const wxString& message,
                                 const wxString& caption,
                                 long style,
                                 const wxPoint& WXUNUSED(pos))
               : wxMessageDialogBase(parent, message, caption, style)
{
    m_yes = _("Yes");
    m_no  = _("No");
    m_ok  = _("OK");
    m_cancel = _("Cancel");
}

bool wxMessageDialog::SetYesNoLabels(const wxString& yes,const wxString& no)
{
    m_yes = yes;
    m_no = no;
    return true;
}

bool wxMessageDialog::SetYesNoCancelLabels(const wxString& yes, const wxString& no, const wxString& cancel)
{
    m_yes = yes;
    m_no = no;
    m_cancel = cancel;
    return true;
}

bool wxMessageDialog::SetOKLabel(const wxString& ok)
{
    m_ok = ok;
    return true;
}

bool wxMessageDialog::SetOKCancelLabels(const wxString& ok, const wxString& cancel)
{
    m_ok = ok;
    m_cancel = cancel;
    return true;
}

int wxMessageDialog::ShowModal()
{
    int resultbutton = wxID_CANCEL;

    const long style = GetMessageDialogStyle();

    wxASSERT_MSG( (style & 0x3F) != wxYES, wxT("this style is not supported on Mac") );

    NSAlertStyle alertType = NSWarningAlertStyle;
    if (style & wxICON_EXCLAMATION)
        alertType = NSCriticalAlertStyle;
    else if (style & wxICON_HAND)
        alertType = NSWarningAlertStyle;
    else if (style & wxICON_INFORMATION)
        alertType = NSInformationalAlertStyle;
    else if (style & wxICON_QUESTION)
        alertType = NSInformationalAlertStyle;


    // work out what to display
    // if the extended text is empty then we use the caption as the title
    // and the message as the text (for backwards compatibility)
    // but if the extended message is not empty then we use the message as the title
    // and the extended message as the text because that makes more sense

    wxString msgtitle,msgtext;
    if(m_extendedMessage.IsEmpty())
    {
        msgtitle = m_caption;
        msgtext  = m_message;
    }
    else
    {
        msgtitle = m_message;
        msgtext  = m_extendedMessage;
    }


    if ( !wxIsMainThread() )
    {
        CFStringRef defaultButtonTitle = NULL;
        CFStringRef alternateButtonTitle = NULL;
        CFStringRef otherButtonTitle = NULL;

        wxCFStringRef cfTitle( msgtitle, GetFont().GetEncoding() );
        wxCFStringRef cfText( msgtext, GetFont().GetEncoding() );

        wxCFStringRef cfNoString( m_no.c_str(), GetFont().GetEncoding() );
        wxCFStringRef cfYesString( m_yes.c_str(), GetFont().GetEncoding() );
        wxCFStringRef cfOKString( m_ok.c_str() , GetFont().GetEncoding()) ;
        wxCFStringRef cfCancelString( m_cancel.c_str(), GetFont().GetEncoding() );

        int buttonId[4] = { 0, 0, 0, wxID_CANCEL /* time-out */ };

        if (style & wxYES_NO)
        {
            if ( style & wxNO_DEFAULT )
            {
                defaultButtonTitle = cfNoString;
                alternateButtonTitle = cfYesString;
                buttonId[0] = wxID_NO;
                buttonId[1] = wxID_YES;
            }
            else
            {
                defaultButtonTitle = cfYesString;
                alternateButtonTitle = cfNoString;
                buttonId[0] = wxID_YES;
                buttonId[1] = wxID_NO;
            }
            if (style & wxCANCEL)
            {
                otherButtonTitle = cfCancelString;
                buttonId[2] = wxID_CANCEL;
            }
        }
        else
        {
            // the MSW implementation even shows an OK button if it is not specified, we'll do the same
            buttonId[0] = wxID_OK;
            // using null as default title does not work on earlier systems
            defaultButtonTitle = cfOKString;
            if (style & wxCANCEL)
            {
                alternateButtonTitle = cfCancelString;
                buttonId[1] = wxID_CANCEL;
            }
        }

        CFOptionFlags exitButton;
        OSStatus err = CFUserNotificationDisplayAlert(
            0, alertType, NULL, NULL, NULL, cfTitle, cfText,
            defaultButtonTitle, alternateButtonTitle, otherButtonTitle, &exitButton );
        if (err == noErr)
            resultbutton = buttonId[exitButton];
    }
    else
    {
        NSAlert* alert = [[NSAlert alloc] init];
        
        wxCFStringRef cfNoString( m_no.c_str(), GetFont().GetEncoding() );
        wxCFStringRef cfYesString( m_yes.c_str(), GetFont().GetEncoding() );
        wxCFStringRef cfOKString( m_ok.c_str(), GetFont().GetEncoding() );
        wxCFStringRef cfCancelString( m_cancel.c_str(), GetFont().GetEncoding() );

        wxCFStringRef cfTitle( msgtitle, GetFont().GetEncoding() );
        wxCFStringRef cfText( msgtext, GetFont().GetEncoding() );

        [alert setMessageText:cfTitle.AsNSString()];
        [alert setInformativeText:cfText.AsNSString()];
        
        int buttonId[3] = { 0, 0, 0 };
        int buttonCount = 0;

        if (style & wxYES_NO)
        {
            if ( style & wxNO_DEFAULT )
            {
                [alert addButtonWithTitle:cfNoString.AsNSString()];
                buttonId[ buttonCount++ ] = wxID_NO;
                [alert addButtonWithTitle:cfYesString.AsNSString()];
                buttonId[ buttonCount++ ] = wxID_YES;
            }
            else
            {
                [alert addButtonWithTitle:cfYesString.AsNSString()];
                buttonId[ buttonCount++ ] = wxID_YES;
                [alert addButtonWithTitle:cfNoString.AsNSString()];
                buttonId[ buttonCount++ ] = wxID_NO;
            }

            if (style & wxCANCEL)
            {
                [alert addButtonWithTitle:cfCancelString.AsNSString()];
                buttonId[ buttonCount++ ] = wxID_CANCEL;
            }
        }
        // the MSW implementation even shows an OK button if it is not specified, we'll do the same
        else
        {
            [alert addButtonWithTitle:cfOKString.AsNSString()];
            buttonId[ buttonCount++ ] = wxID_OK;
            if (style & wxCANCEL)
            {
                [alert addButtonWithTitle:cfCancelString.AsNSString()];
                buttonId[ buttonCount++ ] = wxID_CANCEL;
            }
        }

        int button = [alert runModal];
        
        [alert release];
        
        if ( button < NSAlertFirstButtonReturn )
            resultbutton = wxID_CANCEL;
        else
        {
            if ( button - NSAlertFirstButtonReturn < buttonCount )
                resultbutton = buttonId[ button - NSAlertFirstButtonReturn ];
            else
                resultbutton = wxID_CANCEL;
        }
    }

    return resultbutton;
}
