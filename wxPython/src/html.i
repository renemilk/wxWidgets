/////////////////////////////////////////////////////////////////////////////
// Name:        html.i
// Purpose:     SWIG definitions of html classes
//
// Author:      Robin Dunn
//
// Created:     25-nov-1998
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


%module html

%{
#include "wxPython.h"
#include <wx/html/htmlwin.h>
#include <wx/html/htmprint.h>
#include <wx/image.h>
#include <wx/fs_zip.h>
#include <wx/fs_inet.h>
#include <wx/wfstream.h>
#include <wx/filesys.h>

#include "printfw.h"
%}

//---------------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

%extern wx.i
%extern windows.i
%extern _defs.i
%extern events.i
%extern controls.i
%extern controls2.i
%extern printfw.i
%extern utils.i
%extern filesys.i
%extern streams.i


%pragma(python) code = "import wx"


//----------------------------------------------------------------------

%{
    // Put some wx default wxChar* values into wxStrings.
    static const wxChar* wxHtmlWindowNameStr = wxT("htmlWindow");
    DECLARE_DEF_STRING(HtmlWindowNameStr);

    static const wxChar* wxHtmlPrintoutTitleStr = wxT("Printout");
    DECLARE_DEF_STRING(HtmlPrintoutTitleStr);

    static const wxChar* wxHtmlPrintingTitleStr = wxT("Printing");
    DECLARE_DEF_STRING(HtmlPrintingTitleStr);

    static const wxString wxPyEmptyString(wxT(""));
%}

//---------------------------------------------------------------------------

enum {
    wxHTML_ALIGN_LEFT,
    wxHTML_ALIGN_CENTER,
    wxHTML_ALIGN_RIGHT,
    wxHTML_ALIGN_BOTTOM,
    wxHTML_ALIGN_TOP,

    wxHTML_CLR_FOREGROUND,
    wxHTML_CLR_BACKGROUND,

    wxHTML_UNITS_PIXELS,
    wxHTML_UNITS_PERCENT,

    wxHTML_INDENT_LEFT,
    wxHTML_INDENT_RIGHT,
    wxHTML_INDENT_TOP,
    wxHTML_INDENT_BOTTOM,

    wxHTML_INDENT_HORIZONTAL,
    wxHTML_INDENT_VERTICAL,
    wxHTML_INDENT_ALL,

    wxHTML_COND_ISANCHOR,
    wxHTML_COND_ISIMAGEMAP,
    wxHTML_COND_USER,
};


enum {
    wxHW_SCROLLBAR_NEVER,
    wxHW_SCROLLBAR_AUTO,
};


// enums for wxHtmlWindow::OnOpeningURL
enum wxHtmlOpeningStatus
{
    wxHTML_OPEN,
    wxHTML_BLOCK,
    wxHTML_REDIRECT
};

enum wxHtmlURLType
{
    wxHTML_URL_PAGE,
    wxHTML_URL_IMAGE,
    wxHTML_URL_OTHER
};

//---------------------------------------------------------------------------

class wxHtmlLinkInfo : public wxObject {
public:
    wxHtmlLinkInfo(const wxString& href, const wxString& target = wxPyEmptyString);
    wxString GetHref();
    wxString GetTarget();
    wxMouseEvent* GetEvent();
    wxHtmlCell* GetHtmlCell();

    void SetEvent(const wxMouseEvent *e);
    void SetHtmlCell(const wxHtmlCell * e);
};

//---------------------------------------------------------------------------

class wxHtmlTag : public wxObject {
public:
    // Never need to create a new tag from Python...
    //wxHtmlTag(const wxString& source, int pos, int end_pos, wxHtmlTagsCache* cache);

    wxString GetName();
    bool HasParam(const wxString& par);
    wxString GetParam(const wxString& par, int with_commas = FALSE);

    // Can't do this one as-is, but GetParam should be enough...
    //int ScanParam(const wxString& par, const char *format, void* param);

    wxString GetAllParams();
    bool HasEnding();
    int GetBeginPos();
    int GetEndPos1();
    int GetEndPos2();
};


