/////////////////////////////////////////////////////////////////////////////
// Name:        notebook.h
// Purpose:     documentation for wxNotebookEvent class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxNotebookEvent
    @wxheader{notebook.h}

    This class represents the events generated by a notebook control: currently,
    there are two of them. The PAGE_CHANGING event is sent before the current
    page is changed. It allows the program to examine the current page (which
    can be retrieved with
    wxNotebookEvent::GetOldSelection) and to veto the page
    change by calling wxNotifyEvent::Veto if, for example, the
    current values in the controls of the old page are invalid.

    The second event - PAGE_CHANGED - is sent after the page has been changed and
    the program cannot veto it any more, it just informs it about the page change.

    To summarize, if the program is interested in validating the page values
    before allowing the user to change it, it should process the PAGE_CHANGING
    event, otherwise PAGE_CHANGED is probably enough. In any case, it is probably
    unnecessary to process both events at once.

    @library{wxcore}
    @category{events}

    @seealso
    wxNotebook
*/
class wxNotebookEvent : public wxNotifyEvent
{
public:
    /**
        Constructor (used internally by wxWidgets only).
    */
    wxNotebookEvent(wxEventType eventType = wxEVT_NULL, int id = 0,
                    int sel = -1,
                    int oldSel = -1);

    /**
        Returns the page that was selected before the change, -1 if none was selected.
    */
    int GetOldSelection();

    /**
        Returns the currently selected page, or -1 if none was selected.
        @b NB: under Windows, GetSelection() will return the same value as
        GetOldSelection() when called from
        @c EVT_NOTEBOOK_PAGE_CHANGING handler and not the page which is going to
        be selected. Also note that the values of selection and old selection returned
        for an event generated in response to a call to
        wxNotebook::SetSelection shouldn't be trusted
        as they are currently inconsistent under different platforms (but in this case
        you presumably don't need them anyhow as you already have the corresponding
        information).
    */
    int GetSelection();

    /**
        Sets the id of the page selected before the change.
    */
    void SetOldSelection(int page);

    /**
        Sets the selection member variable.
    */
    void SetSelection(int page);
};


/**
    @class wxNotebook
    @wxheader{notebook.h}

    This class represents a notebook control, which manages multiple windows with
    associated tabs.

    To use the class, create a wxNotebook object and call wxNotebook::AddPage or
    wxNotebook::InsertPage,
    passing a window to be used as the page. Do not explicitly delete the window
    for a page that is currently
    managed by wxNotebook.

    @b wxNotebookPage is a typedef for wxWindow.

    @beginStyleTable
    @style{wxNB_TOP}:
           Place tabs on the top side.
    @style{wxNB_LEFT}:
           Place tabs on the left side.
    @style{wxNB_RIGHT}:
           Place tabs on the right side.
    @style{wxNB_BOTTOM}:
           Place tabs under instead of above the notebook pages.
    @style{wxNB_FIXEDWIDTH}:
           (Windows only) All tabs will have same width.
    @style{wxNB_MULTILINE}:
           (Windows only) There can be several rows of tabs.
    @style{wxNB_NOPAGETHEME}:
           (Windows only) Display a solid colour on notebook pages, and not a
           gradient, which can reduce performance.
    @style{wxNB_FLAT}:
           (Windows CE only) Show tabs in a flat style.
    @endStyleTable

    @library{wxcore}
    @category{miscwnd}

    @seealso
    wxBookCtrl, wxNotebookEvent, wxImageList, @ref overview_samplenotebook
    "notebook sample"
*/
class wxNotebook : public wxBookCtrl overview
{
public:
    //@{
    /**
        Constructs a notebook control.
        Note that sometimes you can reduce flicker by passing the wxCLIP_CHILDREN
        window style.
        
        @param parent
            The parent window. Must be non-@NULL.
        @param id
            The window identifier.
        @param pos
            The window position.
        @param size
            The window size.
        @param style
            The window style. See wxNotebook.
        @param name
            The name of the control (used only under Motif).
    */
    wxNotebook();
    wxNotebook(wxWindow* parent, wxWindowID id,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxString& name = wxNotebookNameStr);
    //@}

    /**
        Destroys the wxNotebook object.
    */
    ~wxNotebook();

    /**
        Adds a new page.
        The call to this function may generate the page changing events.
        
        @param page
            Specifies the new page.
        @param text
            Specifies the text for the new page.
        @param select
            Specifies whether the page should be selected.
        @param imageId
            Specifies the optional image index for the new page.
        
        @returns @true if successful, @false otherwise.
        
        @remarks Do not delete the page, it will be deleted by the notebook.
        
        @see InsertPage()
    */
    bool AddPage(wxNotebookPage* page, const wxString& text,
                 bool select = false,
                 int imageId = -1);

    /**
        Cycles through the tabs.
        The call to this function generates the page changing events.
    */
    void AdvanceSelection(bool forward = true);

    /**
        Sets the image list for the page control and takes ownership of
        the list.
        
        @see wxImageList, SetImageList()
    */
    void AssignImageList(wxImageList* imageList);

