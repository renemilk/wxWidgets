/////////////////////////////////////////////////////////////////////////////
// Name:        wx/palmos/window.h
// Purpose:     wxWindow class
// Author:      William Osborne
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id:
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINDOW_H_
#define _WX_WINDOW_H_

#ifdef __PALMOS__
    #include <PalmOS.h>
#endif


// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "window.h"
#endif

// [at least] some version of Windows send extra mouse move messages after
// a mouse click or a key press - to temporarily fix this problem, set the
// define below to 1
//
// a better solution should be found later...
#define wxUSE_MOUSEEVENT_HACK 0

// ---------------------------------------------------------------------------
// constants
// ---------------------------------------------------------------------------

#if WXWIN_COMPATIBILITY_2_4
// they're unused by wxWidgets...
enum
{
    wxKEY_SHIFT = 1,
    wxKEY_CTRL  = 2
};
#endif

// ---------------------------------------------------------------------------
// wxWindow declaration for Palm
// ---------------------------------------------------------------------------

class WXDLLEXPORT wxWindowPalm : public wxWindowBase
{
public:
    wxWindowPalm() { Init(); }

    wxWindowPalm(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxPanelNameStr)
    {
        Init();
        Create(parent, id, pos, size, style, name);
    }

    virtual ~wxWindowPalm();

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxPanelNameStr);

    // implement base class pure virtuals
    virtual void SetTitle( const wxString& title);
    virtual wxString GetTitle() const;

    virtual void Raise();
    virtual void Lower();

    virtual bool Show( bool show = true );
    virtual bool Enable( bool enable = true );

    virtual void SetFocus();
    virtual void SetFocusFromKbd();

    virtual bool Reparent(wxWindowBase *newParent);

    virtual void WarpPointer(int x, int y);

    virtual void Refresh( bool eraseBackground = true,
                          const wxRect *rect = (const wxRect *) NULL );
    virtual void Update();
    virtual void Freeze();
    virtual void Thaw();

    virtual void SetWindowStyleFlag( long style );
    virtual bool SetCursor( const wxCursor &cursor );
    virtual bool SetFont( const wxFont &font );

    virtual int GetCharHeight() const;
    virtual int GetCharWidth() const;
    virtual void GetTextExtent(const wxString& string,
                               int *x, int *y,
                               int *descent = (int *) NULL,
                               int *externalLeading = (int *) NULL,
                               const wxFont *theFont = (const wxFont *) NULL)
                               const;

#if wxUSE_MENUS_NATIVE
    virtual bool DoPopupMenu( wxMenu *menu, int x, int y );
#endif // wxUSE_MENUS_NATIVE

    virtual void SetScrollbar( int orient, int pos, int thumbVisible,
                               int range, bool refresh = true );
    virtual void SetScrollPos( int orient, int pos, bool refresh = true );
    virtual int GetScrollPos( int orient ) const;
    virtual int GetScrollThumb( int orient ) const;
    virtual int GetScrollRange( int orient ) const;
    virtual void ScrollWindow( int dx, int dy,
                               const wxRect* rect = (wxRect *) NULL );

    virtual bool ScrollLines(int lines);
    virtual bool ScrollPages(int pages);

#if wxUSE_DRAG_AND_DROP
    virtual void SetDropTarget( wxDropTarget *dropTarget );
#endif // wxUSE_DRAG_AND_DROP

    // Accept files for dragging
    virtual void DragAcceptFiles(bool accept);

#if WXWIN_COMPATIBILITY_2_4
    wxDEPRECATED( bool GetUseCtl3D() const );
    wxDEPRECATED( bool GetTransparentBackground() const );
    wxDEPRECATED( void SetTransparent(bool t = true) );
#endif // WXWIN_COMPATIBILITY_2_4

#ifndef __WXUNIVERSAL__
    // Native resource loading (implemented in src/Palm/nativdlg.cpp)
    // FIXME: should they really be all virtual?
    virtual bool LoadNativeDialog(wxWindow* parent, wxWindowID& id);
    virtual bool LoadNativeDialog(wxWindow* parent, const wxString& name);
    wxWindow* GetWindowChild1(wxWindowID id);
    wxWindow* GetWindowChild(wxWindowID id);
#endif // __WXUNIVERSAL__

#if wxUSE_HOTKEY
    // install and deinstall a system wide hotkey
    virtual bool RegisterHotKey(int hotkeyId, int modifiers, int keycode);
    virtual bool UnregisterHotKey(int hotkeyId);
