/////////////////////////////////////////////////////////////////////////////
// Name:        msw/tbar95.cpp
// Purpose:     wxToolBar
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "tbar95.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/intl.h"
    #include "wx/dynarray.h"
    #include "wx/settings.h"
    #include "wx/bitmap.h"
#endif

#if wxUSE_BUTTONBAR && wxUSE_TOOLBAR && defined(__WIN95__)

#include "wx/toolbar.h"

#if !defined(__GNUWIN32__) && !defined(__SALFORDC__)
    #include "malloc.h"
#endif

#include "wx/msw/private.h"

#ifndef __TWIN32__

#ifdef __GNUWIN32_OLD__
    #include "wx/msw/gnuwin32/extra.h"
#else
    #include <commctrl.h>
#endif

#endif // __TWIN32__

#include "wx/msw/dib.h"
#include "wx/app.h"         // for GetComCtl32Version

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// these standard constants are not always defined in compilers headers

// Styles
#ifndef TBSTYLE_FLAT
    #define TBSTYLE_LIST            0x1000
    #define TBSTYLE_FLAT            0x0800
    #define TBSTYLE_TRANSPARENT     0x8000
#endif
 // use TBSTYLE_TRANSPARENT if you use TBSTYLE_FLAT

// Messages
#ifndef TB_GETSTYLE
    #define TB_SETSTYLE             (WM_USER + 56)
    #define TB_GETSTYLE             (WM_USER + 57)
#endif

#ifndef TB_HITTEST
    #define TB_HITTEST              (WM_USER + 0xff)    // FIXME
#endif

// these values correspond to those used by comctl32.dll
#define DEFAULTBITMAPX   16
#define DEFAULTBITMAPY   15
#define DEFAULTBUTTONX   24
#define DEFAULTBUTTONY   24
#define DEFAULTBARHEIGHT 27

// ----------------------------------------------------------------------------
// function prototypes
// ----------------------------------------------------------------------------

static void wxMapBitmap(HBITMAP hBitmap, int width, int height);

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

#if !USE_SHARED_LIBRARY
    IMPLEMENT_DYNAMIC_CLASS(wxToolBar, wxControl)
#endif

BEGIN_EVENT_TABLE(wxToolBar, wxToolBarBase)
    EVT_MOUSE_EVENTS(wxToolBar::OnMouseEvent)
    EVT_SYS_COLOUR_CHANGED(wxToolBar::OnSysColourChanged)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// for us, wxToolBarTool is just the base class - we don't need any extra
// functionality
class wxToolBarTool : public wxToolBarToolBase
{
public:
    wxToolBarTool(wxToolBar *tbar,
                  int id,
                  const wxBitmap& bitmap1,
                  const wxBitmap& bitmap2,
                  bool toggle,
                  wxObject *clientData,
                  const wxString& shortHelpString,
                  const wxString& longHelpString)
        : wxToolBarToolBase(tbar, id, bitmap1, bitmap2, toggle,
                            clientData, shortHelpString, longHelpString)
    {
    }

    wxToolBarTool(wxToolBar *tbar, wxControl *control)
        : wxToolBarToolBase(tbar, control)
    {
    }
};


// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxToolBarTool
// ----------------------------------------------------------------------------

wxToolBarToolBase *wxToolBarToolBase::New(wxToolBar *tbar,
                                          int id,
                                          const wxBitmap& bitmap1,
                                          const wxBitmap& bitmap2,
                                          bool toggle,
                                          wxObject *clientData,
                                          const wxString& shortHelpString,
                                          const wxString& longHelpString)
{
    return new wxToolBarTool(tbar, id, bitmap1, bitmap2, toggle,
                             clientData, shortHelpString, longHelpString);
}

wxToolBarToolBase *wxToolBarToolBase::New(wxToolBar *tbar, wxControl *control)
{
    return new wxToolBarTool(tbar, control);
}

// ----------------------------------------------------------------------------
// wxToolBar construction
// ----------------------------------------------------------------------------

void wxToolBar::Init()
{
    m_hBitmap = 0;
    m_defaultWidth = DEFAULTBITMAPX;
    m_defaultHeight = DEFAULTBITMAPY;
}

