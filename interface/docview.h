/////////////////////////////////////////////////////////////////////////////
// Name:        docview.h
// Purpose:     documentation for wxDocTemplate class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxDocTemplate
    @wxheader{docview.h}

    The wxDocTemplate class is used to model the relationship between a
    document class and a view class.

    @library{wxcore}
    @category{dvf}

    @seealso
    @ref overview_wxdoctemplateoverview "wxDocTemplate overview", wxDocument, wxView
*/
class wxDocTemplate : public wxObject
{
public:
    /**
        Constructor. Create instances dynamically near the start of your application
        after creating
        a wxDocManager instance, and before doing any document or view operations.
        
        @e manager is the document manager object which manages this template.
        
        @e descr is a short description of what the template is for. This string will
        be displayed in the
        file filter list of Windows file selectors.
        
        @e filter is an appropriate file filter such as @c *.txt.
        
        @e dir is the default directory to use for file selectors.
        
        @e ext is the default file extension (such as txt).
        
        @e docTypeName is a name that should be unique for a given type of document,
        used for
        gathering a list of views relevant to a particular document.
        
        @e viewTypeName is a name that should be unique for a given view.
        
        @e docClassInfo is a pointer to the run-time document class information as
        returned
        by the CLASSINFO macro, e.g. CLASSINFO(MyDocumentClass). If this is not
        supplied,
        you will need to derive a new wxDocTemplate class and override the
        CreateDocument
        member to return a new document instance on demand.
        
        @e viewClassInfo is a pointer to the run-time view class information as returned
        by the CLASSINFO macro, e.g. CLASSINFO(MyViewClass). If this is not supplied,
        you will need to derive a new wxDocTemplate class and override the CreateView
        member to return a new view instance on demand.
        
        @e flags is a bit list of the following:
        
         wxTEMPLATE_VISIBLE The template may be displayed to the user in dialogs.
         wxTEMPLATE_INVISIBLE The template may not be displayed to the user in dialogs.
         wxDEFAULT_TEMPLATE_FLAGS Defined as wxTEMPLATE_VISIBLE.
        
        
        
        @b Wx::DocTemplate-new( docmgr, descr, filter, dir,
        ext, docTypeName, viewTypeName, docClassInfo, viewClassInfo, flags
        )
        
        
         will construct document and view objects from the class information
        
        @b Wx::DocTemplate-new( docmgr, descr, filter, dir,
        ext, docTypeName, viewTypeName, docClassName, viewClassName, flags
        )
        
        
         will construct document and view objects from perl packages
        
        @b Wx::DocTemplate-new( docmgr, descr, filter, dir,
        ext, docTypeName, viewTypeName )
        
        
        @c Wx::DocTemplate::CreateDocument() and
        @c Wx::DocTemplate::CreateView() must be overridden
    */
    wxDocTemplate(wxDocManager* manager, const wxString& descr,
                  const wxString& filter,
                  const wxString& dir,
                  const wxString& ext,
                  const wxString& docTypeName,
                  const wxString& viewTypeName,
                  wxClassInfo* docClassInfo = @NULL,
                  wxClassInfo* viewClassInfo = @NULL,
                  long flags = wxDEFAULT_TEMPLATE_FLAGS);

    /**
        Destructor.
    */
    ~wxDocTemplate();

    /**
        Creates a new instance of the associated document class. If you have not
        supplied
        a wxClassInfo parameter to the template constructor, you will need to override
        this
        function to return an appropriate document instance.
        
        This function calls InitDocument() which in turns
        calls wxDocument::OnCreate.
    */
    wxDocument * CreateDocument(const wxString& path, long flags = 0);

    /**
        Creates a new instance of the associated view class. If you have not supplied
        a wxClassInfo parameter to the template constructor, you will need to override
        this
        function to return an appropriate view instance.
    */
    wxView * CreateView(wxDocument * doc, long flags = 0);

    /**
        Returns the default file extension for the document data, as passed to the
        document template constructor.
    */
    wxString GetDefaultExtension();

    /**
        Returns the text description of this template, as passed to the document
        template constructor.
    */
    wxString GetDescription();

    /**
        Returns the default directory, as passed to the document template constructor.
    */
    wxString GetDirectory();

    /**
        Returns a pointer to the document manager instance for which this template was
        created.
    */
    wxDocManager * GetDocumentManager();

    /**
        Returns the document type name, as passed to the document template constructor.
    */
    wxString GetDocumentName();

    /**
        Returns the file filter, as passed to the document template constructor.
    */
    wxString GetFileFilter();

    /**
        Returns the flags, as passed to the document template constructor.
    */
    long GetFlags();

