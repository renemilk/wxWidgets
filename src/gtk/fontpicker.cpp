/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/fontpicker.cpp
// Purpose:     implementation of wxFontButton
// Author:      Francesco Montorsi
// Modified By:
// Created:     15/04/2006
// Id:          $Id$
// Copyright:   (c) Francesco Montorsi
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_FONTPICKERCTRL && defined(__WXGTK24__)

#include "wx/fontpicker.h"

#include "wx/fontutil.h"        // for wxNativeFontInfo
#include "wx/gtk/private.h"

// ============================================================================
// implementation
// ============================================================================

//-----------------------------------------------------------------------------
// "font-set"
//-----------------------------------------------------------------------------

extern "C" {
static void gtk_fontbutton_setfont_callback(GtkFontButton *widget,
                                            wxFontButton *p)
{
    // update the m_selectedFont member of the wxFontButton
    wxASSERT(p);
    p->SetNativeFontInfo(gtk_font_button_get_font_name(widget));

    // fire the colour-changed event
    wxFontPickerEvent event(p, p->GetId(), p->GetSelectedFont());
    p->GetEventHandler()->ProcessEvent(event);
}
}

//-----------------------------------------------------------------------------
// wxFontButton
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxFontButton, wxGenericFontButton)

bool wxFontButton::Create( wxWindow *parent, wxWindowID id,
                        const wxFont &initial,
                        const wxPoint &pos, const wxSize &size,
                        long style, const wxValidator& validator,
                        const wxString &name )
{
    if (!gtk_check_version(2,4,0))
    {
        m_needParent = true;
        m_acceptsFocus = true;

        if (!PreCreation( parent, pos, size ) ||
            !wxControl::CreateBase(parent, id, pos, size, style, validator, name))
        {
            wxFAIL_MSG( wxT("wxFontButton creation failed") );
            return false;
        }

        m_widget = gtk_font_button_new();

        // set initial font
        m_selectedFont = initial;
        UpdateFont();

        // honour the fontbutton styles
        bool showall = (style & wxFNTP_FONTDESC_AS_LABEL) != 0,
             usefont = (style & wxFNTP_USEFONT_FOR_LABEL) != 0;
        gtk_font_button_set_show_style(GTK_FONT_BUTTON(m_widget), showall);
        gtk_font_button_set_show_size(GTK_FONT_BUTTON(m_widget), showall);

        gtk_font_button_set_use_size(GTK_FONT_BUTTON(m_widget), usefont);
        gtk_font_button_set_use_font(GTK_FONT_BUTTON(m_widget), usefont);

        gtk_widget_show( GTK_WIDGET(m_widget) );

        // GtkFontButton signals
        g_signal_connect(m_widget, "font-set",
                        G_CALLBACK(gtk_fontbutton_setfont_callback), this);


        m_parent->DoAddChild( this );

        PostCreation(size);
        SetBestSize(size);
    }
    else
        return wxGenericFontButton::Create(parent, id, initial, pos, size,
                                           style, validator, name);
    return true;
}

wxFontButton::~wxFontButton()
{
}

void wxFontButton::UpdateFont()
{
    if (!gtk_check_version(2,4,0))
    {
        const wxNativeFontInfo *info = m_selectedFont.GetNativeFontInfo();
        wxASSERT_MSG( info, wxT("The fontbutton's internal font is not valid ?") );

        const wxString& fontname = info->ToString();
        gtk_font_button_set_font_name(GTK_FONT_BUTTON(m_widget), wxGTK_CONV(fontname));
    }
    else
        wxGenericFontButton::UpdateFont();
}

#endif      // wxUSE_FONTPICKERCTRL && defined(__WXGTK24__)
