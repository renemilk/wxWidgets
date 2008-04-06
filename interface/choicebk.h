/////////////////////////////////////////////////////////////////////////////
// Name:        choicebk.h
// Purpose:     interface of wxChoicebook
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxChoicebook
    @wxheader{choicebk.h}

    wxChoicebook is a class similar to wxNotebook, but uses a wxChoice control
    to show the labels instead of the tabs.

    There is no documentation for this class yet but its usage is identical to
    wxNotebook (except for the features clearly related to tabs only), so
    please refer to that class documentation for now. You can also use the
    @ref page_samples_notebook to see wxChoicebook in action.

    wxChoicebook allows the use of wxBookCtrlBase::GetControlSizer(), allowing
    a program to add other controls next to the choice control. This is
    particularly useful when screen space is restricted, as it often is when
    wxChoicebook is being employed.

    @beginStyleTable
    @style{wxCHB_DEFAULT}
           Choose the default location for the labels depending on the current
           platform (left everywhere except Mac where it is top).
    @style{wxCHB_TOP}
           Place labels above the page area.
    @style{wxCHB_LEFT}
           Place labels on the left side.
    @style{wxCHB_RIGHT}
           Place labels on the right side.
    @style{wxCHB_BOTTOM}
           Place labels below the page area.
    @endStyleTable

    @beginEventTable{wxChoicebookEvent}
    @event{EVT_CHOICEBOOK_PAGE_CHANGED(id, func)}
           The page selection was changed. Processes a
           wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED event.
    @event{EVT_CHOICEBOOK_PAGE_CHANGING(id, func)}
           The page selection is about to be changed. Processes a
           wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGING event. This event can be
           vetoed (using wxNotifyEvent::Veto()).
    @endEventTable

    @library{wxcore}
    @category{miscwnd}

    @see @ref overview_bookctrl, wxNotebook, @ref page_samples_notebook

    @todo Write up wxBookCtrlBase documentation, where most of this class'
          functionality comes from.
*/
class wxChoicebook : public wxBookCtrlBase
{
public:
    //@{
    /**
        Constructs a choicebook control.
    */
    wxChoicebook();
    wxChoicebook(wxWindow* parent, wxWindowID id,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxString& name = wxEmptyStr);
    //@}

    /**
        Returns the wxChoice associated with the control.
    */
    wxChoice * GetChoiceCtrl() const;
};