    /**
        Returns the view type name, as passed to the document template constructor.
    */
    wxString GetViewName();

    /**
        Initialises the document, calling wxDocument::OnCreate. This is called from
        CreateDocument().
    */
    bool InitDocument(wxDocument* doc, const wxString& path,
                      long flags = 0);

    /**
        Returns @true if the document template can be shown in user dialogs, @false
        otherwise.
    */
    bool IsVisible();

    /**
        Sets the default file extension.
    */
    void SetDefaultExtension(const wxString& ext);

    /**
        Sets the template description.
    */
    void SetDescription(const wxString& descr);

    /**
        Sets the default directory.
    */
    void SetDirectory(const wxString& dir);

    /**
        Sets the pointer to the document manager instance for which this template was
        created.
        Should not be called by the application.
    */
    void SetDocumentManager(wxDocManager * manager);

    /**
        Sets the file filter.
    */
    void SetFileFilter(const wxString& filter);

    /**
        Sets the internal document template flags (see the constructor description for
        more details).
    */
    void SetFlags(long flags);

    /**
        wxString m_defaultExt
        
        The default extension for files of this type.
    */


    /**
        wxString m_description
        
        A short description of this template.
    */


    /**
        wxString m_directory
        
        The default directory for files of this type.
    */


    /**
        wxClassInfo*  m_docClassInfo
        
        Run-time class information that allows document instances to be constructed
        dynamically.
    */


    /**
        wxString m_docTypeName
        
        The named type of the document associated with this template.
    */


    /**
        wxDocTemplate* m_documentManager
        
        A pointer to the document manager for which this template was created.
    */


    /**
        wxString m_fileFilter
        
        The file filter (such as @c *.txt) to be used in file selector dialogs.
    */


    /**
        long m_flags
        
        The flags passed to the constructor.
    */


    /**
        wxClassInfo* m_viewClassInfo
        
        Run-time class information that allows view instances to be constructed
        dynamically.
    */


    /**
        wxString m_viewTypeName
        
        The named type of the view associated with this template.
    */
};


/**
    @class wxDocManager
    @wxheader{docview.h}

    The wxDocManager class is part of the document/view framework supported by
    wxWidgets,
    and cooperates with the wxView, wxDocument
    and wxDocTemplate classes.

    @library{wxcore}
    @category{dvf}

    @seealso
    @ref overview_wxdocmanageroverview "wxDocManager overview", wxDocument, wxView,
    wxDocTemplate, wxFileHistory
*/
class wxDocManager : public wxEvtHandler
{
public:
    /**
        Constructor. Create a document manager instance dynamically near the start of
        your application
        before doing any document or view operations.
        
        @e flags is currently unused.
        
        If @e initialize is @true, the Initialize() function will be called
        to create a default history list object. If you derive from wxDocManager, you
        may wish to call the
        base constructor with @false, and then call Initialize in your own constructor,
        to allow
        your own Initialize or OnCreateFileHistory functions to be called.
    */
    wxDocManager(long flags = wxDEFAULT_DOCMAN_FLAGS,
                 bool initialize = @true);

    /**
        Destructor.
    */
    ~wxDocManager();

    /**
        Sets the current view.
    */
    void ActivateView(wxView* doc, bool activate = @true);

    /**
        Adds the document to the list of documents.
    */
    void AddDocument(wxDocument * doc);

    /**
        Adds a file to the file history list, if we have a pointer to an appropriate
        file menu.
    */
    void AddFileToHistory(const wxString& filename);

    /**
        Adds the template to the document manager's template list.
    */
    void AssociateTemplate(wxDocTemplate * temp);

    /**
        Closes all currently opened documents.
    */
    bool CloseDocuments(bool force = @true);

    /**
        Creates a new document in a manner determined by the @e flags parameter, which
        can be:
        
         wxDOC_NEW Creates a fresh document.
         wxDOC_SILENT Silently loads the given document file.
        
        If wxDOC_NEW is present, a new document will be created and returned, possibly
        after
        asking the user for a template to use if there is more than one document
        template.
        If wxDOC_SILENT is present, a new document will be created and the given file
        loaded
        into it. If neither of these flags is present, the user will be presented with
        a file selector for the file to load, and the template to use will be
        determined by the
        extension (Windows) or by popping up a template choice list (other platforms).
        
        If the maximum number of documents has been reached, this function
        will delete the oldest currently loaded document before creating a new one.
    */
    wxDocument* CreateDocument(const wxString& path, long flags);

    /**
        Creates a new view for the given document. If more than one view is allowed for
        the
        document (by virtue of multiple templates mentioning the same document type), a
        choice
        of view is presented to the user.
    */
    wxView* CreateView(wxDocument* doc, long flags);

