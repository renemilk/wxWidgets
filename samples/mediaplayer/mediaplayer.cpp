///////////////////////////////////////////////////////////////////////////////
// Name:        mediaplayer.cpp
// Purpose:     wxMediaCtrl sample
// Author:      Ryan Norton
// Modified by:
// Created:     11/10/04
// RCS-ID:      $Id$
// Copyright:   (c) Ryan Norton
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// MediaPlayer
//
// This is a somewhat comprehensive example of how to use all the funtionality
// of the wxMediaCtrl class in wxWidgets.
//
// To use this sample, simply select Open File from the file menu,
// select the file you want to play - and MediaPlayer will play the file in a
// the current notebook page, showing video if necessary.
//
// You can select one of the menu options, or move the slider around
// to manipulate what is playing.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Known bugs with wxMediaCtrl:
//
// 1) Certain backends can't play the same media file at the same time (MCI,
//    Cocoa NSMovieView-Quicktime).
// 2) Positioning on Mac Carbon is messed up if put in a sub-control like a
//    Notebook (like this sample does) on OS versions < 10.2.
// 3) On unix the video may not work - it only checks for a few video
//    sinks - xvimagesink, ximagesink and whatever gnome preferences has -
//    if gnome preferences is not available or you have a different video
//    sink then those two (such as sdlvideosink) then you'll get black video
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// ============================================================================
// Definitions
// ============================================================================

// ----------------------------------------------------------------------------
// Pre-compiled header stuff
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

// ----------------------------------------------------------------------------
// Headers
// ----------------------------------------------------------------------------

#include "wx/mediactrl.h"   //for wxMediaCtrl
#include "wx/filedlg.h"     //for opening files from OpenFile
#include "wx/slider.h"      //for a slider for seeking within media
#include "wx/sizer.h"       //for positioning controls/wxBoxSizer
#include "wx/timer.h"       //timer for updating status bar
#include "wx/textdlg.h"     //for getting user text from OpenURL/Debug
#include "wx/notebook.h"    //for wxNotebook and putting movies in pages
#include "wx/cmdline.h"     //for wxCmdLineParser (optional)
#include "wx/listctrl.h"    //for wxListCtrl
#include "wx/dnd.h"         //drag and drop for the playlist
#include "wx/filename.h"    //For wxFileName::GetName()
#include "wx/config.h"      //for native wxConfig

// ----------------------------------------------------------------------------
// Bail out if the user doesn't want one of the
// things we need
// ----------------------------------------------------------------------------

// RN:  I'm not sure why this is here - even minimal doesn't check for
//      wxUSE_GUI.  I may have added it myself though...
#if !wxUSE_GUI
#error "This is a GUI sample"
#endif

#if !wxUSE_MEDIACTRL || !wxUSE_MENUS || !wxUSE_SLIDER || !wxUSE_TIMER || \
    !wxUSE_NOTEBOOK || !wxUSE_LISTCTRL || !wxUSE_DRAG_AND_DROP
#error "Not all required elements are enabled.  Please modify setup.h!"
#endif

// ============================================================================
// Declarations
// ============================================================================

// ----------------------------------------------------------------------------
// Enumurations
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // Menu event IDs
    wxID_LOOP = 1,
    wxID_OPENFILESAMEPAGE,
    wxID_OPENFILENEWPAGE,
    wxID_OPENURLSAMEPAGE,
    wxID_OPENURLNEWPAGE,
    wxID_CLOSECURRENTPAGE,
    wxID_PLAY,
    wxID_PAUSE,
    wxID_NEXT,
    wxID_PREV,
    wxID_SELECTBACKEND,
    wxID_SHOWINTERFACE,
//    wxID_STOP,   [built-in to wxWidgets]
//    wxID_ABOUT,  [built-in to wxWidgets]
//    wxID_EXIT,   [built-in to wxWidgets]
    // Control event IDs
    wxID_SLIDER,
    wxID_NOTEBOOK,
    wxID_MEDIACTRL,
    wxID_BUTTONNEXT,
    wxID_BUTTONPREV,
    wxID_BUTTONSTOP,
    wxID_BUTTONPLAY,
    wxID_BUTTONVD,
    wxID_BUTTONVU,
    wxID_LISTCTRL,
    wxID_GAUGE
};

// ----------------------------------------------------------------------------
// wxMediaPlayerApp
// ----------------------------------------------------------------------------

class wxMediaPlayerApp : public wxApp
{
public:
#ifdef __WXMAC__
    virtual void MacOpenFile(const wxString & fileName );
#endif

    virtual bool OnInit();

protected:
    class wxMediaPlayerFrame* m_frame;
};

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame
// ----------------------------------------------------------------------------

class wxMediaPlayerFrame : public wxFrame
{
public:
    // Ctor/Dtor
    wxMediaPlayerFrame(const wxString& title);
    ~wxMediaPlayerFrame();

    // Menu event handlers
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnOpenFileSamePage(wxCommandEvent& event);
    void OnOpenFileNewPage(wxCommandEvent& event);
    void OnOpenURLSamePage(wxCommandEvent& event);
    void OnOpenURLNewPage(wxCommandEvent& event);
    void OnCloseCurrentPage(wxCommandEvent& event);

    void OnPlay(wxCommandEvent& event);
    void OnPause(wxCommandEvent& event);
    void OnStop(wxCommandEvent& event);
    void OnNext(wxCommandEvent& event);
    void OnPrev(wxCommandEvent& event);
    void OnVolumeDown(wxCommandEvent& event);
    void OnVolumeUp(wxCommandEvent& event);

    void OnLoop(wxCommandEvent& event);
    void OnShowInterface(wxCommandEvent& event);

    void OnSelectBackend(wxCommandEvent& event);

    // Notebook event handlers
    void OnPageChange(wxNotebookEvent& event);

    // Key event handlers
    void OnKeyDown(wxKeyEvent& event);

    // Quickie for playing from command line
    void AddToPlayList(const wxString& szString);

    // ListCtrl event handlers
    void OnChangeSong(wxListEvent& event);

    // Media event handlers
    void OnMediaLoaded(wxMediaEvent& event);

    // Close event handlers
    void OnClose(wxCloseEvent& event);

private:
    // Rebuild base status string (see Implementation)
    void ResetStatus();

    // Common open file code
    void OpenFile(bool bNewPage);
    void OpenURL(bool bNewPage);
    void DoOpenFile(const wxString& path, bool bNewPage);
    void DoPlayFile(const wxString& path);

    // Get the controls of current notebook page
    wxMediaCtrl* GetCurrentMediaCtrl();
    wxSlider*    GetCurrentSlider();
    wxGauge*    GetCurrentGauge();

    int      m_nLastFileId;     //List ID of played file in listctrl
    wxString m_szFile;          //Name of currently playing file/location
    class wxMediaPlayerTimer* m_timer;     //Timer to write info to status bar
    wxString m_basestatus;      //Base status string (see ResetStatus())
    wxNotebook* m_notebook;     //Notebook containing our pages

    // Maybe I should use more accessors, but for simplicity
    // I'll allow the other classes access to our members
    friend class wxMediaPlayerApp;
    friend class wxMediaPlayerNotebookPage;
    friend class wxMediaPlayerTimer;
};