#endif // wxUSE_HOTKEY

    // implementation from now on
    // --------------------------

    // simple accessors
    // ----------------

    WXHWND GetHWND() const { return m_hWnd; }
    void SetHWND(WXHWND hWnd) { m_hWnd = hWnd; }
    virtual WXWidget GetHandle() const { return GetHWND(); }

    // event handlers
    // --------------

    void OnEraseBackground(wxEraseEvent& event);
    void OnPaint(wxPaintEvent& event);

public:
    // For implementation purposes - sometimes decorations make the client area
    // smaller
    virtual wxPoint GetClientAreaOrigin() const;

    // Windows subclassing
    void SubclassWin(WXHWND hWnd);
    void UnsubclassWin();

    WXFARPROC PalmGetOldWndProc() const { return m_oldWndProc; }
    void PalmSetOldWndProc(WXFARPROC proc) { m_oldWndProc = proc; }

    // return true if the window is of a standard (i.e. not wxWidgets') class
    //
    // to understand why does it work, look at SubclassWin() code and comments
    bool IsOfStandardClass() const { return m_oldWndProc != NULL; }

    wxWindow *FindItem(long id) const;
    wxWindow *FindItemByHWND(WXHWND hWnd, bool controlOnly = false) const;

    // Palm only: true if this control is part of the main control
    virtual bool ContainsHWND(WXHWND WXUNUSED(hWnd)) const { return false; };

    // translate wxWidgets style flags for this control into the Windows style
    // and optional extended style for the corresponding native control
    //
    // this is the function that should be overridden in the derived classes,
    // but you will mostly use PalmGetCreateWindowFlags() below
    virtual WXDWORD PalmGetStyle(long flags, WXDWORD *exstyle = NULL) const ;

    // get the Palm window flags corresponding to wxWidgets ones
    //
    // the functions returns the flags (WS_XXX) directly and puts the ext
    // (WS_EX_XXX) flags into the provided pointer if not NULL
    WXDWORD PalmGetCreateWindowFlags(WXDWORD *exflags = NULL) const
        { return PalmGetStyle(GetWindowStyle(), exflags); }

    // translate wxWidgets coords into Windows ones suitable to be passed to
    // ::CreateWindow()
    //
    // returns true if non default coords are returned, false otherwise
    bool PalmGetCreateWindowCoords(const wxPoint& pos,
                                  const wxSize& size,
                                  int& x, int& y,
                                  int& w, int& h) const;

    // get the HWND to be used as parent of this window with CreateWindow()
    virtual WXHWND PalmGetParent() const;

    // creates the window of specified Windows class with given style, extended
    // style, title and geometry (default values
    //
    // returns true if the window has been created, false if creation failed
    bool PalmCreate(const wxChar *wclass,
                   const wxChar *title = NULL,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   WXDWORD style = 0,
                   WXDWORD exendedStyle = 0);

    virtual bool PalmCommand(WXUINT param, WXWORD id);

#ifndef __WXUNIVERSAL__
    // Create an appropriate wxWindow from a HWND
    virtual wxWindow* CreateWindowFromHWND(wxWindow* parent, WXHWND hWnd);

    // Make sure the window style reflects the HWND style (roughly)
    virtual void AdoptAttributesFromHWND();
#endif // __WXUNIVERSAL__

    // Setup background and foreground colours correctly
    virtual void SetupColours();

    // ------------------------------------------------------------------------
    // helpers for message handlers: these perform the same function as the
    // message crackers from <windowsx.h> - they unpack WPARAM and LPARAM into
    // the correct parameters
    // ------------------------------------------------------------------------

    void UnpackCommand(WXWPARAM wParam, WXLPARAM lParam,
                       WXWORD *id, WXHWND *hwnd, WXWORD *cmd);
    void UnpackActivate(WXWPARAM wParam, WXLPARAM lParam,
                        WXWORD *state, WXWORD *minimized, WXHWND *hwnd);
    void UnpackScroll(WXWPARAM wParam, WXLPARAM lParam,
                      WXWORD *code, WXWORD *pos, WXHWND *hwnd);
    void UnpackCtlColor(WXWPARAM wParam, WXLPARAM lParam,
                        WXWORD *nCtlColor, WXHDC *hdc, WXHWND *hwnd);
    void UnpackMenuSelect(WXWPARAM wParam, WXLPARAM lParam,
                          WXWORD *item, WXWORD *flags, WXHMENU *hmenu);

    // ------------------------------------------------------------------------
    // internal handlers for Palm messages: all handlers return a boolean value:
    // true means that the handler processed the event and false that it didn't
    // ------------------------------------------------------------------------

    // there are several cases where we have virtual functions for Windows
    // message processing: this is because these messages often require to be
    // processed in a different manner in the derived classes. For all other
    // messages, however, we do *not* have corresponding PalmOnXXX() function
    // and if the derived class wants to process them, it should override
    // PalmWindowProc() directly.

    // scroll event (both horizontal and vertical)
    virtual bool PalmOnScroll(int orientation, WXWORD nSBCode,
                             WXWORD pos, WXHWND control);

    // child control notifications