    /**
        Removes the template from the list of templates.
    */
    void DisassociateTemplate(wxDocTemplate * temp);

    //@{
    /**
        Appends the files in the history list, to the given menu only.
    */
    void FileHistoryAddFilesToMenu();
    void FileHistoryAddFilesToMenu(wxMenu* menu);
    //@}

    /**
        Loads the file history from a config object.
        
        @sa wxConfig
    */
    void FileHistoryLoad(wxConfigBase& config);

    /**
        Removes the given menu from the list of menus managed by the file history
        object.
    */
    void FileHistoryRemoveMenu(wxMenu* menu);

    /**
        Saves the file history into a config object. This must be called
        explicitly by the application.
        
        @sa wxConfig
    */
    void FileHistorySave(wxConfigBase& resourceFile);

    /**
        Use this menu for appending recently-visited document filenames, for convenient
        access. Calling this function with a valid menu pointer enables the history
        list functionality.
        
        Note that you can add multiple menus using this function, to be managed by the
        file history object.
    */
    void FileHistoryUseMenu(wxMenu* menu);

    /**
        Given a path, try to find template that matches the extension. This is only
        an approximate method of finding a template for creating a document.
    */
    wxDocTemplate * FindTemplateForPath(const wxString& path);

    /**
        Returns the document associated with the currently active view (if any).
    */
    wxDocument * GetCurrentDocument();

    /**
        Returns the currently active view
    */
    wxView * GetCurrentView();

    /**
        Returns a reference to the list of documents.
    */
    wxList GetDocuments();

    /**
        Returns a pointer to file history.
    */
    wxFileHistory * GetFileHistory();

    /**
        Returns the number of files currently stored in the file history.
    */
    size_t GetHistoryFilesCount();

    /**
        Returns the directory last selected by the user when opening a file. Initially
        empty.
    */
    wxString GetLastDirectory();

    /**
        Returns the number of documents that can be open simultaneously.
    */
    int GetMaxDocsOpen();

    /**
        Returns a reference to the list of associated templates.
    */
    wxList GetTemplates();

    /**
        Initializes data; currently just calls OnCreateFileHistory. Some data cannot
        always be initialized in the constructor because the programmer must be given
        the opportunity to override functionality. If OnCreateFileHistory was called
        from the constructor, an overridden virtual OnCreateFileHistory would not be
        called due to C++'s 'interesting' constructor semantics. In fact Initialize
        @e is called from the wxDocManager constructor, but this can be
        vetoed by passing @false to the second argument, allowing the derived class's
        constructor to call Initialize, possibly calling a different OnCreateFileHistory
        from the default.
        
        The bottom line: if you're not deriving from Initialize, forget it and
        construct wxDocManager with no arguments.
    */
    bool Initialize();

    /**
        Return a string containing a suitable default name for a new document. By
        default this is implemented by appending an integer counter to the string
        @b unnamed but can be overridden in the derived classes to do something more
        appropriate.
    */
    wxString MakeNewDocumentName();

    /**
        A hook to allow a derived class to create a different type of file history.
        Called
        from Initialize().
    */
    wxFileHistory * OnCreateFileHistory();

    /**
        Closes and deletes the currently active document.
    */
    void OnFileClose(wxCommandEvent& event);

    /**
        Closes and deletes all the currently opened documents.
    */
    void OnFileCloseAll(wxCommandEvent& event);

    /**
        Creates a document from a list of templates (if more than one template).
    */
    void OnFileNew(wxCommandEvent& event);

    /**
        Creates a new document and reads in the selected file.
    */
    void OnFileOpen(wxCommandEvent& event);

    /**
        Reverts the current document by calling wxDocument::Revert for the current
        document.
    */
    void OnFileRevert(wxCommandEvent& event);

    /**
        Saves the current document by calling wxDocument::Save for the current document.
    */
    void OnFileSave(wxCommandEvent& event);

    /**
        Calls wxDocument::SaveAs for the current document.
    */
    void OnFileSaveAs(wxCommandEvent& event);

    /**
        Removes the document from the list of documents.
    */
    void RemoveDocument(wxDocument * doc);

    /**
        Under Windows, pops up a file selector with a list of filters corresponding to
        document templates.
        The wxDocTemplate corresponding to the selected file's extension is returned.
        
        On other platforms, if there is more than one document template a choice list
        is popped up,
        followed by a file selector.
        
        This function is used in CreateDocument().
        
          (doctemplate, path) = My::DocManager-SelectDocumentPath( ... );
    */
    wxDocTemplate * SelectDocumentPath(wxDocTemplate ** templates,
                                       int noTemplates,
                                       wxString& path,
                                       long flags,
                                       bool save);