// ----------------------------------------------------------------------------
// wxMediaPlayerNotebookPage
// ----------------------------------------------------------------------------

class wxMediaPlayerNotebookPage : public wxPanel
{
    wxMediaPlayerNotebookPage(wxMediaPlayerFrame* parentFrame,
        wxNotebook* book, const wxString& be = wxEmptyString);

    // Slider event handlers
    void OnBeginSeek(wxScrollEvent& event);
    void OnEndSeek(wxScrollEvent& event);

    // Media event handlers
    void OnMediaFinished(wxMediaEvent& event);

public:
    bool IsBeingDragged();      //accessor for m_bIsBeingDragged

    //make wxMediaPlayerFrame able to access the private members
    friend class wxMediaPlayerFrame;

    wxMediaCtrl* m_mediactrl;   //Our media control
    class wxMediaPlayerListCtrl* m_playlist;  //Our playlist
    wxSlider* m_slider;         //The slider below our media control
    int m_nLoops;               //Number of times media has looped
    bool m_bLoop;               //Whether we are looping or not
    bool m_bIsBeingDragged;     //Whether the user is dragging the scroll bar
    wxMediaPlayerFrame* m_parentFrame;  //Main wxFrame of our sample
    wxButton* m_prevButton;     //Go to previous file button
    wxButton* m_playButton;     //Play/pause file button
    wxButton* m_stopButton;     //Stop playing file button
    wxButton* m_nextButton;     //Next file button
    wxButton* m_vdButton;       //Volume down button
    wxButton* m_vuButton;       //Volume up button
    wxGauge*  m_gauge;          //Gauge to keep in line with slider
};

// ----------------------------------------------------------------------------
// wxMediaPlayerTimer
// ----------------------------------------------------------------------------

class wxMediaPlayerTimer : public wxTimer
{
public:
    //Ctor
    wxMediaPlayerTimer(wxMediaPlayerFrame* frame) {m_frame = frame;}

    //Called each time the timer's timeout expires
    void Notify();

    wxMediaPlayerFrame* m_frame;       //The wxMediaPlayerFrame
};

// ----------------------------------------------------------------------------
// wxMediaPlayerListCtrl
// ----------------------------------------------------------------------------
class wxMediaPlayerListCtrl : public wxListCtrl
{
public:
    void AddToPlayList(const wxString& szString)
    {
        wxListItem kNewItem;
        kNewItem.SetAlign(wxLIST_FORMAT_LEFT);

        int nID;

        kNewItem.SetId(nID = this->GetItemCount());
        kNewItem.SetMask(wxLIST_MASK_DATA);
        kNewItem.SetData(new wxString(szString));

        this->InsertItem(kNewItem);
        this->SetItem(nID, 0, _T("*"));
        this->SetItem(nID, 1, wxFileName(szString).GetName());

        if (nID % 2)
        {
            kNewItem.SetBackgroundColour(wxColour(192,192,192));
            this->SetItem(kNewItem);
        }
    }

    void GetSelectedItem(wxListItem& listitem)
    {
        listitem.SetMask(wxLIST_MASK_TEXT |  wxLIST_MASK_DATA);
        int nLast = -1, nLastSelected = -1;
        while ((nLast = this->GetNextItem(nLast,
                                         wxLIST_NEXT_ALL,
                                         wxLIST_STATE_SELECTED)) != -1)
        {
            listitem.SetId(nLast);
            this->GetItem(listitem);
            if ((listitem.GetState() & wxLIST_STATE_FOCUSED) )
                break;
            nLastSelected = nLast;
        }
        if (nLast == -1 && nLastSelected == -1)
            return;
        listitem.SetId(nLastSelected == -1 ? nLast : nLastSelected);
        this->GetItem(listitem);
    }

};

// ----------------------------------------------------------------------------
// wxPlayListDropTarget
//
//  Drop target for playlist (i.e. user drags a file from explorer unto
//  playlist it adds the file)
// ----------------------------------------------------------------------------
class wxPlayListDropTarget : public wxFileDropTarget
{
public:
    wxPlayListDropTarget(wxMediaPlayerListCtrl& list) : m_list(list) {}
    ~wxPlayListDropTarget(){}
        virtual bool OnDropFiles(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y),
                         const wxArrayString& files)
    {
        for (size_t i = 0; i < files.GetCount(); ++i)
        {
            m_list.AddToPlayList(files[i]);
        }
        return true;
    }
    wxMediaPlayerListCtrl& m_list;
};