#ifdef __WIN95__
    virtual bool PalmOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result);
#endif // __WIN95__

    // owner-drawn controls need to process these messages
    virtual bool PalmOnDrawItem(int id, WXDRAWITEMSTRUCT *item);
    virtual bool PalmOnMeasureItem(int id, WXMEASUREITEMSTRUCT *item);

    // the rest are not virtual
    bool HandleCreate(WXLPCREATESTRUCT cs, bool *mayCreate);
    bool HandleInitDialog(WXHWND hWndFocus);
    bool HandleDestroy();

    bool HandlePaint();
    bool HandleEraseBkgnd(WXHDC pDC);

    bool HandleMinimize();
    bool HandleMaximize();
    bool HandleSize(int x, int y, WXUINT flag);
    bool HandleSizing(wxRect& rect);
    bool HandleGetMinMaxInfo(void *mmInfo);

    bool HandleShow(bool show, int status);
    bool HandleActivate(int flag, bool minimized, WXHWND activate);

    bool HandleCommand(WXWORD id, WXWORD cmd, WXHWND control);

    bool HandleCtlColor(WXHBRUSH *hBrush,
                        WXHDC hdc,
                        WXHWND hWnd,
                        WXUINT nCtlColor,
                        WXUINT message,
                        WXWPARAM wParam,
                        WXLPARAM lParam);

    bool HandlePaletteChanged(WXHWND hWndPalChange);
    bool HandleQueryNewPalette();
    bool HandleSysColorChange();
    bool HandleDisplayChange();
    bool HandleCaptureChanged(WXHWND gainedCapture);

    bool HandleQueryEndSession(long logOff, bool *mayEnd);
    bool HandleEndSession(bool endSession, long logOff);

    bool HandleSetFocus(WXHWND wnd);
    bool HandleKillFocus(WXHWND wnd);

    bool HandleDropFiles(WXWPARAM wParam);

    bool HandleMouseEvent(WXUINT msg, int x, int y, WXUINT flags);
    bool HandleMouseMove(int x, int y, WXUINT flags);
    bool HandleMouseWheel(WXWPARAM wParam, WXLPARAM lParam);

    bool HandleChar(WXWPARAM wParam, WXLPARAM lParam, bool isASCII = false);
    bool HandleKeyDown(WXWPARAM wParam, WXLPARAM lParam);
    bool HandleKeyUp(WXWPARAM wParam, WXLPARAM lParam);
#if wxUSE_ACCEL
    bool HandleHotKey(WXWPARAM wParam, WXLPARAM lParam);
#endif
#ifdef __WIN32__
    int HandleMenuChar(int chAccel, WXLPARAM lParam);
#endif

    bool HandleQueryDragIcon(WXHICON *hIcon);

    bool HandleSetCursor(WXHWND hWnd, short nHitTest, int mouseMsg);

    // Window procedure
    virtual WXLRESULT PalmWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);

    // Calls an appropriate default window procedure
    virtual WXLRESULT PalmDefWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);

    // message processing helpers

    // return false if the message shouldn't be translated/preprocessed but
    // dispatched normally
    virtual bool PalmShouldPreProcessMessage(WXMSG* pMsg);

    // return true if the message was preprocessed and shouldn't be dispatched
    virtual bool PalmProcessMessage(WXMSG* pMsg);

    // return true if the message was translated and shouldn't be dispatched
    virtual bool PalmTranslateMessage(WXMSG* pMsg);

    // called when the window is about to be destroyed
    virtual void PalmDestroyWindow();

    // this function should return the brush to paint the window background
    // with or 0 for the default brush
    virtual WXHBRUSH OnCtlColor(WXHDC hDC,
                                WXHWND hWnd,
                                WXUINT nCtlColor,
                                WXUINT message,
                                WXWPARAM wParam,
                                WXLPARAM lParam);

    // Responds to colour changes: passes event on to children.
    void OnSysColourChanged(wxSysColourChangedEvent& event);

    // initialize various fields of wxMouseEvent (common part of PalmOnMouseXXX)
    void InitMouseEvent(wxMouseEvent& event, int x, int y, WXUINT flags);

    // check if mouse is in the window
    bool IsMouseInWindow() const;

    // virtual function for implementing internal idle
    // behaviour
    virtual void OnInternalIdle() ;