    /**
        Returns a document template by asking the user (if there is more than one
        template).
        This function is used in CreateDocument().
        
        @param templates
        Pointer to an array of templates from which to choose a desired template.
        
        @param noTemplates
        Number of templates being pointed to by the templates pointer.
        
        @param sort
        If more than one template is passed in in templates,
        then this parameter indicates whether the list of templates that the user
        will have to choose from is sorted or not when shown the choice box dialog.
        Default is @false.
    */
    wxDocTemplate * SelectDocumentType(wxDocTemplate ** templates,
                                       int noTemplates,
                                       bool sort=@false);

    /**
        Returns a document template by asking the user (if there is more than one
        template),
        displaying a list of valid views. This function is used in CreateView().
        The dialog normally will not appear because the array of templates only contains
        those relevant to the document in question, and often there will only be one
        such.
        
        @param templates
        Pointer to an array of templates from which to choose a desired template.
        
        @param noTemplates
        Number of templates being pointed to by the templates pointer.
        
        @param sort
        If more than one template is passed in in templates,
        then this parameter indicates whether the list of templates that the user
        will have to choose from is sorted or not when shown the choice box dialog.
        Default is @false.
    */
    wxDocTemplate * SelectViewType(wxDocTemplate ** templates,
                                   int noTemplates,
                                   bool sort=@false);

    /**
        Sets the directory to be displayed to the user when opening a file. Initially
        this is empty.
    */
    void SetLastDirectory(const wxString& dir);

    /**
        Sets the maximum number of documents that can be open at a time. By default,
        this
        is 10,000. If you set it to 1, existing documents will be saved and deleted
        when the user tries to open or create a new one (similar to the behaviour
        of Windows Write, for example). Allowing multiple documents gives behaviour
        more akin to MS Word and other Multiple Document Interface applications.
    */
    void SetMaxDocsOpen(int n);

    /**
        wxView* m_currentView
        
        The currently active view.
    */


    /**
        int m_defaultDocumentNameCounter
        
        Stores the integer to be used for the next default document name.
    */


    /**
        wxList m_docs
        
        A list of all documents.
    */


    /**
        wxFileHistory* m_fileHistory
        
        A pointer to an instance of wxFileHistory,
        which manages the history of recently-visited files on the File menu.
    */


    /**
        long m_flags
        
        Stores the flags passed to the constructor.
    */


    /**
        The directory last selected by the user when opening a file.
        
        wxFileHistory* m_fileHistory
    */


    /**
        int m_maxDocsOpen
        
        Stores the maximum number of documents that can be opened before
        existing documents are closed. By default, this is 10,000.
    */
};


/**
    @class wxView
    @wxheader{docview.h}

    The view class can be used to model the viewing and editing component of
    an application's file-based data. It is part of the document/view framework
    supported by wxWidgets,
    and cooperates with the wxDocument, wxDocTemplate
    and wxDocManager classes.

    @library{wxcore}
    @category{dvf}

    @seealso
    @ref overview_wxviewoverview "wxView overview", wxDocument, wxDocTemplate,
    wxDocManager
*/
class wxView : public wxEvtHandler
{
public:
    /**
        Constructor. Define your own default constructor to initialize
        application-specific
        data.
    */
    wxView();

    /**
        Destructor. Removes itself from the document's list of views.
    */
    ~wxView();

    /**
        Call this from your view frame's OnActivate member to tell the framework which
        view is
        currently active. If your windowing system doesn't call OnActivate, you may
        need to
        call this function from any place where you know the view must
        be active, and the framework will need to get the current view.
        
        The prepackaged view frame wxDocChildFrame calls Activate() from its OnActivate
        member.
        
        This function calls OnActivateView().
    */
    virtual void Activate(bool activate);

    /**
        Closes the view by calling OnClose. If @e deleteWindow is @true, this function
        should
        delete the window associated with the view.
    */
    virtual bool Close(bool deleteWindow = @true);

    /**
        Gets a pointer to the document associated with the view.
    */
    wxDocument* GetDocument();

    /**
        Returns a pointer to the document manager instance associated with this view.
    */
    wxDocManager* GetDocumentManager();

    /**
        Gets the frame associated with the view (if any). Note that this "frame'' is
        not a wxFrame at all in the generic MDI implementation which uses the notebook
        pages instead of the frames and this is why this method returns a wxWindow and
        not a wxFrame.
    */
    wxWindow * GetFrame();

    /**
        Gets the name associated with the view (passed to the wxDocTemplate
        constructor).
        Not currently used by the framework.
    */
    wxString GetViewName();

