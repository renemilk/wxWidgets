/////////////////////////////////////////////////////////////////////////////
// Name:        topluniv.cpp
// Author:      Vaclav Slavik
// Id:          $Id$
// Copyright:   (c) 2001 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "univtoplevel.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/defs.h"

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
    #include "wx/settings.h"
#endif

#include "wx/toplevel.h"
#include "wx/univ/renderer.h"
#include "wx/bitmap.h"
#include "wx/image.h"
#include "wx/cshelp.h"
#include "wx/evtloop.h"


// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxTopLevelWindow, wxTopLevelWindowNative)
    WX_EVENT_TABLE_INPUT_CONSUMER(wxTopLevelWindow)
    EVT_NC_PAINT(wxTopLevelWindow::OnNcPaint)
END_EVENT_TABLE()

WX_FORWARD_TO_INPUT_CONSUMER(wxTopLevelWindow)

// ============================================================================
// implementation
// ============================================================================

int wxTopLevelWindow::ms_drawDecorations = -1;
int wxTopLevelWindow::ms_canIconize = -1;

void wxTopLevelWindow::Init()
{
    m_isActive = FALSE;
    m_windowStyle = 0;
    m_pressedButton = 0;
}

bool wxTopLevelWindow::Create(wxWindow *parent,
                              wxWindowID id,
                              const wxString& title,
                              const wxPoint& pos,
                              const wxSize& size,
                              long style,
                              const wxString &name)
{
    // init them to avoid compiler warnings
    long styleOrig = 0,
         exstyleOrig = 0;

    if ( ms_drawDecorations == -1 )
    {
        ms_drawDecorations = 
            !wxSystemSettings::HasFeature(wxSYS_CAN_DRAW_FRAME_DECORATIONS) 
            || wxGetEnv(wxT("WXDECOR"), NULL);
        // FIXME -- wxUniv should provide a way to force non-native decorations!
        //          $WXDECOR is just a hack in absence of better wxUniv solution
    }

    if ( ms_canIconize == -1 )
    {
        ms_canIconize = wxSystemSettings::HasFeature(wxSYS_CAN_ICONIZE_FRAME);
    }

    if ( ms_drawDecorations )
    {
        CreateInputHandler(wxINP_HANDLER_TOPLEVEL);

        styleOrig = style;
        exstyleOrig = GetExtraStyle();
        style &= ~(wxCAPTION | wxMINIMIZE_BOX | wxMAXIMIZE_BOX |
                   wxSYSTEM_MENU | wxRESIZE_BORDER | wxFRAME_TOOL_WINDOW |
                   wxTHICK_FRAME);
        style = wxSIMPLE_BORDER;
        SetExtraStyle(exstyleOrig &
                      ~(wxFRAME_EX_CONTEXTHELP | wxDIALOG_EX_CONTEXTHELP));
    }

    if ( !wxTopLevelWindowNative::Create(parent, id, title, pos,
                                         size, style, name) )
        return FALSE;

    if ( ms_drawDecorations )
    {
        m_windowStyle = styleOrig;
        m_exStyle = exstyleOrig;
    }

    return TRUE;
}

bool wxTopLevelWindow::ShowFullScreen(bool show, long style)
{
    if ( show == IsFullScreen() ) return FALSE;

    if ( ms_drawDecorations )
    {
        if ( show )
        {
            m_fsSavedStyle = m_windowStyle;
            if ( style & wxFULLSCREEN_NOBORDER )
                m_windowStyle |= wxSIMPLE_BORDER;
            if ( style & wxFULLSCREEN_NOCAPTION )
                m_windowStyle &= ~wxCAPTION;
        }
        else
        {
            m_windowStyle = m_fsSavedStyle;
        }
    }

    return wxTopLevelWindowNative::ShowFullScreen(show, style);
}

