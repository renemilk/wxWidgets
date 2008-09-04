/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/mdi.h
// Purpose:     MDI (Multiple Document Interface) classes
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MDI_H_
#define _WX_MDI_H_

#include "wx/frame.h"

extern WXDLLIMPEXP_DATA_CORE(const char) wxStatusLineNameStr[];

class WXDLLIMPEXP_FWD_CORE wxMDIClientWindow;
class WXDLLIMPEXP_FWD_CORE wxMDIChildFrame;

// ---------------------------------------------------------------------------
// wxMDIParentFrame
// ---------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxMDIParentFrame : public wxFrame
{
public:
    wxMDIParentFrame();
    wxMDIParentFrame(wxWindow *parent,
                     wxWindowID id,
                     const wxString& title,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
                     const wxString& name = wxFrameNameStr)
    {
        Create(parent, id, title, pos, size, style, name);
    }

    virtual ~wxMDIParentFrame();

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
                const wxString& name = wxFrameNameStr);

    // accessors
    // ---------

    // Get the active MDI child window
    wxMDIChildFrame *GetActiveChild() const;

    // Get the client window
    wxMDIClientWindow *GetClientWindow() const { return m_clientWindow; }

    // Create the client window class (don't Create the window,
    // just return a new class)
    virtual wxMDIClientWindow *OnCreateClient();

    // MDI windows menu functions
    // --------------------------

    // return the pointer to the current window menu or NULL if we don't have
    // because of wxFRAME_NO_WINDOW_MENU style
    wxMenu *GetWindowMenu() const { return m_windowMenu; }

    // use the given menu instead of the default window menu
    //
    // menu can be NULL to disable the window menu completely
    void SetWindowMenu(wxMenu* menu) ;

    virtual void DoMenuUpdates(wxMenu* menu = NULL);

    // MDI operations
    // --------------
    virtual void Cascade();
    virtual void Tile(wxOrientation orient = wxHORIZONTAL);
    virtual void ArrangeIcons();
    virtual void ActivateNext();
    virtual void ActivatePrevious();


    // implementation only from now on

    // MDI helpers
    // -----------

    // called by wxMDIChildFrame after it was successfully created
    virtual void AddMDIChild(wxMDIChildFrame *child);

    // called by wxMDIChildFrame just before it is destroyed
    virtual void RemoveMDIChild(wxMDIChildFrame *child);

    // handlers
    // --------

    // Responds to colour changes
    void OnSysColourChanged(wxSysColourChangedEvent& event);

    void OnSize(wxSizeEvent& event);
    void OnIconized(wxIconizeEvent& event);

    bool HandleActivate(int state, bool minimized, WXHWND activate);
    bool HandleCommand(WXWORD id, WXWORD cmd, WXHWND control);

    // override window proc for MDI-specific message processing
    virtual WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);

    virtual WXLRESULT MSWDefWindowProc(WXUINT, WXWPARAM, WXLPARAM);
    virtual bool MSWTranslateMessage(WXMSG* msg);

    // override wxFrameBase function to also look in the active child menu bar
    virtual const wxMenuItem *FindItemInMenuBar(int menuId) const;

protected:
#if wxUSE_MENUS_NATIVE
    virtual void InternalSetMenuBar();
#endif // wxUSE_MENUS_NATIVE

    virtual WXHICON GetDefaultIcon() const;

    // set the size of the MDI client window to match the frame size
    void UpdateClientSize();


    wxMDIClientWindow *             m_clientWindow;
    wxMDIChildFrame *               m_currentChild;

    // the current window menu or NULL if we are not using it
    wxMenu *m_windowMenu;

    // true if MDI Frame is intercepting commands, not child
    bool m_parentFrameActive;

private:
    // add/remove window menu if we have it (i.e. m_windowMenu != NULL)
    void AddWindowMenu();
    void RemoveWindowMenu();

    // return the number of child frames we currently have (maybe 0)
    int GetChildFramesCount() const;

    friend class WXDLLIMPEXP_FWD_CORE wxMDIChildFrame;

    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxMDIParentFrame)
    DECLARE_NO_COPY_CLASS(wxMDIParentFrame)
};

// ---------------------------------------------------------------------------
// wxMDIChildFrame
// ---------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxMDIChildFrame : public wxFrame
{
public:
    wxMDIChildFrame() { Init(); }
    wxMDIChildFrame(wxMDIParentFrame *parent,
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

    virtual ~wxMDIChildFrame();

    bool Create(wxMDIParentFrame *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxFrameNameStr);

    virtual bool IsTopLevel() const { return false; }

    // MDI operations
    virtual void Maximize(bool maximize = true);
    virtual void Restore();
    virtual void Activate();

    // Implementation only from now on
    // -------------------------------

    wxMDIParentFrame* GetMDIParent() const
    {
        return wxStaticCast(wxFrame::GetParent(), wxMDIParentFrame);
    }

    // Handlers
    bool HandleMDIActivate(long bActivate, WXHWND, WXHWND);
    bool HandleWindowPosChanging(void *lpPos);
    bool HandleCommand(WXWORD id, WXWORD cmd, WXHWND control);
    bool HandleGetMinMaxInfo(void *mmInfo);

    virtual WXLRESULT MSWWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam);
    virtual WXLRESULT MSWDefWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam);
    virtual bool MSWTranslateMessage(WXMSG *msg);

    virtual void MSWDestroyWindow();

    bool ResetWindowStyle(void *vrect);

    void OnIdle(wxIdleEvent& event);

    virtual bool Show(bool show = true);

protected:
    virtual void DoGetScreenPosition(int *x, int *y) const;
    virtual void DoGetPosition(int *x, int *y) const;
    virtual void DoSetClientSize(int width, int height);
    virtual void InternalSetMenuBar();
    virtual bool IsMDIChild() const { return true; }
    virtual void DetachMenuBar();

    virtual WXHICON GetDefaultIcon() const;

    // common part of all ctors
    void Init();

private:
    bool m_needsInitialShow; // Show must be called in idle time after Creation
    bool m_needsResize; // flag which tells us to artificially resize the frame

    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMDIChildFrame)
};

// ---------------------------------------------------------------------------
// wxMDIClientWindow
// ---------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxMDIClientWindow : public wxWindow
{
public:
    wxMDIClientWindow() { Init(); }
    wxMDIClientWindow(wxMDIParentFrame *parent, long style = 0)
    {
        Init();

        CreateClient(parent, style);
    }

    // Note: this is virtual, to allow overridden behaviour.
    virtual bool CreateClient(wxMDIParentFrame *parent,
                              long style = wxVSCROLL | wxHSCROLL);

    // Explicitly call default scroll behaviour
    void OnScroll(wxScrollEvent& event);

protected:
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);

    void Init() { m_scrollX = m_scrollY = 0; }

    int m_scrollX, m_scrollY;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMDIClientWindow)
};

#endif
    // _WX_MDI_H_