    /**
        Called when a view is activated by means of Activate(). The default
        implementation does
        nothing.
    */
    virtual void OnActivateView(bool activate, wxView * activeView,
                                wxView * deactiveView);

    /**
        Called when the filename has changed. The default implementation constructs a
        suitable title and sets the title of the view frame (if any).
    */
    virtual void OnChangeFilename();

    /**
        Implements closing behaviour. The default implementation calls wxDocument::Close
        to close the associated document. Does not delete the view. The application
        may wish to do some cleaning up operations in this function, @e if a
        call to wxDocument::Close succeeded. For example, if your views
        all share the same window, you need to disassociate the window from the view
        and perhaps clear the window. If @e deleteWindow is @true, delete the
        frame associated with the view.
    */
    virtual bool OnClose(bool deleteWindow);

    /**
        Override this to clean up the view when the document is being
        closed.
    */
    virtual void OnClosingDocument();

    /**
        wxDocManager or wxDocument creates a wxView via a wxDocTemplate.
        Just after the wxDocTemplate creates the wxView, it calls
        OnCreate(). In its OnCreate member function, the wxView can create a
        wxDocChildFrame
        or a derived class. This wxDocChildFrame provides user interface
        elements to view and/or edit the contents of the wxDocument.
        
        By default, simply returns @true. If the function returns @false, the
        view will be deleted.
    */
    virtual bool OnCreate(wxDocument* doc, long flags);

    /**
        If the printing framework is enabled in the library, this function returns a
        wxPrintout object for the purposes of printing. It should create a new object
        every time it is called; the framework will delete objects it creates.
        
        By default, this function returns an instance of wxDocPrintout, which prints
        and previews one page by calling OnDraw().
        
        Override to return an instance of a class other than wxDocPrintout.
    */
    virtual wxPrintout* OnCreatePrintout();

    /**
        Override this function to render the view on the given device context.
    */
    virtual void OnDraw(wxDC* dc);

    /**
        Called when the view should be updated. @e sender is a pointer to the view
        that sent the update request, or @NULL if no single view requested the update
        (for instance,
        when the document is opened). @e hint is as yet unused but may in future contain
        application-specific information for making updating more efficient.
    */
    virtual void OnUpdate(wxView* sender, wxObject* hint);

    /**
        Associates the given document with the view. Normally called by the
        framework.
    */
    void SetDocument(wxDocument* doc);

    /**
        Sets the frame associated with this view. The application should call this
        if possible, to tell the view about the frame.
        
        See GetFrame() for the explanation about the mismatch
        between the "Frame'' in the method name and the type of its parameter.
    */
    void SetFrame(wxWindow* frame);

    /**
        Sets the view type name. Should only be called by the framework.
    */
    void SetViewName(const wxString& name);

    /**
        wxDocument* m_viewDocument
        
        The document associated with this view. There may be more than one view per
        document, but there can never be more than one document for one view.
    */


    /**
        wxFrame* m_viewFrame
        
        Frame associated with the view, if any.
    */


    /**
        wxString m_viewTypeName
        
        The view type name given to the wxDocTemplate constructor, copied to this
        variable when the view is created. Not currently used by the framework.
    */
};


/**
    @class wxDocChildFrame
    @wxheader{docview.h}

    The wxDocChildFrame class provides a default frame for displaying documents
    on separate windows. This class can only be used for SDI (not MDI) child frames.

    The class is part of the document/view framework supported by wxWidgets,
    and cooperates with the wxView, wxDocument,
    wxDocManager and wxDocTemplate classes.

    See the example application in @c samples/docview.

    @library{wxcore}
    @category{dvf}

    @seealso
    @ref overview_docviewoverview "Document/view overview", wxFrame
*/
class wxDocChildFrame : public wxFrame
{
public:
    /**
        Constructor.
    */
    wxDocChildFrame(wxDocument* doc, wxView* view, wxFrame* parent,
                    wxWindowID id,
                    const wxString& title,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxDEFAULT_FRAME_STYLE,
                    const wxString& name = "frame");

    /**
        Destructor.
    */
    ~wxDocChildFrame();

    /**
        Returns the document associated with this frame.
    */
    wxDocument* GetDocument();

    /**
        Returns the view associated with this frame.
    */
    wxView* GetView();

    /**
        Sets the currently active view to be the frame's view. You may need
        to override (but still call) this function in order to set the keyboard
        focus for your subwindow.
    */
    void OnActivate(wxActivateEvent event);

    /**
        Closes and deletes the current view and document.
    */
    void OnCloseWindow(wxCloseEvent& event);

    /**
        Sets the document for this frame.
    */
    void SetDocument(wxDocument * doc);