bool wxToolBar::Create(wxWindow *parent,
                         wxWindowID id,
                         const wxPoint& pos,
                         const wxSize& size,
                         long style,
                         const wxString& name)
{
    wxASSERT_MSG( (style & wxTB_VERTICAL) == 0,
                  wxT("Sorry, wxToolBar under Windows 95 only "
                      "supports horizontal orientation.") );

    // common initialisation
    if ( !CreateControl(parent, id, pos, size, style, name) )
        return FALSE;

    // prepare flags
    DWORD msflags = 0;      // WS_VISIBLE | WS_CHILD always included
    if (style & wxBORDER)
        msflags |= WS_BORDER;
    msflags |= TBSTYLE_TOOLTIPS;

    if (style & wxTB_FLAT)
    {
        if (wxTheApp->GetComCtl32Version() > 400)
            msflags |= TBSTYLE_FLAT;
    }

    // MSW-specific initialisation
    if ( !wxControl::MSWCreateControl(TOOLBARCLASSNAME, msflags) )
        return FALSE;

    // toolbar-specific post initialisation
    ::SendMessage(GetHwnd(), TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);

    // set up the colors and fonts
    wxRGBToColour(m_backgroundColour, GetSysColor(COLOR_BTNFACE));
    m_foregroundColour = *wxBLACK;

    SetFont(wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT));

    // position it
    int x = pos.x;
    int y = pos.y;
    int width = size.x;
    int height = size.y;

    if (width <= 0)
        width = 100;
    if (height <= 0)
        height = m_defaultHeight;
    if (x < 0)
        x = 0;
    if (y < 0)
        y = 0;

    SetSize(x, y, width, height);

    return TRUE;
}

wxToolBar::~wxToolBar()
{
    UnsubclassWin();

    if (m_hBitmap)
    {
        ::DeleteObject((HBITMAP) m_hBitmap);
    }
}

// ----------------------------------------------------------------------------
// adding/removing tools
// ----------------------------------------------------------------------------

bool wxToolBar::DoInsertTool(size_t WXUNUSED(pos),
                             wxToolBarTool *tool)
{
    // nothing special to do here - we really create the toolbar buttons in
    // Realize() later
    tool->Attach(this);

    return TRUE;
}

bool wxToolBar::DoDeleteTool(size_t pos, wxToolBarTool *tool)
{
    if ( !::SendMessage(GetHwnd(), TB_DELETEBUTTON, pos, 0) )
    {
        wxLogLastError("TB_DELETEBUTTON");

        return FALSE;
    }

    tool->Detach();

    return TRUE;
}

