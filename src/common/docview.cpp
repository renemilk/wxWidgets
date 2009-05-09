/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/docview.cpp
// Purpose:     Document/view classes
// Author:      Julian Smart
// Modified by: Vadim Zeitlin
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_DOC_VIEW_ARCHITECTURE

#include "wx/docview.h"

#ifndef WX_PRECOMP
    #include "wx/list.h"
    #include "wx/string.h"
    #include "wx/utils.h"
    #include "wx/app.h"
    #include "wx/dc.h"
    #include "wx/dialog.h"
    #include "wx/menu.h"
    #include "wx/filedlg.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/msgdlg.h"
    #include "wx/mdi.h"
    #include "wx/choicdlg.h"
#endif

#if wxUSE_PRINTING_ARCHITECTURE
    #include "wx/prntbase.h"
    #include "wx/printdlg.h"
#endif

#include "wx/confbase.h"
#include "wx/filename.h"
#include "wx/file.h"
#include "wx/ffile.h"
#include "wx/cmdproc.h"
#include "wx/tokenzr.h"
#include "wx/filename.h"
#include "wx/stdpaths.h"
#include "wx/vector.h"
#include "wx/scopedarray.h"
#include "wx/scopedptr.h"
#include "wx/except.h"

#if wxUSE_STD_IOSTREAM
    #include "wx/ioswrap.h"
    #include "wx/beforestd.h"
    #if wxUSE_IOSTREAMH
        #include <fstream.h>
    #else
        #include <fstream>
    #endif
    #include "wx/afterstd.h"
#else
    #include "wx/wfstream.h"
#endif

typedef wxVector<wxDocTemplate *> wxDocTemplates;

// ----------------------------------------------------------------------------
// wxWidgets macros
// ----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxDocument, wxEvtHandler)
IMPLEMENT_ABSTRACT_CLASS(wxView, wxEvtHandler)
IMPLEMENT_ABSTRACT_CLASS(wxDocTemplate, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxDocManager, wxEvtHandler)
IMPLEMENT_CLASS(wxDocChildFrame, wxFrame)
IMPLEMENT_CLASS(wxDocParentFrame, wxFrame)

#if wxUSE_PRINTING_ARCHITECTURE
    IMPLEMENT_DYNAMIC_CLASS(wxDocPrintout, wxPrintout)
#endif

IMPLEMENT_DYNAMIC_CLASS(wxFileHistory, wxObject)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// private helpers
// ----------------------------------------------------------------------------

namespace
{

wxString FindExtension(const wxString& path)
{
    wxString ext;
    wxFileName::SplitPath(path, NULL, NULL, &ext);

    // VZ: extensions are considered not case sensitive - is this really a good
    //     idea?
    return ext.MakeLower();
}

// return the string used for the MRU list items in the menu
//
// NB: the index n is 0-based, as usual, but the strings start from 1
wxString GetMRUEntryLabel(int n, const wxString& path)
{
    // we need to quote '&' characters which are used for mnemonics
    wxString pathInMenu(path);
    pathInMenu.Replace("&", "&&");

    return wxString::Format("&%d %s", n + 1, pathInMenu);
}

} // anonymous namespace

// ----------------------------------------------------------------------------
// Definition of wxDocument
// ----------------------------------------------------------------------------

wxDocument::wxDocument(wxDocument *parent)
{
    m_documentModified = false;
    m_documentParent = parent;
    m_documentTemplate = NULL;
    m_commandProcessor = NULL;
    m_savedYet = false;
}

bool wxDocument::DeleteContents()
{
    return true;
}

wxDocument::~wxDocument()
{
    DeleteContents();

    delete m_commandProcessor;

    if (GetDocumentManager())
        GetDocumentManager()->RemoveDocument(this);

    // Not safe to do here, since it'll invoke virtual view functions
    // expecting to see valid derived objects: and by the time we get here,
    // we've called destructors higher up.
    //DeleteAllViews();
}

bool wxDocument::Close()
{
    if ( !OnSaveModified() )
        return false;

    return OnCloseDocument();
}

bool wxDocument::OnCloseDocument()
{
    // Tell all views that we're about to close
    NotifyClosing();
    DeleteContents();
    Modify(false);
    return true;
}

// Note that this implicitly deletes the document when the last view is
// deleted.
bool wxDocument::DeleteAllViews()
{
    wxDocManager* manager = GetDocumentManager();

    // first check if all views agree to be closed
    const wxList::iterator end = m_documentViews.end();
    for ( wxList::iterator i = m_documentViews.begin(); i != end; ++i )
    {
        wxView *view = (wxView *)*i;
        if ( !view->Close() )
            return false;
    }

    // all views agreed to close, now do close them
    if ( m_documentViews.empty() )
    {
        // normally the document would be implicitly deleted when the last view
        // is, but if don't have any views, do it here instead
        if ( manager && manager->GetDocuments().Member(this) )
            delete this;
    }
    else // have views
    {
        // as we delete elements we iterate over, don't use the usual "from
        // begin to end" loop
        for ( ;; )
        {
            wxView *view = (wxView *)*m_documentViews.begin();

            bool isLastOne = m_documentViews.size() == 1;

            // this always deletes the node implicitly and if this is the last
            // view also deletes this object itself (also implicitly, great),
            // so we can't test for m_documentViews.empty() after calling this!
            delete view;

            if ( isLastOne )
                break;
        }
    }

    return true;
}

wxView *wxDocument::GetFirstView() const
{
    if ( m_documentViews.empty() )
        return NULL;

    return static_cast<wxView *>(m_documentViews.GetFirst()->GetData());
}

wxDocManager *wxDocument::GetDocumentManager() const
{
    return m_documentTemplate ? m_documentTemplate->GetDocumentManager() : NULL;
}

bool wxDocument::OnNewDocument()
{
    // notice that there is no need to neither reset nor even check the
    // modified flag here as the document itself is a new object (this is only
    // called from CreateDocument()) and so it shouldn't be saved anyhow even
    // if it is modified -- this could happen if the user code creates
    // documents pre-filled with some user-entered (and which hence must not be
    // lost) information

    SetDocumentSaved(false);

    const wxString name = GetDocumentManager()->MakeNewDocumentName();
    SetTitle(name);
    SetFilename(name, true);

    return true;
}

bool wxDocument::Save()
{
    if ( AlreadySaved() )
        return true;

    if ( m_documentFile.empty() || !m_savedYet )
        return SaveAs();

    return OnSaveDocument(m_documentFile);
}

bool wxDocument::SaveAs()
{
    wxDocTemplate *docTemplate = GetDocumentTemplate();
    if (!docTemplate)
        return false;

#ifdef wxHAS_MULTIPLE_FILEDLG_FILTERS
    wxString filter = docTemplate->GetDescription() + wxT(" (") +
        docTemplate->GetFileFilter() + wxT(")|") +
        docTemplate->GetFileFilter();

    // Now see if there are some other template with identical view and document
    // classes, whose filters may also be used.
    if (docTemplate->GetViewClassInfo() && docTemplate->GetDocClassInfo())
    {
        wxList::compatibility_iterator
            node = docTemplate->GetDocumentManager()->GetTemplates().GetFirst();
        while (node)
        {
            wxDocTemplate *t = (wxDocTemplate*) node->GetData();

            if (t->IsVisible() && t != docTemplate &&
                t->GetViewClassInfo() == docTemplate->GetViewClassInfo() &&
                t->GetDocClassInfo() == docTemplate->GetDocClassInfo())
            {
                // add a '|' to separate this filter from the previous one
                if ( !filter.empty() )
                    filter << wxT('|');

                filter << t->GetDescription()
                       << wxT(" (") << t->GetFileFilter() << wxT(") |")
                       << t->GetFileFilter();
            }

            node = node->GetNext();
        }
    }
#else
    wxString filter = docTemplate->GetFileFilter() ;
#endif

    wxString defaultDir = docTemplate->GetDirectory();
    if ( defaultDir.empty() )
    {
        defaultDir = wxPathOnly(GetFilename());
        if ( defaultDir.empty() )
            defaultDir = GetDocumentManager()->GetLastDirectory();
    }

    wxString fileName = wxFileSelector(_("Save As"),
            defaultDir,
            wxFileNameFromPath(GetFilename()),
            docTemplate->GetDefaultExtension(),
            filter,
            wxFD_SAVE | wxFD_OVERWRITE_PROMPT,
            GetDocumentWindow());

    if (fileName.empty())
        return false; // cancelled by user

    wxString ext;
    wxFileName::SplitPath(fileName, NULL, NULL, &ext);

    if (ext.empty())
    {
        fileName += wxT(".");
        fileName += docTemplate->GetDefaultExtension();
    }

    // Files that were not saved correctly are not added to the FileHistory.
    if (!OnSaveDocument(fileName))
        return false;

    SetTitle(wxFileNameFromPath(fileName));
    SetFilename(fileName, true);    // will call OnChangeFileName automatically

   // A file that doesn't use the default extension of its document template
   // cannot be opened via the FileHistory, so we do not add it.
   if (docTemplate->FileMatchesTemplate(fileName))
   {
       GetDocumentManager()->AddFileToHistory(fileName);
   }
   //else: the user will probably not be able to open the file again, so we
   //      could warn about the wrong file-extension here

   return true;
}

