/////////////////////////////////////////////////////////////////////////////
// Name:        tbargtk.h
// Purpose:     GTK toolbar
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_TBARGTK_H_
#define _WX_GTK_TBARGTK_H_

#if wxUSE_TOOLBAR

// ----------------------------------------------------------------------------
// wxToolBar
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxToolBar : public wxToolBarBase
{
public:
    // construction/destruction
    wxToolBar() { Init(); }
    wxToolBar( wxWindow *parent,
               wxWindowID id,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxTB_HORIZONTAL,
               const wxString& name = wxToolBarNameStr )
    {
        Init();

        Create(parent, id, pos, size, style, name);
    }

    bool Create( wxWindow *parent,
                 wxWindowID id,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxString& name = wxToolBarNameStr );

    virtual ~wxToolBar();

    // override base class virtuals
    virtual void SetMargins(int x, int y);
    virtual void SetToolSeparation(int separation);

    virtual wxToolBarToolBase *FindToolForPosition(wxCoord x, wxCoord y) const;

    virtual void SetToolShortHelp(int id, const wxString& helpString);

    virtual void SetWindowStyleFlag( long style );

    virtual void SetToolNormalBitmap(int id, const wxBitmap& bitmap);
    virtual void SetToolDisabledBitmap(int id, const wxBitmap& bitmap);

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    // implementation from now on
    // --------------------------

    GtkToolbar   *m_toolbar;

    bool          m_blockEvent;

    void OnInternalIdle();

protected:
    // common part of all ctors
    void Init();

    // set the GTK toolbar style and orientation
    void GtkSetStyle();

    virtual GdkWindow *GTKGetWindow(wxArrayGdkWindows& windows) const;

    // implement base class pure virtuals
    virtual bool DoInsertTool(size_t pos, wxToolBarToolBase *tool);
    virtual bool DoDeleteTool(size_t pos, wxToolBarToolBase *tool);

    virtual void DoEnableTool(wxToolBarToolBase *tool, bool enable);
    virtual void DoToggleTool(wxToolBarToolBase *tool, bool toggle);
    virtual void DoSetToggle(wxToolBarToolBase *tool, bool toggle);

    virtual wxToolBarToolBase *CreateTool(int id,
                                          const wxString& label,
                                          const wxBitmap& bitmap1,
                                          const wxBitmap& bitmap2,
                                          wxItemKind kind,
                                          wxObject *clientData,
                                          const wxString& shortHelpString,
                                          const wxString& longHelpString);
    virtual wxToolBarToolBase *CreateTool(wxControl *control,
                                          const wxString& label);

private:
    DECLARE_DYNAMIC_CLASS(wxToolBar)
};

#endif // wxUSE_TOOLBAR

#endif
    // _WX_GTK_TBARGTK_H_