bool wxToolBar::Realize()
{
    size_t nTools = GetToolsCount();
    if ( nTools == 0 )
    {
        // nothing to do
        return TRUE;
    }

    // First, add the bitmap: we use one bitmap for all toolbar buttons
    // ----------------------------------------------------------------

    // if we already have a bitmap, we'll replace the existing one - otherwise
    // we'll install a new one
    HBITMAP oldToolBarBitmap = (HBITMAP)m_hBitmap;

    int totalBitmapWidth = (int)(m_defaultWidth * nTools);
    int totalBitmapHeight = (int)m_defaultHeight;

    // Create a bitmap for all the tool bitmaps
    HBITMAP hBitmap = ::CreateCompatibleBitmap(ScreenHDC(),
                                               totalBitmapWidth,
                                               totalBitmapHeight);
    if ( !m_hBitmap )
    {
        wxLogLastError(_T("CreateCompatibleBitmap"));

        return FALSE;
    }

    m_hBitmap = (WXHBITMAP)hBitmap;

    // Now blit all the tools onto this bitmap
    HDC memoryDC = ::CreateCompatibleDC(NULL);
    HBITMAP oldBitmap = (HBITMAP) ::SelectObject(memoryDC, hBitmap);

    HDC memoryDC2 = ::CreateCompatibleDC(NULL);

    // the button position
    wxCoord x = 0;

    // the number of buttons (not separators)
    int noButtons = 0;

    wxToolBarToolsList::Node *node = m_tools.GetFirst();
    while ( node )
    {
        wxToolBarToolBase *tool = node->GetData();
        if ( tool->IsButton() )
        {
            HBITMAP hbmp = GetHbitmapOf(tool->GetBitmap1());
            if ( hbmp )
            {
                HBITMAP oldBitmap2 = (HBITMAP)::SelectObject(memoryDC2, hbmp);
                if ( !BitBlt(memoryDC, x, 0,  m_defaultWidth, m_defaultHeight,
                             memoryDC2, 0, 0, SRCCOPY) )
                {
                    wxLogLastError("BitBlt");
                }

                ::SelectObject(memoryDC2, oldBitmap2);

                x += m_defaultWidth;
                noButtons++;
            }
            else
            {
                wxFAIL_MSG( _T("invalid tool button bitmap") );
            }
        }

        node = node->GetNext();
    }

    ::SelectObject(memoryDC, oldBitmap);
    ::DeleteDC(memoryDC);
    ::DeleteDC(memoryDC2);

    // Map to system colours
    wxMapBitmap(hBitmap, totalBitmapWidth, totalBitmapHeight);

    if ( oldToolBarBitmap )
    {
        TBREPLACEBITMAP replaceBitmap;
        replaceBitmap.hInstOld = NULL;
        replaceBitmap.hInstNew = NULL;
        replaceBitmap.nIDOld = (UINT) oldToolBarBitmap;
        replaceBitmap.nIDNew = (UINT) hBitmap;
        replaceBitmap.nButtons = noButtons;
        if ( ::SendMessage(GetHwnd(), TB_REPLACEBITMAP,
                           0, (LPARAM) &replaceBitmap) == -1 )
        {
            wxFAIL_MSG(wxT("Could not add bitmap to toolbar"));
        }

        ::DeleteObject(oldToolBarBitmap);

        // Now delete all the buttons
        int i = 0;
        while ( TRUE )
        {
            // TODO: What about separators???? They don't have an id!
            if ( ! ::SendMessage( GetHwnd(), TB_DELETEBUTTON, i, 0 ) )
                break;
        }
    }
    else // no old bitmap
    {
        TBADDBITMAP addBitmap;
        addBitmap.hInst = 0;
        addBitmap.nID = (UINT) hBitmap;
        if ( ::SendMessage(GetHwnd(), TB_ADDBITMAP,
                           (WPARAM) noButtons, (LPARAM)&addBitmap) == -1 )
        {
            wxFAIL_MSG(wxT("Could not add bitmap to toolbar"));
        }
    }

    // Next add the buttons and separators
    // -----------------------------------

    TBBUTTON *buttons = new TBBUTTON[nTools];

    // this array will hold the indices of all controls in the toolbar
    wxArrayInt controlIds;

    int i = 0;
    int bitmapId = 0;

    node = m_tools.GetFirst();
    while ( node )
    {
        wxToolBarToolBase *tool = node->GetData();
        TBBUTTON& button = buttons[i];

        wxZeroMemory(button);

        switch ( tool->GetStyle() )
        {
            case wxTOOL_STYLE_CONTROL:
                // fall through: create just a separator too

            case wxTOOL_STYLE_SEPARATOR:
                button.fsState = TBSTATE_ENABLED;
                button.fsStyle = TBSTYLE_SEP;
                break;

            case wxTOOL_STYLE_BUTTON:
                button.iBitmap = bitmapId;
                button.idCommand = tool->GetId();

                if ( tool->IsEnabled() )
                    button.fsState |= TBSTATE_ENABLED;
                if ( tool->IsToggled() )
                    button.fsState |= TBSTATE_CHECKED;

                button.fsStyle = tool->CanBeToggled() ? TBSTYLE_CHECK
                                                      : TBSTYLE_BUTTON;

                bitmapId++;
                break;
        }

        i++;

        node = node->GetNext();
    }

    if ( !::SendMessage(GetHwnd(), TB_ADDBUTTONS,
                        (WPARAM)i, (LPARAM)buttons) )
    {
        wxLogLastError("TB_ADDBUTTONS");
    }

    delete [] buttons;

    // Deal with the controls finally
    // ------------------------------

    // adjust the controls size to fit nicely in the toolbar
    size_t index = 0;
    for ( node = m_tools.GetFirst(); node; node = node->GetNext(), index++ )
    {
        wxToolBarToolBase *tool = node->GetData();
        if ( !tool->IsControl() )
            continue;

        wxControl *control = tool->GetControl();

        wxSize size = control->GetSize();

        // the position of the leftmost controls corner
        int left = -1;

        // TB_SETBUTTONINFO message is only supported by comctl32.dll 4.71+
        #if defined(_WIN32_IE) && (_WIN32_IE >= 0x400 )
            // available in headers, now check whether it is available now
            // (during run-time)
            if ( wxTheApp->GetComCtl32Version() >= 471 )
            {
                // set the (underlying) separators width to be that of the
                // control
                TBBUTTONINFO tbbi;
                tbbi.cbSize = sizeof(tbbi);
                tbbi.dwMask = TBIF_SIZE;
                tbbi.cx = size.x;
                if ( !SendMessage(GetHwnd(), TB_SETBUTTONINFO,
                                  tool->GetId(), (LPARAM)&tbbi) )
                {
                    // the id is probably invalid?
                    wxLogLastError("TB_SETBUTTONINFO");
                }

            }
            else
        #endif // comctl32.dll 4.71
            // TB_SETBUTTONINFO unavailable
            {
                // try adding several separators to fit the controls width
                RECT r;
                if ( !SendMessage(GetHwnd(), TB_GETITEMRECT,
                                  tool->GetId(), (LPARAM)(LPRECT)&r) )
                {
                    wxLogLastError("TB_GETITEMRECT");
                }

                int widthSep = r.right - r.left;
                left = r.left;

                TBBUTTON tbb;
                wxZeroMemory(tbb);
                tbb.idCommand = 0;
                tbb.fsState = TBSTATE_ENABLED;
                tbb.fsStyle = TBSTYLE_SEP;

                size_t nSeparators = size.x / widthSep;
                for ( size_t nSep = 0; nSep < nSeparators; nSep++ )
                {
                    if ( !SendMessage(GetHwnd(), TB_INSERTBUTTON,
                                      index, (LPARAM)&tbb) )
                    {
                        wxLogLastError("TB_INSERTBUTTON");
                    }
                }

                // adjust the controls width to exactly cover the separators
                control->SetSize((nSeparators + 1)*widthSep, -1);
            }

        // and position the control itself correctly vertically
        RECT r;
        if ( !SendMessage(GetHwnd(), TB_GETITEMRECT,
                          tool->GetId(), (LPARAM)(LPRECT)&r) )
        {
            wxLogLastError("TB_GETRECT");
        }

        int height = r.bottom - r.top;
        int diff = height - size.y;
        if ( diff < 0 )
        {
            // the control is too high, resize to fit
            control->SetSize(-1, height - 2);

            diff = 2;
        }

        control->Move(left == -1 ? r.left : left, r.top + diff / 2);
    }

    (void)::SendMessage(GetHwnd(), TB_AUTOSIZE, (WPARAM)0, (LPARAM) 0);

    SetRows(m_maxRows);

    return TRUE;
}

