///////////////////////////////////////////////////////////////////////////////
// Name:        ribbon/art.h
// Purpose:     interface of wxRibbonArtProvider
// Author:      Peter Cawley
// RCS-ID:      $Id$
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/**
    Identifiers for common settings on ribbon art providers which can be used
    to tweak the appearance of the art provider.
    
    @see wxRibbonArtProvider::GetColour()
    @see wxRibbonArtProvider::GetFont()
    @see wxRibbonArtProvider::GetMetric()
    @see wxRibbonArtProvider::SetColour()
    @see wxRibbonArtProvider::SetFont()
    @see wxRibbonArtProvider::SetMetric()
*/
enum wxRibbonArtSetting
{
    wxRIBBON_ART_TAB_SEPARATION_SIZE,
    wxRIBBON_ART_PAGE_BORDER_LEFT_SIZE,
    wxRIBBON_ART_PAGE_BORDER_TOP_SIZE,
    wxRIBBON_ART_PAGE_BORDER_RIGHT_SIZE,
    wxRIBBON_ART_PAGE_BORDER_BOTTOM_SIZE,
    wxRIBBON_ART_PANEL_X_SEPARATION_SIZE,
    wxRIBBON_ART_PANEL_Y_SEPARATION_SIZE,
    wxRIBBON_ART_PANEL_LABEL_FONT,
    wxRIBBON_ART_BUTTON_BAR_LABEL_FONT,
    wxRIBBON_ART_TAB_LABEL_FONT,
    wxRIBBON_ART_BUTTON_BAR_LABEL_COLOUR,
    wxRIBBON_ART_BUTTON_BAR_HOVER_BORDER_COLOUR,
    wxRIBBON_ART_BUTTON_BAR_HOVER_BACKGROUND_TOP_COLOUR,
    wxRIBBON_ART_BUTTON_BAR_HOVER_BACKGROUND_TOP_GRADIENT_COLOUR,
    wxRIBBON_ART_BUTTON_BAR_HOVER_BACKGROUND_COLOUR,
    wxRIBBON_ART_BUTTON_BAR_HOVER_BACKGROUND_GRADIENT_COLOUR,
    wxRIBBON_ART_TAB_LABEL_COLOUR,
    wxRIBBON_ART_TAB_SEPARATOR_COLOUR,
    wxRIBBON_ART_TAB_SEPARATOR_GRADIENT_COLOUR,
    wxRIBBON_ART_TAB_CTRL_BACKGROUND_COLOUR,
    wxRIBBON_ART_TAB_HOVER_BACKGROUND_TOP_COLOUR,
    wxRIBBON_ART_TAB_HOVER_BACKGROUND_TOP_GRADIENT_COLOUR,
    wxRIBBON_ART_TAB_HOVER_BACKGROUND_COLOUR,
    wxRIBBON_ART_TAB_HOVER_BACKGROUND_GRADIENT_COLOUR,
    wxRIBBON_ART_TAB_ACTIVE_BACKGROUND_COLOUR,
    wxRIBBON_ART_TAB_ACTIVE_BACKGROUND_GRADIENT_COLOUR,
    wxRIBBON_ART_TAB_BORDER_COLOUR,
    wxRIBBON_ART_PANEL_BORDER_COLOUR,
    wxRIBBON_ART_PANEL_BORDER_GRADIENT_COLOUR,
    wxRIBBON_ART_PANEL_MINIMISED_BORDER_COLOUR,
    wxRIBBON_ART_PANEL_MINIMISED_BORDER_GRADIENT_COLOUR,
    wxRIBBON_ART_PANEL_LABEL_BACKGROUND_COLOUR,
    wxRIBBON_ART_PANEL_LABEL_COLOUR,
    wxRIBBON_ART_PANEL_HOVER_LABEL_BACKGROUND_COLOUR,
    wxRIBBON_ART_PANEL_HOVER_LABEL_COLOUR,
    wxRIBBON_ART_PANEL_MINIMISED_LABEL_COLOUR,
    wxRIBBON_ART_PANEL_ACTIVE_BACKGROUND_TOP_COLOUR,
    wxRIBBON_ART_PANEL_ACTIVE_BACKGROUND_TOP_GRADIENT_COLOUR,
    wxRIBBON_ART_PANEL_ACTIVE_BACKGROUND_COLOUR,
    wxRIBBON_ART_PANEL_ACTIVE_BACKGROUND_GRADIENT_COLOUR,
    wxRIBBON_ART_PAGE_BORDER_COLOUR,
    wxRIBBON_ART_PAGE_BACKGROUND_TOP_COLOUR,
    wxRIBBON_ART_PAGE_BACKGROUND_TOP_GRADIENT_COLOUR,
    wxRIBBON_ART_PAGE_BACKGROUND_COLOUR,
    wxRIBBON_ART_PAGE_BACKGROUND_GRADIENT_COLOUR,
    wxRIBBON_ART_PAGE_HOVER_BACKGROUND_TOP_COLOUR,
    wxRIBBON_ART_PAGE_HOVER_BACKGROUND_TOP_GRADIENT_COLOUR,
    wxRIBBON_ART_PAGE_HOVER_BACKGROUND_COLOUR,
    wxRIBBON_ART_PAGE_HOVER_BACKGROUND_GRADIENT_COLOUR,
};