bool wxDocument::OnSaveDocument(const wxString& file)
{
    if ( !file )
        return false;

    if ( !DoSaveDocument(file) )
        return false;

    Modify(false);
    SetFilename(file);
    SetDocumentSaved(true);
#if defined( __WXOSX_MAC__ ) && wxOSX_USE_CARBON
    wxFileName fn(file) ;
    fn.MacSetDefaultTypeAndCreator() ;
#endif
    return true;
}

bool wxDocument::OnOpenDocument(const wxString& file)
{
    // notice that there is no need to check the modified flag here for the
    // reasons explained in OnNewDocument()

    if ( !DoOpenDocument(file) )
        return false;

    SetFilename(file, true);

    // stretching the logic a little this does make sense because the document
    // had been saved into the file we just loaded it from, it just could have
    // happened during a previous program execution, it's just that the name of
    // this method is a bit unfortunate, it should probably have been called
    // HasAssociatedFileName()
    SetDocumentSaved(true);

    UpdateAllViews();

    return true;
}

#if wxUSE_STD_IOSTREAM
wxSTD istream& wxDocument::LoadObject(wxSTD istream& stream)
#else
wxInputStream& wxDocument::LoadObject(wxInputStream& stream)
#endif
{
    return stream;
}

#if wxUSE_STD_IOSTREAM
wxSTD ostream& wxDocument::SaveObject(wxSTD ostream& stream)
#else
wxOutputStream& wxDocument::SaveObject(wxOutputStream& stream)
#endif
{
    return stream;
}

bool wxDocument::Revert()
{
    return false;
}


// Get title, or filename if no title, else unnamed
#if WXWIN_COMPATIBILITY_2_8
bool wxDocument::GetPrintableName(wxString& buf) const
{
    // this function can not only be overridden by the user code but also
    // called by it so we need to ensure that we return the same thing as
    // GetUserReadableName() but we can't call it because this would result in
    // an infinite recursion, hence we use the helper DoGetUserReadableName()
    buf = DoGetUserReadableName();

    return true;
}
#endif // WXWIN_COMPATIBILITY_2_8

wxString wxDocument::GetUserReadableName() const
{
#if WXWIN_COMPATIBILITY_2_8
    // we need to call the old virtual function to ensure that the overridden
    // version of it is still called
    wxString name;
    if ( GetPrintableName(name) )
        return name;
#endif // WXWIN_COMPATIBILITY_2_8

    return DoGetUserReadableName();
}

wxString wxDocument::DoGetUserReadableName() const
{
    if ( !m_documentTitle.empty() )
        return m_documentTitle;

    if ( !m_documentFile.empty() )
        return wxFileNameFromPath(m_documentFile);

    return _("unnamed");
}

wxWindow *wxDocument::GetDocumentWindow() const
{
    wxView * const view = GetFirstView();

    return view ? view->GetFrame() : wxTheApp->GetTopWindow();
}

wxCommandProcessor *wxDocument::OnCreateCommandProcessor()
{
    return new wxCommandProcessor;
}

// true if safe to close
bool wxDocument::OnSaveModified()
{
    if ( IsModified() )
    {
        switch ( wxMessageBox
                 (
                    wxString::Format
                    (
                     _("Do you want to save changes to %s?"),
                     GetUserReadableName()
                    ),
                    wxTheApp->GetAppDisplayName(),
                    wxYES_NO | wxCANCEL | wxICON_QUESTION | wxCENTRE
                 ) )
        {
            case wxNO:
                Modify(false);
                break;

            case wxYES:
                return Save();

            case wxCANCEL:
                return false;
        }
    }

    return true;
}

bool wxDocument::Draw(wxDC& WXUNUSED(context))
{
    return true;
}

bool wxDocument::AddView(wxView *view)
{
    if ( !m_documentViews.Member(view) )
    {
        m_documentViews.Append(view);
        OnChangedViewList();
    }
    return true;
}

bool wxDocument::RemoveView(wxView *view)
{
    (void)m_documentViews.DeleteObject(view);
    OnChangedViewList();
    return true;
}

bool wxDocument::OnCreate(const wxString& WXUNUSED(path), long flags)
{
    return GetDocumentTemplate()->CreateView(this, flags) != NULL;
}

// Called after a view is added or removed.
// The default implementation deletes the document if
// there are no more views.
void wxDocument::OnChangedViewList()
{
    if ( m_documentViews.empty() && OnSaveModified() )
        delete this;
}

void wxDocument::UpdateAllViews(wxView *sender, wxObject *hint)
{
    wxList::compatibility_iterator node = m_documentViews.GetFirst();
    while (node)
    {
        wxView *view = (wxView *)node->GetData();
        if (view != sender)
            view->OnUpdate(sender, hint);
        node = node->GetNext();
    }
}

void wxDocument::NotifyClosing()
{
    wxList::compatibility_iterator node = m_documentViews.GetFirst();
    while (node)
    {
        wxView *view = (wxView *)node->GetData();
        view->OnClosingDocument();
        node = node->GetNext();
    }
}

void wxDocument::SetFilename(const wxString& filename, bool notifyViews)
{
    m_documentFile = filename;
    OnChangeFilename(notifyViews);
}

void wxDocument::OnChangeFilename(bool notifyViews)
{
    if ( notifyViews )
    {
        // Notify the views that the filename has changed
        wxList::compatibility_iterator node = m_documentViews.GetFirst();
        while (node)
        {
            wxView *view = (wxView *)node->GetData();
            view->OnChangeFilename();
            node = node->GetNext();
        }
    }
}

bool wxDocument::DoSaveDocument(const wxString& file)
{
#if wxUSE_STD_IOSTREAM
    wxSTD ofstream store(file.mb_str(), wxSTD ios::binary);
    if ( !store )
#else
    wxFileOutputStream store(file);
    if ( store.GetLastError() != wxSTREAM_NO_ERROR )
#endif
    {
        wxLogError(_("File \"%s\" could not be opened for writing."), file);
        return false;
    }

    if (!SaveObject(store))
    {
        wxLogError(_("Failed to save document to the file \"%s\"."), file);
        return false;
    }

    return true;
}

bool wxDocument::DoOpenDocument(const wxString& file)
{
#if wxUSE_STD_IOSTREAM
    wxSTD ifstream store(file.mb_str(), wxSTD ios::binary);
    if ( !store )
#else
    wxFileInputStream store(file);
    if (store.GetLastError() != wxSTREAM_NO_ERROR || !store.IsOk())
#endif
    {
        wxLogError(_("File \"%s\" could not be opened for reading."), file);
        return false;
    }

#if wxUSE_STD_IOSTREAM
    LoadObject(store);
    if ( !store )
#else
    int res = LoadObject(store).GetLastError();
    if ( res != wxSTREAM_NO_ERROR && res != wxSTREAM_EOF )
#endif
    {
        wxLogError(_("Failed to read document from the file \"%s\"."), file);
        return false;
    }

    return true;
}