// ----------------------------------------------------------------------------
// message handlers
// ----------------------------------------------------------------------------

bool wxToolBar::MSWCommand(WXUINT cmd, WXWORD id)
{
    wxToolBarToolBase *tool = FindById((int)id);
    if ( !tool )
        return FALSE;

    if ( tool->CanBeToggled() )
    {
        LRESULT state = ::SendMessage(GetHwnd(), TB_GETSTATE, id, 0);
        tool->SetToggle((state & TBSTATE_CHECKED) != 0);
    }

    bool toggled = tool->IsToggled();

    // OnLeftClick() can veto the button state change - for buttons which may
    // be toggled only, of couse
    if ( !OnLeftClick((int)id, toggled) && tool->CanBeToggled() )
    {
        // revert back
        toggled = !toggled;
        tool->SetToggle(toggled);

        ::SendMessage(GetHwnd(), TB_CHECKBUTTON, id, MAKELONG(toggled, 0));
    }

    return TRUE;
}

bool wxToolBar::MSWOnNotify(int WXUNUSED(idCtrl),
                            WXLPARAM lParam,
                            WXLPARAM *result)
{
    // First check if this applies to us
    NMHDR *hdr = (NMHDR *)lParam;

    // the tooltips control created by the toolbar is sometimes Unicode, even
    // in an ANSI application - this seems to be a bug in comctl32.dll v5
    int code = (int)hdr->code;
    if ( (code != TTN_NEEDTEXTA) && (code != TTN_NEEDTEXTW) )
        return FALSE;

    HWND toolTipWnd = (HWND)::SendMessage((HWND)GetHWND(), TB_GETTOOLTIPS, 0, 0);
    if ( toolTipWnd != hdr->hwndFrom )
        return FALSE;

    LPTOOLTIPTEXT ttText = (LPTOOLTIPTEXT)lParam;
    int id = (int)ttText->hdr.idFrom;

    wxToolBarToolBase *tool = FindById(id);
    if ( !tool )
        return FALSE;

    const wxString& help = tool->GetShortHelp();

    if ( !help.IsEmpty() )
    {
        if ( code == TTN_NEEDTEXTA )
        {
            ttText->lpszText = (wxChar *)help.c_str();
        }
#if (_WIN32_IE >= 0x0300)
        else
        {
            // FIXME this is a temp hack only until I understand better what
            //       must be done in both ANSI and Unicode builds

            size_t lenAnsi = help.Len();
            #ifdef __MWERKS__
                // MetroWerks doesn't like calling mbstowcs with NULL argument
                size_t lenUnicode = 2*lenAnsi;
            #else
                size_t lenUnicode = mbstowcs(NULL, help, lenAnsi);
            #endif

            // using the pointer of right type avoids us doing all sorts of
            // pointer arithmetics ourselves
            wchar_t *dst = (wchar_t *)ttText->szText,
                    *pwz = new wchar_t[lenUnicode + 1];
            mbstowcs(pwz, help, lenAnsi + 1);
            memcpy(dst, pwz, lenUnicode*sizeof(wchar_t));

            // put the terminating _wide_ NUL
            dst[lenUnicode] = 0;

            delete [] pwz;
        }
#endif // _WIN32_IE >= 0x0300
    }

    // For backward compatibility...
    OnMouseEnter(tool->GetId());

    return TRUE;
}