protected:
    // the window handle
    WXHWND                m_hWnd;
    FormType *FrameForm;

    FormType *GetFormPtr();
    void SetFormPtr(FormType *FormPtr);

    // the old window proc (we subclass all windows)
    WXFARPROC             m_oldWndProc;

    // additional (Palm specific) flags
    bool                  m_mouseInWindow:1;
    bool                  m_lastKeydownProcessed:1;

    // the size of one page for scrolling
    int                   m_xThumbSize;
    int                   m_yThumbSize;

#if wxUSE_MOUSEEVENT_HACK
    // the coordinates of the last mouse event and the type of it
    long                  m_lastMouseX,
                          m_lastMouseY;
    int                   m_lastMouseEvent;
#endif // wxUSE_MOUSEEVENT_HACK

    WXHMENU               m_hMenu; // Menu, if any

    // implement the base class pure virtuals
    virtual void DoClientToScreen( int *x, int *y ) const;
    virtual void DoScreenToClient( int *x, int *y ) const;
    virtual void DoGetPosition( int *x, int *y ) const;
    virtual void DoGetSize( int *width, int *height ) const;
    virtual void DoGetClientSize( int *width, int *height ) const;
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);
    virtual void DoSetClientSize(int width, int height);

    virtual void DoCaptureMouse();
    virtual void DoReleaseMouse();

    // move the window to the specified location and resize it: this is called
    // from both DoSetSize() and DoSetClientSize() and would usually just call
    // ::MoveWindow() except for composite controls which will want to arrange
    // themselves inside the given rectangle
    virtual void DoMoveWindow(int x, int y, int width, int height);

#if wxUSE_TOOLTIPS
    virtual void DoSetToolTip( wxToolTip *tip );

    // process TTN_NEEDTEXT message properly (i.e. fixing the bugs in
    // comctl32.dll in our code -- see the function body for more info)
    bool HandleTooltipNotify(WXUINT code,
                             WXLPARAM lParam,
                             const wxString& ttip);
#endif // wxUSE_TOOLTIPS

    // the helper functions used by HandleChar/KeyXXX methods
    wxKeyEvent CreateKeyEvent(wxEventType evType, int id,
                              WXLPARAM lParam = 0, WXWPARAM wParam = 0) const;

private:
    // common part of all ctors
    void Init();

    // the (non-virtual) handlers for the events
    bool HandleMove(int x, int y);
    bool HandleMoving(wxRect& rect);
    bool HandleJoystickEvent(WXUINT msg, int x, int y, WXUINT flags);

#ifdef __WIN95__
    bool HandleNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result);
#endif // __WIN95__

    // list of disabled children before last call to our Disable()
    wxWindowList *m_childrenDisabled;

    // number of calls to Freeze() minus number of calls to Thaw()
    unsigned int m_frozenness;

    DECLARE_DYNAMIC_CLASS(wxWindowPalm)
    DECLARE_NO_COPY_CLASS(wxWindowPalm)
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// inline functions
// ----------------------------------------------------------------------------

#if WXWIN_COMPATIBILITY_2_4

inline bool wxWindowPalm::GetUseCtl3D() const { return false; }
inline bool wxWindowPalm::GetTransparentBackground() const { return false; }
inline void wxWindowPalm::SetTransparent(bool WXUNUSED(t)) { }

#endif // WXWIN_COMPATIBILITY_2_4

// ---------------------------------------------------------------------------
// global functions
// ---------------------------------------------------------------------------

// kbd code translation
WXDLLEXPORT int wxCharCodePalmToWX(int keySym, WXLPARAM lParam = 0);
WXDLLEXPORT int wxCharCodeWXToPalm(int id, bool *IsVirtual);

// window creation helper class: before creating a new HWND, instantiate an
// object of this class on stack - this allows to process the messages sent to
// the window even before CreateWindow() returns
class wxWindowCreationHook
{
public:
    wxWindowCreationHook(wxWindowPalm *winBeingCreated);
    ~wxWindowCreationHook();
};

// ----------------------------------------------------------------------------
// global objects
// ----------------------------------------------------------------------------

// notice that this hash must be defined after wxWindow declaration as it
// needs to "see" its dtor and not just forward declaration
#include "wx/hash.h"

// pseudo-template HWND <-> wxWindow hash table
#if WXWIN_COMPATIBILITY_2_4
WX_DECLARE_HASH(wxWindow, wxWindowList, wxWinHashTable);
#else
WX_DECLARE_HASH(wxWindowPalm, wxWindowList, wxWinHashTable);
#endif

extern wxWinHashTable *wxWinHandleHash;

#endif
    // _WX_WINDOW_H_