// ----------------------------------------------------------------------------
// Document view
// ----------------------------------------------------------------------------

wxView::wxView()
{
    m_viewDocument = NULL;

    m_viewFrame = NULL;

    m_docChildFrame = NULL;
}

wxView::~wxView()
{
    GetDocumentManager()->ActivateView(this, false);

    // reset our frame view first, before removing it from the document as
    // SetView(NULL) is a simple call while RemoveView() may result in user
    // code being executed and this user code can, for example, show a message
    // box which would result in an activation event for m_docChildFrame and so
    // could reactivate the view being destroyed -- unless we reset it first
    if ( m_docChildFrame && m_docChildFrame->GetView() == this )
    {
        // prevent it from doing anything with us
        m_docChildFrame->SetView(NULL);

        // it doesn't make sense to leave the frame alive if its associated
        // view doesn't exist any more so unconditionally close it as well
        //
        // notice that we only get here if m_docChildFrame is non-NULL in the
        // first place and it will be always NULL if we're deleted because our
        // frame was closed, so this only catches the case of directly deleting
        // the view, as it happens if its creation fails in wxDocTemplate::
        // CreateView() for example
        m_docChildFrame->GetWindow()->Destroy();
    }

    if ( m_viewDocument )
        m_viewDocument->RemoveView(this);
}

void wxView::SetDocChildFrame(wxDocChildFrameAnyBase *docChildFrame)
{
    SetFrame(docChildFrame ? docChildFrame->GetWindow() : NULL);
    m_docChildFrame = docChildFrame;
}

bool wxView::TryBefore(wxEvent& event)
{
    wxDocument * const doc = GetDocument();
    return doc && doc->ProcessEventHere(event);
}

void wxView::OnActivateView(bool WXUNUSED(activate),
                            wxView *WXUNUSED(activeView),
                            wxView *WXUNUSED(deactiveView))
{
}

void wxView::OnPrint(wxDC *dc, wxObject *WXUNUSED(info))
{
    OnDraw(dc);
}

void wxView::OnUpdate(wxView *WXUNUSED(sender), wxObject *WXUNUSED(hint))
{
}

void wxView::OnChangeFilename()
{
    // GetFrame can return wxWindow rather than wxTopLevelWindow due to
    // generic MDI implementation so use SetLabel rather than SetTitle.
    // It should cause SetTitle() for top level windows.
    wxWindow *win = GetFrame();
    if (!win) return;

    wxDocument *doc = GetDocument();
    if (!doc) return;

    win->SetLabel(doc->GetUserReadableName());
}

void wxView::SetDocument(wxDocument *doc)
{
    m_viewDocument = doc;
    if (doc)
        doc->AddView(this);
}

bool wxView::Close(bool deleteWindow)
{
    return OnClose(deleteWindow);
}

void wxView::Activate(bool activate)
{
    if (GetDocument() && GetDocumentManager())
    {
        OnActivateView(activate, this, GetDocumentManager()->GetCurrentView());
        GetDocumentManager()->ActivateView(this, activate);
    }
}

bool wxView::OnClose(bool WXUNUSED(deleteWindow))
{
    return GetDocument() ? GetDocument()->Close() : true;
}

#if wxUSE_PRINTING_ARCHITECTURE
wxPrintout *wxView::OnCreatePrintout()
{
    return new wxDocPrintout(this);
}
#endif // wxUSE_PRINTING_ARCHITECTURE

// ----------------------------------------------------------------------------
// wxDocTemplate
// ----------------------------------------------------------------------------

wxDocTemplate::wxDocTemplate(wxDocManager *manager,
                             const wxString& descr,
                             const wxString& filter,
                             const wxString& dir,
                             const wxString& ext,
                             const wxString& docTypeName,
                             const wxString& viewTypeName,
                             wxClassInfo *docClassInfo,
                             wxClassInfo *viewClassInfo,
                             long flags)
{
    m_documentManager = manager;
    m_description = descr;
    m_directory = dir;
    m_defaultExt = ext;
    m_fileFilter = filter;
    m_flags = flags;
    m_docTypeName = docTypeName;
    m_viewTypeName = viewTypeName;
    m_documentManager->AssociateTemplate(this);

    m_docClassInfo = docClassInfo;
    m_viewClassInfo = viewClassInfo;
}

wxDocTemplate::~wxDocTemplate()
{
    m_documentManager->DisassociateTemplate(this);
}

// Tries to dynamically construct an object of the right class.
wxDocument *wxDocTemplate::CreateDocument(const wxString& path, long flags)
{
    // InitDocument() is supposed to delete the document object if its
    // initialization fails so don't use wxScopedPtr<> here: this is fragile
    // but unavoidable because the default implementation uses CreateView()
    // which may -- or not -- create a wxView and if it does create it and its
    // initialization fails then the view destructor will delete the document
    // (via RemoveView()) and as we can't distinguish between the two cases we
    // just have to assume that it always deletes it in case of failure
    wxDocument * const doc = DoCreateDocument();

    return doc && InitDocument(doc, path, flags) ? doc : NULL;
}

bool
wxDocTemplate::InitDocument(wxDocument* doc, const wxString& path, long flags)
{
    doc->SetFilename(path);
    doc->SetDocumentTemplate(this);
    GetDocumentManager()->AddDocument(doc);
    doc->SetCommandProcessor(doc->OnCreateCommandProcessor());

    if (doc->OnCreate(path, flags))
        return true;

    if (GetDocumentManager()->GetDocuments().Member(doc))
        doc->DeleteAllViews();
    return false;
}

wxView *wxDocTemplate::CreateView(wxDocument *doc, long flags)
{
    wxScopedPtr<wxView> view(DoCreateView());
    if ( !view )
        return NULL;

    view->SetDocument(doc);
    if ( !view->OnCreate(doc, flags) )
        return NULL;

    return view.release();
}

// The default (very primitive) format detection: check is the extension is
// that of the template
bool wxDocTemplate::FileMatchesTemplate(const wxString& path)
{
    wxStringTokenizer parser (GetFileFilter(), wxT(";"));
    wxString anything = wxT ("*");
    while (parser.HasMoreTokens())
    {
        wxString filter = parser.GetNextToken();
        wxString filterExt = FindExtension (filter);
        if ( filter.IsSameAs (anything)    ||
             filterExt.IsSameAs (anything) ||
             filterExt.IsSameAs (FindExtension (path)) )
            return true;
    }
    return GetDefaultExtension().IsSameAs(FindExtension(path));
}

wxDocument *wxDocTemplate::DoCreateDocument()
{
    if (!m_docClassInfo)
        return NULL;

    return static_cast<wxDocument *>(m_docClassInfo->CreateObject());
}

wxView *wxDocTemplate::DoCreateView()
{
    if (!m_viewClassInfo)
        return NULL;

    return static_cast<wxView *>(m_viewClassInfo->CreateObject());
}