// ----------------------------------------------------------------------------
// sizing stuff
// ----------------------------------------------------------------------------

void wxToolBar::SetToolBitmapSize(const wxSize& size)
{
    wxToolBarBase::SetToolBitmapSize(size);

    ::SendMessage(GetHwnd(), TB_SETBITMAPSIZE, 0, MAKELONG(size.x, size.y));
}

void wxToolBar::SetRows(int nRows)
{
    // TRUE in wParam means to create at least as many rows
    RECT rect;
    ::SendMessage(GetHwnd(), TB_SETROWS,
                  MAKEWPARAM(nRows, TRUE), (LPARAM) &rect);

    m_maxRows = nRows;
}

// The button size is bigger than the bitmap size
wxSize wxToolBar::GetToolSize() const
{
    // FIXME: this is completely bogus (VZ)
    return wxSize(m_defaultWidth + 8, m_defaultHeight + 7);
}

wxToolBarTool *wxToolBar::FindToolForPosition(wxCoord x, wxCoord y) const
{
    POINT pt;
    pt.x = x;
    pt.y = y;
    int index = (int)::SendMessage(GetHwnd(), TB_HITTEST, 0, (LPARAM)&pt);
    if ( index < 0 )
    {
        // it's a separator or there is no tool at all there
        return (wxToolBarTool *)NULL;
    }

    return (wxToolBarTool *)m_tools.Item((size_t)index)->GetData();
}

// ----------------------------------------------------------------------------
// tool state
// ----------------------------------------------------------------------------

void wxToolBar::DoEnableTool(wxToolBarTool *tool, bool enable)
{
    ::SendMessage(GetHwnd(), TB_ENABLEBUTTON,
                  (WPARAM)tool->GetId(), (LPARAM)MAKELONG(enable, 0));
}

void wxToolBar::DoToggleTool(wxToolBarTool *tool, bool toggle)
{
    ::SendMessage(GetHwnd(), TB_CHECKBUTTON,
                  (WPARAM)tool->GetId(), (LPARAM)MAKELONG(toggle, 0));
}