long wxTopLevelWindow::GetDecorationsStyle() const
{
    long style = 0;

    if ( m_windowStyle & wxCAPTION )
    {
        style |= wxTOPLEVEL_TITLEBAR | wxTOPLEVEL_BUTTON_CLOSE;
        if ( (m_windowStyle & wxMINIMIZE_BOX) && ms_canIconize )
            style |= wxTOPLEVEL_BUTTON_ICONIZE;
        if ( m_windowStyle & wxMAXIMIZE_BOX )
        {
            if ( IsMaximized() )
                style |= wxTOPLEVEL_BUTTON_RESTORE;
            else
                style |= wxTOPLEVEL_BUTTON_MAXIMIZE;
        }
#if wxUSE_HELP
        if ( m_exStyle & (wxFRAME_EX_CONTEXTHELP | wxDIALOG_EX_CONTEXTHELP))
            style |= wxTOPLEVEL_BUTTON_HELP;
#endif
    }
    if ( (m_windowStyle & (wxSIMPLE_BORDER | wxNO_BORDER)) == 0 )
        style |= wxTOPLEVEL_BORDER;
    if ( m_windowStyle & (wxRESIZE_BORDER | wxTHICK_FRAME) )
        style |= wxTOPLEVEL_RESIZEABLE;

    if ( IsMaximized() )
        style |= wxTOPLEVEL_MAXIMIZED;
    if ( GetIcon().Ok() )
        style |= wxTOPLEVEL_ICON;
    if ( m_isActive )
        style |= wxTOPLEVEL_ACTIVE;

    return style;
}

void wxTopLevelWindow::RefreshTitleBar()
{
    wxNcPaintEvent event(GetId());
    event.SetEventObject(this);
    GetEventHandler()->ProcessEvent(event);
}

// ----------------------------------------------------------------------------
// client area handling
// ----------------------------------------------------------------------------

wxPoint wxTopLevelWindow::GetClientAreaOrigin() const
{
    if ( ms_drawDecorations )
    {
        int w, h;
        wxTopLevelWindowNative::DoGetClientSize(&w, &h);
        wxRect rect = wxRect(wxTopLevelWindowNative::GetClientAreaOrigin(),
                             wxSize(w, h));
        rect = m_renderer->GetFrameClientArea(rect,
                                              GetDecorationsStyle());
        return rect.GetPosition();
    }
    else
    {
        return wxTopLevelWindowNative::GetClientAreaOrigin();
    }
}

void wxTopLevelWindow::DoGetClientSize(int *width, int *height) const
{
    if ( ms_drawDecorations )
    {
        int w, h;
        wxTopLevelWindowNative::DoGetClientSize(&w, &h);
        wxRect rect = wxRect(wxTopLevelWindowNative::GetClientAreaOrigin(),
                             wxSize(w, h));
        rect = m_renderer->GetFrameClientArea(rect,
                                              GetDecorationsStyle());
        if ( width )
            *width = rect.width;
        if ( height )
            *height = rect.height;
    }
    else
        wxTopLevelWindowNative::DoGetClientSize(width, height);
}

void wxTopLevelWindow::DoSetClientSize(int width, int height)
{
    if ( ms_drawDecorations )
    {
        wxSize size = m_renderer->GetFrameTotalSize(wxSize(width, height),
                                               GetDecorationsStyle());
        wxTopLevelWindowNative::DoSetClientSize(size.x, size.y);
    }
    else
        wxTopLevelWindowNative::DoSetClientSize(width, height);
}

void wxTopLevelWindow::OnNcPaint(wxPaintEvent& event)
{
    if ( !ms_drawDecorations || !m_renderer )
        event.Skip();
    else
    {
        // get the window rect
        wxRect rect;
        wxSize size = GetSize();
        rect.x =
        rect.y = 0;
        rect.width = size.x;
        rect.height = size.y;

        wxWindowDC dc(this);
        m_renderer->DrawFrameTitleBar(dc, rect,
                                      GetTitle(), m_titlebarIcon,
                                      GetDecorationsStyle(),
                                      m_pressedButton,
                                      wxCONTROL_PRESSED);
    }
}

long wxTopLevelWindow::HitTest(const wxPoint& pt) const
{
    int w, h;
    wxTopLevelWindowNative::DoGetClientSize(&w, &h);
    wxRect rect(wxTopLevelWindowNative::GetClientAreaOrigin(), wxSize(w, h));

    return m_renderer->HitTestFrame(rect, pt+GetClientAreaOrigin(), GetDecorationsStyle());
}

int wxTopLevelWindow::GetMinWidth() const
{
    if ( ms_drawDecorations )
    {
        return wxMax(wxTopLevelWindowNative::GetMinWidth(),
                     m_renderer->GetFrameMinSize(GetDecorationsStyle()).x);
    }
    else
        return wxTopLevelWindowNative::GetMinWidth();
}

int wxTopLevelWindow::GetMinHeight() const
{
    if ( ms_drawDecorations )
    {
        return wxMax(wxTopLevelWindowNative::GetMinHeight(),
                     m_renderer->GetFrameMinSize(GetDecorationsStyle()).y);
    }
    else
        return wxTopLevelWindowNative::GetMinHeight();
}