// ----------------------------------------------------------------------------
// wxDocManager
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxDocManager, wxEvtHandler)
    EVT_MENU(wxID_OPEN, wxDocManager::OnFileOpen)
    EVT_MENU(wxID_CLOSE, wxDocManager::OnFileClose)
    EVT_MENU(wxID_CLOSE_ALL, wxDocManager::OnFileCloseAll)
    EVT_MENU(wxID_REVERT, wxDocManager::OnFileRevert)
    EVT_MENU(wxID_NEW, wxDocManager::OnFileNew)
    EVT_MENU(wxID_SAVE, wxDocManager::OnFileSave)
    EVT_MENU(wxID_SAVEAS, wxDocManager::OnFileSaveAs)
    EVT_MENU(wxID_UNDO, wxDocManager::OnUndo)
    EVT_MENU(wxID_REDO, wxDocManager::OnRedo)

    EVT_UPDATE_UI(wxID_OPEN, wxDocManager::OnUpdateFileOpen)
    EVT_UPDATE_UI(wxID_CLOSE, wxDocManager::OnUpdateDisableIfNoDoc)
    EVT_UPDATE_UI(wxID_CLOSE_ALL, wxDocManager::OnUpdateDisableIfNoDoc)
    EVT_UPDATE_UI(wxID_REVERT, wxDocManager::OnUpdateDisableIfNoDoc)
    EVT_UPDATE_UI(wxID_NEW, wxDocManager::OnUpdateFileNew)
    EVT_UPDATE_UI(wxID_SAVE, wxDocManager::OnUpdateFileSave)
    EVT_UPDATE_UI(wxID_SAVEAS, wxDocManager::OnUpdateDisableIfNoDoc)
    EVT_UPDATE_UI(wxID_UNDO, wxDocManager::OnUpdateUndo)
    EVT_UPDATE_UI(wxID_REDO, wxDocManager::OnUpdateRedo)

#if wxUSE_PRINTING_ARCHITECTURE
    EVT_MENU(wxID_PRINT, wxDocManager::OnPrint)
    EVT_MENU(wxID_PREVIEW, wxDocManager::OnPreview)

    EVT_UPDATE_UI(wxID_PRINT, wxDocManager::OnUpdateDisableIfNoDoc)
    EVT_UPDATE_UI(wxID_PREVIEW, wxDocManager::OnUpdateDisableIfNoDoc)
#endif
END_EVENT_TABLE()

wxDocManager* wxDocManager::sm_docManager = NULL;

wxDocManager::wxDocManager(long WXUNUSED(flags), bool initialize)
{
    wxASSERT_MSG( !sm_docManager, "multiple wxDocManagers not allowed" );

    sm_docManager = this;

    m_defaultDocumentNameCounter = 1;
    m_currentView = NULL;
    m_maxDocsOpen = INT_MAX;
    m_fileHistory = NULL;
    if ( initialize )
        Initialize();
}

wxDocManager::~wxDocManager()
{
    Clear();
    delete m_fileHistory;
    sm_docManager = NULL;
}

// closes the specified document
bool wxDocManager::CloseDocument(wxDocument* doc, bool force)
{
    if ( !doc->Close() && !force )
        return false;

    // Implicitly deletes the document when
    // the last view is deleted
    doc->DeleteAllViews();

    // Check we're really deleted
    if (m_docs.Member(doc))
        delete doc;

    return true;
}

bool wxDocManager::CloseDocuments(bool force)
{
    wxList::compatibility_iterator node = m_docs.GetFirst();
    while (node)
    {
        wxDocument *doc = (wxDocument *)node->GetData();
        wxList::compatibility_iterator next = node->GetNext();

        if (!CloseDocument(doc, force))
            return false;

        // This assumes that documents are not connected in
        // any way, i.e. deleting one document does NOT
        // delete another.
        node = next;
    }
    return true;
}

bool wxDocManager::Clear(bool force)
{
    if (!CloseDocuments(force))
        return false;

    m_currentView = NULL;

    wxList::compatibility_iterator node = m_templates.GetFirst();
    while (node)
    {
        wxDocTemplate *templ = (wxDocTemplate*) node->GetData();
        wxList::compatibility_iterator next = node->GetNext();
        delete templ;
        node = next;
    }
    return true;
}

bool wxDocManager::Initialize()
{
    m_fileHistory = OnCreateFileHistory();
    return true;
}

wxString wxDocManager::GetLastDirectory() const
{
    // use the system-dependent default location for the document files if
    // we're being opened for the first time
    if ( m_lastDirectory.empty() )
    {
        wxDocManager * const self = const_cast<wxDocManager *>(this);
        self->m_lastDirectory = wxStandardPaths::Get().GetAppDocumentsDir();
    }

    return m_lastDirectory;
}

wxFileHistory *wxDocManager::OnCreateFileHistory()
{
    return new wxFileHistory;
}

void wxDocManager::OnFileClose(wxCommandEvent& WXUNUSED(event))
{
    wxDocument *doc = GetCurrentDocument();
    if (!doc)
        return;
    if (doc->Close())
    {
        doc->DeleteAllViews();
        if (m_docs.Member(doc))
            delete doc;
    }
}

void wxDocManager::OnFileCloseAll(wxCommandEvent& WXUNUSED(event))
{
    CloseDocuments(false);
}

void wxDocManager::OnFileNew(wxCommandEvent& WXUNUSED(event))
{
    CreateNewDocument();
}

void wxDocManager::OnFileOpen(wxCommandEvent& WXUNUSED(event))
{
    if ( !CreateDocument("") )
    {
        OnOpenFileFailure();
    }
}

void wxDocManager::OnFileRevert(wxCommandEvent& WXUNUSED(event))
{
    wxDocument *doc = GetCurrentDocument();
    if (!doc)
        return;
    doc->Revert();
}

void wxDocManager::OnFileSave(wxCommandEvent& WXUNUSED(event))
{
    wxDocument *doc = GetCurrentDocument();
    if (!doc)
        return;
    doc->Save();
}

void wxDocManager::OnFileSaveAs(wxCommandEvent& WXUNUSED(event))
{
    wxDocument *doc = GetCurrentDocument();
    if (!doc)
        return;
    doc->SaveAs();
}

void wxDocManager::OnPrint(wxCommandEvent& WXUNUSED(event))
{
#if wxUSE_PRINTING_ARCHITECTURE
    wxView *view = GetActiveView();
    if (!view)
        return;

    wxPrintout *printout = view->OnCreatePrintout();
    if (printout)
    {
        wxPrinter printer;
        printer.Print(view->GetFrame(), printout, true);

        delete printout;
    }
#endif // wxUSE_PRINTING_ARCHITECTURE
}

void wxDocManager::OnPreview(wxCommandEvent& WXUNUSED(event))
{
#if wxUSE_PRINTING_ARCHITECTURE
    wxView *view = GetActiveView();
    if (!view)
        return;

    wxPrintout *printout = view->OnCreatePrintout();
    if (printout)
    {
        // Pass two printout objects: for preview, and possible printing.
        wxPrintPreviewBase *
            preview = new wxPrintPreview(printout, view->OnCreatePrintout());
        if ( !preview->Ok() )
        {
            delete preview;
            wxLogError(_("Print preview creation failed."));
            return;
        }

        wxPreviewFrame *
            frame = new wxPreviewFrame(preview, wxTheApp->GetTopWindow(),
                                       _("Print Preview"));
        frame->Centre(wxBOTH);
        frame->Initialize();
        frame->Show(true);
    }
#endif // wxUSE_PRINTING_ARCHITECTURE
}

void wxDocManager::OnUndo(wxCommandEvent& event)
{
    wxCommandProcessor * const cmdproc = GetCurrentCommandProcessor();
    if ( !cmdproc )
    {
        event.Skip();
        return;
    }

    cmdproc->Undo();
}

void wxDocManager::OnRedo(wxCommandEvent& event)
{
    wxCommandProcessor * const cmdproc = GetCurrentCommandProcessor();
    if ( !cmdproc )
    {
        event.Skip();
        return;
    }

    cmdproc->Redo();
}

// Handlers for UI update commands

void wxDocManager::OnUpdateFileOpen(wxUpdateUIEvent& event)
{
    // CreateDocument() (which is called from OnFileOpen) may succeed
    // only when there is at least a template:
    event.Enable( GetTemplates().GetCount()>0 );
}

void wxDocManager::OnUpdateDisableIfNoDoc(wxUpdateUIEvent& event)
{
    event.Enable( GetCurrentDocument() != NULL );
}