    /**
        Changes the selection for the given page, returning the previous selection.
        The call to this function does not generate the page changing events.
        This is the only difference with SetSelection().
        See @ref overview_progevent "this topic" for more info.
    */
    int ChangeSelection(size_t page);

    /**
        Creates a notebook control. See wxNotebook() for a description
        of the parameters.
    */
    bool Create(wxWindow* parent, wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size, long style = 0,
                const wxString& name = wxNotebookNameStr);

    /**
        Deletes all pages.
    */
    bool DeleteAllPages();

    /**
        Deletes the specified page, and the associated window.
        The call to this function generates the page changing events.
    */
    bool DeletePage(size_t page);

    /**
        Returns the currently selected notebook page or @NULL.
    */
    wxWindow* GetCurrentPage();

    /**
        Returns the associated image list.
        
        @see wxImageList, SetImageList()
    */
    wxImageList* GetImageList();

    /**
        Returns the window at the given page position.
    */
    wxNotebookPage* GetPage(size_t page);

    /**
        Returns the number of pages in the notebook control.
    */
    size_t GetPageCount();

    /**
        Returns the image index for the given page.
    */
    int GetPageImage(size_t nPage);

    /**
        Returns the string for the given page.
    */
    wxString GetPageText(size_t nPage);

    /**
        Returns the number of rows in the notebook control.
    */
    int GetRowCount();

    /**
        Returns the currently selected page, or -1 if none was selected.
        Note that this method may return either the previously or newly selected page
        when called from the @c EVT_NOTEBOOK_PAGE_CHANGED handler depending on
        the platform and so
        wxNotebookEvent::GetSelection should be
        used instead in this case.
    */
    int GetSelection();

    /**
        If running under Windows and themes are enabled for the application, this
        function
        returns a suitable colour for painting the background of a notebook page, and
        can be passed
        to @c SetBackgroundColour. Otherwise, an uninitialised colour will be returned.
    */
    wxColour GetThemeBackgroundColour();

    /**
        Returns the index of the tab at the specified position or @c wxNOT_FOUND
        if none. If @a flags parameter is non-@NULL, the position of the point
        inside the tab is returned as well.
        
        @param pt
            Specifies the point for the hit test.
        @param flags
            Return value for detailed information. One of the following values:
        
        
        
        
        
        
        
            wxBK_HITTEST_NOWHERE
        
        
        
        
            There was no tab under this point.
        
        
        
        
        
            wxBK_HITTEST_ONICON
        
        
        
        
            The point was over an icon (currently wxMSW only).
        
        
        
        
        
            wxBK_HITTEST_ONLABEL
        
        
        
        
            The point was over a label (currently wxMSW only).
        
        
        
        
        
            wxBK_HITTEST_ONITEM
        
        
        
        
            The point was over an item, but not on the label or icon.
        
        
        
        
        
            wxBK_HITTEST_ONPAGE
        
        
        
        
            The point was over a currently selected page, not over any tab. Note that
        this flag is present only if wxNOT_FOUND is returned.
        
        @returns Returns the zero-based tab index or wxNOT_FOUND if there is no
                 tab is at the specified position.
    */
    int HitTest(const wxPoint& pt, long flags = NULL);

    /**
        Inserts a new page at the specified position.
        
        @param index
            Specifies the position for the new page.
        @param page
            Specifies the new page.
        @param text
            Specifies the text for the new page.
        @param select
            Specifies whether the page should be selected.
        @param imageId
            Specifies the optional image index for the new page.
        
        @returns @true if successful, @false otherwise.
        
        @remarks Do not delete the page, it will be deleted by the notebook.
        
        @see AddPage()
    */
    bool InsertPage(size_t index, wxNotebookPage* page,
                    const wxString& text,
                    bool select = false,
                    int imageId = -1);

    /**
        An event handler function, called when the page selection is changed.
        
        @see wxNotebookEvent
    */
    void OnSelChange(wxNotebookEvent& event);

    /**
        Deletes the specified page, without deleting the associated window.
    */
    bool RemovePage(size_t page);

    /**
        Sets the image list for the page control. It does not take
        ownership of the image list, you must delete it yourself.
        
        @see wxImageList, AssignImageList()
    */
    void SetImageList(wxImageList* imageList);

    /**
        Sets the amount of space around each page's icon and label, in pixels.
        @b NB: The vertical padding cannot be changed in wxGTK.
    */
    void SetPadding(const wxSize& padding);

    /**
        Sets the image index for the given page. @a image is an index into
        the image list which was set with SetImageList().
    */
    bool SetPageImage(size_t page, int image);

    /**
        Sets the width and height of the pages.
        @b NB: This method is currently not implemented for wxGTK.
    */
    void SetPageSize(const wxSize& size);

    /**
        Sets the text for the given page.
    */
    bool SetPageText(size_t page, const wxString& text);

    /**
        Sets the selection for the given page, returning the previous selection.
        The call to this function generates the page changing events.
        This function is deprecated and should not be used in new code. Please use the
        ChangeSelection() function instead.
        
        @see GetSelection()
    */
    int SetSelection(size_t page);
};
