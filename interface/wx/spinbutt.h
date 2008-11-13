/////////////////////////////////////////////////////////////////////////////
// Name:        spinbutt.h
// Purpose:     interface of wxSpinEvent, wxSpinButton
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxSpinEvent

    This event class is used for the events generated by wxSpinButton and wxSpinCtrl.

    @beginEventTable{wxSpinEvent}
    @event{EVT_SPIN(id, func)}
        Generated whenever an arrow is pressed.
    @event{EVT_SPIN_UP(id, func)}
        Generated when left/up arrow is pressed.
    @event{EVT_SPIN_DOWN(id, func)}
        Generated when right/down arrow is pressed.
    @endEventTable

    Note that if you handle both SPIN and UP or DOWN events, you will be notified
    about each of them twice: first the UP/DOWN event will be received and then,
    if it wasn't vetoed, the SPIN event will be sent.

    @library{wxcore}
    @category{events}

    @see wxSpinButton and wxSpinCtrl
*/
class wxSpinEvent : public wxNotifyEvent
{
public:
    /**
        The constructor is not normally used by the user code.
    */
    wxSpinEvent(wxEventType commandType = wxEVT_NULL, int id = 0);

    /**
        Retrieve the current spin button or control value.
    */
    int GetPosition() const;

    /**
        Set the value associated with the event.
    */
    void SetPosition(int pos);
};



/**
    @class wxSpinButton

    A wxSpinButton has two small up and down (or left and right) arrow buttons.

    It is often used next to a text control for increment and decrementing a value.
    Portable programs should try to use wxSpinCtrl instead as wxSpinButton is not
    implemented for all platforms but wxSpinCtrl is as it degenerates to a simple
    wxTextCtrl on such platforms.

    @note the range supported by this control (and wxSpinCtrl) depends on the
          platform but is at least @c -0x8000 to @c 0x7fff. Under GTK and
          Win32 with sufficiently new version of @c comctrl32.dll (at least 4.71
          is required, 5.80 is recommended) the full 32 bit range is supported.

    @beginStyleTable
    @style{wxSP_HORIZONTAL}
           Specifies a horizontal spin button (note that this style is not
           supported in wxGTK).
    @style{wxSP_VERTICAL}
           Specifies a vertical spin button.
    @style{wxSP_ARROW_KEYS}
           The user can use arrow keys to change the value.
    @style{wxSP_WRAP}
           The value wraps at the minimum and maximum.
    @endStyleTable

    @beginEventTable{wxSpinEvent}
    @event{EVT_SPIN(id, func)}
        Generated whenever an arrow is pressed.
    @event{EVT_SPIN_UP(id, func)}
        Generated when left/up arrow is pressed.
    @event{EVT_SPIN_DOWN(id, func)}
        Generated when right/down arrow is pressed.
    @endEventTable

    Note that if you handle both SPIN and UP or DOWN events, you will be notified
    about each of them twice: first the UP/DOWN event will be received and then,
    if it wasn't vetoed, the SPIN event will be sent.

    @library{wxcore}
    @category{ctrl}
    @appearance{spinbutton.png}

    @see wxSpinCtrl
*/
class wxSpinButton : public wxControl
{
public:
    /**
        Default constructor.
    */
    wxSpinButton();

    /**
        Constructor, creating and showing a spin button.

        @param parent
            Parent window. Must not be @NULL.
        @param id
            Window identifier. The value wxID_ANY indicates a default value.
        @param pos
            Window position.
            If wxDefaultPosition is specified then a default position is chosen.
        @param size
            Window size.
            If wxDefaultSize is specified then a default size is chosen.
        @param style
            Window style. See wxSpinButton class description.
        @param name
            Window name.

        @see Create()
    */
    wxSpinButton(wxWindow* parent, wxWindowID id = -1,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxSP_VERTICAL,
                 const wxString& name = "spinButton");

    /**
        Destructor, destroys the spin button control.
    */
    virtual ~wxSpinButton();

    /**
        Scrollbar creation function called by the spin button constructor.
        See wxSpinButton() for details.
    */
    bool Create(wxWindow* parent, wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSP_VERTICAL,
                const wxString& name = "wxSpinButton");

    /**
        Returns the maximum permissible value.

        @see SetRange()
    */
    virtual int GetMax() const;

    /**
        Returns the minimum permissible value.

        @see SetRange()
    */
    virtual int GetMin() const;

    /**
        Returns the current spin button value.

        @see SetValue()
    */
    virtual int GetValue() const;

    /**
        Sets the range of the spin button.

        @param min
            The minimum value for the spin button.
        @param max
            The maximum value for the spin button.

        @see GetMin(), GetMax()
    */
    virtual void SetRange(int min, int max);

    /**
        Sets the value of the spin button.

        @param value
            The value for the spin button.
    */
    virtual void SetValue(int value);
};