    /**
        Sets the view for this frame.
    */
    void SetView(wxView * view);

    /**
        wxDocument* m_childDocument
        
        The document associated with the frame.
    */


    /**
        wxView* m_childView
        
        The view associated with the frame.
    */
};


/**
    @class wxDocParentFrame
    @wxheader{docview.h}

    The wxDocParentFrame class provides a default top-level frame for
    applications using the document/view framework. This class can only be used for
    SDI (not MDI) parent frames.

    It cooperates with the wxView, wxDocument,
    wxDocManager and wxDocTemplates classes.

    See the example application in @c samples/docview.

    @library{wxcore}
    @category{dvf}

    @seealso
    @ref overview_docviewoverview "Document/view overview", wxFrame
*/
class wxDocParentFrame : public wxFrame
{
public:
    //@{
    /**
        Constructor.
    */
    wxDocParentFrame();
    wxDocParentFrame(wxDocManager* manager, wxFrame * parent,
                     wxWindowID id,
                     const wxString& title,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxDEFAULT_FRAME_STYLE,
                     const wxString& name = "frame");
    //@}

    /**
        Destructor.
    */
    ~wxDocParentFrame();

    /**
        Used in two-step construction.
    */
    bool Create(wxDocManager* manager, wxFrame * parent,
                wxWindowID id, const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = "frame");

    /**
        Returns the associated @ref overview_wxdocmanager "document manager object".
    */
    wxDocManager * GetDocumentManager();

    /**
        Deletes all views and documents. If no user input cancelled the
        operation, the frame will be destroyed and the application will exit.
        
        Since understanding how document/view clean-up takes place can be difficult,
        the implementation of this function is shown below.
    */
    void OnCloseWindow(wxCloseEvent& event);
};


/**
    @class wxDocument
    @wxheader{docview.h}

    The document class can be used to model an application's file-based
    data. It is part of the document/view framework supported by wxWidgets,
    and cooperates with the wxView, wxDocTemplate
    and wxDocManager classes.

    @library{wxcore}
    @category{dvf}

    @seealso
    @ref overview_wxdocumentoverview "wxDocument overview", wxView, wxDocTemplate,
    wxDocManager
*/
class wxDocument : public wxEvtHandler
{
public:
    /**
        Constructor. Define your own default constructor to initialize
        application-specific
        data.
    */
    wxDocument();

    /**
        Destructor. Removes itself from the document manager.
    */
    ~wxDocument();

    /**
        If the view is not already in the list of views, adds the view and calls
        OnChangedViewList.
    */
    virtual bool AddView(wxView * view);

    /**
        Closes the document, by calling OnSaveModified and then (if this returned @true)
        OnCloseDocument.
        This does not normally delete the document object: use DeleteAllViews to do
        this implicitly.
    */
    virtual bool Close();

    /**
        Calls wxView::Close and deletes each view. Deleting the final view will
        implicitly
        delete the document itself, because the wxView destructor calls RemoveView. This
        in turns calls OnChangedViewList(), whose default implemention is to
        save and delete the document if no views exist.
    */
    virtual bool DeleteAllViews();

    /**
        Returns a pointer to the command processor associated with this document.
        
        See wxCommandProcessor.
    */
    wxCommandProcessor* GetCommandProcessor();

    /**
        Gets a pointer to the associated document manager.
    */
    wxDocManager* GetDocumentManager();

    /**
        Gets the document type name for this document. See the comment for
        documentTypeName.
    */
    wxString GetDocumentName();

    /**
        Gets a pointer to the template that created the document.
    */
    wxDocTemplate* GetDocumentTemplate();

    /**
        Intended to return a suitable window for using as a parent for document-related
        dialog boxes. By default, uses the frame associated with the first view.
    */
    wxWindow* GetDocumentWindow();

    /**
        Gets the filename associated with this document, or "" if none is
        associated.
    */
    wxString GetFilename();

    /**
        A convenience function to get the first view for a document, because
        in many cases a document will only have a single view.
        
        See also: GetViews()
    */
    wxView * GetFirstView();

    /**
        Gets the title for this document. The document title is used for an associated
        frame (if any), and is usually constructed by the framework from
        the filename.
    */
    wxString GetTitle();

    /**
        Return the document name suitable to be shown to the user. The default
        implementation uses the document title, if any, of the name part of the
        document filename if it was set or, otherwise, the string @b unnamed.
    */
    virtual wxString GetUserReadableName();

    /**
        Returns the list whose elements are the views on the document.
        
        See also: GetFirstView()
    */
    wxList GetViews();