void wxDocManager::OnUpdateFileNew(wxUpdateUIEvent& event)
{
    // CreateDocument() (which is called from OnFileNew) may succeed
    // only when there is at least a template:
    event.Enable( GetTemplates().GetCount()>0 );
}

void wxDocManager::OnUpdateFileSave(wxUpdateUIEvent& event)
{
    wxDocument * const doc = GetCurrentDocument();
    event.Enable( doc && !doc->AlreadySaved() );
}

void wxDocManager::OnUpdateUndo(wxUpdateUIEvent& event)
{
    wxCommandProcessor * const cmdproc = GetCurrentCommandProcessor();
    if ( !cmdproc )
    {
        event.Enable(false);
        return;
    }

    event.Enable(cmdproc->CanUndo());
    cmdproc->SetMenuStrings();
}

void wxDocManager::OnUpdateRedo(wxUpdateUIEvent& event)
{
    wxCommandProcessor * const cmdproc = GetCurrentCommandProcessor();
    if ( !cmdproc )
    {
        event.Enable(false);
        return;
    }

    event.Enable(cmdproc->CanRedo());
    cmdproc->SetMenuStrings();
}

wxView *wxDocManager::GetActiveView() const
{
    wxView *view = GetCurrentView();

    if ( !view && !m_docs.empty() )
    {
        // if we have exactly one document, consider its view to be the current
        // one
        //
        // VZ: I'm not exactly sure why is this needed but this is how this
        //     code used to behave before the bug #9518 was fixed and it seems
        //     safer to preserve the old logic
        wxList::compatibility_iterator node = m_docs.GetFirst();
        if ( !node->GetNext() )
        {
            wxDocument *doc = static_cast<wxDocument *>(node->GetData());
            view = doc->GetFirstView();
        }
        //else: we have more than one document
    }

    return view;
}

bool wxDocManager::TryBefore(wxEvent& event)
{
    wxView * const view = GetActiveView();
    return view && view->ProcessEventHere(event);
}

namespace
{

// helper function: return only the visible templates
wxDocTemplates GetVisibleTemplates(const wxList& allTemplates)
{
    // select only the visible templates
    const size_t totalNumTemplates = allTemplates.GetCount();
    wxDocTemplates templates;
    if ( totalNumTemplates )
    {
        templates.reserve(totalNumTemplates);

        for ( wxList::const_iterator i = allTemplates.begin(),
                                   end = allTemplates.end();
              i != end;
              ++i )
        {
            wxDocTemplate * const temp = (wxDocTemplate *)*i;
            if ( temp->IsVisible() )
                templates.push_back(temp);
        }
    }

    return templates;
}

} // anonymous namespace

wxDocument *wxDocManager::CreateDocument(const wxString& pathOrig, long flags)
{
    // this ought to be const but SelectDocumentType/Path() are not
    // const-correct and can't be changed as, being virtual, this risks
    // breaking user code overriding them
    wxDocTemplates templates(GetVisibleTemplates(m_templates));
    const size_t numTemplates = templates.size();
    if ( !numTemplates )
    {
        // no templates can be used, can't create document
        return NULL;
    }


    // normally user should select the template to use but wxDOC_SILENT flag we
    // choose one ourselves
    wxString path = pathOrig;   // may be modified below
    wxDocTemplate *temp;
    if ( flags & wxDOC_SILENT )
    {
        wxASSERT_MSG( !path.empty(),
                      "using empty path with wxDOC_SILENT doesn't make sense" );

        temp = FindTemplateForPath(path);
        if ( !temp )
        {
            wxLogWarning(_("The format of file '%s' couldn't be determined."),
                         path);
        }
    }
    else // not silent, ask the user
    {
        // for the new file we need just the template, for an existing one we
        // need the template and the path, unless it's already specified
        if ( (flags & wxDOC_NEW) || !path.empty() )
            temp = SelectDocumentType(&templates[0], numTemplates);
        else
            temp = SelectDocumentPath(&templates[0], numTemplates, path, flags);
    }

    if ( !temp )
        return NULL;

    // check whether the document with this path is already opened
    if ( !path.empty() )
    {
        const wxFileName fn(path);
        for ( wxList::const_iterator i = m_docs.begin(); i != m_docs.end(); ++i )
        {
            wxDocument * const doc = (wxDocument*)*i;

            if ( fn == doc->GetFilename() )
            {
                // file already open, just activate it and return
                if ( doc->GetFirstView() )
                {
                    ActivateView(doc->GetFirstView());
                    if ( doc->GetDocumentWindow() )
                        doc->GetDocumentWindow()->SetFocus();
                    return doc;
                }
            }
        }
    }


    // no, we need to create a new document


    // if we've reached the max number of docs, close the first one.
    if ( (int)GetDocuments().GetCount() >= m_maxDocsOpen )
    {
        if ( !CloseDocument((wxDocument *)GetDocuments().GetFirst()->GetData()) )
        {
            // can't open the new document if closing the old one failed
            return NULL;
        }
    }


    // do create and initialize the new document finally
    wxDocument * const docNew = temp->CreateDocument(path, flags);
    if ( !docNew )
        return NULL;

    docNew->SetDocumentName(temp->GetDocumentName());
    docNew->SetDocumentTemplate(temp);

    wxTRY
    {
        // call the appropriate function depending on whether we're creating a
        // new file or opening an existing one
        if ( !(flags & wxDOC_NEW ? docNew->OnNewDocument()
                                 : docNew->OnOpenDocument(path)) )
        {
            docNew->DeleteAllViews();
            return NULL;
        }
    }
    wxCATCH_ALL( docNew->DeleteAllViews(); throw; )

    // add the successfully opened file to MRU, but only if we're going to be
    // able to reopen it successfully later which requires the template for
    // this document to be retrievable from the file extension
    if ( !(flags & wxDOC_NEW) && temp->FileMatchesTemplate(path) )
        AddFileToHistory(path);

    return docNew;
}

wxView *wxDocManager::CreateView(wxDocument *doc, long flags)
{
    wxDocTemplates templates(GetVisibleTemplates(m_templates));
    const size_t numTemplates = templates.size();

    if ( numTemplates == 0 )
        return NULL;

    wxDocTemplate * const
    temp = numTemplates == 1 ? templates[0]
                             : SelectViewType(&templates[0], numTemplates);

    if ( !temp )
        return NULL;

    wxView *view = temp->CreateView(doc, flags);
    if ( view )
        view->SetViewName(temp->GetViewName());
    return view;
}

// Not yet implemented
void
wxDocManager::DeleteTemplate(wxDocTemplate *WXUNUSED(temp), long WXUNUSED(flags))
{
}

// Not yet implemented
bool wxDocManager::FlushDoc(wxDocument *WXUNUSED(doc))
{
    return false;
}

wxDocument *wxDocManager::GetCurrentDocument() const
{
    wxView * const view = GetActiveView();
    return view ? view->GetDocument() : NULL;
}

wxCommandProcessor *wxDocManager::GetCurrentCommandProcessor() const
{
    wxDocument * const doc = GetCurrentDocument();
    return doc ? doc->GetCommandProcessor() : NULL;
}

// Make a default name for a new document
#if WXWIN_COMPATIBILITY_2_8
bool wxDocManager::MakeDefaultName(wxString& WXUNUSED(name))
{
    // we consider that this function can only be overridden by the user code,
    // not called by it as it only makes sense to call it internally, so we
    // don't bother to return anything from here
    return false;
}
#endif // WXWIN_COMPATIBILITY_2_8

wxString wxDocManager::MakeNewDocumentName()
{
    wxString name;

#if WXWIN_COMPATIBILITY_2_8
    if ( !MakeDefaultName(name) )
#endif // WXWIN_COMPATIBILITY_2_8
    {
        name.Printf(_("unnamed%d"), m_defaultDocumentNameCounter);
        m_defaultDocumentNameCounter++;
    }

    return name;
}