//---------------------------------------------------------------------------

class wxHtmlParser : public wxObject {
public:
    // wxHtmlParser();  This is an abstract base class...

    void SetFS(wxFileSystem *fs);
    wxFileSystem* GetFS();
    wxObject* Parse(const wxString& source);
    void InitParser(const wxString& source);
    void DoneParser();
    void DoParsing(int begin_pos, int end_pos);
    void StopParsing();
    // wxObject* GetProduct();

    void AddTagHandler(wxHtmlTagHandler *handler);
    wxString* GetSource();
    void PushTagHandler(wxHtmlTagHandler* handler, wxString tags);
    void PopTagHandler();

    // virtual wxFSFile *OpenURL(wxHtmlURLType type, const wxString& url) const;

    // void AddText(const char* txt) = 0;
    // void AddTag(const wxHtmlTag& tag);
};


//---------------------------------------------------------------------------

class wxHtmlWinParser : public wxHtmlParser {
public:
    wxHtmlWinParser(wxPyHtmlWindow *wnd = NULL);

    void SetDC(wxDC *dc);
    wxDC* GetDC();
    int GetCharHeight();
    int GetCharWidth();
    wxPyHtmlWindow* GetWindow();
    // Sets fonts to be used when displaying HTML page. (if size null then default sizes used).
    %addmethods {
        void SetFonts(wxString normal_face, wxString fixed_face, PyObject* sizes=NULL) {
            int* temp = NULL;
            if (sizes) temp = int_LIST_helper(sizes);
            self->SetFonts(normal_face, fixed_face, temp);
            if (temp)
                delete [] temp;
        }
    }

    wxHtmlContainerCell* GetContainer();
    wxHtmlContainerCell* OpenContainer();
    wxHtmlContainerCell *SetContainer(wxHtmlContainerCell *c);
    wxHtmlContainerCell* CloseContainer();

    int GetFontSize();
    void SetFontSize(int s);
    int GetFontBold();
    void SetFontBold(int x);
    int GetFontItalic();
    void SetFontItalic(int x);
    int GetFontUnderlined();
    void SetFontUnderlined(int x);
    int GetFontFixed();
    void SetFontFixed(int x);
    int GetAlign();
    void SetAlign(int a);
    wxColour GetLinkColor();
    void SetLinkColor(const wxColour& clr);
    wxColour GetActualColor();
    void SetActualColor(const wxColour& clr);
    void SetLink(const wxString& link);
    wxFont* CreateCurrentFont();
    wxHtmlLinkInfo GetLink();

};



//---------------------------------------------------------------------------

%{
class wxPyHtmlTagHandler : public wxHtmlTagHandler {
    DECLARE_DYNAMIC_CLASS(wxPyHtmlTagHandler);
public:
    wxPyHtmlTagHandler() : wxHtmlTagHandler() {};

    wxHtmlParser* GetParser() { return m_Parser; }
    void ParseInner(const wxHtmlTag& tag) { wxHtmlTagHandler::ParseInner(tag); }

    DEC_PYCALLBACK_STRING__pure(GetSupportedTags);
    DEC_PYCALLBACK_BOOL_TAG_pure(HandleTag);

    PYPRIVATE;
};

IMPLEMENT_DYNAMIC_CLASS(wxPyHtmlTagHandler, wxHtmlTagHandler);

IMP_PYCALLBACK_STRING__pure(wxPyHtmlTagHandler, wxHtmlTagHandler, GetSupportedTags);
IMP_PYCALLBACK_BOOL_TAG_pure(wxPyHtmlTagHandler, wxHtmlTagHandler, HandleTag);
%}