    /**
        Returns @true if the document has been modified since the last save, @false
        otherwise.
        You may need to override this if your document view maintains its own
        record of being modified (for example if using wxTextWindow to view and edit
        the document).
        
        See also Modify().
    */
    virtual bool IsModified();

    //@{
    /**
        Override this function and call it from your own LoadObject before
        streaming your own data. LoadObject is called by the framework
        automatically when the document contents need to be loaded.
        
        Note that only one of these forms exists, depending on how wxWidgets
        was configured.
    */
    virtual istream LoadObject(istream& stream);
    virtual wxInputStream LoadObject(wxInputStream& stream);
    //@}

    /**
        Call with @true to mark the document as modified since the last save, @false
        otherwise.
        You may need to override this if your document view maintains its own
        record of being modified (for example if using wxTextWindow to view and edit
        the document).
        
        See also IsModified().
    */
    virtual void Modify(bool modify);

    /**
        Called when a view is added to or deleted from this document. The default
        implementation saves and deletes the document if no views exist (the last
        one has just been removed).
    */
    virtual void OnChangedViewList();

    /**
        The default implementation calls DeleteContents (an empty implementation)
        sets the modified flag to @false. Override this to
        supply additional behaviour when the document is closed with Close.
    */
    virtual bool OnCloseDocument();

    /**
        Called just after the document object is created to give it a chance
        to initialize itself. The default implementation uses the
        template associated with the document to create an initial view.
        If this function returns @false, the document is deleted.
    */
    virtual bool OnCreate(const wxString& path, long flags);

    /**
        Override this function if you want a different (or no) command processor
        to be created when the document is created. By default, it returns
        an instance of wxCommandProcessor.
        
        See wxCommandProcessor.
    */
    virtual wxCommandProcessor* OnCreateCommandProcessor();

    /**
        The default implementation calls OnSaveModified and DeleteContents, makes a
        default title for the
        document, and notifies the views that the filename (in fact, the title) has
        changed.
    */
    virtual bool OnNewDocument();

    /**
        Constructs an input file stream for the given filename (which must not be
        empty),
        and calls LoadObject. If LoadObject returns @true, the document is set to
        unmodified; otherwise, an error message box is displayed. The document's
        views are notified that the filename has changed, to give windows an opportunity
        to update their titles. All of the document's views are then updated.
    */
    virtual bool OnOpenDocument(const wxString& filename);

    /**
        Constructs an output file stream for the given filename (which must not be
        empty),
        and calls SaveObject. If SaveObject returns @true, the document is set to
        unmodified; otherwise, an error message box is displayed.
    */
    virtual bool OnSaveDocument(const wxString& filename);

    /**
        If the document has been modified, prompts the user to ask if the changes should
        be changed. If the user replies Yes, the Save function is called. If No, the
        document is marked as unmodified and the function succeeds. If Cancel, the
        function fails.
    */
    virtual bool OnSaveModified();

    /**
        Removes the view from the document's list of views, and calls OnChangedViewList.
    */
    virtual bool RemoveView(wxView* view);

    /**
        Saves the document by calling OnSaveDocument if there is an associated filename,
        or SaveAs if there is no filename.
    */
    virtual bool Save();

    /**
        Prompts the user for a file to save to, and then calls OnSaveDocument.
    */
    virtual bool SaveAs();

    //@{
    /**
        Override this function and call it from your own SaveObject before
        streaming your own data. SaveObject is called by the framework
        automatically when the document contents need to be saved.
        
        Note that only one of these forms exists, depending on how wxWidgets
        was configured.
    */
    virtual ostream SaveObject(ostream& stream);
    virtual wxOutputStream SaveObject(wxOutputStream& stream);
    //@}

    /**
        Sets the command processor to be used for this document. The document will then
        be responsible
        for its deletion. Normally you should not call this; override
        OnCreateCommandProcessor
        instead.
        
        See wxCommandProcessor.
    */
    virtual void SetCommandProcessor(wxCommandProcessor * processor);

    /**
        Sets the document type name for this document. See the comment for
        documentTypeName.
    */
    void SetDocumentName(const wxString& name);

    /**
        Sets the pointer to the template that created the document. Should only be
        called by the
        framework.
    */
    void SetDocumentTemplate(wxDocTemplate* templ);

    /**
        Sets the filename for this document. Usually called by the framework.
        
        If @e notifyViews is @true, wxView::OnChangeFilename is called for all views.
    */
    void SetFilename(const wxString& filename,
                     bool notifyViews = @false);

    /**
        Sets the title for this document. The document title is used for an associated
        frame (if any), and is usually constructed by the framework from
        the filename.
    */
    void SetTitle(const wxString& title);