// ----------------------------------------------------------------------------
// icons
// ----------------------------------------------------------------------------

void wxTopLevelWindow::SetIcon(const wxIcon& icon)
{
    wxTopLevelWindowNative::SetIcon(icon);

    if ( ms_drawDecorations && m_renderer )
    {
        wxSize size = m_renderer->GetFrameIconSize();

        if ( !icon.Ok() || size.x == -1  )
            m_titlebarIcon = icon;
        else
        {
            wxBitmap bmp1;
            bmp1.CopyFromIcon(icon);
            if ( !bmp1.Ok() )
                m_titlebarIcon = wxNullIcon;
            else if ( bmp1.GetWidth() == size.x && bmp1.GetHeight() == size.y )
                m_titlebarIcon = icon;
            else
            {
                wxImage img = bmp1.ConvertToImage();
                img.Rescale(size.x, size.y);
                m_titlebarIcon.CopyFromBitmap(wxBitmap(img));
            }
        }
    }
}

// ----------------------------------------------------------------------------
// interactive manipulation
// ----------------------------------------------------------------------------

#define wxINTERACTIVE_RESIZE_DIR \
          (wxINTERACTIVE_RESIZE_W | wxINTERACTIVE_RESIZE_E | \
           wxINTERACTIVE_RESIZE_S | wxINTERACTIVE_RESIZE_N)

struct wxInteractiveMoveData
{
    wxTopLevelWindowBase *m_window;
    wxEventLoop          *m_evtLoop;
    int                   m_flags;
    wxRect                m_rect;
    wxRect                m_rectOrig;
    wxPoint               m_pos;
    wxSize                m_minSize, m_maxSize;
};

class wxInteractiveMoveHandler : public wxEvtHandler
{
public:
    wxInteractiveMoveHandler(wxInteractiveMoveData& data) : m_data(data) {}
    
private:
    DECLARE_EVENT_TABLE()
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    void OnKeyDown(wxKeyEvent& event);

    wxInteractiveMoveData& m_data;
};

BEGIN_EVENT_TABLE(wxInteractiveMoveHandler, wxEvtHandler)
    EVT_MOTION(wxInteractiveMoveHandler::OnMouseMove)
    EVT_LEFT_DOWN(wxInteractiveMoveHandler::OnMouseDown)
    EVT_LEFT_UP(wxInteractiveMoveHandler::OnMouseUp)
    EVT_KEY_DOWN(wxInteractiveMoveHandler::OnKeyDown)
END_EVENT_TABLE()


static inline LINKAGEMODE 
void wxApplyResize(wxInteractiveMoveData& data, const wxPoint& diff)
{
    if ( data.m_flags & wxINTERACTIVE_RESIZE_W )
    {
        data.m_rect.x += diff.x;
        data.m_rect.width -= diff.x;
    }
    else if ( data.m_flags & wxINTERACTIVE_RESIZE_E )
    {
        data.m_rect.width += diff.x;
    }
    if ( data.m_flags & wxINTERACTIVE_RESIZE_N )
    {
        data.m_rect.y += diff.y;
        data.m_rect.height -= diff.y;
    }
    else if ( data.m_flags & wxINTERACTIVE_RESIZE_S )
    {
        data.m_rect.height += diff.y;
    }
    
    if ( data.m_minSize.x != -1 && data.m_rect.width < data.m_minSize.x )
    {
        if ( data.m_flags & wxINTERACTIVE_RESIZE_W )
            data.m_rect.x -= data.m_minSize.x - data.m_rect.width;
        data.m_rect.width = data.m_minSize.x;
    }
    if ( data.m_maxSize.x != -1 && data.m_rect.width > data.m_maxSize.x )
    {
        if ( data.m_flags & wxINTERACTIVE_RESIZE_W )
            data.m_rect.x -= data.m_minSize.x - data.m_rect.width;
        data.m_rect.width = data.m_maxSize.x;
    }
    if ( data.m_minSize.y != -1 && data.m_rect.height < data.m_minSize.y )
    {
        if ( data.m_flags & wxINTERACTIVE_RESIZE_N )
            data.m_rect.y -= data.m_minSize.y - data.m_rect.height;
        data.m_rect.height = data.m_minSize.y;
    }
    if ( data.m_maxSize.y != -1 && data.m_rect.height > data.m_maxSize.y )
    {
        if ( data.m_flags & wxINTERACTIVE_RESIZE_N )
            data.m_rect.y -= data.m_minSize.y - data.m_rect.height;
        data.m_rect.height = data.m_maxSize.y;
    }
}