%name(wxHtmlTagHandler) class wxPyHtmlTagHandler : public wxObject {
public:
    wxPyHtmlTagHandler();

    void _setCallbackInfo(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:self._setCallbackInfo(self, wxHtmlTagHandler)"

    void SetParser(wxHtmlParser *parser);
    wxHtmlParser* GetParser();
    void ParseInner(const wxHtmlTag& tag);
};


//---------------------------------------------------------------------------

%{
class wxPyHtmlWinTagHandler : public wxHtmlWinTagHandler {
    DECLARE_DYNAMIC_CLASS(wxPyHtmlWinTagHandler);
public:
    wxPyHtmlWinTagHandler() : wxHtmlWinTagHandler() {};

    wxHtmlWinParser* GetParser() { return m_WParser; }
    void ParseInner(const wxHtmlTag& tag)
        { wxHtmlWinTagHandler::ParseInner(tag); }

    DEC_PYCALLBACK_STRING__pure(GetSupportedTags);
    DEC_PYCALLBACK_BOOL_TAG_pure(HandleTag);

    PYPRIVATE;
};

IMPLEMENT_DYNAMIC_CLASS( wxPyHtmlWinTagHandler, wxHtmlWinTagHandler);

IMP_PYCALLBACK_STRING__pure(wxPyHtmlWinTagHandler, wxHtmlWinTagHandler, GetSupportedTags);
IMP_PYCALLBACK_BOOL_TAG_pure(wxPyHtmlWinTagHandler, wxHtmlWinTagHandler, HandleTag);
%}


%name(wxHtmlWinTagHandler) class wxPyHtmlWinTagHandler : public wxPyHtmlTagHandler {
public:
    wxPyHtmlWinTagHandler();

    void _setCallbackInfo(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:self._setCallbackInfo(self, wxHtmlWinTagHandler)"

    void SetParser(wxHtmlParser *parser);
    wxHtmlWinParser* GetParser();
    void ParseInner(const wxHtmlTag& tag);
};


//---------------------------------------------------------------------------

%{

class wxPyHtmlTagsModule : public wxHtmlTagsModule {
public:
    wxPyHtmlTagsModule(PyObject* thc) : wxHtmlTagsModule() {
        m_tagHandlerClass = thc;
        Py_INCREF(m_tagHandlerClass);
        RegisterModule(this);
        wxHtmlWinParser::AddModule(this);
    }

    void OnExit() {
        wxPyBeginBlockThreads();
        Py_DECREF(m_tagHandlerClass);
        m_tagHandlerClass = NULL;
        for (size_t x=0; x < m_objArray.GetCount(); x++) {
            PyObject* obj = (PyObject*)m_objArray.Item(x);
            Py_DECREF(obj);
        }
        wxPyEndBlockThreads();
    };

    void FillHandlersTable(wxHtmlWinParser *parser) {
        // Wave our magic wand...  (if it works it's a miracle!  ;-)

        // First, make a new instance of the tag handler
        wxPyBeginBlockThreads();
        PyObject* arg = Py_BuildValue("()");
        PyObject* obj = PyInstance_New(m_tagHandlerClass, arg, NULL);
        Py_DECREF(arg);
        wxPyEndBlockThreads();

        // now figure out where it's C++ object is...
        wxPyHtmlWinTagHandler* thPtr;
        if (SWIG_GetPtrObj(obj, (void **)&thPtr, "_wxPyHtmlWinTagHandler_p"))
            return;

        // add it,
        parser->AddTagHandler(thPtr);

        // and track it.
        m_objArray.Add(obj);
    }

private:
    PyObject*           m_tagHandlerClass;
    wxArrayPtrVoid      m_objArray;

};
%}



%inline %{
    void wxHtmlWinParser_AddTagHandler(PyObject* tagHandlerClass) {
        // Dynamically create a new wxModule.  Refcounts tagHandlerClass
        // and adds itself to the wxModules list and to the wxHtmlWinParser.
        new wxPyHtmlTagsModule(tagHandlerClass);
    }
%}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class wxHtmlCell : public wxObject {
public:
    wxHtmlCell();

    int GetPosX();
    int GetPosY();
    int GetWidth();
    int GetHeight();
    int GetDescent();
    wxHtmlLinkInfo* GetLink(int x = 0, int y = 0);
    wxHtmlCell* GetNext();
    wxHtmlContainerCell* GetParent();
    void SetLink(const wxHtmlLinkInfo& link);
    void SetNext(wxHtmlCell *cell);
    void SetParent(wxHtmlContainerCell *p);
    void SetPos(int x, int y);
    void Layout(int w);
    void Draw(wxDC& dc, int x, int y, int view_y1, int view_y2);
    void DrawInvisible(wxDC& dc, int x, int y);
    const wxHtmlCell* Find(int condition, const void* param);

    bool AdjustPagebreak(int* INOUT);
    void SetCanLiveOnPagebreak(bool can);

};


class  wxHtmlWordCell : public wxHtmlCell
{
public:
    wxHtmlWordCell(const wxString& word, wxDC& dc);
};


class wxHtmlContainerCell : public wxHtmlCell {
public:
    wxHtmlContainerCell(wxHtmlContainerCell *parent);

    void InsertCell(wxHtmlCell *cell);
    void SetAlignHor(int al);
    int GetAlignHor();
    void SetAlignVer(int al);
    int GetAlignVer();
    void SetIndent(int i, int what, int units = wxHTML_UNITS_PIXELS);
    int GetIndent(int ind);
    int GetIndentUnits(int ind);
    void SetAlign(const wxHtmlTag& tag);
    void SetWidthFloat(int w, int units);
    %name(SetWidthFloatFromTag)void SetWidthFloat(const wxHtmlTag& tag);
    void SetMinHeight(int h, int align = wxHTML_ALIGN_TOP);
    void SetBackgroundColour(const wxColour& clr);
    wxColour GetBackgroundColour();
    void SetBorder(const wxColour& clr1, const wxColour& clr2);
    wxHtmlCell* GetFirstCell();
};



class wxHtmlColourCell : public wxHtmlCell {
public:
    wxHtmlColourCell(wxColour clr, int flags = wxHTML_CLR_FOREGROUND);

};


class  wxHtmlFontCell : public wxHtmlCell
{
public:
    wxHtmlFontCell(wxFont *font);
};


class wxHtmlWidgetCell : public wxHtmlCell {
public:
    wxHtmlWidgetCell(wxWindow* wnd, int w = 0);

};


//---------------------------------------------------------------------------
// wxHtmlFilter
//---------------------------------------------------------------------------


%{ // here's the C++ version
class wxPyHtmlFilter : public wxHtmlFilter {
    DECLARE_ABSTRACT_CLASS(wxPyHtmlFilter);
public:
    wxPyHtmlFilter() : wxHtmlFilter() {}

    // returns TRUE if this filter is able to open&read given file
    virtual bool CanRead(const wxFSFile& file) const {
        bool rval = FALSE;
        bool found;
        wxPyBeginBlockThreads();
        if ((found = wxPyCBH_findCallback(m_myInst, "CanRead"))) {
            PyObject* obj = wxPyMake_wxObject((wxFSFile*)&file);  // cast away const
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", obj));
            Py_DECREF(obj);
        }
        wxPyEndBlockThreads();
        return rval;
    }


    // Reads given file and returns HTML document.
    // Returns empty string if opening failed
    virtual wxString ReadFile(const wxFSFile& file) const {
        wxString rval;
        bool found;
        wxPyBeginBlockThreads();
        if ((found = wxPyCBH_findCallback(m_myInst, "ReadFile"))) {
            PyObject* obj = wxPyMake_wxObject((wxFSFile*)&file);  // cast away const
            PyObject* ro;
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(O)", obj));
            Py_DECREF(obj);
            if (ro) {
                rval = Py2wxString(ro);
                Py_DECREF(ro);
            }
        }
        wxPyEndBlockThreads();
        return rval;
    }

    PYPRIVATE;
};

IMPLEMENT_ABSTRACT_CLASS(wxPyHtmlFilter, wxHtmlFilter);
%}


// And now the version seen by SWIG

%name(wxHtmlFilter) class wxPyHtmlFilter : public wxObject {
public:
    wxPyHtmlFilter();

    void _setCallbackInfo(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:self._setCallbackInfo(self, wxHtmlFilter)"
};


// TODO: wxHtmlFilterHTML


//---------------------------------------------------------------------------
// wxHtmlWindow
//---------------------------------------------------------------------------

%{
class wxPyHtmlWindow : public wxHtmlWindow {
    DECLARE_ABSTRACT_CLASS(wxPyHtmlWindow);
public:
    wxPyHtmlWindow(wxWindow *parent, wxWindowID id = -1,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxHW_SCROLLBAR_AUTO,
                   const wxString& name = wxPyHtmlWindowNameStr)
        : wxHtmlWindow(parent, id, pos, size, style, name)  {};
    wxPyHtmlWindow() : wxHtmlWindow() {};

    bool ScrollToAnchor(const wxString& anchor) {
        return wxHtmlWindow::ScrollToAnchor(anchor);
    }

    bool HasAnchor(const wxString& anchor) {
        const wxHtmlCell *c = m_Cell->Find(wxHTML_COND_ISANCHOR, &anchor);
        return c!=NULL;
    }

    void OnLinkClicked(const wxHtmlLinkInfo& link);
    void base_OnLinkClicked(const wxHtmlLinkInfo& link);

    wxHtmlOpeningStatus OnOpeningURL(wxHtmlURLType type,
                                      const wxString& url,
                                      wxString *redirect) const;

    DEC_PYCALLBACK__STRING(OnSetTitle);
    DEC_PYCALLBACK__CELLINTINT(OnCellMouseHover);
    DEC_PYCALLBACK__CELLINTINTME(OnCellClicked);
    PYPRIVATE;
};

IMPLEMENT_ABSTRACT_CLASS( wxPyHtmlWindow, wxHtmlWindow );
IMP_PYCALLBACK__STRING(wxPyHtmlWindow, wxHtmlWindow, OnSetTitle);
IMP_PYCALLBACK__CELLINTINT(wxPyHtmlWindow, wxHtmlWindow, OnCellMouseHover);
IMP_PYCALLBACK__CELLINTINTME(wxPyHtmlWindow, wxHtmlWindow, OnCellClicked);


void wxPyHtmlWindow::OnLinkClicked(const wxHtmlLinkInfo& link) {
    bool found;
    wxPyBeginBlockThreads();
    if ((found = wxPyCBH_findCallback(m_myInst, "OnLinkClicked"))) {
        PyObject* obj = wxPyConstructObject((void*)&link, wxT("wxHtmlLinkInfo"), 0);
        wxPyCBH_callCallback(m_myInst, Py_BuildValue("(O)", obj));
        Py_DECREF(obj);
    }
    wxPyEndBlockThreads();
    if (! found)
        wxHtmlWindow::OnLinkClicked(link);
}
void wxPyHtmlWindow::base_OnLinkClicked(const wxHtmlLinkInfo& link) {
    wxHtmlWindow::OnLinkClicked(link);
}


wxHtmlOpeningStatus wxPyHtmlWindow::OnOpeningURL(wxHtmlURLType type,
                                                 const wxString& url,
                                                 wxString *redirect) const {
    bool found;
    wxHtmlOpeningStatus rval;
    wxPyBeginBlockThreads();
    if ((found = wxPyCBH_findCallback(m_myInst, "OnOpeningURL"))) {
        PyObject* ro;
        PyObject* s = wx2PyString(url);
        ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(iO)", type, s));
        Py_DECREF(s);
        if (PyString_Check(ro)
#if PYTHON_API_VERSION >= 1009
            || PyUnicode_Check(ro)
#endif
            ) {
            *redirect = Py2wxString(ro);
            rval = wxHTML_REDIRECT;
        }
        else {
            PyObject* num = PyNumber_Int(ro);
            rval = (wxHtmlOpeningStatus)PyInt_AsLong(num);
            Py_DECREF(num);
        }
        Py_DECREF(ro);
    }
    wxPyEndBlockThreads();
    if (! found)
        rval = wxHtmlWindow::OnOpeningURL(type, url, redirect);
    return rval;
}


%}



%name(wxHtmlWindow) class wxPyHtmlWindow : public wxScrolledWindow {
public:
    wxPyHtmlWindow(wxWindow *parent, int id = -1,
                 wxPoint& pos = wxDefaultPosition,
                 wxSize& size = wxDefaultSize,
                 int style=wxHW_SCROLLBAR_AUTO,
                 const wxString& name = wxPyHtmlWindowNameStr);
    %name(wxPreHtmlWindow)wxPyHtmlWindow();

    bool Create(wxWindow *parent, int id = -1,
                wxPoint& pos = wxDefaultPosition,
                wxSize& size = wxDefaultSize,
                int style=wxHW_SCROLLBAR_AUTO,
                const wxString& name = wxPyHtmlWindowNameStr);


    void _setCallbackInfo(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:self._setCallbackInfo(self, wxHtmlWindow)"
    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreHtmlWindow:val._setOORInfo(val)"

    // Set HTML page and display it. !! source is HTML document itself,
    // it is NOT address/filename of HTML document. If you want to
    // specify document location, use LoadPage() istead
    // Return value : FALSE if an error occured, TRUE otherwise
    bool SetPage(const wxString& source);

    // Load HTML page from given location. Location can be either
    // a) /usr/wxGTK2/docs/html/wx.htm
    // b) http://www.somewhere.uk/document.htm
    // c) ftp://ftp.somesite.cz/pub/something.htm
    // In case there is no prefix (http:,ftp:), the method
    // will try to find it itself (1. local file, then http or ftp)
    // After the page is loaded, the method calls SetPage() to display it.
    // Note : you can also use path relative to previously loaded page
    // Return value : same as SetPage
    bool LoadPage(const wxString& location);

    // Loads HTML page from file
    bool LoadFile(const wxString& filename);

    // Append to current page
    bool AppendToPage(const wxString& source);

     // Returns full location of opened page
    wxString GetOpenedPage();

    // Returns anchor within opened page
    wxString GetOpenedAnchor();

    // Returns <TITLE> of opened page or empty string otherwise
    wxString GetOpenedPageTitle();

    // Sets frame in which page title will  be displayed. Format is format of
    // frame title, e.g. "HtmlHelp : %s". It must contain exactly one %s
    void SetRelatedFrame(wxFrame* frame, const wxString& format);
    wxFrame* GetRelatedFrame();

    // After(!) calling SetRelatedFrame, this sets statusbar slot where messages
    // will be displayed. Default is -1 = no messages.
    void SetRelatedStatusBar(int bar);

    // Sets fonts to be used when displaying HTML page.
    %addmethods {
        void SetFonts(wxString normal_face, wxString fixed_face, PyObject* sizes=NULL) {
            int* temp = NULL;
            if (sizes) temp = int_LIST_helper(sizes);
            self->SetFonts(normal_face, fixed_face, temp);
            if (temp)
                delete [] temp;
        }
    }

    void SetTitle(const wxString& title);

    // Sets space between text and window borders.
    void SetBorders(int b);

    // Saves custom settings into cfg config. it will use the path 'path'
    // if given, otherwise it will save info into currently selected path.
    // saved values : things set by SetFonts, SetBorders.
    void ReadCustomization(wxConfigBase *cfg, wxString path = wxPyEmptyString);
    void WriteCustomization(wxConfigBase *cfg, wxString path = wxPyEmptyString);

    // Goes to previous/next page (in browsing history)
    // Returns TRUE if successful, FALSE otherwise
    bool HistoryBack();
    bool HistoryForward();
    bool HistoryCanBack();
    bool HistoryCanForward();

    // Resets History
    void HistoryClear();

    // Returns pointer to conteiners/cells structure.
    wxHtmlContainerCell* GetInternalRepresentation();

    // Returns a pointer to the parser.
    wxHtmlWinParser* GetParser();

    bool ScrollToAnchor(const wxString& anchor);
    bool HasAnchor(const wxString& anchor);

    //Adds input filter
    static void AddFilter(wxPyHtmlFilter *filter);


    void base_OnLinkClicked(const wxHtmlLinkInfo& link);
    void base_OnSetTitle(const wxString& title);
    void base_OnCellMouseHover(wxHtmlCell *cell, wxCoord x, wxCoord y);
    void base_OnCellClicked(wxHtmlCell *cell,
                            wxCoord x, wxCoord y,
                            const wxMouseEvent& event);
};



//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


class wxHtmlDCRenderer : public wxObject {
public:
    wxHtmlDCRenderer();
    ~wxHtmlDCRenderer();

    void SetDC(wxDC *dc, int maxwidth);
    void SetSize(int width, int height);
    void SetHtmlText(const wxString& html,
                     const wxString& basepath = wxPyEmptyString,
                     bool isdir = TRUE);
    // Sets fonts to be used when displaying HTML page. (if size null then default sizes used).
    %addmethods {
        void SetFonts(wxString normal_face, wxString fixed_face, PyObject* sizes=NULL) {
            int* temp = NULL;
            if (sizes) temp = int_LIST_helper(sizes);
            self->SetFonts(normal_face, fixed_face, temp);
            if (temp)
                delete [] temp;
        }
    }
    int Render(int x, int y, int from = 0, int dont_render = FALSE);
    int GetTotalHeight();
                // returns total height of the html document
                // (compare Render's return value with this)
};

enum {
    wxPAGE_ODD,
    wxPAGE_EVEN,
    wxPAGE_ALL
};


class wxHtmlPrintout : public wxPyPrintout {
public:
    wxHtmlPrintout(const wxString& title = wxPyHtmlPrintoutTitleStr);
    //~wxHtmlPrintout();   wxPrintPreview object takes ownership...

    void SetHtmlText(const wxString& html,
                     const wxString &basepath = wxPyEmptyString,
                     bool isdir = TRUE);
    void SetHtmlFile(const wxString &htmlfile);
    void SetHeader(const wxString& header, int pg = wxPAGE_ALL);
    void SetFooter(const wxString& footer, int pg = wxPAGE_ALL);
    // Sets fonts to be used when displaying HTML page. (if size null then default sizes used).
    %addmethods {
        void SetFonts(wxString normal_face, wxString fixed_face, PyObject* sizes=NULL) {
            int* temp = NULL;
            if (sizes) temp = int_LIST_helper(sizes);
            self->SetFonts(normal_face, fixed_face, temp);
            if (temp)
                delete [] temp;
        }
    }
    void SetMargins(float top = 25.2, float bottom = 25.2,
                    float left = 25.2, float right = 25.2,
                    float spaces = 5);
};



class wxHtmlEasyPrinting : public wxObject {
public:
    wxHtmlEasyPrinting(const wxString& name = wxPyHtmlPrintingTitleStr,
                       wxFrame *parent_frame = NULL);
    ~wxHtmlEasyPrinting();

    void PreviewFile(const wxString &htmlfile);
    void PreviewText(const wxString &htmltext, const wxString& basepath = wxPyEmptyString);
    void PrintFile(const wxString &htmlfile);
    void PrintText(const wxString &htmltext, const wxString& basepath = wxPyEmptyString);
    void PrinterSetup();
    void PageSetup();
    void SetHeader(const wxString& header, int pg = wxPAGE_ALL);
    void SetFooter(const wxString& footer, int pg = wxPAGE_ALL);

    wxPrintData *GetPrintData() {return m_PrintData;}
    wxPageSetupDialogData *GetPageSetupData() {return m_PageSetupData;}

};



//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

%{
    extern "C" SWIGEXPORT(void) inithtmlhelpc();
%}


%init %{

    inithtmlhelpc();

    wxClassInfo::CleanUpClasses();
    wxClassInfo::InitializeClasses();

    wxPyPtrTypeMap_Add("wxHtmlTagHandler", "wxPyHtmlTagHandler");
    wxPyPtrTypeMap_Add("wxHtmlWinTagHandler", "wxPyHtmlWinTagHandler");
    wxPyPtrTypeMap_Add("wxHtmlWindow", "wxPyHtmlWindow");
    wxPyPtrTypeMap_Add("wxHtmlFilter", "wxPyHtmlFilter");
%}

//----------------------------------------------------------------------
// And this gets appended to the shadow class file.
//----------------------------------------------------------------------

%pragma(python) include="_htmlextras.py";

//---------------------------------------------------------------------------