    /**
        Updates all views. If @e sender is non-@NULL, does not update this view.
        
        @e hint represents optional information to allow a view to optimize its update.
    */
    void UpdateAllViews(wxView* sender = @NULL, wxObject* hint = @NULL);

    /**
        wxCommandProcessor* m_commandProcessor
        
        A pointer to the command processor associated with this document.
    */


    /**
        wxString m_documentFile
        
        Filename associated with this document ("" if none).
    */


    /**
        bool m_documentModified
        
        @true if the document has been modified, @false otherwise.
    */


    /**
        wxDocTemplate * m_documentTemplate
        
        A pointer to the template from which this document was created.
    */


    /**
        wxString m_documentTitle
        
        Document title. The document title is used for an associated
        frame (if any), and is usually constructed by the framework from
        the filename.
    */


    /**
        wxString m_documentTypeName
        
        The document type name given to the wxDocTemplate constructor, copied to this
        variable when the document is created. If several document templates are
        created that use the same document type, this variable is used in
        wxDocManager::CreateView
        to collate a list of alternative view types that can be used on this kind of
        document. Do not change the value of this variable.
    */


    /**
        wxList m_documentViews
        
        List of wxView instances associated with this document.
    */
};


/**
    @class wxFileHistory
    @wxheader{docview.h}

    The wxFileHistory encapsulates a user interface convenience, the
    list of most recently visited files as shown on a menu (usually the File menu).

    wxFileHistory can manage one or more file menus. More than one menu may be
    required
    in an MDI application, where the file history should appear on each MDI child
    menu
    as well as the MDI parent frame.

    @library{wxcore}
    @category{FIXME}

    @seealso
    @ref overview_wxfilehistoryoverview "wxFileHistory overview", wxDocManager
*/
class wxFileHistory : public wxObject
{
public:
    /**
        Constructor. Pass the maximum number of files that should be stored and
        displayed.
        
        @e idBase defaults to wxID_FILE1 and represents the id given to the first
        history menu item.  Since menu items can't share the same ID you should change
        idBase (To one of your own defined IDs) when using more than one wxFileHistory
        in your application.
    */
    wxFileHistory(size_t maxFiles = 9,
                  wxWindowID idBase = wxID_FILE1);

    /**
        Destructor.
    */
    ~wxFileHistory();

    /**
        Adds a file to the file history list, if the object has a pointer to an
        appropriate file menu.
    */
    void AddFileToHistory(const wxString& filename);

    //@{
    /**
        Appends the files in the history list, to the given menu only.
    */
    void AddFilesToMenu();
    void AddFilesToMenu(wxMenu* menu);
    //@}

    /**
        Returns the base identifier for the range used for appending items.
    */
    wxWindowID GetBaseId();

    /**
        Returns the number of files currently stored in the file history.
    */
    size_t GetCount();

    /**
        Returns the file at this index (zero-based).
    */
    wxString GetHistoryFile(size_t index);

    /**
        Returns the maximum number of files that can be stored.
    */
    int GetMaxFiles();

    /**
        Returns the list of menus that are managed by this file history object.
        
        @sa UseMenu()
    */
    const wxList GetMenus();

    /**
        Loads the file history from the given config object. This function should be
        called explicitly by the application.
        
        @sa wxConfig
    */
    void Load(wxConfigBase& config);

    /**
        Removes the specified file from the history.
    */
    void RemoveFileFromHistory(size_t i);

    /**
        Removes this menu from the list of those managed by this object.
    */
    void RemoveMenu(wxMenu* menu);

    /**
        Saves the file history into the given config object. This must be called
        explicitly by the application.
        
        @sa wxConfig
    */
    void Save(wxConfigBase& config);

    /**
        Sets the base identifier for the range used for appending items.
    */
    void SetBaseId(wxWindowID baseId);

    /**
        Adds this menu to the list of those menus that are managed by this file history
        object.
        Also see AddFilesToMenu() for
        initializing the menu with filenames that are already in the history when this
        function is called, as this is not done automatically.
    */
    void UseMenu(wxMenu* menu);

    /**
        char** m_fileHistory
        
        A character array of strings corresponding to the most recently opened
        files.
    */


    /**
        size_t m_fileHistoryN
        
        The number of files stored in the history array.
    */


    /**
        size_t m_fileMaxFiles
        
        The maximum number of files to be stored and displayed on the menu.
    */


    /**
        wxMenu* m_fileMenu
        
        The file menu used to display the file history list (if enabled).
    */
};


// ============================================================================
// Global functions/macros
// ============================================================================

/**
    Copies the given file to @e stream. Useful when converting an old application to
    use streams (within the document/view framework, for example).
*/
bool wxTransferFileToStream(const wxString& filename,
                            ostream& stream);