void wxInteractiveMoveHandler::OnMouseMove(wxMouseEvent& event)
{
    if ( m_data.m_flags & wxINTERACTIVE_WAIT_FOR_INPUT )
        event.Skip();

    else if ( m_data.m_flags & wxINTERACTIVE_MOVE )
    {
        wxPoint diff = wxGetMousePosition() - m_data.m_pos;
        m_data.m_rect = m_data.m_rectOrig;
        m_data.m_rect.Offset(diff);
        m_data.m_window->Move(m_data.m_rect.GetPosition());
    }

    else if ( m_data.m_flags & wxINTERACTIVE_RESIZE )
    {
        wxPoint diff = wxGetMousePosition() - m_data.m_pos;
        m_data.m_rect = m_data.m_rectOrig;
        wxApplyResize(m_data, diff);
        m_data.m_window->SetSize(m_data.m_rect);
    }
}

void wxInteractiveMoveHandler::OnMouseDown(wxMouseEvent& event)
{
    if ( m_data.m_flags & wxINTERACTIVE_WAIT_FOR_INPUT )
    {
        m_data.m_flags &= ~wxINTERACTIVE_WAIT_FOR_INPUT;
        m_data.m_pos = wxGetMousePosition();
    }
}

void wxInteractiveMoveHandler::OnKeyDown(wxKeyEvent& event)
{
    if ( m_data.m_flags & wxINTERACTIVE_WAIT_FOR_INPUT )
    {
        m_data.m_flags &= ~wxINTERACTIVE_WAIT_FOR_INPUT;
        m_data.m_pos = wxGetMousePosition();
    }
    
    wxPoint diff(-1,-1);
    
    switch ( event.GetKeyCode() )
    {
        case WXK_UP:    diff = wxPoint(0, -16); break;
        case WXK_DOWN:  diff = wxPoint(0, 16);  break;
        case WXK_LEFT:  diff = wxPoint(-16, 0); break;
        case WXK_RIGHT: diff = wxPoint(16, 0);  break;
        case WXK_ESCAPE:
            m_data.m_window->SetSize(m_data.m_rectOrig);
            m_data.m_evtLoop->Exit();
            return;
        case WXK_RETURN:
            m_data.m_evtLoop->Exit();
            return;
    }
    
    if ( diff.x != -1 )
    {
        if ( m_data.m_flags & wxINTERACTIVE_MOVE )
        {
            m_data.m_rect.Offset(diff);
            m_data.m_window->Move(m_data.m_rect.GetPosition());
        }
        else /* wxINTERACTIVE_RESIZE */
        {
            if ( !(m_data.m_flags & wxINTERACTIVE_RESIZE_DIR) )
            {
                if ( diff.y < 0 )
                    m_data.m_flags |= wxINTERACTIVE_RESIZE_N;
                else if ( diff.y > 0 )
                    m_data.m_flags |= wxINTERACTIVE_RESIZE_S;
                if ( diff.x < 0 )
                    m_data.m_flags |= wxINTERACTIVE_RESIZE_W;
                else if ( diff.x > 0 )
                    m_data.m_flags |= wxINTERACTIVE_RESIZE_E;
            }

            wxApplyResize(m_data, diff);
            m_data.m_window->SetSize(m_data.m_rect);
        }
    }
}

void wxInteractiveMoveHandler::OnMouseUp(wxMouseEvent& event)
{
    m_data.m_evtLoop->Exit();
}


void wxTopLevelWindow::InteractiveMove(int flags)
{
    wxASSERT_MSG( !((flags & wxINTERACTIVE_MOVE) && (flags & wxINTERACTIVE_RESIZE)),
                  wxT("can't move and resize window at the same time") );

    wxASSERT_MSG( !(flags & wxINTERACTIVE_RESIZE) || 
                  (flags & wxINTERACTIVE_WAIT_FOR_INPUT) || 
                  (flags & wxINTERACTIVE_RESIZE_DIR),
                  wxT("direction of resizing not specified") );

    wxInteractiveMoveData data;
    wxEventLoop loop;
    wxWindow *focus = FindFocus();
    
    // FIXME - display resize cursor if waiting for initial input

    data.m_window = this;
    data.m_evtLoop = &loop;
    data.m_flags = flags;
    data.m_rect = data.m_rectOrig = GetRect();
    data.m_pos = wxGetMousePosition();
    data.m_minSize = wxSize(GetMinWidth(), GetMinHeight());
    data.m_maxSize = wxSize(GetMaxWidth(), GetMaxHeight());

    this->PushEventHandler(new wxInteractiveMoveHandler(data));
    if ( focus )
        focus->PushEventHandler(new wxInteractiveMoveHandler(data));

    CaptureMouse();
    loop.Run();
    ReleaseMouse();

    this->PopEventHandler(TRUE/*delete*/);
    if ( focus )
        focus->PopEventHandler(TRUE/*delete*/);
}