// Make a frame title (override this to do something different)
// If docName is empty, a document is not currently active.
wxString wxDocManager::MakeFrameTitle(wxDocument* doc)
{
    wxString appName = wxTheApp->GetAppDisplayName();
    wxString title;
    if (!doc)
        title = appName;
    else
    {
        wxString docName = doc->GetUserReadableName();
        title = docName + wxString(_(" - ")) + appName;
    }
    return title;
}


// Not yet implemented
wxDocTemplate *wxDocManager::MatchTemplate(const wxString& WXUNUSED(path))
{
    return NULL;
}

// File history management
void wxDocManager::AddFileToHistory(const wxString& file)
{
    if (m_fileHistory)
        m_fileHistory->AddFileToHistory(file);
}

void wxDocManager::RemoveFileFromHistory(size_t i)
{
    if (m_fileHistory)
        m_fileHistory->RemoveFileFromHistory(i);
}

wxString wxDocManager::GetHistoryFile(size_t i) const
{
    wxString histFile;

    if (m_fileHistory)
        histFile = m_fileHistory->GetHistoryFile(i);

    return histFile;
}

void wxDocManager::FileHistoryUseMenu(wxMenu *menu)
{
    if (m_fileHistory)
        m_fileHistory->UseMenu(menu);
}

void wxDocManager::FileHistoryRemoveMenu(wxMenu *menu)
{
    if (m_fileHistory)
        m_fileHistory->RemoveMenu(menu);
}

#if wxUSE_CONFIG
void wxDocManager::FileHistoryLoad(const wxConfigBase& config)
{
    if (m_fileHistory)
        m_fileHistory->Load(config);
}

void wxDocManager::FileHistorySave(wxConfigBase& config)
{
    if (m_fileHistory)
        m_fileHistory->Save(config);
}
#endif

void wxDocManager::FileHistoryAddFilesToMenu(wxMenu* menu)
{
    if (m_fileHistory)
        m_fileHistory->AddFilesToMenu(menu);
}

void wxDocManager::FileHistoryAddFilesToMenu()
{
    if (m_fileHistory)
        m_fileHistory->AddFilesToMenu();
}

size_t wxDocManager::GetHistoryFilesCount() const
{
    return m_fileHistory ? m_fileHistory->GetCount() : 0;
}


// Find out the document template via matching in the document file format
// against that of the template
wxDocTemplate *wxDocManager::FindTemplateForPath(const wxString& path)
{
    wxDocTemplate *theTemplate = NULL;

    // Find the template which this extension corresponds to
    for (size_t i = 0; i < m_templates.GetCount(); i++)
    {
        wxDocTemplate *temp = (wxDocTemplate *)m_templates.Item(i)->GetData();
        if ( temp->FileMatchesTemplate(path) )
        {
            theTemplate = temp;
            break;
        }
    }
    return theTemplate;
}

// Prompts user to open a file, using file specs in templates.
// Must extend the file selector dialog or implement own; OR
// match the extension to the template extension.

wxDocTemplate *wxDocManager::SelectDocumentPath(wxDocTemplate **templates,
                                                int noTemplates,
                                                wxString& path,
                                                long WXUNUSED(flags),
                                                bool WXUNUSED(save))
{
#ifdef wxHAS_MULTIPLE_FILEDLG_FILTERS
    wxString descrBuf;

    for (int i = 0; i < noTemplates; i++)
    {
        if (templates[i]->IsVisible())
        {
            // add a '|' to separate this filter from the previous one
            if ( !descrBuf.empty() )
                descrBuf << wxT('|');

            descrBuf << templates[i]->GetDescription()
                << wxT(" (") << templates[i]->GetFileFilter() << wxT(") |")
                << templates[i]->GetFileFilter();
        }
    }
#else
    wxString descrBuf = wxT("*.*");
    wxUnusedVar(noTemplates);
#endif

    int FilterIndex = -1;

    wxString pathTmp = wxFileSelectorEx(_("Open File"),
                                        GetLastDirectory(),
                                        wxEmptyString,
                                        &FilterIndex,
                                        descrBuf);

    wxDocTemplate *theTemplate = NULL;
    if (!pathTmp.empty())
    {
        if (!wxFileExists(pathTmp))
        {
            wxString msgTitle;
            if (!wxTheApp->GetAppDisplayName().empty())
                msgTitle = wxTheApp->GetAppDisplayName();
            else
                msgTitle = wxString(_("File error"));

            wxMessageBox(_("Sorry, could not open this file."),
                         msgTitle,
                         wxOK | wxICON_EXCLAMATION | wxCENTRE);

            path = wxEmptyString;
            return NULL;
        }

        SetLastDirectory(wxPathOnly(pathTmp));

        path = pathTmp;

        // first choose the template using the extension, if this fails (i.e.
        // wxFileSelectorEx() didn't fill it), then use the path
        if ( FilterIndex != -1 )
            theTemplate = templates[FilterIndex];
        if ( !theTemplate )
            theTemplate = FindTemplateForPath(path);
        if ( !theTemplate )
        {
            // Since we do not add files with non-default extensions to the
            // file history this can only happen if the application changes the
            // allowed templates in runtime.
            wxMessageBox(_("Sorry, the format for this file is unknown."),
                         _("Open File"),
                         wxOK | wxICON_EXCLAMATION | wxCENTRE);
        }
    }
    else
    {
        path.clear();
    }

    return theTemplate;
}

wxDocTemplate *wxDocManager::SelectDocumentType(wxDocTemplate **templates,
                                                int noTemplates, bool sort)
{
    wxArrayString strings;
    wxScopedArray<wxDocTemplate *> data(new wxDocTemplate *[noTemplates]);
    int i;
    int n = 0;

    for (i = 0; i < noTemplates; i++)
    {
        if (templates[i]->IsVisible())
        {
            int j;
            bool want = true;
            for (j = 0; j < n; j++)
            {
                //filter out NOT unique documents + view combinations
                if ( templates[i]->m_docTypeName == data[j]->m_docTypeName &&
                     templates[i]->m_viewTypeName == data[j]->m_viewTypeName
                   )
                    want = false;
            }

            if ( want )
            {
                strings.Add(templates[i]->m_description);

                data[n] = templates[i];
                n ++;
            }
        }
    }  // for

    if (sort)
    {
        strings.Sort(); // ascending sort
        // Yes, this will be slow, but template lists
        // are typically short.
        int j;
        n = strings.Count();
        for (i = 0; i < n; i++)
        {
            for (j = 0; j < noTemplates; j++)
            {
                if (strings[i] == templates[j]->m_description)
                    data[i] = templates[j];
            }
        }
    }

    wxDocTemplate *theTemplate;

    switch ( n )
    {
        case 0:
            // no visible templates, hence nothing to choose from
            theTemplate = NULL;
            break;

        case 1:
            // don't propose the user to choose if he has no choice
            theTemplate = data[0];
            break;

        default:
            // propose the user to choose one of several
            theTemplate = (wxDocTemplate *)wxGetSingleChoiceData
                          (
                            _("Select a document template"),
                            _("Templates"),
                            strings,
                            (void **)data.get()
                          );
    }

    return theTemplate;
}

