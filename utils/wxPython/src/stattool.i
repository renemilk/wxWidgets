/////////////////////////////////////////////////////////////////////////////
// Name:        stattool.i
// Purpose:     SWIG definitions for StatusBar and ToolBar classes
//
// Author:      Robin Dunn
//
// Created:     08/24/1998
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


%module stattool

%{
#include "helpers.h"
#include <wx/toolbar.h>
#include <wx/tbarsmpl.h>
%}

//----------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

// Import some definitions of other classes, etc.
%import _defs.i
%import misc.i
%import windows.i
%import controls.i

%pragma(python) code = "import wx"

//---------------------------------------------------------------------------

class wxStatusBar : public wxWindow {
public:
    wxStatusBar(wxWindow* parent, wxWindowID id,
                const wxPoint& pos = wxPyDefaultPosition,
                const wxSize& size = wxPyDefaultSize,
                long style = wxST_SIZEGRIP,
                char* name = "statusBar");

    %pragma(python) addtomethod = "__init__:wx._StdWindowCallbacks(self)"

    %addmethods {
        %new wxRect* GetFieldRect(long item) {
            wxRect* rect= new wxRect;
            self->GetFieldRect(item, *rect);
            return rect;
        }
    }
    int GetFieldsCount(void);
    wxString GetStatusText(int ir = 0);
    void DrawField(wxDC& dc, int i);
    void DrawFieldText(wxDC& dc, int i);
    void InitColours(void);

    // OnSysColourChanged(wxSysColourChangedEvent& event);

    void SetFieldsCount(int number = 1);
    void SetStatusText(const wxString& text, int i = 0);
    void SetStatusWidths(int LCOUNT, int* LIST);
};


//---------------------------------------------------------------------------

class wxToolBarTool {
public:
    wxToolBarTool();
    ~wxToolBarTool();
#ifdef __WXMSW__
    void SetSize( long w, long h ) { m_width = w; m_height = h; }
    long GetWidth () const { return m_width; }
    long GetHeight () const { return m_height; }
#endif

public:
    int                   m_toolStyle;
    wxObject *            m_clientData;
    int                   m_index;
#ifdef __WXMSW__
    long                  m_x;
    long                  m_y;
    long                  m_width;
    long                  m_height;
#endif
    bool                  m_toggleState;
    bool                  m_isToggle;
    bool                  m_deleteSecondBitmap;
    bool                  m_enabled;
    wxBitmap              m_bitmap1;
    wxBitmap              m_bitmap2;
    bool                  m_isMenuCommand;
    wxString              m_shortHelpString;
    wxString              m_longHelpString;
};



//  class wxToolBarBase : public wxControl {
//  public:

class wxToolBar : public wxControl {
public:
    wxToolBar(wxWindow* parent, wxWindowID id,
              const wxPoint& pos = wxPyDefaultPosition,
              const wxSize& size = wxPyDefaultSize,
              long style = wxTB_HORIZONTAL | wxNO_BORDER,
              char* name = "toolBar");

    %pragma(python) addtomethod = "__init__:wx._StdWindowCallbacks(self)"


    void AddSeparator();

    // Ignoge the clientData for now...
    %addmethods {
        wxToolBarTool* AddTool(int toolIndex,
                               const wxBitmap& bitmap1,
                               const wxBitmap& bitmap2 = wxNullBitmap,
                               int  isToggle = FALSE,
                               long xPos = -1,
                               long yPos = -1,
                               //wxObject* clientData = NULL,
                               const wxString& shortHelpString = wxPyEmptyStr,
                               const wxString& longHelpString = wxPyEmptyStr) {
            return self->AddTool(toolIndex, bitmap1, bitmap2,
                                 isToggle, xPos, yPos, NULL,
                                 shortHelpString, longHelpString);
        }

        wxToolBarTool* AddSimpleTool(int toolIndex,
                               const wxBitmap& bitmap,
                               const wxString& shortHelpString = wxPyEmptyStr,
                               const wxString& longHelpString = wxPyEmptyStr) {
            return self->AddTool(toolIndex, bitmap, wxNullBitmap,
                                 FALSE, -1, -1, NULL,
                                 shortHelpString, longHelpString);
        }
    }


//    void DrawTool(wxMemoryDC& memDC, wxToolBarTool* tool);
    void EnableTool(int toolIndex, const bool enable);
#ifdef __WXMSW__
    wxToolBarTool* FindToolForPosition(const float x, const float y);
    wxSize GetToolSize();
    wxSize GetToolBitmapSize();
    void SetToolBitmapSize(const wxSize& size);
//    wxSize GetMargins();
    wxSize GetMaxSize();
//    wxObject* GetToolClientData(int toolIndex);
#endif
    bool GetToolEnabled(int toolIndex);
    wxString GetToolLongHelp(int toolIndex);
    int GetToolPacking();
    int GetToolSeparation();
    wxString GetToolShortHelp(int toolIndex);
    bool GetToolState(int toolIndex);

    // TODO:  figure out how to handle these
    //bool OnLeftClick(int toolIndex, bool toggleDown);
    //void OnMouseEnter(int toolIndex);
    //void OnRightClick(int toolIndex, float x, float y);

    bool Realize();

    void SetToolLongHelp(int toolIndex, const wxString& helpString);
    void SetToolShortHelp(int toolIndex, const wxString& helpString);
    void SetMargins(const wxSize& size);
    void SetToolPacking(int packing);
    void SetToolSeparation(int separation);
    void ToggleTool(int toolIndex, const bool toggle);
};




//---------------------------------------------------------------------------