// ----------------------------------------------------------------------------
// actions
// ----------------------------------------------------------------------------

void wxTopLevelWindow::ClickTitleBarButton(long button)
{
    switch ( button )
    {
        case wxTOPLEVEL_BUTTON_CLOSE:
            Close();
            break;

        case wxTOPLEVEL_BUTTON_ICONIZE:
            Iconize();
            break;

        case wxTOPLEVEL_BUTTON_MAXIMIZE:
            Maximize();
            break;

        case wxTOPLEVEL_BUTTON_RESTORE:
            Restore();
            break;

        case wxTOPLEVEL_BUTTON_HELP:
#if wxUSE_HELP
            {
            wxContextHelp contextHelp(this);
            }
#endif
            break;

        default:
            wxFAIL_MSG(wxT("incorrect button specification"));
    }
}

bool wxTopLevelWindow::PerformAction(const wxControlAction& action,
                                     long numArg,
                                     const wxString& strArg)
{
    bool isActive = numArg != 0;

    if ( action == wxACTION_TOPLEVEL_ACTIVATE )
    {
        if ( m_isActive != isActive )
        {
            m_isActive = isActive;
            RefreshTitleBar();
        }
        return TRUE;
    }

    else if ( action == wxACTION_TOPLEVEL_BUTTON_PRESS )
    {
        m_pressedButton = numArg;
        RefreshTitleBar();
        return TRUE;
    }

    else if ( action == wxACTION_TOPLEVEL_BUTTON_RELEASE )
    {
        m_pressedButton = 0;
        RefreshTitleBar();
        return TRUE;
    }

    else if ( action == wxACTION_TOPLEVEL_BUTTON_CLICK )
    {
        m_pressedButton = 0;
        RefreshTitleBar();
        ClickTitleBarButton(numArg);
        return TRUE;
    }

    else if ( action == wxACTION_TOPLEVEL_MOVE )
    {
        InteractiveMove(wxINTERACTIVE_MOVE);
        return TRUE;
    }

    else if ( action == wxACTION_TOPLEVEL_RESIZE )
    {
        int flags = wxINTERACTIVE_RESIZE;
        if ( numArg & wxHT_TOPLEVEL_BORDER_N )
            flags |= wxINTERACTIVE_RESIZE_N;
        if ( numArg & wxHT_TOPLEVEL_BORDER_S )
            flags |= wxINTERACTIVE_RESIZE_S;
        if ( numArg & wxHT_TOPLEVEL_BORDER_W )
            flags |= wxINTERACTIVE_RESIZE_W;
        if ( numArg & wxHT_TOPLEVEL_BORDER_E )
            flags |= wxINTERACTIVE_RESIZE_E;
        InteractiveMove(flags);
        return TRUE;
    }

    else
        return FALSE;
}


// ============================================================================
// wxStdFrameInputHandler: handles focus, resizing and titlebar buttons clicks
// ============================================================================

wxStdFrameInputHandler::wxStdFrameInputHandler(wxInputHandler *inphand)
            : wxStdInputHandler(inphand)
{
    m_winCapture = NULL;
    m_winHitTest = 0;
    m_winPressed = 0;
    m_borderCursorOn = FALSE;
}

