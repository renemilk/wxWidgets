/////////////////////////////////////////////////////////////////////////////
// Name:        frame.h
// Purpose:     wxFrame class
// Author:      David Webster
// Modified by:
// Created:     10/27/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FRAME_H_
#define _WX_FRAME_H_

class WXDLLEXPORT wxFrame : public wxFrameBase
{
public:
    // construction
    wxFrame() { Init(); }
    wxFrame(wxWindow *parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxDEFAULT_FRAME_STYLE,
            const wxString& name = wxFrameNameStr)
    {
        Init();

        Create(parent, id, title, pos, size, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxFrameNameStr);

    virtual ~wxFrame();

    // implement base class pure virtuals
    virtual void Maximize(bool maximize = TRUE);
    virtual bool IsMaximized() const;
    virtual void Iconize(bool iconize = TRUE);
    virtual bool IsIconized() const;
    virtual void Restore();
    virtual void SetMenuBar(wxMenuBar *menubar);
    virtual void SetIcon(const wxIcon& icon);

    // implementation only from now on
    // -------------------------------

    // override some more virtuals
    virtual bool Show(bool show = TRUE);

    // event handlers
    void OnActivate(wxActivateEvent& event);
    void OnSysColourChanged(wxSysColourChangedEvent& event);

    // Toolbar
#if wxUSE_TOOLBAR
    virtual wxToolBar* CreateToolBar(long style = wxNO_BORDER | wxTB_HORIZONTAL | wxTB_FLAT,
                                     wxWindowID id = -1,
                                     const wxString& name = wxToolBarNameStr);

    virtual void PositionToolBar();
#endif // wxUSE_TOOLBAR

    // Status bar
#if wxUSE_STATUSBAR
    virtual wxStatusBar* OnCreateStatusBar(int number = 1,
                                           long style = wxST_SIZEGRIP,
                                           wxWindowID id = 0,
                                           const wxString& name = wxStatusLineNameStr);

    virtual void PositionStatusBar();

    // Hint to tell framework which status bar to use: the default is to use
    // native one for the platforms which support it (Win32), the generic one
    // otherwise

    // TODO: should this go into a wxFrameworkSettings class perhaps?
    static void UseNativeStatusBar(bool useNative)
        { m_useNativeStatusBar = useNative; };
    static bool UsesNativeStatusBar()
        { return m_useNativeStatusBar; };
#endif // wxUSE_STATUSBAR

    WXHMENU GetWinMenu() const { return m_hMenu; }

    // Returns the origin of client area (may be different from (0,0) if the
    // frame has a toolbar)
    virtual wxPoint GetClientAreaOrigin() const;

    // event handlers
    bool HandlePaint();
    bool HandleSize(int x, int y, WXUINT flag);
    bool HandleCommand(WXWORD id, WXWORD cmd, WXHWND control);
    bool HandleMenuSelect(WXWORD nItem, WXWORD nFlags, WXHMENU hMenu);

    bool OS2Create(int id, wxWindow *parent, const wxChar *wclass,
                   wxWindow *wx_win, const wxChar *title,
                   int x, int y, int width, int height, long style);

    // tooltip management
#if wxUSE_TOOLTIPS
    WXHWND GetToolTipCtrl() const { return m_hwndToolTip; }
    void SetToolTipCtrl(WXHWND hwndTT) { m_hwndToolTip = hwndTT; }
#endif // tooltips

protected:
    // common part of all ctors
    void Init();

    // common part of Iconize(), Maximize() and Restore()
    void DoShowWindow(int nShowCmd);

    // override base class virtuals
    virtual void DoGetClientSize(int *width, int *height) const;
    virtual void DoGetSize(int *width, int *height) const;
    virtual void DoGetPosition(int *x, int *y) const;

    virtual void DoSetClientSize(int width, int height);

    virtual void DoClientToScreen(int *x, int *y) const;
    virtual void DoScreenToClient(int *x, int *y) const;

    // helper
    void DetachMenuBar();

    // a plug in for MDI frame classes which need to do something special when
    // the menubar is set
    virtual void InternalSetMenuBar();

    // propagate our state change to all child frames
    void IconizeChildFrames(bool bIconize);

    // we add menu bar accel processing
    bool OS2TranslateMessage(WXMSG* pMsg);

    // window proc for the frames
    MRESULT OS2WindowProc(HWND hwnd, WXUINT message, WXWPARAM wParam, WXLPARAM lParam);

    bool                  m_iconized;
    WXHICON               m_defaultIcon;

#if wxUSE_STATUSBAR
    static bool           m_useNativeStatusBar;
#endif // wxUSE_STATUSBAR

private:
#if wxUSE_TOOLTIPS
    WXHWND                m_hwndToolTip;
#endif // tooltips

    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxFrame)
};

#endif
    // _WX_FRAME_H_