wxDocTemplate *wxDocManager::SelectViewType(wxDocTemplate **templates,
                                            int noTemplates, bool sort)
{
    wxArrayString strings;
    wxScopedArray<wxDocTemplate *> data(new wxDocTemplate *[noTemplates]);
    int i;
    int n = 0;

    for (i = 0; i < noTemplates; i++)
    {
        wxDocTemplate *templ = templates[i];
        if ( templ->IsVisible() && !templ->GetViewName().empty() )
        {
            int j;
            bool want = true;
            for (j = 0; j < n; j++)
            {
                //filter out NOT unique views
                if ( templates[i]->m_viewTypeName == data[j]->m_viewTypeName )
                    want = false;
            }

            if ( want )
            {
                strings.Add(templ->m_viewTypeName);
                data[n] = templ;
                n ++;
            }
        }
    }

    if (sort)
    {
        strings.Sort(); // ascending sort
        // Yes, this will be slow, but template lists
        // are typically short.
        int j;
        n = strings.Count();
        for (i = 0; i < n; i++)
        {
            for (j = 0; j < noTemplates; j++)
            {
                if (strings[i] == templates[j]->m_viewTypeName)
                    data[i] = templates[j];
            }
        }
    }

    wxDocTemplate *theTemplate;

    // the same logic as above
    switch ( n )
    {
        case 0:
            theTemplate = NULL;
            break;

        case 1:
            theTemplate = data[0];
            break;

        default:
            theTemplate = (wxDocTemplate *)wxGetSingleChoiceData
                          (
                            _("Select a document view"),
                            _("Views"),
                            strings,
                            (void **)data.get()
                          );

    }

    return theTemplate;
}

void wxDocManager::AssociateTemplate(wxDocTemplate *temp)
{
    if (!m_templates.Member(temp))
        m_templates.Append(temp);
}

void wxDocManager::DisassociateTemplate(wxDocTemplate *temp)
{
    m_templates.DeleteObject(temp);
}

// Add and remove a document from the manager's list
void wxDocManager::AddDocument(wxDocument *doc)
{
    if (!m_docs.Member(doc))
        m_docs.Append(doc);
}

void wxDocManager::RemoveDocument(wxDocument *doc)
{
    m_docs.DeleteObject(doc);
}

// Views or windows should inform the document manager
// when a view is going in or out of focus
void wxDocManager::ActivateView(wxView *view, bool activate)
{
    if ( activate )
    {
        m_currentView = view;
    }
    else // deactivate
    {
        if ( m_currentView == view )
        {
            // don't keep stale pointer
            m_currentView = NULL;
        }
    }
}

// ----------------------------------------------------------------------------
// wxDocChildFrameAnyBase
// ----------------------------------------------------------------------------

bool wxDocChildFrameAnyBase::CloseView(wxCloseEvent& event)
{
    if ( m_childView )
    {
        if ( event.CanVeto() && !m_childView->Close(false) )
        {
            event.Veto();
            return false;
        }

        m_childView->Activate(false);

        // it is important to reset m_childView frame pointer to NULL before
        // deleting it because while normally it is the frame which deletes the
        // view when it's closed, the view also closes the frame if it is
        // deleted directly not by us as indicated by its doc child frame
        // pointer still being set
        m_childView->SetDocChildFrame(NULL);
        delete m_childView;
        m_childView = NULL;
    }

    m_childDocument = NULL;

    return true;
}

// ----------------------------------------------------------------------------
// Default parent frame
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxDocParentFrame, wxFrame)
    EVT_MENU(wxID_EXIT, wxDocParentFrame::OnExit)
    EVT_MENU_RANGE(wxID_FILE1, wxID_FILE9, wxDocParentFrame::OnMRUFile)
    EVT_CLOSE(wxDocParentFrame::OnCloseWindow)
END_EVENT_TABLE()

wxDocParentFrame::wxDocParentFrame()
{
    m_docManager = NULL;
}

wxDocParentFrame::wxDocParentFrame(wxDocManager *manager,
                                   wxFrame *frame,
                                   wxWindowID id,
                                   const wxString& title,
                                   const wxPoint& pos,
                                   const wxSize& size,
                                   long style,
                                   const wxString& name)
                : wxFrame(frame, id, title, pos, size, style, name)
{
    m_docManager = manager;
}

bool wxDocParentFrame::Create(wxDocManager *manager,
                              wxFrame *frame,
                              wxWindowID id,
                              const wxString& title,
                              const wxPoint& pos,
                              const wxSize& size,
                              long style,
                              const wxString& name)
{
    m_docManager = manager;
    return base_type::Create(frame, id, title, pos, size, style, name);
}

void wxDocParentFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close();
}

void wxDocParentFrame::OnMRUFile(wxCommandEvent& event)
{
    int n = event.GetId() - wxID_FILE1;  // the index in MRU list
    wxString filename(m_docManager->GetHistoryFile(n));
    if ( filename.empty() )
        return;

    wxString errMsg; // must contain exactly one "%s" if non-empty
    if ( wxFile::Exists(filename) )
    {
        // try to open it
        if ( m_docManager->CreateDocument(filename, wxDOC_SILENT) )
            return;

        errMsg = _("The file '%s' couldn't be opened.");
    }
    else // file doesn't exist
    {
        errMsg = _("The file '%s' doesn't exist and couldn't be opened.");
    }


    wxASSERT_MSG( !errMsg.empty(), "should have an error message" );

    // remove the file which we can't open from the MRU list
    m_docManager->RemoveFileFromHistory(n);

    // and tell the user about it
    wxLogError(errMsg + '\n' +
               _("It has been removed from the most recently used files list."),
               filename);
}

// Extend event processing to search the view's event table
bool wxDocParentFrame::TryBefore(wxEvent& event)
{
    if ( m_docManager && m_docManager->ProcessEventHere(event) )
        return true;

    return wxFrame::TryBefore(event);
}

// Define the behaviour for the frame closing
// - must delete all frames except for the main one.
void wxDocParentFrame::OnCloseWindow(wxCloseEvent& event)
{
    if (m_docManager->Clear(!event.CanVeto()))
    {
        Destroy();
    }
    else
        event.Veto();
}

#if wxUSE_PRINTING_ARCHITECTURE

wxDocPrintout::wxDocPrintout(wxView *view, const wxString& title)
             : wxPrintout(title)
{
    m_printoutView = view;
}

bool wxDocPrintout::OnPrintPage(int WXUNUSED(page))
{
    wxDC *dc = GetDC();

    // Get the logical pixels per inch of screen and printer
    int ppiScreenX, ppiScreenY;
    GetPPIScreen(&ppiScreenX, &ppiScreenY);
    wxUnusedVar(ppiScreenY);
    int ppiPrinterX, ppiPrinterY;
    GetPPIPrinter(&ppiPrinterX, &ppiPrinterY);
    wxUnusedVar(ppiPrinterY);

    // This scales the DC so that the printout roughly represents the
    // the screen scaling. The text point size _should_ be the right size
    // but in fact is too small for some reason. This is a detail that will
    // need to be addressed at some point but can be fudged for the
    // moment.
    float scale = (float)((float)ppiPrinterX/(float)ppiScreenX);

    // Now we have to check in case our real page size is reduced
    // (e.g. because we're drawing to a print preview memory DC)
    int pageWidth, pageHeight;
    int w, h;
    dc->GetSize(&w, &h);
    GetPageSizePixels(&pageWidth, &pageHeight);
    wxUnusedVar(pageHeight);

    // If printer pageWidth == current DC width, then this doesn't
    // change. But w might be the preview bitmap width, so scale down.
    float overallScale = scale * (float)(w/(float)pageWidth);
    dc->SetUserScale(overallScale, overallScale);

    if (m_printoutView)
    {
        m_printoutView->OnDraw(dc);
    }
    return true;
}

bool wxDocPrintout::HasPage(int pageNum)
{
    return (pageNum == 1);
}

bool wxDocPrintout::OnBeginDocument(int startPage, int endPage)
{
    if (!wxPrintout::OnBeginDocument(startPage, endPage))
        return false;

    return true;
}

void wxDocPrintout::GetPageInfo(int *minPage, int *maxPage,
                                int *selPageFrom, int *selPageTo)
{
    *minPage = 1;
    *maxPage = 1;
    *selPageFrom = 1;
    *selPageTo = 1;
}

#endif // wxUSE_PRINTING_ARCHITECTURE

// ----------------------------------------------------------------------------
// File history (a.k.a. MRU, most recently used, files list)
// ----------------------------------------------------------------------------

wxFileHistory::wxFileHistory(size_t maxFiles, wxWindowID idBase)
{
    m_fileMaxFiles = maxFiles;
    m_idBase = idBase;
}