/**
    Flags used to describe the direction, state, and/or purpose of a
    ribbon-style scroll button.
    
    @see wxRibbonArtProvider::DrawScrollButton()
    @see wxRibbonArtProvider::GetScrollButtonMinimumSize()
*/
enum wxRibbonScrollButtonStyle
{
    wxRIBBON_SCROLL_BTN_LEFT = 0, /**< Button will scroll to the left. */
    wxRIBBON_SCROLL_BTN_RIGHT = 1, /**< Button will scroll to the right. */
    wxRIBBON_SCROLL_BTN_UP = 2, /**< Button will scroll upward. */
    wxRIBBON_SCROLL_BTN_DOWN = 3, /**< Button will scroll downward. */

  /** A mask to extract direction from a combination of flags. */
    wxRIBBON_SCROLL_BTN_DIRECTION_MASK = 3,

    wxRIBBON_SCROLL_BTN_NORMAL = 0, /**< Button is not active or hovered. */
    wxRIBBON_SCROLL_BTN_HOVERED = 4, /**< Button has a cursor hovering over it. */
    wxRIBBON_SCROLL_BTN_ACTIVE = 8, /**< Button is being pressed. */

  /** A mask to extract state from a combination of flags. */
    wxRIBBON_SCROLL_BTN_STATE_MASK = 12,

    wxRIBBON_SCROLL_BTN_FOR_OTHER = 0, /**< Button is not for scrolling tabs nor pages. */
    wxRIBBON_SCROLL_BTN_FOR_TABS = 16, /**< Button is for scrolling tabs. */
    wxRIBBON_SCROLL_BTN_FOR_PAGE = 32, /**< Button is for scrolling pages. */

  /** A mask to extract purpose from a combination of flags. */
    wxRIBBON_SCROLL_BTN_FOR_MASK = 48,
};

/**
    @class wxRibbonArtProvider

    wxRibbonArtProvider is responsible for drawing all the components of the ribbon
    interface. This allows a ribbon bar to have a pluggable look-and-feel, while
    retaining the same underlying behaviour. As a single art provider is used for
    all ribbon components, a ribbon bar usually has a consistent (though unique)
    appearance.

    By default, a wxRibbonBar uses an instance of this class called
    wxRibbonDefaultArtProvider, which resolves to wxRibbonAUIArtProvider,
    wxRibbonMSWArtProvider, or wxRibbonOSXArtProvider - whichever is most appropriate
    to the current platform. These art providers are all slightly configurable with
    regard to colours and fonts, but for larger modifications, you can derive from
    one of these classes, or write a completely new art provider class.
    Call wxRibbonBar::SetArtProvider to change the art provider being used.

    @library{wxribbon}
    @category{ribbon}

    @see wxRibbonBar
*/
class wxRibbonArtProvider
{
public:
    /**
        Constructor.
    */
    wxRibbonArtProvider();
    
    /**
        Destructor.
    */
    virtual ~wxRibbonArtProvider();

    /**
        Create a new art provider which is a clone of this one.
    */
    virtual wxRibbonArtProvider* Clone() = 0;
    
    /**
        Set the style flags.
        
        Normally called automatically by wxRibbonBar::SetArtProvider with the ribbon
        bar's style flags, so that the art provider has the same flags as the bar which
        it is serving.
    */
    virtual void SetFlags(long flags) = 0;
    
    /**
        Get the previously set style flags.
    */
    virtual long GetFlags() = 0;

    /**
        Get the value of a certain integer setting.
        @a id can be one of the size values of @ref wxRibbonArtSetting.
    */
    virtual int GetMetric(int id) = 0;
    
