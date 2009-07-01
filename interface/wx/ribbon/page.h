///////////////////////////////////////////////////////////////////////////////
// Name:        ribbon/page.h
// Purpose:     interface of wxRibbonPage
// Author:      Peter Cawley
// RCS-ID:      $Id$
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/**
    @class wxRibbonPage

    Container for related ribbon panels, and a tab within a ribbon bar.
    
    @see wxRibbonBar
    @see wxRibbonPanel

    @library{wxribbon}
    @category{ribbon}
*/
class wxRibbonPage : public wxRibbonControl
{
public:
  /**
    Default constructor. 
  */
	wxRibbonPage();

  /**
    Constructs a ribbon page, which must be a child of a ribbon bar.
    
    @param parent
        Pointer to a parent wxRibbonBar (unlike most controls, a wxRibbonPage
        can only have wxRibbonBar as a parent).
    @param id
        Window identifier.
    @param label
        Label to be used in the wxRibbonBar's tab list for this page (if the
        ribbon bar is set to display labels).
    @param icon
        Icon to be used in the wxRibbonBar's tab list for this page (if the
        ribbon bar is set to display icons).
    @param style
        Currently unused, should be zero.
  */
	wxRibbonPage(wxRibbonBar* parent,
				 wxWindowID id = wxID_ANY,
				 const wxString& label = wxEmptyString,
				 const wxBitmap& icon = wxNullBitmap,
				 long style = 0);

  /**
    Destructor.
  */
	virtual ~wxRibbonPage();

  /**
    Set the art provider to be used. Normally called automatically by
    wxRibbonBar when the page is created, or the art provider changed on the
    bar.
    
    The new art provider will be propagated to the children of the frame.
  */
	void SetArtProvider(wxRibbonArtProvider* art);

  /**
    Get the icon used for the page in the ribbon bar tab area (only
    displayed if the ribbon bar is actuallt showing icons).
  */
	wxBitmap& GetIcon();
  
  /**
    Set the size of the page and the external scroll buttons (if any).
    
    When a page is too small to display all of its children, scroll buttons
    will appear (and if the page is sized up enough, they will disappear again).
    Slightly counter-intuively, these buttons are created as siblings of the
    page rather than children of the page (to achieve correct cropping and
    paint ordering of the children and the buttons). When there are no scroll
    buttons, this function behaves the same as SetSize(), however when there
    are scroll buttons, it positions them at the edges of the given area, and
    then calls SetSize() with the remaining area.
    
    This is provided as a separate function to SetSize() rather than within
    the implementation of SetSize(), as iteracting algorithms may not expect
    SetSize() to also set the size of siblings.
  */
	void SetSizeWithScrollButtonAdjustment(int x, int y, int width, int height);

  /**
    Scroll the page by some amount up / down / left / right.
    
    When the page's children are too big to fit in the onscreen area given to
    the page, scroll buttons will appear, and the page can be programatically
    scrolled. Positive values of @a lines will scroll right or down, while
    negative values will scroll up or left (depending on the direction in which
    panels are stacked). A line is equivalent to a constant number of pixels.
    
    @return @true if the page scrolled at least one pixel in the given
    direction, @false if it did not scroll.
    
    @see GetMajorAxis()
    @see ScrollPixels()
  */
	virtual bool ScrollLines(int lines);
  
  /**
    Scroll the page by a set number of pixels up / down / left / right.
    
    When the page's children are too big to fit in the onscreen area given to
    the page, scroll buttons will appear, and the page can be programatically
    scrolled. Positive values of @a lines will scroll right or down, while
    negative values will scroll up or left (depending on the direction in which
    panels are stacked).
    
    @return @true if the page scrolled at least one pixel in the given
    direction, @false if it did not scroll.
    
    @see GetMajorAxis()
    @see ScrollLines()
  */
	bool ScrollPixels(int pixels);

  /**
      Get the direction in which ribbon panels are stacked within the page.
      
      This is controlled by the style of the containing wxRibbonBar, meaning
      that all pages within a bar will have the same major axis. As well as
      being the direction in which panels are stacked, it is also the axis in
      which scrolling will occur (when required).
      
      @return wxHORIZONTAL or wxVERTICAL (never wxBOTH).
  */
	wxOrientation GetMajorAxis() const;
};