void wxFileHistory::AddFileToHistory(const wxString& file)
{
    // check if we don't already have this file
    const wxFileName fnNew(file);
    size_t i,
           numFiles = m_fileHistory.size();
    for ( i = 0; i < numFiles; i++ )
    {
        if ( fnNew == m_fileHistory[i] )
        {
            // we do have it, move it to the top of the history
            RemoveFileFromHistory(i);
            numFiles--;
            break;
        }
    }

    // if we already have a full history, delete the one at the end
    if ( numFiles == m_fileMaxFiles )
    {
        RemoveFileFromHistory(--numFiles);
    }

    // add a new menu item to all file menus (they will be updated below)
    for ( wxList::compatibility_iterator node = m_fileMenus.GetFirst();
        node;
        node = node->GetNext() )
    {
        wxMenu * const menu = (wxMenu *)node->GetData();

        if ( !numFiles && menu->GetMenuItemCount() )
            menu->AppendSeparator();

        // label doesn't matter, it will be set below anyhow, but it can't
        // be empty (this is supposed to indicate a stock item)
        menu->Append(m_idBase + numFiles, " ");
    }

    // insert the new file in the beginning of the file history
    m_fileHistory.insert(m_fileHistory.begin(), file);
    numFiles++;

    // update the labels in all menus
    for ( i = 0; i < numFiles; i++ )
    {
        // if in same directory just show the filename; otherwise the full path
        const wxFileName fnOld(m_fileHistory[i]);

        wxString pathInMenu;
        if ( fnOld.GetPath() == fnNew.GetPath() )
        {
            pathInMenu = fnOld.GetFullName();
        }
        else // file in different directory
        {
            // absolute path; could also set relative path
            pathInMenu = m_fileHistory[i];
        }

        for ( wxList::compatibility_iterator node = m_fileMenus.GetFirst();
              node;
              node = node->GetNext() )
        {
            wxMenu * const menu = (wxMenu *)node->GetData();

            menu->SetLabel(m_idBase + i, GetMRUEntryLabel(i, pathInMenu));
        }
    }
}

void wxFileHistory::RemoveFileFromHistory(size_t i)
{
    size_t numFiles = m_fileHistory.size();
    wxCHECK_RET( i < numFiles,
                 wxT("invalid index in wxFileHistory::RemoveFileFromHistory") );

    // delete the element from the array
    m_fileHistory.RemoveAt(i);
    numFiles--;

    for ( wxList::compatibility_iterator node = m_fileMenus.GetFirst();
          node;
          node = node->GetNext() )
    {
        wxMenu * const menu = (wxMenu *) node->GetData();

        // shift filenames up
        for ( size_t j = i; j < numFiles; j++ )
        {
            menu->SetLabel(m_idBase + j, GetMRUEntryLabel(j, m_fileHistory[j]));
        }

        // delete the last menu item which is unused now
        const wxWindowID lastItemId = m_idBase + numFiles;
        if ( menu->FindItem(lastItemId) )
            menu->Delete(lastItemId);

        // delete the last separator too if no more files are left
        if ( m_fileHistory.empty() )
        {
            const wxMenuItemList::compatibility_iterator
                nodeLast = menu->GetMenuItems().GetLast();
            if ( nodeLast )
            {
                wxMenuItem * const lastMenuItem = nodeLast->GetData();
                if ( lastMenuItem->IsSeparator() )
                    menu->Delete(lastMenuItem);
            }
            //else: menu is empty somehow
        }
    }
}

void wxFileHistory::UseMenu(wxMenu *menu)
{
    if ( !m_fileMenus.Member(menu) )
        m_fileMenus.Append(menu);
}

void wxFileHistory::RemoveMenu(wxMenu *menu)
{
    m_fileMenus.DeleteObject(menu);
}

#if wxUSE_CONFIG
void wxFileHistory::Load(const wxConfigBase& config)
{
    m_fileHistory.Clear();

    wxString buf;
    buf.Printf(wxT("file%d"), 1);

    wxString historyFile;
    while ((m_fileHistory.GetCount() < m_fileMaxFiles) &&
           config.Read(buf, &historyFile) && !historyFile.empty())
    {
        m_fileHistory.Add(historyFile);

        buf.Printf(wxT("file%d"), (int)m_fileHistory.GetCount()+1);
        historyFile = wxEmptyString;
    }

    AddFilesToMenu();
}

void wxFileHistory::Save(wxConfigBase& config)
{
    size_t i;
    for (i = 0; i < m_fileMaxFiles; i++)
    {
        wxString buf;
        buf.Printf(wxT("file%d"), (int)i+1);
        if (i < m_fileHistory.GetCount())
            config.Write(buf, wxString(m_fileHistory[i]));
        else
            config.Write(buf, wxEmptyString);
    }
}
#endif // wxUSE_CONFIG

void wxFileHistory::AddFilesToMenu()
{
    if ( m_fileHistory.empty() )
        return;

    for ( wxList::compatibility_iterator node = m_fileMenus.GetFirst();
          node;
          node = node->GetNext() )
    {
        AddFilesToMenu((wxMenu *) node->GetData());
    }
}

void wxFileHistory::AddFilesToMenu(wxMenu* menu)
{
    if ( m_fileHistory.empty() )
        return;

    if ( menu->GetMenuItemCount() )
        menu->AppendSeparator();

    for ( size_t i = 0; i < m_fileHistory.GetCount(); i++ )
    {
        menu->Append(m_idBase + i, GetMRUEntryLabel(i, m_fileHistory[i]));
    }
}

// ----------------------------------------------------------------------------
// Permits compatibility with existing file formats and functions that
// manipulate files directly
// ----------------------------------------------------------------------------

#if wxUSE_STD_IOSTREAM

bool wxTransferFileToStream(const wxString& filename, wxSTD ostream& stream)
{
    wxFFile file(filename, _T("rb"));
    if ( !file.IsOpened() )
        return false;

    char buf[4096];

    size_t nRead;
    do
    {
        nRead = file.Read(buf, WXSIZEOF(buf));
        if ( file.Error() )
            return false;

        stream.write(buf, nRead);
        if ( !stream )
            return false;
    }
    while ( !file.Eof() );

    return true;
}

bool wxTransferStreamToFile(wxSTD istream& stream, const wxString& filename)
{
    wxFFile file(filename, _T("wb"));
    if ( !file.IsOpened() )
        return false;

    char buf[4096];
    do
    {
        stream.read(buf, WXSIZEOF(buf));
        if ( !stream.bad() ) // fail may be set on EOF, don't use operator!()
        {
            if ( !file.Write(buf, stream.gcount()) )
                return false;
        }
    }
    while ( !stream.eof() );

    return true;
}

#else // !wxUSE_STD_IOSTREAM

bool wxTransferFileToStream(const wxString& filename, wxOutputStream& stream)
{
    wxFFile file(filename, _T("rb"));
    if ( !file.IsOpened() )
        return false;

    char buf[4096];

    size_t nRead;
    do
    {
        nRead = file.Read(buf, WXSIZEOF(buf));
        if ( file.Error() )
            return false;

        stream.Write(buf, nRead);
        if ( !stream )
            return false;
    }
    while ( !file.Eof() );

    return true;
}

bool wxTransferStreamToFile(wxInputStream& stream, const wxString& filename)
{
    wxFFile file(filename, _T("wb"));
    if ( !file.IsOpened() )
        return false;

    char buf[4096];
    for ( ;; )
    {
        stream.Read(buf, WXSIZEOF(buf));

        const size_t nRead = stream.LastRead();
        if ( !nRead )
        {
            if ( stream.Eof() )
                break;

            return false;
        }

        if ( !file.Write(buf, nRead) )
            return false;
    }

    return true;
}

#endif // wxUSE_STD_IOSTREAM/!wxUSE_STD_IOSTREAM

#endif // wxUSE_DOC_VIEW_ARCHITECTURE