void wxToolBar::DoSetToggle(wxToolBarTool *tool, bool toggle)
{
    // VZ: AFAIK, the button has to be created either with TBSTYLE_CHECK or
    //     without, so we really need to delete the button and recreate it here
    wxFAIL_MSG( _T("not implemented") );
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

// Responds to colour changes, and passes event on to children.
void wxToolBar::OnSysColourChanged(wxSysColourChangedEvent& event)
{
    m_backgroundColour = wxColour(GetRValue(GetSysColor(COLOR_BTNFACE)),
          GetGValue(GetSysColor(COLOR_BTNFACE)), GetBValue(GetSysColor(COLOR_BTNFACE)));

    // Remap the buttons
    Realize();

    Refresh();

    // Propagate the event to the non-top-level children
    wxWindow::OnSysColourChanged(event);
}

void wxToolBar::OnMouseEvent(wxMouseEvent& event)
{
    if (event.RightDown())
    {
        // For now, we don't have an id. Later we could
        // try finding the tool.
        OnRightClick((int)-1, event.GetX(), event.GetY());
    }
    else
    {
        event.Skip();
    }
}


// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

// These are the default colors used to map the bitmap colors to the current
// system colors. Note that they are in BGR format because this is what Windows
// wants (and not RGB)

#define BGR_BUTTONTEXT      (RGB(000,000,000))  // black
#define BGR_BUTTONSHADOW    (RGB(128,128,128))  // dark grey
#define BGR_BUTTONFACE      (RGB(192,192,192))  // bright grey
#define BGR_BUTTONHILIGHT   (RGB(255,255,255))  // white
#define BGR_BACKGROUNDSEL   (RGB(255,000,000))  // blue
#define BGR_BACKGROUND      (RGB(255,000,255))  // magenta

void wxMapBitmap(HBITMAP hBitmap, int width, int height)
{
  COLORMAP ColorMap[] =
  {
    {BGR_BUTTONTEXT,    COLOR_BTNTEXT},     // black
    {BGR_BUTTONSHADOW,  COLOR_BTNSHADOW},   // dark grey
    {BGR_BUTTONFACE,    COLOR_BTNFACE},     // bright grey
    {BGR_BUTTONHILIGHT, COLOR_BTNHIGHLIGHT},// white
    {BGR_BACKGROUNDSEL, COLOR_HIGHLIGHT},   // blue
    {BGR_BACKGROUND,    COLOR_WINDOW}       // magenta
  };

  int NUM_MAPS = (sizeof(ColorMap)/sizeof(COLORMAP));
  int n;
  for ( n = 0; n < NUM_MAPS; n++)
  {
    ColorMap[n].to = ::GetSysColor(ColorMap[n].to);
  }

  HBITMAP hbmOld;
  HDC hdcMem = CreateCompatibleDC(NULL);

  if (hdcMem)
  {
    hbmOld = (HBITMAP) SelectObject(hdcMem, hBitmap);

    int i, j, k;
    for ( i = 0; i < width; i++)
    {
        for ( j = 0; j < height; j++)
        {
            COLORREF pixel = ::GetPixel(hdcMem, i, j);
/*
            BYTE red = GetRValue(pixel);
            BYTE green = GetGValue(pixel);
            BYTE blue = GetBValue(pixel);
*/

            for ( k = 0; k < NUM_MAPS; k ++)
            {
                if ( ColorMap[k].from == pixel )
                {
                    /* COLORREF actualPixel = */ ::SetPixel(hdcMem, i, j, ColorMap[k].to);
                    break;
                }
            }
        }
    }


    SelectObject(hdcMem, hbmOld);
    DeleteObject(hdcMem);
  }

}

// Some experiments...
#if 0
  // What we want to do is create another bitmap which has a depth of 4,
  // and set the bits. So probably we want to convert this HBITMAP into a
  // DIB, then call SetDIBits.
  // AAAGH. The stupid thing is that if newBitmap has a depth of 4 (less than that of
  // the screen), then SetDIBits fails.
  HBITMAP newBitmap = ::CreateBitmap(totalBitmapWidth, totalBitmapHeight, 1, 4, NULL);
  HANDLE newDIB = ::BitmapToDIB((HBITMAP) m_hBitmap, NULL);
  LPBITMAPINFOHEADER lpbmi = (LPBITMAPINFOHEADER) GlobalLock(newDIB);

  dc = ::GetDC(NULL);
//  LPBITMAPINFOHEADER lpbmi = (LPBITMAPINFOHEADER) newDIB;

  int result = ::SetDIBits(dc, newBitmap, 0, lpbmi->biHeight, FindDIBBits((LPSTR)lpbmi), (LPBITMAPINFO)lpbmi,
    DIB_PAL_COLORS);
  DWORD err = GetLastError();

  ::ReleaseDC(NULL, dc);

  // Delete the DIB
  GlobalUnlock (newDIB);
  GlobalFree (newDIB);

//  WXHBITMAP hBitmap2 = wxCreateMappedBitmap((WXHINSTANCE) wxGetInstance(), (WXHBITMAP) m_hBitmap);
  // Substitute our new bitmap for the old one
  ::DeleteObject((HBITMAP) m_hBitmap);
  m_hBitmap = (WXHBITMAP) newBitmap;
#endif


#endif // !(wxUSE_TOOLBAR && Win95)