// ============================================================================
//
// Implementation
//
// ============================================================================

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// [Functions]
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ----------------------------------------------------------------------------
// wxGetMediaStateText
//
// Converts a wxMediaCtrl state into something useful that we can display
// to the user
// ----------------------------------------------------------------------------
const wxChar* wxGetMediaStateText(int nState)
{
    switch(nState)
    {
        case wxMEDIASTATE_PLAYING:
            return wxT("Playing");
        case wxMEDIASTATE_STOPPED:
            return wxT("Stopped");
        ///case wxMEDIASTATE_PAUSED:
        default:
            return wxT("Paused");
    }
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// wxMediaPlayerApp
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ----------------------------------------------------------------------------
// This sets up this wxApp as the global wxApp that gui calls in wxWidgets
// use.  For example, if you were to be in windows and use a file dialog,
// wxWidgets would use wxTheApp->GetHInstance() which would get the instance
// handle of the application.  These routines in wx _DO NOT_ check to see if
// the wxApp exists, and thus will crash the application if you try it.
//
// IMPLEMENT_APP does this, and also implements the platform-specific entry
// routine, such as main or WinMain().  Use IMPLEMENT_APP_NO_MAIN if you do
// not desire this behavior.
// ----------------------------------------------------------------------------
IMPLEMENT_APP(wxMediaPlayerApp)

// ----------------------------------------------------------------------------
// wxMediaPlayerApp::OnInit
//
// Where execution starts - akin to a main or WinMain.
// 1) Create the frame and show it to the user
// 2) Process filenames from the commandline
// 3) return true specifying that we want execution to continue past OnInit
// ----------------------------------------------------------------------------
bool wxMediaPlayerApp::OnInit()
{
    wxMediaPlayerFrame *frame =
        new wxMediaPlayerFrame(_T("MediaPlayer wxWidgets Sample"));
    frame->Show(true);

#if wxUSE_CMDLINE_PARSER
    //
    //  What this does is get all the command line arguments
    //  and treat each one as a file to put to the initial playlist
    //
    wxCmdLineEntryDesc cmdLineDesc[2];
    cmdLineDesc[0].kind = wxCMD_LINE_PARAM;
    cmdLineDesc[0].shortName = NULL;
    cmdLineDesc[0].longName = NULL;
    cmdLineDesc[0].description = wxT("input files");
    cmdLineDesc[0].type = wxCMD_LINE_VAL_STRING;
    cmdLineDesc[0].flags = wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_PARAM_MULTIPLE;

    cmdLineDesc[1].kind = wxCMD_LINE_NONE;

    //gets the passed media files from cmd line
    wxCmdLineParser parser (cmdLineDesc, argc, argv);

    // get filenames from the commandline
    if (parser.Parse() == 0)
    {
        for (size_t paramNr=0; paramNr < parser.GetParamCount(); ++paramNr)
        {
            frame->AddToPlayList((parser.GetParam (paramNr)));
        }
        wxCommandEvent emptyevt;
        frame->OnNext(emptyevt);
    }
#endif

    return true;
}

#ifdef __WXMAC__

void wxMediaPlayerApp::MacOpenFile(const wxString & fileName )
{
    //Called when a user drags a file over our app
    m_frame->DoOpenFile(fileName, true /* new page */);
}

#endif // __WXMAC__

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// wxMediaPlayerFrame
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame Constructor
//
// 1) Create our menus
// 2) Create our notebook control and add it to the frame
// 3) Create our status bar
// 4) Connect our events
// 5) Start our timer
// ----------------------------------------------------------------------------

wxMediaPlayerFrame::wxMediaPlayerFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(600,600)),
       m_nLastFileId(-1)
{
    //
    //  Create Menus
    //
    wxMenu *fileMenu = new wxMenu;
    wxMenu *controlsMenu = new wxMenu;
    wxMenu *optionsMenu = new wxMenu;
    wxMenu *helpMenu = new wxMenu;
    wxMenu *debugMenu = new wxMenu;

    fileMenu->Append(wxID_OPENFILESAMEPAGE, _T("&Open File\tCtrl-Shift-O"),
                        _T("Open a File in the current notebook page"));
    fileMenu->Append(wxID_OPENFILENEWPAGE, _T("&Open File in a new page"),
                        _T("Open a File in a new notebook page"));
    fileMenu->Append(wxID_OPENURLSAMEPAGE, _T("&Open URL"),
                        _T("Open a URL in the current notebook page"));
    fileMenu->Append(wxID_OPENURLNEWPAGE, _T("&Open URL in a new page"),
                        _T("Open a URL in a new notebook page"));
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_CLOSECURRENTPAGE, _T("&Close Current Page\tCtrl-C"),
                        _T("Close current notebook page"));
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT,
                     _T("E&xit\tAlt-X"),
                     _T("Quit this program"));

    controlsMenu->Append(wxID_PLAY, _T("&Play/Pause\tCtrl-P"), _T("Resume/Pause playback"));
    controlsMenu->Append(wxID_STOP, _T("&Stop\tCtrl-S"), _T("Stop playback"));
    controlsMenu->AppendSeparator();
    controlsMenu->Append(wxID_PREV, _T("&Previous\tCtrl-B"), _T("Go to previous track"));
    controlsMenu->Append(wxID_NEXT, _T("&Next\tCtrl-N"), _T("Skip to next track"));

    optionsMenu->AppendCheckItem(wxID_LOOP,
                              _T("&Loop\tCtrl-L"),
                              _T("Loop Selected Media"));
    optionsMenu->AppendCheckItem(wxID_SHOWINTERFACE,
                              _T("&Show Interface\tCtrl-I"),
                              _T("Show wxMediaCtrl native controls"));

    debugMenu->Append(wxID_SELECTBACKEND,
                     _T("&Select Backend...\tCtrl-D"),
                     _T("Select a backend manually"));

    helpMenu->Append(wxID_ABOUT,
                     _T("&About...\tF1"),
                     _T("Show about dialog"));


    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, _T("&File"));
    menuBar->Append(controlsMenu, _T("&Controls"));
    menuBar->Append(optionsMenu, _T("&Options"));
    menuBar->Append(debugMenu, _T("&Debug"));
    menuBar->Append(helpMenu, _T("&Help"));
    SetMenuBar(menuBar);

    //
    // Create our notebook - using wxNotebook is luckily pretty
    // simple and self-explanatory in most cases
    //
    m_notebook = new wxNotebook(this, wxID_NOTEBOOK);

    //
    //  Create our status bar
    //