bool wxStdFrameInputHandler::HandleMouse(wxInputConsumer *consumer,
                                         const wxMouseEvent& event)
{
    // the button has 2 states: pressed and normal with the following
    // transitions between them:
    //
    //      normal -> left down -> capture mouse and go to pressed state
    //      pressed -> left up inside -> generate click -> go to normal
    //                         outside ------------------>
    //
    // the other mouse buttons are ignored
    if ( event.Button(1) )
    {
        if ( event.ButtonDown(1) )
        {
            wxTopLevelWindow *w = wxStaticCast(consumer->GetInputWindow(), wxTopLevelWindow);
            long hit = w->HitTest(event.GetPosition());

            if ( hit & wxHT_TOPLEVEL_ANY_BUTTON )
            {
                m_winCapture = w;
                m_winCapture->CaptureMouse();
                m_winHitTest = hit;
                m_winPressed = hit;
                consumer->PerformAction(wxACTION_TOPLEVEL_BUTTON_PRESS, m_winPressed);
                return TRUE;
            }
            else if ( hit & wxHT_TOPLEVEL_TITLEBAR )
            {
                consumer->PerformAction(wxACTION_TOPLEVEL_MOVE);
                return TRUE;
            }
            else if ( (consumer->GetInputWindow()->GetWindowStyle() & wxRESIZE_BORDER)
                      && (hit & wxHT_TOPLEVEL_ANY_BORDER) )
            {
                consumer->PerformAction(wxACTION_TOPLEVEL_RESIZE, hit);
                return TRUE;
            }
        }

        else // up
        {
            if ( m_winCapture )
            {
                m_winCapture->ReleaseMouse();
                m_winCapture = NULL;

                if ( m_winHitTest == m_winPressed )
                {
                    consumer->PerformAction(wxACTION_TOPLEVEL_BUTTON_CLICK, m_winPressed);
                    return TRUE;
                }
            }
            //else: the mouse was released outside the window, this doesn't
            //      count as a click
        }
    }

    return wxStdInputHandler::HandleMouse(consumer, event);
}

bool wxStdFrameInputHandler::HandleMouseMove(wxInputConsumer *consumer,
                                             const wxMouseEvent& event)
{
    if ( event.GetEventObject() == m_winCapture )
    {
        long hit = m_winCapture->HitTest(event.GetPosition());

        if ( hit != m_winHitTest )
        {
            if ( hit != m_winPressed )
                consumer->PerformAction(wxACTION_TOPLEVEL_BUTTON_RELEASE, m_winPressed);
            else
                consumer->PerformAction(wxACTION_TOPLEVEL_BUTTON_PRESS, m_winPressed);

            m_winHitTest = hit;
            return TRUE;
        }
    }
    else if ( consumer->GetInputWindow()->GetWindowStyle() & wxRESIZE_BORDER )
    {
        wxTopLevelWindow *win = wxStaticCast(consumer->GetInputWindow(),
                                             wxTopLevelWindow);
        long hit = win->HitTest(event.GetPosition());

        if ( hit != m_winHitTest )
        {
            m_winHitTest = hit;

            if ( m_borderCursorOn )
            {
                m_borderCursorOn = FALSE;
                win->SetCursor(m_origCursor);
            }

            if ( hit & wxHT_TOPLEVEL_ANY_BORDER )
            {
                m_borderCursorOn = TRUE;
                wxCursor cur;

                switch (hit)
                {
                    case wxHT_TOPLEVEL_BORDER_N:
                    case wxHT_TOPLEVEL_BORDER_S:
                        cur = wxCursor(wxCURSOR_SIZENS);
                        break;
                    case wxHT_TOPLEVEL_BORDER_W:
                    case wxHT_TOPLEVEL_BORDER_E:
                        cur = wxCursor(wxCURSOR_SIZEWE);
                        break;
                    case wxHT_TOPLEVEL_BORDER_NE:
                    case wxHT_TOPLEVEL_BORDER_SW:
                        cur = wxCursor(wxCURSOR_SIZENESW);
                        break;
                    case wxHT_TOPLEVEL_BORDER_NW:
                    case wxHT_TOPLEVEL_BORDER_SE:
                        cur = wxCursor(wxCURSOR_SIZENWSE);
                        break;
                    default:
                        m_borderCursorOn = FALSE;
                        break;
                }
                if ( m_borderCursorOn )
                {
                    m_origCursor = win->GetCursor();
                    win->SetCursor(cur);
                }
            }
        }
    }

    return wxStdInputHandler::HandleMouseMove(consumer, event);
}

bool wxStdFrameInputHandler::HandleActivation(wxInputConsumer *consumer,
                                              bool activated)
{
    if ( m_borderCursorOn )
    {
        consumer->GetInputWindow()->SetCursor(m_origCursor);
        m_borderCursorOn = FALSE;
    }
    consumer->PerformAction(wxACTION_TOPLEVEL_ACTIVATE, activated);
    return FALSE;
}