    /**
        Set the value of a certain integer setting to the value @e new_val.
        @a id can be one of the size values of @ref wxRibbonArtSetting.
    */
    virtual void SetMetric(int id, int new_val) = 0;
    
    /**
        Set the value of a certain font setting to the value @e font.
        @a id can be one of the font values of @ref wxRibbonArtSetting.
    */
    virtual void SetFont(int id, const wxFont& font) = 0;
    
    /**
        Get the value of a certain font setting.
        @a id can be one of the font values of @ref wxRibbonArtSetting.
    */
    virtual wxFont GetFont(int id) = 0;
    
    /**
        Get the value of a certain colour setting.
        @a id can be one of the colour values of @ref wxRibbonArtSetting.
    */
    virtual wxColour GetColour(int id) = 0;
    
    /**
        Set the value of a certain colour setting to the value @e colour.
        @a id can be one of the colour values of @ref wxRibbonArtSetting, though
        not all colour settings will have an affect on every art provider.
    */
    virtual void SetColour(int id, const wxColor& colour) = 0;
    
    /**
        \see wxRibbonArtProvider::GetColour()
    */
    wxColour GetColor(int id);
    
    /**
        \see wxRibbonArtProvider::SetColour()
    */
    void SetColor(int id, const wxColour& color);

    /**
        Draw the background of the tab region of a ribbon bar.
        
        @param dc
            The device context to draw onto.
        @param wnd
            The window which is being drawn onto.
        @param rect
            The rectangle within which to draw.
    */
    virtual void DrawTabCtrlBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect) = 0;

    /**
        Draw a single tab in the tab region of a ribbon bar.
        
        @param dc
            The device context to draw onto.
        @param wnd
            The window which is being drawn onto (not the wxRibbonPage
            associated with the tab being drawn).
        @param tab
            The rectangle within which to draw, and also the tab label, icon,
            and state (active and/or hovered). The drawing rectangle will be
            entirely within a rectangle on the same device context previously
            painted with DrawTabCtrlBackground(). The rectangle's width will
            be at least the minimum value returned by GetBarTabWidth(), and
            height will be the value returned by GetTabCtrlHeight().
    */
    virtual void DrawTab(wxDC& dc,
                        wxWindow* wnd,
                        const wxRibbonPageTabInfo& tab) = 0;

    /**
        Draw a separator between two tabs in a ribbon bar.
        
        @param dc
            The device context to draw onto.
        @param wnd
            The window which is being drawn onto.
        @param rect
            The rectangle within which to draw, which will be entirely within a
            rectangle on the same device context previously painted with
            DrawTabCtrlBackground().
        @param visibility
            The opacity with which to draw the separator. Values are in the range
            [0, 1], with 0 being totally transparent, and 1 being totally opaque.
    */
    virtual void DrawTabSeparator(wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect,
                        double visibility) = 0;

    /**
        Draw the background of a ribbon page.
        
        @param dc
            The device context to draw onto.
        @param wnd
            The window which is being drawn onto (which is commonly the wxRibbonPage
            whose background is being drawn, but doesn't have to be).
        @param rect
            The rectangle within which to draw.
        
        @sa GetPageBackgroundRedrawArea
    */
    virtual void DrawPageBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect) = 0;

    /**
        Draw a ribbon-style scroll button.
        
        @param dc
            The device context to draw onto.
        @param wnd
            The window which is being drawn onto.
        @param rect
            The rectangle within which to draw. The size of this rectangle
            will be at least the size returned by GetScrollButtonMinimumSize()
            for a scroll button with the same style. For tab scroll buttons,
            this rectangle will be entirely within a rectangle on the same
            device context previously painted with DrawTabCtrlBackground(), but
            this is not guaranteed for other types of button (for example,
            page scroll buttons will not be painted on an area previously
            painted with DrawPageBackground()).
        @param style
            A combination of flags from @ref wxRibbonScrollButtonStyle, including
            a direction, a for flag, and one or more states.
    */
    virtual void DrawScrollButton(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect,
                        long style) = 0;

    /**
        Draw the background and chrome for a ribbon panel. This should draw
        the border, background, label, and any other items of a panel which
        are outside the client area of a panel.
        
        Note that when a panel is minimised, this function is not called - only
        DrawMinimisedPanel() is called, so a background should be explicitly
        painted by that if required.
        
        @param dc
            The device context to draw onto.
        @param wnd
            The window which is being drawn onto, which is always the panel
            whose background and chrome is being drawn. The panel label and
            other panel attributes can be obtained by querying this.
        @param rect
            The rectangle within which to draw.
    */
    virtual void DrawPanelBackground(
                        wxDC& dc,
                        wxRibbonPanel* wnd,
                        const wxRect& rect) = 0;
    
    /**
        Draw a minimised ribbon panel.
        
        @param dc
            The device context to draw onto.
        @param wnd
            The window which is being drawn onto, which is always the panel
            which is minimised. The panel label can be obtained from this
            window. The minimised icon obtained from querying the window may
            not be the size requested by GetMinimisedPanelMinimumSize() - the
            @a bitmap argument contains the icon in the requested size.
        @param rect
            The rectangle within which to draw. The size of the rectangle will
            be at least the size returned by GetMinimisedPanelMinimumSize().
        @param bitmap
            A copy of the panel's minimised bitmap rescaled to the size
            returned by GetMinimisedPanelMinimumSize().
    */
    virtual void DrawMinimisedPanel(
                        wxDC& dc,
                        wxRibbonPanel* wnd,
                        const wxRect& rect,
                        wxBitmap& bitmap) = 0;
    
    /**
        Draw the background for a wxRibbonButtonBar control.
        
        @param dc
            The device context to draw onto.
        @param wnd
            The window which is being drawn onto (which will typically be the
            button bar itself, though this is not guaranteed).
        @param rect
            The rectangle within which to draw.
    */
    virtual void DrawButtonBarBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect) = 0;

    /**
        Draw a single button for a wxRibbonButtonBar control.
        
        @param dc
            The device context to draw onto.
        @param wnd
            The window which is being drawn onto.
        @param rect
            The rectangle within which to draw. The size of this rectangle will
            be a size previously returned by GetButtonBarButtonSize(), and the
            rectangle will be entirely within a rectangle on the same device
            context previously painted with DrawButtonBarBackground().
        @param kind
            The kind of button to draw (normal, dropdown or hybrid).
        @param state
            Combination of a size flag and state flags from the
            wxRibbonButtonBarButtonState enumeration.
        @param label
            The label of the button.
        @param bitmap_large
            The large bitmap of the button (or the large disabled bitmap when
            wxRIBBON_BUTTONBAR_BUTTON_DISABLED is set in @a state).
        @param bitmap_small
            The small bitmap of the button (or the small disabled bitmap when
            wxRIBBON_BUTTONBAR_BUTTON_DISABLED is set in @a state).
    */
    virtual void DrawButtonBarButton(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect,
                        wxRibbonButtonBarButtonKind kind,
                        long state,
                        const wxString& label,
                        const wxBitmap& bitmap_large,
                        const wxBitmap& bitmap_small) = 0;

    /**
        Calculate the ideal and minimum width (in pixels) of a tab in a ribbon
        bar.
        
        @param dc
            A device context to use when one is required for size calculations.
        @param wnd
            The window onto which the tab will eventually be drawn.
        @param label
            The tab's label (or wxEmptyString if it has none).
        @param bitmap
            The tab's icon (or wxNullBitmap if it has none).
        @param[out] ideal
            The ideal width (in pixels) of the tab.
        @param[out] small_begin_need_separator
            A size less than the @a ideal size, at which a tab separator should
            begin to be drawn (i.e. drawn, but still fairly transparent).
        @param[out] small_must_have_separator
            A size less than the @a small_begin_need_separator size, at which a
            tab separator must be drawn (i.e. drawn at full opacity).
        @param[out] minimum
            A size less than the @a small_must_have_separator size, and greater
            than or equal to zero, which is the minimum pixel width for the tab.
    */
    virtual void GetBarTabWidth(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxString& label,
                        const wxBitmap& bitmap,
                        int* ideal,
                        int* small_begin_need_separator,
                        int* small_must_have_separator,
                        int* minimum) = 0;

    /**
        Calculate the height (in pixels) of the tab region of a ribbon bar.
        Note that as the tab region can contain scroll buttons, the height
        should be greater than or equal to the minimum height for a tab scroll
        button.
        
        @param dc
            A device context to use when one is required for size calculations.
        @param wnd
            The window onto which the tabs will eventually be drawn.
        @param pages
            The tabs which will acquire the returned height.
    */
    virtual int GetTabCtrlHeight(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRibbonPageTabInfoArray& pages) = 0;

    /**
        Calculate the minimum size (in pixels) of a scroll button.
        
        @param dc
            A device context to use when one is required for size calculations.
        @param wnd
            The window onto which the scroll button will eventually be drawn.
        @param style
            A combination of flags from @ref wxRibbonScrollButtonStyle, including
            a direction, and a for flag (state flags may be given too, but
            should be ignored, as a button should retain a constant size,
            regardless of its state).
    */
    virtual wxSize GetScrollButtonMinimumSize(
                        wxDC& dc,
                        wxWindow* wnd,
                        long style) = 0;

    /**
        Calculate the size of a panel for a given client size. This should
        increment the given size by enough to fit the panel label and other
        chrome.
        
        @param dc
            A device context to use if one is required for size calculations.
        @param wnd
            The ribbon panel in question.
        @param client_size
            The client size.
        @param[out] client_offset
            The offset where the client rectangle begins within the panel (may
            be NULL).
            
        @sa GetPanelClientSize()
    */
    virtual wxSize GetPanelSize(
                        wxDC& dc,
                        const wxRibbonPanel* wnd,
                        wxSize client_size,
                        wxPoint* client_offset) = 0;
    
    /**
        Calculate the client size of a panel for a given overall size. This
        should act as the inverse to GetPanelSize(), and decrement the given
        size by enough to fit the panel label and other chrome.
        
        @param dc
            A device context to use if one is required for size calculations.
        @param wnd
            The ribbon panel in question.
        @param size
            The overall size to calculate client size for.
        @param[out] client_offset
            The offset where the returned client size begins within the given
            @a size (may be NULL).
        
        @sa GetPanelSize()
    */
    virtual wxSize GetPanelClientSize(
                        wxDC& dc,
                        const wxRibbonPanel* wnd,
                        wxSize size,
                        wxPoint* client_offset) = 0;

    /**
        Calculate the portion of a page background which needs to be redrawn
        when a page is resized. To optimise the drawing of page backgrounds, as
        small an area as possible should be returned. Of couse, if the way in
        which a background is drawn means that the entire background needs to
        be repainted on resize, then the entire new size should be returned.
        
        @param dc
            A device context to use when one is required for size calculations.
        @param wnd
            The page which is being resized.
        @param page_old_size
            The size of the page prior to the resize (which has already been
            painted).
        @param page_new_size
            The size of the page after the resize.
    */
    virtual wxRect GetPageBackgroundRedrawArea(
                        wxDC& dc,
                        const wxRibbonPage* wnd,
                        wxSize page_old_size,
                        wxSize page_new_size) = 0;
    
    /**
        Calculate the size of a button within a wxRibbonButtonBar.
        
        @param dc
            A device context to use when one is required for size calculations.
        @param wnd
            The window onto which the button will eventually be drawn (which is
            normally a wxRibbonButtonBar, though this is not guaranteed).
        @param kind
            The kind of button.
        @param size
            The size-class to calculate the size for. Buttons on a button bar
            can have three distinct sizes: wxRIBBON_BUTTONBAR_BUTTON_SMALL,
            wxRIBBON_BUTTONBAR_BUTTON_MEDIUM, and wxRIBBON_BUTTONBAR_BUTTON_LARGE.
            If the requested size-class is not applicable, then @false should
            be returned.
        @param label
            The label of the button.
        @param bitmap_size_large
            The size of all "large" bitmaps on the button bar.
        @param bitmap_size_small
            The size of all "small" bitmaps on the button bar.
        @param[out] button_size
            The size, in pixels, of the button.
        @param[out] normal_region
            The region of the button which constitutes the normal button.
        @param[out] dropdown_region
            The region of the button which constitutes the dropdown button.
        
        @return @true if a size exists for the button, @false otherwise.
    */
    virtual bool GetButtonBarButtonSize(
                        wxDC& dc,
                        wxWindow* wnd,
                        wxRibbonButtonBarButtonKind kind,
                        wxRibbonButtonBarButtonState size,
                        const wxString& label,
                        wxSize bitmap_size_large,
                        wxSize bitmap_size_small,
                        wxSize* button_size,
                        wxRect* normal_region,
                        wxRect* dropdown_region) = 0;
    
    /**
        Calculate the size of a minimised ribbon panel.
        
        @param dc
            A device context to use when one is required for size calculations.
        @param wnd
            The ribbon panel in question. Attributes like the panel label can
            be queried from this.
        @param[out] desired_bitmap_size
            
    */
    virtual wxSize GetMinimisedPanelMinimumSize(
                        wxDC& dc,
                        const wxRibbonPanel* wnd,
                        wxSize* desired_bitmap_size,
                        wxDirection* expanded_panel_direction) = 0;
};