#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(1);
#endif // wxUSE_STATUSBAR

    //
    //  Connect events.
    //
    //  There are two ways in wxWidgets to use events -
    //  Message Maps and Connections.
    //
    //  Message Maps are implemented by putting
    //  DECLARE_MESSAGE_MAP in your wxEvtHandler-derived
    //  class you want to use for events, such as wxMediaPlayerFrame.
    //
    //  Then after your class declaration you put
    //  BEGIN_EVENT_TABLE(wxMediaPlayerFrame, wxFrame)
    //  EVT_XXX(XXX)...
    //  END_EVENT_TABLE()
    //
    //  Where wxMediaPlayerFrame is the class with the DECLARE_MESSAGE_MAP
    //  in it.  EVT_XXX(XXX) are each of your handlers, such
    //  as EVT_MENU for menu events and the XXX inside
    //  is the parameters to the event macro - in the case
    //  of EVT_MENU the menu id and then the function to call.
    //
    //  However, with wxEvtHandler::Connect you can avoid a
    //  global message map for your class and those annoying
    //  macros.  You can also change the context in which
    //  the call the handler (more later).
    //
    //  The downside is that due to the limitation that
    //  wxWidgets doesn't use templates in certain areas,
    //  You have to triple-cast the event function.
    //
    //  There are five parameters to wxEvtHandler::Connect -
    //
    //  The first is the id of the instance whose events
    //  you want to handle - i.e. a menu id for menus,
    //  a control id for controls (wxControl::GetId())
    //  and so on.
    //
    //  The second is the event id.  This is the same
    //  as the message maps (EVT_MENU) except prefixed
    //  with "wx" (wxEVT_MENU).
    //
    //  The third is the function handler for the event -
    //  You need to cast it to the specific event handler
    //  type, then to a wxEventFunction, then to a
    //  wxObjectEventFunction - I.E.
    //  (wxObjectEventFunction)(wxEventFunction)
    //  (wxCommandEventFunction) &wxMediaPlayerFrame::MyHandler
    //
    //  Or, you can use the new (2.5.5+) event handler
    //  conversion macros - for instance the above could
    //  be done as
    //  wxCommandEventHandler(wxMediaPlayerFrame::MyHandler)
    //  pretty simple, eh?
    //
    //  The fourth is an optional userdata param -
    //  this is of historical relevance only and is
    //  there only for backwards compatibility.
    //
    //  The fifth is the context in which to call the
    //  handler - by default (this param is optional)
    //  this.  For example in your event handler
    //  if you were to call "this->MyFunc()"
    //  it would literally do this->MyFunc.  However,
    //  if you were to pass myHandler as the fifth
    //  parameter, for instance, you would _really_
    //  be calling myHandler->MyFunc, even though
    //  the compiler doesn't really know it.
    //

    //
    // Menu events
    //
    this->Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED,
                  wxCommandEventHandler(wxMediaPlayerFrame::OnQuit));

    this->Connect(wxID_ABOUT, wxEVT_COMMAND_MENU_SELECTED,
                  wxCommandEventHandler(wxMediaPlayerFrame::OnAbout));

    this->Connect(wxID_LOOP, wxEVT_COMMAND_MENU_SELECTED,
                  wxCommandEventHandler(wxMediaPlayerFrame::OnLoop));

    this->Connect(wxID_SHOWINTERFACE, wxEVT_COMMAND_MENU_SELECTED,
                  wxCommandEventHandler(wxMediaPlayerFrame::OnShowInterface));

    this->Connect(wxID_OPENFILENEWPAGE, wxEVT_COMMAND_MENU_SELECTED,
                  wxCommandEventHandler(wxMediaPlayerFrame::OnOpenFileNewPage));

    this->Connect(wxID_OPENFILESAMEPAGE, wxEVT_COMMAND_MENU_SELECTED,
                  wxCommandEventHandler(wxMediaPlayerFrame::OnOpenFileSamePage));

    this->Connect(wxID_OPENURLNEWPAGE, wxEVT_COMMAND_MENU_SELECTED,
                  wxCommandEventHandler(wxMediaPlayerFrame::OnOpenURLNewPage));

    this->Connect(wxID_OPENURLSAMEPAGE, wxEVT_COMMAND_MENU_SELECTED,
                  wxCommandEventHandler(wxMediaPlayerFrame::OnOpenURLSamePage));

    this->Connect(wxID_CLOSECURRENTPAGE, wxEVT_COMMAND_MENU_SELECTED,
                  wxCommandEventHandler(wxMediaPlayerFrame::OnCloseCurrentPage));

    this->Connect(wxID_PLAY, wxEVT_COMMAND_MENU_SELECTED,
                  wxCommandEventHandler(wxMediaPlayerFrame::OnPlay));

    this->Connect(wxID_STOP, wxEVT_COMMAND_MENU_SELECTED,
                  wxCommandEventHandler(wxMediaPlayerFrame::OnStop));

    this->Connect(wxID_NEXT, wxEVT_COMMAND_MENU_SELECTED,
                  wxCommandEventHandler(wxMediaPlayerFrame::OnNext));

    this->Connect(wxID_PREV, wxEVT_COMMAND_MENU_SELECTED,
                  wxCommandEventHandler(wxMediaPlayerFrame::OnPrev));

    this->Connect(wxID_SELECTBACKEND, wxEVT_COMMAND_MENU_SELECTED,
                  wxCommandEventHandler(wxMediaPlayerFrame::OnSelectBackend));

    //
    // Notebook events
    //
    this->Connect(wxID_NOTEBOOK, wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,
                  wxNotebookEventHandler(wxMediaPlayerFrame::OnPageChange));

    //
    // Key events
    //
    wxTheApp->Connect(wxID_ANY, wxEVT_KEY_DOWN,
                  wxKeyEventHandler(wxMediaPlayerFrame::OnKeyDown),
                  (wxObject*)0, this);

    //
    // Close events
    //
    this->Connect(wxID_ANY, wxEVT_CLOSE_WINDOW,
                wxCloseEventHandler(wxMediaPlayerFrame::OnClose));

    //
    // End of Events
    //

    //
    //  Create an initial notebook page so the user has something
    //  to work with without having to go file->open every time :).
    //
    m_notebook->AddPage(new wxMediaPlayerNotebookPage(this, m_notebook),
                        wxT(""),
                        true);

    //
    //  Create a timer to update our status bar
    //
    m_timer = new wxMediaPlayerTimer(this);
    m_timer->Start(100);
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame Destructor
//
// 1) Deletes child objects implicitly
// 2) Delete our timer explicitly
// ----------------------------------------------------------------------------
wxMediaPlayerFrame::~wxMediaPlayerFrame()
{
    delete m_timer;

}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnClose
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnClose(wxCloseEvent& event)
{
    //
    //  Here we save our info to the registry or whatever
    //  mechanism the OS uses.
    //
    //  This makes it so that when mediaplayer loads up again
    //  it restores the same files that were in the playlist
    //  this time, rather than the user manually re-adding them.
    //
    //  We need to do conf->DeleteAll() here because by default
    //  the config still contains the same files as last time
    //  so we need to clear it before writing our new ones.
    //
    //  TODO:  Maybe you could add a menu option to the
    //  options menu to delete the configuration on exit -
    //  all you'd need to do is just remove everything after
    //  conf->DeleteAll() here
    //
    wxMediaPlayerListCtrl* m_playlist =
        ((wxMediaPlayerNotebookPage*)m_notebook->GetCurrentPage())->m_playlist;

    wxConfigBase* conf = wxConfigBase::Get();
    conf->DeleteAll();

    for(int i = 0; i < m_playlist->GetItemCount(); ++i)
    {
        wxString* pData = (wxString*) m_playlist->GetItemData(i);
        wxString s;
        s << i;
        conf->Write(s, *(pData));
        delete pData;
    }

    event.Skip(); //really close the frame
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::AddToPlayList
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::AddToPlayList(const wxString& szString)
{
    wxMediaPlayerNotebookPage* currentpage =
        ((wxMediaPlayerNotebookPage*)m_notebook->GetCurrentPage());

    currentpage->m_playlist->AddToPlayList(szString);
}


// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::ResetStatus
//
// Here we just make a simple status string with some useful info about
// the media that we won't change later - such as the length of the media.
//
// We then append some other info that changes in wxMediaPlayerTimer::Notify, then
// set the status bar to this text.
//
// In real applications, you'd want to find a better way to do this,
// such as static text controls (wxStaticText).
//
// We display info here in seconds (wxMediaCtrl uses milliseconds - that's why
// we divide by 1000).
//
// We also reset our loop counter here.
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::ResetStatus()
{
    wxMediaCtrl* currentMediaCtrl = GetCurrentMediaCtrl();

    m_basestatus = wxString::Format(_T("Size(x,y):%i,%i ")
                                    _T("Length(Seconds):%u Speed:%1.1fx"),
    currentMediaCtrl->GetBestSize().x,
    currentMediaCtrl->GetBestSize().y,
    (unsigned)((currentMediaCtrl->Length() / 1000)),
    currentMediaCtrl->GetPlaybackRate()
    );
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::GetCurrentMediaCtrl
//
// Obtains the media control of the current page, or NULL if there are no
// pages open
// ----------------------------------------------------------------------------
wxMediaCtrl* wxMediaPlayerFrame::GetCurrentMediaCtrl()
{
    return ((wxMediaPlayerNotebookPage*)m_notebook->GetCurrentPage())->m_mediactrl;
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::GetCurrentSlider
//
// Obtains the slider of the current page
// ----------------------------------------------------------------------------
wxSlider*    wxMediaPlayerFrame::GetCurrentSlider()
{
    return ((wxMediaPlayerNotebookPage*)m_notebook->GetCurrentPage())->m_slider;
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::GetCurrentGauge
//
// Obtains the gauge of the current page
// ----------------------------------------------------------------------------
wxGauge*    wxMediaPlayerFrame::GetCurrentGauge()
{
    return ((wxMediaPlayerNotebookPage*)m_notebook->GetCurrentPage())->m_gauge;
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnQuit
//
// Called from file->quit.
// Closes this application.
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnAbout
//
// Called from help->about.
// Gets some info about this application.
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( _T("This is a test of wxMediaCtrl.\n")
                _T("Welcome to %s"), wxVERSION_STRING);

    wxMessageBox(msg, _T("About wxMediaCtrl test"), wxOK | wxICON_INFORMATION, this);
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnLoop
//
// Called from file->loop.
// Changes the state of whether we want to loop or not.
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnLoop(wxCommandEvent& WXUNUSED(event))
{
    if(!m_notebook->GetCurrentPage())
    {
        wxMessageBox(wxT("No files are currently open!"));
        return;
    }

    ((wxMediaPlayerNotebookPage*)m_notebook->GetCurrentPage())->m_bLoop =
            !((wxMediaPlayerNotebookPage*)m_notebook->GetCurrentPage())->m_bLoop;
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnLoop
//
// Called from file->loop.
// Changes the state of whether we want to loop or not.
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnShowInterface(wxCommandEvent& event)
{
    if(!m_notebook->GetCurrentPage())
    {
        wxMessageBox(wxT("No files are currently open!"));
        return;
    }

    GetCurrentMediaCtrl()->ShowPlayerControls(event.IsChecked() ?
            wxMEDIACTRLPLAYERCONTROLS_DEFAULT :
             wxMEDIACTRLPLAYERCONTROLS_NONE);
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnOpenFileSamePage
//
// Called from file->openfile.
// Opens and plays a media file in the current notebook page
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnOpenFileSamePage(wxCommandEvent& WXUNUSED(event))
{
    OpenFile(false);
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnOpenFileNewPage
//
// Called from file->openfileinnewpage.
// Opens and plays a media file in a new notebook page
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnOpenFileNewPage(wxCommandEvent& WXUNUSED(event))
{
    OpenFile(true);
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OpenFile
//
// Opens a file dialog asking the user for a filename, then
// calls DoOpenFile which will add the file to the playlist and play it
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OpenFile(bool bNewPage)
{
    wxFileDialog fd(this);

    if(fd.ShowModal() == wxID_OK)
    {
        DoOpenFile(fd.GetPath(), bNewPage);
    }
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::DoOpenFile
//
// Adds the file to our playlist, selects it in the playlist,
// and then calls DoPlayFile to play it
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::DoOpenFile(const wxString& path, bool bNewPage)
{
    if(bNewPage)
    {
        m_notebook->AddPage(
            new wxMediaPlayerNotebookPage(this, m_notebook),
            path,
            true);
    }

    wxMediaPlayerListCtrl* m_playlist =
        ((wxMediaPlayerNotebookPage*)m_notebook->GetCurrentPage())->m_playlist;

    if(m_nLastFileId != -1)
        m_playlist->SetItemState(m_nLastFileId, 0, wxLIST_STATE_SELECTED);

    wxListItem newlistitem;
    newlistitem.SetAlign(wxLIST_FORMAT_LEFT);

    int nID;

    newlistitem.SetId(nID = m_playlist->GetItemCount());
    newlistitem.SetMask(wxLIST_MASK_DATA | wxLIST_MASK_STATE);
    newlistitem.SetState(wxLIST_STATE_SELECTED);
    newlistitem.SetData(new wxString(path));

    m_playlist->InsertItem(newlistitem);
    m_playlist->SetItem(nID, 0, _T("*"));
    m_playlist->SetItem(nID, 1, wxFileName(path).GetName());

    if (nID % 2)
    {
        newlistitem.SetBackgroundColour(wxColour(192,192,192));
        m_playlist->SetItem(newlistitem);
    }

    DoPlayFile(path);
     //        m_playlist->Focus(nID);
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::DoPlayFile
//
// Pauses the file if its the currently playing file,
// otherwise it plays the file
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::DoPlayFile(const wxString& path)
{
    wxMediaPlayerListCtrl* m_playlist =
        ((wxMediaPlayerNotebookPage*)m_notebook->GetCurrentPage())->m_playlist;

    wxListItem listitem;
    m_playlist->GetSelectedItem(listitem);

    if(listitem.GetData() &&
       m_szFile.compare(path) == 0 &&
       m_nLastFileId == listitem.GetId())
    {
        if(GetCurrentMediaCtrl()->GetState() == wxMEDIASTATE_PLAYING)
    {
            if( !GetCurrentMediaCtrl()->Pause() )
                wxMessageBox(wxT("Couldn't pause movie!"));
            else
                m_playlist->SetItem(listitem.GetId(), 0, _T("||"));
        }
        else
        {
            if( !GetCurrentMediaCtrl()->Play() )
                wxMessageBox(wxT("Couldn't pause movie!"));
            else
                m_playlist->SetItem(listitem.GetId(), 0, _T(">"));
        }
    }
    else
    {
        m_notebook->SetPageText(m_notebook->GetSelection(),
                                wxFileName(path).GetName());

        if(m_nLastFileId != -1)
            m_playlist->SetItem(m_nLastFileId, 0, _T("*"));

        wxURI uripath(path);
        if( uripath.IsReference() )
        {
            if( !GetCurrentMediaCtrl()->Load(path) )
            {
                wxMessageBox(wxT("Couldn't load file!"));
                m_playlist->SetItem(listitem.GetId(), 0, _T("E"));
            }
            else
            {
                m_playlist->SetItem(listitem.GetId(), 0, _T("O"));
            }
        }
        else
        {
            if( !GetCurrentMediaCtrl()->Load(uripath) )
            {
            wxMessageBox(wxT("Couldn't load file!"));
                m_playlist->SetItem(listitem.GetId(), 0, _T("E"));
            }
            else
            {
                m_playlist->SetItem(listitem.GetId(), 0, _T("O"));
            }
        }

        m_nLastFileId = listitem.GetId();
        m_szFile = path;
        m_playlist->SetItem(m_nLastFileId, 1, wxFileName(path).GetName());
        m_playlist->SetItem(m_nLastFileId, 2, wxT(""));
    }
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnMediaLoaded
//
// Called when the media is ready to be played - and does
// so, also gets the length of media and shows that in the list control
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnMediaLoaded(wxMediaEvent& WXUNUSED(evt))
{
    wxMediaPlayerListCtrl* m_playlist =
        ((wxMediaPlayerNotebookPage*)m_notebook->GetCurrentPage())->m_playlist;
    wxListItem listitem;
    m_playlist->GetSelectedItem(listitem);

        if( !GetCurrentMediaCtrl()->Play() )
    {
            wxMessageBox(wxT("Couldn't play movie!"));
        m_playlist->SetItem(listitem.GetId(), 0, _T("E"));
    }
    else
    {
        m_playlist->SetItem(listitem.GetId(), 0, _T(">"));
    }

    m_playlist->SetItem(listitem.GetId(), 2, wxString::Format(wxT("%u"),
                        (unsigned) GetCurrentMediaCtrl()->Length() / 1000) );

        ResetStatus();

        GetCurrentSlider()->SetRange(0,
                        (int)(GetCurrentMediaCtrl()->Length() / 1000));
    GetCurrentGauge()->SetRange((int)(GetCurrentMediaCtrl()->Length() / 1000));
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnSelectBackend
//
// Little debugging routine - enter the class name of a backend and it
// will use that instead of letting wxMediaCtrl search the wxMediaBackend
// RTTI class list.
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnSelectBackend(wxCommandEvent& WXUNUSED(evt))
{
    wxString sBackend = wxGetTextFromUser(wxT("Enter backend to use"));

    if(sBackend.empty() == false)  //could have been cancelled by the user
    {
        int sel = m_notebook->GetSelection();

        if (sel != wxNOT_FOUND)
        {
            m_notebook->DeletePage(sel);
        }

        m_notebook->AddPage(new wxMediaPlayerNotebookPage(this, m_notebook,
                                                        sBackend
                                                        ), wxT(""), true);
        DoOpenFile(m_szFile, false);
    }
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnOpenURLSamePage
//
// Called from file->openurl.
// Opens and plays a media file from a URL in the current notebook page
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnOpenURLSamePage(wxCommandEvent& WXUNUSED(event))
{
    OpenURL(false);
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnOpenURLNewPage
//
// Called from file->openurlinnewpage.
// Opens and plays a media file from a URL in a new notebook page
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnOpenURLNewPage(wxCommandEvent& WXUNUSED(event))
{
    OpenURL(true);
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OpenURL
//
// Just calls DoOpenFile with the url path - which calls DoPlayFile
// which handles the real dirty work
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OpenURL(bool bNewPage)
{
    wxString sUrl = wxGetTextFromUser(
        wxT("Enter the URL that has the movie to play")
                                     );

    if(sUrl.empty() == false) //could have been cancelled by user
    {
        DoOpenFile(sUrl, bNewPage);
    }
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnCloseCurrentPage
//
// Called when the user wants to close the current notebook page
//
// 1) Get the current page number (wxControl::GetSelection)
// 2) If there is no current page, break out
// 3) Delete the current page
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnCloseCurrentPage(wxCommandEvent& WXUNUSED(event))
{
    if( m_notebook->GetPageCount() > 1 )
    {
    int sel = m_notebook->GetSelection();

    if (sel != wxNOT_FOUND)
    {
        m_notebook->DeletePage(sel);
    }
    }
    else
    {
        wxMessageBox(wxT("Cannot close main page"));
    }
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnPlay
//
// Called from file->play.
// Resumes the media if it is paused or stopped.
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnPlay(wxCommandEvent& WXUNUSED(event))
{
    wxMediaPlayerListCtrl* m_playlist =
        ((wxMediaPlayerNotebookPage*)m_notebook->GetCurrentPage())->m_playlist;

    wxListItem listitem;
    m_playlist->GetSelectedItem(listitem);
    if ( !listitem.GetData() )
    {
        int nLast = -1;
        if ((nLast = m_playlist->GetNextItem(nLast,
                                         wxLIST_NEXT_ALL,
                                         wxLIST_STATE_DONTCARE)) == -1)
        {
            //no items in list
            wxMessageBox(_T("No items in playlist!"));
        return;
    }
        wxListItem listitem;
        listitem.SetId(nLast);
        m_playlist->GetItem(listitem);
        listitem.SetMask(listitem.GetMask() | wxLIST_MASK_STATE);
        listitem.SetState(listitem.GetState() | wxLIST_STATE_SELECTED);
        m_playlist->SetItem(listitem);
        wxListEvent event;
        OnChangeSong(event);
    }
    else
    {
        wxListEvent event;
        OnChangeSong(event);
    }
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnKeyDown
//
// Deletes all selected files from the playlist if the backspace key is pressed
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnKeyDown(wxKeyEvent& event)
{
   if(event.GetKeyCode() == WXK_BACK/*DELETE*/)
    {
    wxMediaPlayerListCtrl* m_playlist =
        ((wxMediaPlayerNotebookPage*)m_notebook->GetCurrentPage())->m_playlist;
       //delete all selected items
       while(true)
       {
           wxInt32 nSelectedItem = m_playlist->GetNextItem(
                    -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
           if (nSelectedItem == -1)
               break;

           wxListItem listitem;
           listitem.SetId(nSelectedItem);
           m_playlist->GetItem(listitem);
           delete (wxString*) listitem.GetData();

           m_playlist->DeleteItem(nSelectedItem);
       }
    }

   //Could be wxGetTextFromUser or something else important
   if(event.GetEventObject() != this)
       event.Skip();
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnStop
//
// Called from file->stop.
// Where it stops depends on whether you can seek in the
// media control or not - if you can it stops and seeks to the beginning,
// otherwise it will appear to be at the end - but it will start over again
// when Play() is called
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnStop(wxCommandEvent& WXUNUSED(evt))
{
    wxMediaPlayerListCtrl* m_playlist =
        ((wxMediaPlayerNotebookPage*)m_notebook->GetCurrentPage())->m_playlist;

    wxListItem listitem;
    m_playlist->GetSelectedItem(listitem);
    m_playlist->SetItem(listitem.GetId(), 0, _T("[]"));

    if(!m_notebook->GetCurrentPage())
    {
        wxMessageBox(wxT("No files are currently open!"));
        return;
    }

    if( !GetCurrentMediaCtrl()->Stop() )
        wxMessageBox(wxT("Couldn't stop movie!"));
}


// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnChangeSong
//
// Routine that plays the currently selected file in the playlist.
// Called when the user actives the song from the playlist,
// and from other various places in the sample
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnChangeSong(wxListEvent& WXUNUSED(evt))
{
    wxMediaPlayerListCtrl* m_playlist =
        ((wxMediaPlayerNotebookPage*)m_notebook->GetCurrentPage())->m_playlist;

    wxListItem listitem;
    m_playlist->GetSelectedItem(listitem);
    DoPlayFile((*((wxString*) listitem.GetData())));
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnPrev
//
// Tedious wxListCtrl stuff.  Goes to prevous song in list, or if at the
// beginning goes to the last in the list.
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnPrev(wxCommandEvent& WXUNUSED(event))
{
    wxMediaPlayerListCtrl* m_playlist =
        ((wxMediaPlayerNotebookPage*)m_notebook->GetCurrentPage())->m_playlist;

    if (m_playlist->GetItemCount() == 0)
        return;

    wxInt32 nLastSelectedItem = -1;
    while(true)
    {
        wxInt32 nSelectedItem = m_playlist->GetNextItem(nLastSelectedItem,
                                                     wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if (nSelectedItem == -1)
            break;
        nLastSelectedItem = nSelectedItem;
        m_playlist->SetItemState(nSelectedItem, 0, wxLIST_STATE_SELECTED);
    }

    if (nLastSelectedItem <= 0)
        nLastSelectedItem = m_playlist->GetItemCount() - 1;
    else
        nLastSelectedItem -= 1;

    wxListItem listitem;
    listitem.SetId(nLastSelectedItem);
    m_playlist->GetItem(listitem);
    listitem.SetMask(listitem.GetMask() | wxLIST_MASK_STATE);
    listitem.SetState(listitem.GetState() | wxLIST_STATE_SELECTED);
    m_playlist->SetItem(listitem);

    wxListEvent emptyEvent;
    OnChangeSong(emptyEvent);
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnNext
//
// Tedious wxListCtrl stuff.  Goes to next song in list, or if at the
// end goes to the first in the list.
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnNext(wxCommandEvent& WXUNUSED(event))
{
    wxMediaPlayerListCtrl* m_playlist =
        ((wxMediaPlayerNotebookPage*)m_notebook->GetCurrentPage())->m_playlist;

    if (m_playlist->GetItemCount() == 0)
        return;

    wxInt32 nLastSelectedItem = -1;
    while(true)
    {
        wxInt32 nSelectedItem = m_playlist->GetNextItem(nLastSelectedItem,
                                                     wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if (nSelectedItem == -1)
            break;
        nLastSelectedItem = nSelectedItem;
        m_playlist->SetItemState(nSelectedItem, 0, wxLIST_STATE_SELECTED);
    }

    if (nLastSelectedItem == -1)
        nLastSelectedItem = 0;
    else
    {
        if (nLastSelectedItem == m_playlist->GetItemCount() - 1)
            nLastSelectedItem = 0;
        else
            nLastSelectedItem += 1;
    }

    wxListItem listitem;
    listitem.SetId(nLastSelectedItem);
    m_playlist->GetItem(listitem);
    listitem.SetMask(listitem.GetMask() | wxLIST_MASK_STATE);
    listitem.SetState(listitem.GetState() | wxLIST_STATE_SELECTED);
    m_playlist->SetItem(listitem);

    wxListEvent emptyEvent;
    OnChangeSong(emptyEvent);
}


// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnVolumeDown
//
// Lowers the volume of the media control by 10%
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnVolumeDown(wxCommandEvent& WXUNUSED(event))
{
    double dVolume = GetCurrentMediaCtrl()->GetVolume();
    GetCurrentMediaCtrl()->SetVolume(dVolume < 0.1 ? 0.0 : dVolume - .1);
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnVolumeUp
//
// Increases the volume of the media control by 10%
// ----------------------------------------------------------------------------
void wxMediaPlayerFrame::OnVolumeUp(wxCommandEvent& WXUNUSED(event))
{
    double dVolume = GetCurrentMediaCtrl()->GetVolume();
    GetCurrentMediaCtrl()->SetVolume(dVolume > 0.9 ? 1.0 : dVolume + .1);
}

// ----------------------------------------------------------------------------
// wxMediaPlayerFrame::OnCloseCurrentPage
//
// Called when the user wants to closes the current notebook page
// ----------------------------------------------------------------------------

void wxMediaPlayerFrame::OnPageChange(wxNotebookEvent& WXUNUSED(event))
{
    ResetStatus();
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// wxMediaPlayerTimer
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ----------------------------------------------------------------------------
// wxMediaPlayerTimer::Notify
//
// 1) Update our slider with the position were are in in the media
// 2) Update our status bar with the base text from wxMediaPlayerFrame::ResetStatus,
//    append some non-static (changing) info to it, then set the
//    status bar text to that result
// ----------------------------------------------------------------------------
void wxMediaPlayerTimer::Notify()
{
    if(m_frame->m_notebook->GetCurrentPage())
    {
            // get some control pointers from current notebook page
        wxMediaCtrl* mediactrl =
            ((wxMediaPlayerNotebookPage*)m_frame->m_notebook->GetCurrentPage())->m_mediactrl;
        wxSlider* slider =
            ((wxMediaPlayerNotebookPage*)m_frame->m_notebook->GetCurrentPage())->m_slider;
        wxGauge* gauge =
            ((wxMediaPlayerNotebookPage*)m_frame->m_notebook->GetCurrentPage())->m_gauge;

        // if the slider is being dragged then update it with the song position
        if(((wxMediaPlayerNotebookPage*)m_frame->m_notebook->GetCurrentPage())->IsBeingDragged() == false)
        {
            long lPosition = (long)( mediactrl->Tell() / 1000 );
            slider->SetValue(lPosition);
        }

        // update guage with value from slider
        gauge->SetValue(slider->GetValue());
#if wxUSE_STATUSBAR
        m_frame->SetStatusText(wxString::Format(
                        wxT("%s Pos:%u State:%s Loops:%i D/T:[%i]/[%i] V:%i%%"),
                        m_frame->m_basestatus.c_str(),
                        slider->GetValue(),
                        wxGetMediaStateText(mediactrl->GetState()),
                        ((wxMediaPlayerNotebookPage*)m_frame->m_notebook->GetCurrentPage())->m_nLoops,
                        (int)mediactrl->GetDownloadProgress(),
                        (int)mediactrl->GetDownloadTotal(),
                        (int)(mediactrl->GetVolume() * 100)));
#endif // wxUSE_STATUSBAR
    }
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// wxMediaPlayerNotebookPage
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ----------------------------------------------------------------------------
// wxMediaPlayerNotebookPage Constructor
//
// Creates a media control and slider and adds it to this panel,
// along with some sizers for positioning
// ----------------------------------------------------------------------------

wxMediaPlayerNotebookPage::wxMediaPlayerNotebookPage(wxMediaPlayerFrame* parentFrame,
                                                     wxNotebook* theBook,
                                                     const wxString& szBackend)
                         : wxPanel(theBook, wxID_ANY),
                           m_nLoops(0),
                           m_bLoop(false),
                           m_bIsBeingDragged(false),
                           m_parentFrame(parentFrame)
{

    //
    //  Layout
    //
    //  [wxMediaCtrl]
    //  [playlist]
    //  [5 control buttons]
    //  [slider]
    //  [gauge]
    //

    //
    //  Create and attach the sizer
    //
    wxFlexGridSizer* sizer = new wxFlexGridSizer(2, 1, 0, 0);
    this->SetSizer(sizer);
    this->SetAutoLayout(true);
    sizer->AddGrowableRow(0);
    sizer->AddGrowableCol(0);

    //
    //  Create our media control
    //
    m_mediactrl = new wxMediaCtrl();

    //  Make sure creation was successful
    bool bOK = m_mediactrl->Create(this, wxID_MEDIACTRL, wxEmptyString,
                                    wxDefaultPosition, wxDefaultSize, 0,
//you could specify a macrod backend here like
//wxMEDIABACKEND_QUICKTIME);
                                   szBackend);
//you could change the cursor here like
//    m_mediactrl->SetCursor(wxCURSOR_BLANK);
//note that this may not effect it if SetPlayerControls
//is set to something else than wxMEDIACTRLPLAYERCONTROLS_NONE
    wxASSERT_MSG(bOK, wxT("Could not create media control!"));
    wxUnusedVar(bOK);

    sizer->Add(m_mediactrl, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND, 5);

    //
    //  Create the playlist/listctrl
    //
    m_playlist = new wxMediaPlayerListCtrl();
    m_playlist->Create(this, wxID_LISTCTRL, wxDefaultPosition,
                    wxDefaultSize,
                    wxLC_REPORT //wxLC_LIST
                    | wxSUNKEN_BORDER);

    //  Set the background of our listctrl to white
    m_playlist->SetBackgroundColour(wxColour(255,255,255));

    //  The layout of the headers of the listctrl are like
    //  |   | File               |  Length
    //
    //  Where Column one is a character representing the state the file is in:
    //  * - not the current file
    //  E - Error has occured
    //  > - Currently Playing
    //  [] - Stopped
    //  || - Paused
    //  (( - Volume Down 10%
    //  )) - Volume Up 10%
    //
    //  Column two is the name of the file
    //
    //  Column three is the length in seconds of the file
    m_playlist->InsertColumn(0,_(""), wxLIST_FORMAT_CENTER, 20);
    m_playlist->InsertColumn(1,_("File"), wxLIST_FORMAT_LEFT, /*wxLIST_AUTOSIZE_USEHEADER*/305);
    m_playlist->InsertColumn(2,_("Length"), wxLIST_FORMAT_CENTER, 75);

    m_playlist->SetDropTarget(new wxPlayListDropTarget(*m_playlist));
    sizer->Add(m_playlist, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND, 5);

    //
    //  Here we load the our configuration -
    //  in our case we load all the files that were left in
    //  the playlist the last time the user closed our application
    //
    //  TODO:  This is probably not the best practice since
    //  the user will load multiple notebook pages with multiple
    //  wxMediaCtrl elements.
    //
    //  As an exercise to the reader try modifying it so that
    //  it properly loads the playlist for each page without
    //  conflicting (loading the same data) with the other ones.
    //
    wxConfigBase* conf = wxConfigBase::Get();
    wxString key, outstring;
    for(int i = 0; ; ++i)
    {
        key.clear();
        key << i;
        if(!conf->Read(key, &outstring))
            break;
        m_playlist->AddToPlayList(outstring);
    }

    //
    //  Create the control buttons
    //  TODO/FIXME/HACK:  This part about sizers is really a nice hack
    //                    and probably isn't proper
    //
    wxBoxSizer* horsizer1 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* vertsizer = new wxBoxSizer(wxHORIZONTAL);

    m_prevButton = new wxButton();
    m_playButton = new wxButton();
    m_stopButton = new wxButton();
    m_nextButton = new wxButton();
    m_vdButton = new wxButton();
    m_vuButton = new wxButton();

    m_prevButton->Create(this, wxID_BUTTONPREV, _T("|<"));
    m_playButton->Create(this, wxID_BUTTONPLAY, _T(">"));
    m_stopButton->Create(this, wxID_BUTTONSTOP, _T("[]"));
    m_nextButton->Create(this, wxID_BUTTONNEXT, _T(">|"));
    m_vdButton->Create(this, wxID_BUTTONVD, _T("(("));
    m_vuButton->Create(this, wxID_BUTTONVU, _T("))"));
    vertsizer->Add(m_prevButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    vertsizer->Add(m_playButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    vertsizer->Add(m_stopButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    vertsizer->Add(m_nextButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    vertsizer->Add(m_vdButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    vertsizer->Add(m_vuButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    horsizer1->Add(vertsizer, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    sizer->Add(horsizer1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);


    //
    //  Create our slider
    //
    m_slider = new wxSlider(this, wxID_SLIDER, 0, //init
                            0, //start
                            0, //end
                            wxDefaultPosition, wxDefaultSize,
                            wxSL_HORIZONTAL );
    sizer->Add(m_slider, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND , 5);


    //
    //  Create the gauge
    //
    m_gauge = new wxGauge();
    m_gauge->Create(this, wxID_GAUGE, 0, wxDefaultPosition, wxDefaultSize,
                        wxGA_HORIZONTAL | wxGA_SMOOTH);
    sizer->Add(m_gauge, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND , 5);

    //
    // ListCtrl events
    //
    this->Connect( wxID_LISTCTRL, wxEVT_COMMAND_LIST_ITEM_ACTIVATED,
        wxListEventHandler(wxMediaPlayerFrame::OnChangeSong),
        (wxObject*)0, parentFrame);

    //
    // Slider events
    //
    this->Connect(wxID_SLIDER, wxEVT_SCROLL_THUMBTRACK,
                  wxScrollEventHandler(wxMediaPlayerNotebookPage::OnBeginSeek));
    this->Connect(wxID_SLIDER, wxEVT_SCROLL_THUMBRELEASE,
                  wxScrollEventHandler(wxMediaPlayerNotebookPage::OnEndSeek));

    //
    // Media Control events
    //
    this->Connect(wxID_MEDIACTRL, wxEVT_MEDIA_FINISHED,
                  wxMediaEventHandler(wxMediaPlayerNotebookPage::OnMediaFinished));
    this->Connect(wxID_MEDIACTRL, wxEVT_MEDIA_LOADED,
                  wxMediaEventHandler(wxMediaPlayerFrame::OnMediaLoaded),
                  (wxObject*)0, parentFrame);

    //
    // Button events
    //
    this->Connect( wxID_BUTTONPREV, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(wxMediaPlayerFrame::OnPrev),
        (wxObject*)0, parentFrame);
    this->Connect( wxID_BUTTONPLAY, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(wxMediaPlayerFrame::OnPlay),
        (wxObject*)0, parentFrame);
    this->Connect( wxID_BUTTONSTOP, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(wxMediaPlayerFrame::OnStop),
        (wxObject*)0, parentFrame);
    this->Connect( wxID_BUTTONNEXT, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(wxMediaPlayerFrame::OnNext),
        (wxObject*)0, parentFrame);
    this->Connect( wxID_BUTTONVD, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(wxMediaPlayerFrame::OnVolumeDown),
        (wxObject*)0, parentFrame);
    this->Connect( wxID_BUTTONVU, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(wxMediaPlayerFrame::OnVolumeUp),
        (wxObject*)0, parentFrame);
}

// ----------------------------------------------------------------------------
// MyNotebook::OnBeginSeek
//
// Sets m_bIsBeingDragged to true to stop the timer from changing the position
// of our slider
// ----------------------------------------------------------------------------
void wxMediaPlayerNotebookPage::OnBeginSeek(wxScrollEvent& WXUNUSED(event))
{
    m_bIsBeingDragged = true;
}

// ----------------------------------------------------------------------------
// MyNotebook::OnEndSeek
//
// Called from file->seek.
// Called when the user moves the slider -
// seeks to a position within the media
// then sets m_bIsBeingDragged to false to ok the timer to change the position
// ----------------------------------------------------------------------------
void wxMediaPlayerNotebookPage::OnEndSeek(wxScrollEvent& WXUNUSED(event))
{
    if( m_mediactrl->Seek(
            m_slider->GetValue() * 1000
                                   ) == wxInvalidOffset )
        wxMessageBox(wxT("Couldn't seek in movie!"));

    m_bIsBeingDragged = false;
}

// ----------------------------------------------------------------------------
// wxMediaPlayerNotebookPage::IsBeingDragged
//
// Returns true if the user is dragging the slider
// ----------------------------------------------------------------------------
bool wxMediaPlayerNotebookPage::IsBeingDragged()
{
    return m_bIsBeingDragged;
}

// ----------------------------------------------------------------------------
// OnMediaFinished
//
// Called when the media stops playing.
// Here we loop it if the user wants to (has been selected from file menu)
// ----------------------------------------------------------------------------
void wxMediaPlayerNotebookPage::OnMediaFinished(wxMediaEvent& WXUNUSED(event))
{
    if(m_bLoop)
    {
        if ( !m_mediactrl->Play() )
        {
            wxMessageBox(wxT("Couldn't loop movie!"));
            m_playlist->SetItem(m_parentFrame->m_nLastFileId, 0, _T("E"));
        }
        else
            ++m_nLoops;
    }
    else
    {
        m_playlist->SetItem(m_parentFrame->m_nLastFileId, 0, _T("[]"));
    }
}

//
// End of MediaPlayer sample
//
