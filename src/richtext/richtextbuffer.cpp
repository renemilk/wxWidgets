/////////////////////////////////////////////////////////////////////////////
// Name:        src/richtext/richtextbuffer.cpp
// Purpose:     Buffer for wxRichTextCtrl
// Author:      Julian Smart
// Modified by:
// Created:     2005-09-30
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_RICHTEXT

#include "wx/richtext/richtextbuffer.h"

#ifndef WX_PRECOMP
    #include "wx/dc.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/dataobj.h"
    #include "wx/module.h"
#endif

#include "wx/settings.h"
#include "wx/filename.h"
#include "wx/clipbrd.h"
#include "wx/wfstream.h"
#include "wx/mstream.h"
#include "wx/sstream.h"
#include "wx/textfile.h"
#include "wx/hashmap.h"
#include "wx/dynarray.h"

#include "wx/richtext/richtextctrl.h"
#include "wx/richtext/richtextstyles.h"

#include "wx/listimpl.cpp"

WX_DEFINE_LIST(wxRichTextObjectList)
WX_DEFINE_LIST(wxRichTextLineList)

// Switch off if the platform doesn't like it for some reason
#define wxRICHTEXT_USE_OPTIMIZED_DRAWING 1

// Use GetPartialTextExtents for platforms that support it natively
#define wxRICHTEXT_USE_PARTIAL_TEXT_EXTENTS 1

const wxChar wxRichTextLineBreakChar = (wxChar) 29;

// Helper classes for floating layout
struct FloatRectMap
{
    FloatRectMap(int sY, int eY, int w)
    {
        startY = sY;
        endY = eY;
        width = w;
    }

    int startY, endY;
    int width;
};

WX_DEFINE_SORTED_ARRAY(FloatRectMap*, FloatRectMapArray);

int FloatRectMapCmp(FloatRectMap* r1, FloatRectMap* r2)
{
    return r1->startY - r2->startY;
}

class wxFloatCollector
{
public:
    wxFloatCollector(int width);
    ~wxFloatCollector();

    // Collect the floating objects info in the given paragraph
    void CollectFloat(wxRichTextParagraph* para);
    void CollectFloat(wxRichTextParagraph* para, wxRichTextObject* floating);
    // Return the last paragraph we collected
    wxRichTextParagraph* LastParagraph();
    // Given the start y position and the height of the line,
    // find out how wide the line can be
    wxRect GetAvailableRect(int startY, int endY);
    // Given a floating box, find its fit position
    long GetFitPosition(int direction, int start, int height);
    long GetFitPosition(const FloatRectMapArray& array, int start, int height);
    // Find the last y position
    int GetLastRectBottom();
private:
    FloatRectMapArray m_left;
    FloatRectMapArray m_right;
    int m_width;
    wxRichTextParagraph* m_para;
};

/*
 * Binary search helper function
 * The argument point is the Y coordinate, and this fuction
 * always return the floating rect that contain this coordinate
 * or under this coordinate.
 */
int SearchAdjacentRect(const FloatRectMapArray& array, int point)
{
    int end = array.GetCount() - 1;
    int start = 0;
    int ret = 0;

    assert(end >= 0);

    while (true)
    {
        if (start > end)
        {
            break;
        }

        int mid = (start + end) / 2;
        if (array[mid]->startY <= point && array[mid]->endY >= point)
            return mid;
        else if (array[mid]->startY > point)
        {
            end = mid - 1;
            ret = mid;
        }
        else if (array[mid]->endY < point)
        {
            start = mid + 1;
            ret = start;
        }
    }

    return ret;
}

int GetWidthFromFloatRect(const FloatRectMapArray& array, int index, int startY, int endY)
{
    int ret = 0;
    int len = array.GetCount();

    assert(index >= 0 && index < len);

    if (array[index]->startY < startY)
    {
        assert(array[index]->endY > startY);
        ret = ret < array[index]->width ? array[index]->width : ret;
        while (array[index]->startY <= endY && index < len)
        {
            ret = ret < array[index]->width ? array[index]->width : ret;
            index++;
        }
    }
    else
    {
        while (array[index]->startY <= endY && index < len)
        {
            ret = ret < array[index]->width ? array[index]->width : ret;
            index++;
        }
    }

    return ret;
}

wxFloatCollector::wxFloatCollector(int width) : m_left(FloatRectMapCmp), m_right(FloatRectMapCmp)
{
    m_width = width;
    m_para = NULL;
}

wxFloatCollector::~wxFloatCollector()
{
}

long wxFloatCollector::GetFitPosition(const FloatRectMapArray& array, int start, int height)
{
    if (array.GetCount() == 0)
        return start;

    int i = SearchAdjacentRect(array, start);
    int last = start;
    while (i < array.GetCount())
    {
        if (array[i]->startY - last >= height)
            return last;
        last = array[i]->endY;
        i++;
    }

    return array[i]->endY + 1;
}

long wxFloatCollector::GetFitPosition(int direction, int start, int height)
{
    if (direction == wxRICHTEXT_FLOAT_LEFT)
        return GetFitPosition(m_left, start, height);
    else if (direction == wxRICHTEXT_FLOAT_RIGHT)
        return GetFitPosition(m_right, start, height);
    else
    {
        assert("Never should be here");
        return start;
    }
}

void wxFloatCollector::CollectFloat(wxRichTextParagraph* para, wxRichTextObject* floating)
{
        int direction = floating->GetFloatDirection();
        wxPoint pos = floating->GetPosition();
        wxSize size = floating->GetCachedSize();
        FloatRectMap *map = new FloatRectMap(pos.y, pos.y+size.y, size.x);

        switch (direction)
        {
            case wxRICHTEXT_FLOAT_NONE:
                break;
            case wxRICHTEXT_FLOAT_LEFT:
                // Just a not-enough simple assertion
                assert(m_left.Index(map) == wxNOT_FOUND);
                m_left.Add(map);
                break;
            case wxRICHTEXT_FLOAT_RIGHT:
                assert(m_right.Index(map) == wxNOT_FOUND);
                m_right.Add(map);
                break;
            default:
                assert("Must some error occurs");
        }

        m_para = para;
}

void wxFloatCollector::CollectFloat(wxRichTextParagraph* para)
{
    wxRichTextObjectList::compatibility_iterator node = para->m_anchoredObjects.GetFirst();
    while (node)
    {
        wxRichTextObject* floating = node->GetData();
        assert(floating->IsFloatable());
        wxRichTextPlaceHoldingObject* ph = wxDynamicCast(floating, wxRichTextPlaceHoldingObject);
        floating = ph->GetRealObject();
        CollectFloat(para, floating);
        node = node->GetNext();
    }

    m_para = para;
}

wxRichTextParagraph* wxFloatCollector::LastParagraph()
{
    return m_para;
}

wxRect wxFloatCollector::GetAvailableRect(int startY, int endY)
{
    int widthLeft = 0, widthRight = 0;
    if (m_left.GetCount() != 0)
    {
        int i = SearchAdjacentRect(m_left, startY);
        if (i >= 0)
            widthLeft = GetWidthFromFloatRect(m_left, i, startY, endY);
    }
    if (m_right.GetCount() != 0)
    {
        int j = SearchAdjacentRect(m_right, startY);
        if (j >= 0)
            widthRight = GetWidthFromFloatRect(m_right, j, startY, endY);
    }

    return wxRect(widthLeft, 0, m_width - widthLeft - widthRight, 0);
}

int wxFloatCollector::GetLastRectBottom()
{
    int ret = 0;
    int len = m_left.GetCount();
    if (len) {
        ret = ret > m_left[len-1]->endY ? ret : m_left[len-1]->endY;
    }
    len = m_right.GetCount();
    if (len) {
        ret = ret > m_right[len-1]->endY ? ret : m_right[len-1]->endY;
    }

    return ret;
}

// Helpers for efficiency

inline void wxCheckSetFont(wxDC& dc, const wxFont& font)
{
#if 0
    const wxFont& font1 = dc.GetFont();
    if (font1.IsOk() && font.IsOk())
    {
        if (font1.GetPointSize() == font.GetPointSize() &&
            font1.GetFamily() == font.GetFamily() &&
            font1.GetStyle() == font.GetStyle() &&
            font1.GetWeight() == font.GetWeight() &&
            font1.GetUnderlined() == font.GetUnderlined() &&
            font1.GetFamily() == font.GetFamily() &&
            font1.GetFaceName() == font.GetFaceName())
            return;
    }
#endif
    dc.SetFont(font);
}

inline void wxCheckSetPen(wxDC& dc, const wxPen& pen)
{
    const wxPen& pen1 = dc.GetPen();
    if (pen1.IsOk() && pen.IsOk())
    {
        if (pen1.GetWidth() == pen.GetWidth() &&
            pen1.GetStyle() == pen.GetStyle() &&
            pen1.GetColour() == pen.GetColour())
            return;
    }
    dc.SetPen(pen);
}

inline void wxCheckSetBrush(wxDC& dc, const wxBrush& brush)
{
    const wxBrush& brush1 = dc.GetBrush();
    if (brush1.IsOk() && brush.IsOk())
    {
        if (brush1.GetStyle() == brush.GetStyle() &&
            brush1.GetColour() == brush.GetColour())
            return;
    }
    dc.SetBrush(brush);
}

wxRichTextAnchoredObjectAttr::wxRichTextAnchoredObjectAttr()
{
    m_anchored = false;
    m_align = wxRICHTEXT_CENTRE;
    m_floating = wxRICHTEXT_FLOAT_NONE;
}

wxRichTextAnchoredObjectAttr::wxRichTextAnchoredObjectAttr(const wxRichTextAnchoredObjectAttr& attr)
{
    Copy(attr);
}

void wxRichTextAnchoredObjectAttr::operator= (const wxRichTextAnchoredObjectAttr& attr)
{
    Copy(attr);
}

void wxRichTextAnchoredObjectAttr::Copy(const wxRichTextAnchoredObjectAttr& attr)
{
    m_anchored = attr.m_anchored;
    m_align = attr.m_align;
    m_floating = attr.m_floating;
}

/*!
 * wxRichTextImageAttr
 */
wxRichTextImageAttr::wxRichTextImageAttr(const wxRichTextImageAttr& attr)
{
    Copy(attr);
}

void wxRichTextImageAttr::operator= (const wxRichTextImageAttr& attr)
{
    Copy(attr);
}

void wxRichTextImageAttr::Copy(const wxRichTextImageAttr& attr)
{
    wxRichTextAnchoredObjectAttr::Copy(attr);
    m_scaleW = attr.m_scaleW;
    m_scaleH = attr.m_scaleH;
    m_width = attr.m_width;
    m_height = attr.m_height;
}

/*!
 * wxRichTextObject
 * This is the base for drawable objects.
 */

IMPLEMENT_CLASS(wxRichTextObject, wxObject)

wxRichTextObject::wxRichTextObject(wxRichTextObject* parent)
{
    m_dirty = false;
    m_refCount = 1;
    m_parent = parent;
    m_leftMargin = 0;
    m_rightMargin = 0;
    m_topMargin = 0;
    m_bottomMargin = 0;
    m_descent = 0;
}

wxRichTextObject::~wxRichTextObject()
{
}

void wxRichTextObject::Dereference()
{
    m_refCount --;
    if (m_refCount <= 0)
        delete this;
}

/// Copy
void wxRichTextObject::Copy(const wxRichTextObject& obj)
{
    m_size = obj.m_size;
    m_pos = obj.m_pos;
    m_dirty = obj.m_dirty;
    m_range = obj.m_range;
    m_attributes = obj.m_attributes;
    m_descent = obj.m_descent;
}

void wxRichTextObject::SetMargins(int margin)
{
    m_leftMargin = m_rightMargin = m_topMargin = m_bottomMargin = margin;
}

void wxRichTextObject::SetMargins(int leftMargin, int rightMargin, int topMargin, int bottomMargin)
{
    m_leftMargin = leftMargin;
    m_rightMargin = rightMargin;
    m_topMargin = topMargin;
    m_bottomMargin = bottomMargin;
}

// Convert units in tenths of a millimetre to device units
int wxRichTextObject::ConvertTenthsMMToPixels(wxDC& dc, int units) const
{
    int p = ConvertTenthsMMToPixels(dc.GetPPI().x, units);

    // Unscale
    wxRichTextBuffer* buffer = GetBuffer();
    if (buffer)
        p = (int) ((double)p / buffer->GetScale());
    return p;
}

// Convert units in tenths of a millimetre to device units
int wxRichTextObject::ConvertTenthsMMToPixels(int ppi, int units)
{
    // There are ppi pixels in 254.1 "1/10 mm"

    double pixels = ((double) units * (double)ppi) / 254.1;

    return (int) pixels;
}

/// Dump to output stream for debugging
void wxRichTextObject::Dump(wxTextOutputStream& stream)
{
    stream << GetClassInfo()->GetClassName() << wxT("\n");
    stream << wxString::Format(wxT("Size: %d,%d. Position: %d,%d, Range: %ld,%ld"), m_size.x, m_size.y, m_pos.x, m_pos.y, m_range.GetStart(), m_range.GetEnd()) << wxT("\n");
    stream << wxString::Format(wxT("Text colour: %d,%d,%d."), (int) m_attributes.GetTextColour().Red(), (int) m_attributes.GetTextColour().Green(), (int) m_attributes.GetTextColour().Blue()) << wxT("\n");
}

/// Gets the containing buffer
wxRichTextBuffer* wxRichTextObject::GetBuffer() const
{
    const wxRichTextObject* obj = this;
    while (obj && !obj->IsKindOf(CLASSINFO(wxRichTextBuffer)))
        obj = obj->GetParent();
    return wxDynamicCast(obj, wxRichTextBuffer);
}

/*!
 * wxRichTextCompositeObject
 * This is the base for drawable objects.
 */

IMPLEMENT_CLASS(wxRichTextCompositeObject, wxRichTextObject)

wxRichTextCompositeObject::wxRichTextCompositeObject(wxRichTextObject* parent):
    wxRichTextObject(parent)
{
}

wxRichTextCompositeObject::~wxRichTextCompositeObject()
{
    DeleteChildren();
}

/// Get the nth child
wxRichTextObject* wxRichTextCompositeObject::GetChild(size_t n) const
{
    wxASSERT ( n < m_children.GetCount() );

    return m_children.Item(n)->GetData();
}

/// Append a child, returning the position
size_t wxRichTextCompositeObject::AppendChild(wxRichTextObject* child)
{
    m_children.Append(child);
    child->SetParent(this);
    return m_children.GetCount() - 1;
}

/// Insert the child in front of the given object, or at the beginning
bool wxRichTextCompositeObject::InsertChild(wxRichTextObject* child, wxRichTextObject* inFrontOf)
{
    if (inFrontOf)
    {
        wxRichTextObjectList::compatibility_iterator node = m_children.Find(inFrontOf);
        m_children.Insert(node, child);
    }
    else
        m_children.Insert(child);
    child->SetParent(this);

    return true;
}

/// Delete the child
bool wxRichTextCompositeObject::RemoveChild(wxRichTextObject* child, bool deleteChild)
{
    wxRichTextObjectList::compatibility_iterator node = m_children.Find(child);
    if (node)
    {
        wxRichTextObject* obj = node->GetData();
        m_children.Erase(node);
        if (deleteChild)
            delete obj;

        return true;
    }
    return false;
}

/// Delete all children
bool wxRichTextCompositeObject::DeleteChildren()
{
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObjectList::compatibility_iterator oldNode = node;

        wxRichTextObject* child = node->GetData();
        child->Dereference(); // Only delete if reference count is zero

        node = node->GetNext();
        m_children.Erase(oldNode);
    }

    return true;
}

/// Get the child count
size_t wxRichTextCompositeObject::GetChildCount() const
{
    return m_children.GetCount();
}

/// Copy
void wxRichTextCompositeObject::Copy(const wxRichTextCompositeObject& obj)
{
    wxRichTextObject::Copy(obj);

    DeleteChildren();

    wxRichTextObjectList::compatibility_iterator node = obj.m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();
        wxRichTextObject* newChild = child->Clone();
        newChild->SetParent(this);
        m_children.Append(newChild);

        node = node->GetNext();
    }
}

/// Hit-testing: returns a flag indicating hit test details, plus
/// information about position
int wxRichTextCompositeObject::HitTest(wxDC& dc, const wxPoint& pt, long& textPosition)
{
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();

        int ret = child->HitTest(dc, pt, textPosition);
        if (ret != wxRICHTEXT_HITTEST_NONE)
            return ret;

        node = node->GetNext();
    }

    textPosition = GetRange().GetEnd()-1;
    return wxRICHTEXT_HITTEST_AFTER|wxRICHTEXT_HITTEST_OUTSIDE;
}

/// Finds the absolute position and row height for the given character position
bool wxRichTextCompositeObject::FindPosition(wxDC& dc, long index, wxPoint& pt, int* height, bool forceLineStart)
{
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();

        if (child->FindPosition(dc, index, pt, height, forceLineStart))
            return true;

        node = node->GetNext();
    }

    return false;
}

/// Calculate range
void wxRichTextCompositeObject::CalculateRange(long start, long& end)
{
    long current = start;
    long lastEnd = current;

    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();
        long childEnd = 0;

        child->CalculateRange(current, childEnd);
        lastEnd = childEnd;

        current = childEnd + 1;

        node = node->GetNext();
    }

    end = lastEnd;

    // An object with no children has zero length
    if (m_children.GetCount() == 0)
        end --;

    m_range.SetRange(start, end);
}

/// Delete range from layout.
bool wxRichTextCompositeObject::DeleteRange(const wxRichTextRange& range)
{
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();

    while (node)
    {
        wxRichTextObject* obj = (wxRichTextObject*) node->GetData();
        wxRichTextObjectList::compatibility_iterator next = node->GetNext();

        // Delete the range in each paragraph

        // When a chunk has been deleted, internally the content does not
        // now match the ranges.
        // However, so long as deletion is not done on the same object twice this is OK.
        // If you may delete content from the same object twice, recalculate
        // the ranges inbetween DeleteRange calls by calling CalculateRanges, and
        // adjust the range you're deleting accordingly.

        if (!obj->GetRange().IsOutside(range))
        {
            obj->DeleteRange(range);

            // Delete an empty object, or paragraph within this range.
            if (obj->IsEmpty() ||
                (range.GetStart() <= obj->GetRange().GetStart() && range.GetEnd() >= obj->GetRange().GetEnd()))
            {
                // An empty paragraph has length 1, so won't be deleted unless the
                // whole range is deleted.
                RemoveChild(obj, true);
            }
        }

        node = next;
    }

    return true;
}

/// Get any text in this object for the given range
wxString wxRichTextCompositeObject::GetTextForRange(const wxRichTextRange& range) const
{
    wxString text;
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();
        wxRichTextRange childRange = range;
        if (!child->GetRange().IsOutside(range))
        {
            childRange.LimitTo(child->GetRange());

            wxString childText = child->GetTextForRange(childRange);

            text += childText;
        }
        node = node->GetNext();
    }

    return text;
}

/// Recursively merge all pieces that can be merged.
bool wxRichTextCompositeObject::Defragment(const wxRichTextRange& range)
{
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();
        if (range == wxRICHTEXT_ALL || !child->GetRange().IsOutside(range))
        {
            wxRichTextCompositeObject* composite = wxDynamicCast(child, wxRichTextCompositeObject);
            if (composite)
                composite->Defragment();

            if (node->GetNext())
            {
                wxRichTextObject* nextChild = node->GetNext()->GetData();
                if (child->CanMerge(nextChild) && child->Merge(nextChild))
                {
                    nextChild->Dereference();
                    m_children.Erase(node->GetNext());

                    // Don't set node -- we'll see if we can merge again with the next
                    // child.
                }
                else
                    node = node->GetNext();
            }
            else
                node = node->GetNext();
        }
        else
            node = node->GetNext();
    }

    return true;
}

/// Dump to output stream for debugging
void wxRichTextCompositeObject::Dump(wxTextOutputStream& stream)
{
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();
        child->Dump(stream);
        node = node->GetNext();
    }
}


/*!
 * wxRichTextBox
 * This defines a 2D space to lay out objects
 */

IMPLEMENT_DYNAMIC_CLASS(wxRichTextBox, wxRichTextCompositeObject)

wxRichTextBox::wxRichTextBox(wxRichTextObject* parent):
    wxRichTextCompositeObject(parent)
{
}

/// Draw the item
bool wxRichTextBox::Draw(wxDC& dc, const wxRichTextRange& range, const wxRichTextRange& selectionRange, const wxRect& WXUNUSED(rect), int descent, int style)
{
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();

        wxRect childRect = wxRect(child->GetPosition(), child->GetCachedSize());
        child->Draw(dc, range, selectionRange, childRect, descent, style);

        node = node->GetNext();
    }
    return true;
}

/// Lay the item out
bool wxRichTextBox::Layout(wxDC& dc, const wxRect& rect, int style)
{
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();
        child->Layout(dc, rect, style);

        node = node->GetNext();
    }
    m_dirty = false;
    return true;
}

/// Get/set the size for the given range. Assume only has one child.
bool wxRichTextBox::GetRangeSize(const wxRichTextRange& range, wxSize& size, int& descent, wxDC& dc, int flags, wxPoint position, wxArrayInt* partialExtents) const
{
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    if (node)
    {
        wxRichTextObject* child = node->GetData();
        return child->GetRangeSize(range, size, descent, dc, flags, position, partialExtents);
    }
    else
        return false;
}

/// Copy
void wxRichTextBox::Copy(const wxRichTextBox& obj)
{
    wxRichTextCompositeObject::Copy(obj);
}


/*!
 * wxRichTextParagraphLayoutBox
 * This box knows how to lay out paragraphs.
 */

IMPLEMENT_DYNAMIC_CLASS(wxRichTextParagraphLayoutBox, wxRichTextBox)

wxRichTextParagraphLayoutBox::wxRichTextParagraphLayoutBox(wxRichTextObject* parent):
    wxRichTextBox(parent)
{
    Init();
}

/// Initialize the object.
void wxRichTextParagraphLayoutBox::Init()
{
    m_ctrl = NULL;

    // For now, assume is the only box and has no initial size.
    m_range = wxRichTextRange(0, -1);

    m_invalidRange.SetRange(-1, -1);
    m_leftMargin = 4;
    m_rightMargin = 4;
    m_topMargin = 4;
    m_bottomMargin = 4;
    m_partialParagraph = false;
    m_floatCollector = NULL;
}

/// Draw the item
bool wxRichTextParagraphLayoutBox::Draw(wxDC& dc, const wxRichTextRange& range, const wxRichTextRange& selectionRange, const wxRect& rect, int descent, int style)
{
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextParagraph* child = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        wxASSERT (child != NULL);

        if (child && !child->GetRange().IsOutside(range))
        {
            wxRect childRect(child->GetPosition(), child->GetCachedSize());

            if (((style & wxRICHTEXT_DRAW_IGNORE_CACHE) == 0) && childRect.GetTop() > rect.GetBottom())
            {
                // Stop drawing
                break;
            }
            else if (((style & wxRICHTEXT_DRAW_IGNORE_CACHE) == 0) && childRect.GetBottom() < rect.GetTop())
            {
                // Skip
            }
            else
                child->Draw(dc, range, selectionRange, rect, descent, style);
        }

        node = node->GetNext();
    }
    return true;
}

/// Lay the item out
bool wxRichTextParagraphLayoutBox::Layout(wxDC& dc, const wxRect& rect, int style)
{
    wxRect availableSpace;
    bool formatRect = (style & wxRICHTEXT_LAYOUT_SPECIFIED_RECT) == wxRICHTEXT_LAYOUT_SPECIFIED_RECT;

    // If only laying out a specific area, the passed rect has a different meaning:
    // the visible part of the buffer. This is used in wxRichTextCtrl::OnSize,
    // so that during a size, only the visible part will be relaid out, or
    // it would take too long causing flicker. As an approximation, we assume that
    // everything up to the start of the visible area is laid out correctly.
    if (formatRect)
    {
        availableSpace = wxRect(0 + m_leftMargin,
                          0 + m_topMargin,
                          rect.width - m_leftMargin - m_rightMargin,
                          rect.height);

        // Invalidate the part of the buffer from the first visible line
        // to the end. If other parts of the buffer are currently invalid,
        // then they too will be taken into account if they are above
        // the visible point.
        long startPos = 0;
        wxRichTextLine* line = GetLineAtYPosition(rect.y);
        if (line)
            startPos = line->GetAbsoluteRange().GetStart();

        Invalidate(wxRichTextRange(startPos, GetRange().GetEnd()));
    }
    else
        availableSpace = wxRect(rect.x + m_leftMargin,
                          rect.y + m_topMargin,
                          rect.width - m_leftMargin - m_rightMargin,
                          rect.height - m_topMargin - m_bottomMargin);

    int maxWidth = 0;

    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();

    bool layoutAll = true;

    // Get invalid range, rounding to paragraph start/end.
    wxRichTextRange invalidRange = GetInvalidRange(true);

    if (invalidRange == wxRICHTEXT_NONE && !formatRect)
        return true;

    if (invalidRange == wxRICHTEXT_ALL)
        layoutAll = true;
    else    // If we know what range is affected, start laying out from that point on.
        if (invalidRange.GetStart() >= GetRange().GetStart())
    {
        wxRichTextParagraph* firstParagraph = GetParagraphAtPosition(invalidRange.GetStart());
        if (firstParagraph)
        {
            wxRichTextObjectList::compatibility_iterator firstNode = m_children.Find(firstParagraph);
            wxRichTextObjectList::compatibility_iterator previousNode;
            if ( firstNode )
                previousNode = firstNode->GetPrevious();
            if (firstNode)
            {
                if (previousNode)
                {
                    wxRichTextParagraph* previousParagraph = wxDynamicCast(previousNode->GetData(), wxRichTextParagraph);
                    availableSpace.y = previousParagraph->GetPosition().y + previousParagraph->GetCachedSize().y;
                }

                // Now we're going to start iterating from the first affected paragraph.
                node = firstNode;

                layoutAll = false;
            }
        }
    }

    // Collect the floats layout state
    if (m_floatCollector)
        delete m_floatCollector;
    m_floatCollector = new wxFloatCollector(availableSpace.width);
    wxRichTextObjectList::compatibility_iterator para = m_children.GetFirst();
    while (para && para != node)
    {
        wxRichTextParagraph* paragraph = wxDynamicCast(para->GetData(), wxRichTextParagraph);
        m_floatCollector->CollectFloat(paragraph);
        para = para->GetNext();
    }

    // A way to force speedy rest-of-buffer layout (the 'else' below)
    bool forceQuickLayout = false;

    while (node)
    {
        // Assume this box only contains paragraphs

        wxRichTextParagraph* child = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        wxCHECK_MSG( child, false, wxT("Unknown object in layout") );

        // TODO: what if the child hasn't been laid out (e.g. involved in Undo) but still has 'old' lines
        if ( !forceQuickLayout &&
                (layoutAll ||
                    child->GetLines().IsEmpty() ||
                        !child->GetRange().IsOutside(invalidRange)) )
        {
            child->Layout(dc, availableSpace, style);

            // Layout must set the cached size
            availableSpace.y += child->GetCachedSize().y;
            maxWidth = wxMax(maxWidth, child->GetCachedSize().x);

            // If we're just formatting the visible part of the buffer,
            // and we're now past the bottom of the window, start quick
            // layout.
            if (formatRect && child->GetPosition().y > rect.GetBottom())
                forceQuickLayout = true;
        }
        else
        {
            // We're outside the immediately affected range, so now let's just
            // move everything up or down. This assumes that all the children have previously
            // been laid out and have wrapped line lists associated with them.
            // TODO: check all paragraphs before the affected range.

            int inc = availableSpace.y - child->GetPosition().y;

            while (node)
            {
                wxRichTextParagraph* child = wxDynamicCast(node->GetData(), wxRichTextParagraph);
                if (child)
                {
                    if (child->GetLines().GetCount() == 0)
                        child->Layout(dc, availableSpace, style);
                    else
                        child->SetPosition(wxPoint(child->GetPosition().x, child->GetPosition().y + inc));

                    availableSpace.y += child->GetCachedSize().y;
                    maxWidth = wxMax(maxWidth, child->GetCachedSize().x);
                }

                node = node->GetNext();
            }
            break;
        }

        node = node->GetNext();
    }

    SetCachedSize(wxSize(maxWidth, availableSpace.y));

    m_dirty = false;
    m_invalidRange = wxRICHTEXT_NONE;

    return true;
}

/// Copy
void wxRichTextParagraphLayoutBox::Copy(const wxRichTextParagraphLayoutBox& obj)
{
    wxRichTextBox::Copy(obj);

    m_partialParagraph = obj.m_partialParagraph;
    m_defaultAttributes = obj.m_defaultAttributes;
}

/// Get/set the size for the given range.
bool wxRichTextParagraphLayoutBox::GetRangeSize(const wxRichTextRange& range, wxSize& size, int& descent, wxDC& dc, int flags, wxPoint position, wxArrayInt* WXUNUSED(partialExtents)) const
{
    wxSize sz;

    wxRichTextObjectList::compatibility_iterator startPara = wxRichTextObjectList::compatibility_iterator();
    wxRichTextObjectList::compatibility_iterator endPara = wxRichTextObjectList::compatibility_iterator();

    // First find the first paragraph whose starting position is within the range.
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        // child is a paragraph
        wxRichTextObject* child = node->GetData();
        const wxRichTextRange& r = child->GetRange();

        if (r.GetStart() <= range.GetStart() && r.GetEnd() >= range.GetStart())
        {
            startPara = node;
            break;
        }

        node = node->GetNext();
    }

    // Next find the last paragraph containing part of the range
    node = m_children.GetFirst();
    while (node)
    {
        // child is a paragraph
        wxRichTextObject* child = node->GetData();
        const wxRichTextRange& r = child->GetRange();

        if (r.GetStart() <= range.GetEnd() && r.GetEnd() >= range.GetEnd())
        {
            endPara = node;
            break;
        }

        node = node->GetNext();
    }

    if (!startPara || !endPara)
        return false;

    // Now we can add up the sizes
    for (node = startPara; node ; node = node->GetNext())
    {
        // child is a paragraph
        wxRichTextObject* child = node->GetData();
        const wxRichTextRange& childRange = child->GetRange();
        wxRichTextRange rangeToFind = range;
        rangeToFind.LimitTo(childRange);

        wxSize childSize;

        int childDescent = 0;
        child->GetRangeSize(rangeToFind, childSize, childDescent, dc, flags, position);

        descent = wxMax(childDescent, descent);

        sz.x = wxMax(sz.x, childSize.x);
        sz.y += childSize.y;

        if (node == endPara)
            break;
    }

    size = sz;

    return true;
}

/// Get the paragraph at the given position
wxRichTextParagraph* wxRichTextParagraphLayoutBox::GetParagraphAtPosition(long pos, bool caretPosition) const
{
    if (caretPosition)
        pos ++;

    // First find the first paragraph whose starting position is within the range.
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        // child is a paragraph
        wxRichTextParagraph* child = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        wxASSERT (child != NULL);

        // Return first child in buffer if position is -1
        // if (pos == -1)
        //    return child;

        if (child->GetRange().Contains(pos))
            return child;

        node = node->GetNext();
    }
    return NULL;
}

/// Get the line at the given position
wxRichTextLine* wxRichTextParagraphLayoutBox::GetLineAtPosition(long pos, bool caretPosition) const
{
    if (caretPosition)
        pos ++;

    // First find the first paragraph whose starting position is within the range.
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* obj = (wxRichTextObject*) node->GetData();
        if (obj->GetRange().Contains(pos))
        {
            // child is a paragraph
            wxRichTextParagraph* child = wxDynamicCast(obj, wxRichTextParagraph);
            wxASSERT (child != NULL);

            wxRichTextLineList::compatibility_iterator node2 = child->GetLines().GetFirst();
            while (node2)
            {
                wxRichTextLine* line = node2->GetData();

                wxRichTextRange range = line->GetAbsoluteRange();

                if (range.Contains(pos) ||

                    // If the position is end-of-paragraph, then return the last line of
                    // of the paragraph.
                    ((range.GetEnd() == child->GetRange().GetEnd()-1) && (pos == child->GetRange().GetEnd())))
                    return line;

                node2 = node2->GetNext();
            }
        }

        node = node->GetNext();
    }

    int lineCount = GetLineCount();
    if (lineCount > 0)
        return GetLineForVisibleLineNumber(lineCount-1);
    else
        return NULL;
}

/// Get the line at the given y pixel position, or the last line.
wxRichTextLine* wxRichTextParagraphLayoutBox::GetLineAtYPosition(int y) const
{
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextParagraph* child = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        wxASSERT (child != NULL);

        wxRichTextLineList::compatibility_iterator node2 = child->GetLines().GetFirst();
        while (node2)
        {
            wxRichTextLine* line = node2->GetData();

            wxRect rect(line->GetRect());

            if (y <= rect.GetBottom())
                return line;

            node2 = node2->GetNext();
        }

        node = node->GetNext();
    }

    // Return last line
    int lineCount = GetLineCount();
    if (lineCount > 0)
        return GetLineForVisibleLineNumber(lineCount-1);
    else
        return NULL;
}

/// Get the number of visible lines
int wxRichTextParagraphLayoutBox::GetLineCount() const
{
    int count = 0;

    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextParagraph* child = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        wxASSERT (child != NULL);

        count += child->GetLines().GetCount();
        node = node->GetNext();
    }
    return count;
}


/// Get the paragraph for a given line
wxRichTextParagraph* wxRichTextParagraphLayoutBox::GetParagraphForLine(wxRichTextLine* line) const
{
    return GetParagraphAtPosition(line->GetAbsoluteRange().GetStart());
}

/// Get the line size at the given position
wxSize wxRichTextParagraphLayoutBox::GetLineSizeAtPosition(long pos, bool caretPosition) const
{
    wxRichTextLine* line = GetLineAtPosition(pos, caretPosition);
    if (line)
    {
        return line->GetSize();
    }
    else
        return wxSize(0, 0);
}


/// Convenience function to add a paragraph of text
wxRichTextRange wxRichTextParagraphLayoutBox::AddParagraph(const wxString& text, wxTextAttr* paraStyle)
{
    // Don't use the base style, just the default style, and the base style will
    // be combined at display time.
    // Divide into paragraph and character styles.

    wxTextAttr defaultCharStyle;
    wxTextAttr defaultParaStyle;

    // If the default style is a named paragraph style, don't apply any character formatting
    // to the initial text string.
    if (GetDefaultStyle().HasParagraphStyleName() && GetStyleSheet())
    {
        wxRichTextParagraphStyleDefinition* def = GetStyleSheet()->FindParagraphStyle(GetDefaultStyle().GetParagraphStyleName());
        if (def)
            defaultParaStyle = def->GetStyleMergedWithBase(GetStyleSheet());
    }
    else
        wxRichTextSplitParaCharStyles(GetDefaultStyle(), defaultParaStyle, defaultCharStyle);

    wxTextAttr* pStyle = paraStyle ? paraStyle : (wxTextAttr*) & defaultParaStyle;
    wxTextAttr* cStyle = & defaultCharStyle;

    wxRichTextParagraph* para = new wxRichTextParagraph(text, this, pStyle, cStyle);

    AppendChild(para);

    UpdateRanges();
    SetDirty(true);

    return para->GetRange();
}

/// Adds multiple paragraphs, based on newlines.
wxRichTextRange wxRichTextParagraphLayoutBox::AddParagraphs(const wxString& text, wxTextAttr* paraStyle)
{
    // Don't use the base style, just the default style, and the base style will
    // be combined at display time.
    // Divide into paragraph and character styles.

    wxTextAttr defaultCharStyle;
    wxTextAttr defaultParaStyle;

    // If the default style is a named paragraph style, don't apply any character formatting
    // to the initial text string.
    if (GetDefaultStyle().HasParagraphStyleName() && GetStyleSheet())
    {
        wxRichTextParagraphStyleDefinition* def = GetStyleSheet()->FindParagraphStyle(GetDefaultStyle().GetParagraphStyleName());
        if (def)
            defaultParaStyle = def->GetStyleMergedWithBase(GetStyleSheet());
    }
    else
        wxRichTextSplitParaCharStyles(GetDefaultStyle(), defaultParaStyle, defaultCharStyle);

    wxTextAttr* pStyle = paraStyle ? paraStyle : (wxTextAttr*) & defaultParaStyle;
    wxTextAttr* cStyle = & defaultCharStyle;

    wxRichTextParagraph* firstPara = NULL;
    wxRichTextParagraph* lastPara = NULL;

    wxRichTextRange range(-1, -1);

    size_t i = 0;
    size_t len = text.length();
    wxString line;
    wxRichTextParagraph* para = new wxRichTextParagraph(wxEmptyString, this, pStyle, cStyle);

    AppendChild(para);

    firstPara = para;
    lastPara = para;

    while (i < len)
    {
        wxChar ch = text[i];
        if (ch == wxT('\n') || ch == wxT('\r'))
        {
            if (i != (len-1))
            {
                wxRichTextPlainText* plainText = (wxRichTextPlainText*) para->GetChildren().GetFirst()->GetData();
                plainText->SetText(line);

                para = new wxRichTextParagraph(wxEmptyString, this, pStyle, cStyle);

                AppendChild(para);

                lastPara = para;
                line = wxEmptyString;
            }
        }
        else
            line += ch;

        i ++;
    }

    if (!line.empty())
    {
        wxRichTextPlainText* plainText = (wxRichTextPlainText*) para->GetChildren().GetFirst()->GetData();
        plainText->SetText(line);
    }

    UpdateRanges();

    SetDirty(false);

    return wxRichTextRange(firstPara->GetRange().GetStart(), lastPara->GetRange().GetEnd());
}

/// Convenience function to add an image
wxRichTextRange wxRichTextParagraphLayoutBox::AddImage(const wxImage& image, wxTextAttr* paraStyle)
{
    // Don't use the base style, just the default style, and the base style will
    // be combined at display time.
    // Divide into paragraph and character styles.

    wxTextAttr defaultCharStyle;
    wxTextAttr defaultParaStyle;

    // If the default style is a named paragraph style, don't apply any character formatting
    // to the initial text string.
    if (GetDefaultStyle().HasParagraphStyleName() && GetStyleSheet())
    {
        wxRichTextParagraphStyleDefinition* def = GetStyleSheet()->FindParagraphStyle(GetDefaultStyle().GetParagraphStyleName());
        if (def)
            defaultParaStyle = def->GetStyleMergedWithBase(GetStyleSheet());
    }
    else
        wxRichTextSplitParaCharStyles(GetDefaultStyle(), defaultParaStyle, defaultCharStyle);

    wxTextAttr* pStyle = paraStyle ? paraStyle : (wxTextAttr*) & defaultParaStyle;
    wxTextAttr* cStyle = & defaultCharStyle;

    wxRichTextParagraph* para = new wxRichTextParagraph(this, pStyle);
    AppendChild(para);
    para->AppendChild(new wxRichTextImage(image, this, cStyle));

    UpdateRanges();
    SetDirty(true);

    return para->GetRange();
}


/// Insert fragment into this box at the given position. If partialParagraph is true,
/// it is assumed that the last (or only) paragraph is just a piece of data with no paragraph
/// marker.

bool wxRichTextParagraphLayoutBox::InsertFragment(long position, wxRichTextParagraphLayoutBox& fragment)
{
    SetDirty(true);

    // First, find the first paragraph whose starting position is within the range.
    wxRichTextParagraph* para = GetParagraphAtPosition(position);
    if (para)
    {
        wxTextAttrEx originalAttr = para->GetAttributes();

        wxRichTextObjectList::compatibility_iterator node = m_children.Find(para);

        // Now split at this position, returning the object to insert the new
        // ones in front of.
        wxRichTextObject* nextObject = para->SplitAt(position);

        // Special case: partial paragraph, just one paragraph. Might be a small amount of
        // text, for example, so let's optimize.

        if (fragment.GetPartialParagraph() && fragment.GetChildren().GetCount() == 1)
        {
            // Add the first para to this para...
            wxRichTextObjectList::compatibility_iterator firstParaNode = fragment.GetChildren().GetFirst();
            if (!firstParaNode)
                return false;

            // Iterate through the fragment paragraph inserting the content into this paragraph.
            wxRichTextParagraph* firstPara = wxDynamicCast(firstParaNode->GetData(), wxRichTextParagraph);
            wxASSERT (firstPara != NULL);

            wxRichTextObjectList::compatibility_iterator objectNode = firstPara->GetChildren().GetFirst();
            while (objectNode)
            {
                wxRichTextObject* newObj = objectNode->GetData()->Clone();

                if (!nextObject)
                {
                    // Append
                    para->AppendChild(newObj);
                }
                else
                {
                    // Insert before nextObject
                    para->InsertChild(newObj, nextObject);
                }

                objectNode = objectNode->GetNext();
            }

            return true;
        }
        else
        {
            // Procedure for inserting a fragment consisting of a number of
            // paragraphs:
            //
            // 1. Remove and save the content that's after the insertion point, for adding
            //    back once we've added the fragment.
            // 2. Add the content from the first fragment paragraph to the current
            //    paragraph.
            // 3. Add remaining fragment paragraphs after the current paragraph.
            // 4. Add back the saved content from the first paragraph. If partialParagraph
            //    is true, add it to the last paragraph added and not a new one.

            // 1. Remove and save objects after split point.
            wxList savedObjects;
            if (nextObject)
                para->MoveToList(nextObject, savedObjects);

            // 2. Add the content from the 1st fragment paragraph.
            wxRichTextObjectList::compatibility_iterator firstParaNode = fragment.GetChildren().GetFirst();
            if (!firstParaNode)
                return false;

            wxRichTextParagraph* firstPara = wxDynamicCast(firstParaNode->GetData(), wxRichTextParagraph);
            wxASSERT(firstPara != NULL);

            if (!(fragment.GetAttributes().GetFlags() & wxTEXT_ATTR_KEEP_FIRST_PARA_STYLE))
                para->SetAttributes(firstPara->GetAttributes());

            // Save empty paragraph attributes for appending later
            // These are character attributes deliberately set for a new paragraph. Without this,
            // we couldn't pass default attributes when appending a new paragraph.
            wxTextAttrEx emptyParagraphAttributes;

            wxRichTextObjectList::compatibility_iterator objectNode = firstPara->GetChildren().GetFirst();

            if (objectNode && firstPara->GetChildren().GetCount() == 1 && objectNode->GetData()->IsEmpty())
                emptyParagraphAttributes = objectNode->GetData()->GetAttributes();

            while (objectNode)
            {
                wxRichTextObject* newObj = objectNode->GetData()->Clone();

                // Append
                para->AppendChild(newObj);

                objectNode = objectNode->GetNext();
            }

            // 3. Add remaining fragment paragraphs after the current paragraph.
            wxRichTextObjectList::compatibility_iterator nextParagraphNode = node->GetNext();
            wxRichTextObject* nextParagraph = NULL;
            if (nextParagraphNode)
                nextParagraph = nextParagraphNode->GetData();

            wxRichTextObjectList::compatibility_iterator i = fragment.GetChildren().GetFirst()->GetNext();
            wxRichTextParagraph* finalPara = para;

            bool needExtraPara = (!i || !fragment.GetPartialParagraph());

            // If there was only one paragraph, we need to insert a new one.
            while (i)
            {
                wxRichTextParagraph* para = wxDynamicCast(i->GetData(), wxRichTextParagraph);
                wxASSERT( para != NULL );

                finalPara = (wxRichTextParagraph*) para->Clone();

                if (nextParagraph)
                    InsertChild(finalPara, nextParagraph);
                else
                    AppendChild(finalPara);

                i = i->GetNext();
            }

            // If there was only one paragraph, or we have full paragraphs in our fragment,
            // we need to insert a new one.
            if (needExtraPara)
            {
                finalPara = new wxRichTextParagraph;

                if (nextParagraph)
                    InsertChild(finalPara, nextParagraph);
                else
                    AppendChild(finalPara);
            }

            // 4. Add back the remaining content.
            if (finalPara)
            {
                if (nextObject)
                    finalPara->MoveFromList(savedObjects);

                // Ensure there's at least one object
                if (finalPara->GetChildCount() == 0)
                {
                    wxRichTextPlainText* text = new wxRichTextPlainText(wxEmptyString);
                    text->SetAttributes(emptyParagraphAttributes);

                    finalPara->AppendChild(text);
                }
            }

            if ((fragment.GetAttributes().GetFlags() & wxTEXT_ATTR_KEEP_FIRST_PARA_STYLE) && firstPara)
                finalPara->SetAttributes(firstPara->GetAttributes());
            else if (finalPara && finalPara != para)
                finalPara->SetAttributes(originalAttr);

            return true;
        }
    }
    else
    {
        // Append
        wxRichTextObjectList::compatibility_iterator i = fragment.GetChildren().GetFirst();
        while (i)
        {
            wxRichTextParagraph* para = wxDynamicCast(i->GetData(), wxRichTextParagraph);
            wxASSERT( para != NULL );

            AppendChild(para->Clone());

            i = i->GetNext();
        }

        return true;
    }
}

/// Make a copy of the fragment corresponding to the given range, putting it in 'fragment'.
/// If there was an incomplete paragraph at the end, partialParagraph is set to true.
bool wxRichTextParagraphLayoutBox::CopyFragment(const wxRichTextRange& range, wxRichTextParagraphLayoutBox& fragment)
{
    wxRichTextObjectList::compatibility_iterator i = GetChildren().GetFirst();
    while (i)
    {
        wxRichTextParagraph* para = wxDynamicCast(i->GetData(), wxRichTextParagraph);
        wxASSERT( para != NULL );

        if (!para->GetRange().IsOutside(range))
        {
            fragment.AppendChild(para->Clone());
        }
        i = i->GetNext();
    }

    // Now top and tail the first and last paragraphs in our new fragment (which might be the same).
    if (!fragment.IsEmpty())
    {
        wxRichTextRange topTailRange(range);

        wxRichTextParagraph* firstPara = wxDynamicCast(fragment.GetChildren().GetFirst()->GetData(), wxRichTextParagraph);
        wxASSERT( firstPara != NULL );

        // Chop off the start of the paragraph
        if (topTailRange.GetStart() > firstPara->GetRange().GetStart())
        {
            wxRichTextRange r(firstPara->GetRange().GetStart(), topTailRange.GetStart()-1);
            firstPara->DeleteRange(r);

            // Make sure the numbering is correct
            long end;
            fragment.CalculateRange(firstPara->GetRange().GetStart(), end);

            // Now, we've deleted some positions, so adjust the range
            // accordingly.
            topTailRange.SetEnd(topTailRange.GetEnd() - r.GetLength());
        }

        wxRichTextParagraph* lastPara = wxDynamicCast(fragment.GetChildren().GetLast()->GetData(), wxRichTextParagraph);
        wxASSERT( lastPara != NULL );

        if (topTailRange.GetEnd() < (lastPara->GetRange().GetEnd()-1))
        {
            wxRichTextRange r(topTailRange.GetEnd()+1, lastPara->GetRange().GetEnd()-1); /* -1 since actual text ends 1 position before end of para marker */
            lastPara->DeleteRange(r);

            // Make sure the numbering is correct
            long end;
            fragment.CalculateRange(firstPara->GetRange().GetStart(), end);

            // We only have part of a paragraph at the end
            fragment.SetPartialParagraph(true);
        }
        else
        {
            if (topTailRange.GetEnd() == (lastPara->GetRange().GetEnd() - 1))
                // We have a partial paragraph (don't save last new paragraph marker)
                fragment.SetPartialParagraph(true);
            else
                // We have a complete paragraph
                fragment.SetPartialParagraph(false);
        }
    }

    return true;
}

/// Given a position, get the number of the visible line (potentially many to a paragraph),
/// starting from zero at the start of the buffer.
long wxRichTextParagraphLayoutBox::GetVisibleLineNumber(long pos, bool caretPosition, bool startOfLine) const
{
    if (caretPosition)
        pos ++;

    int lineCount = 0;

    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextParagraph* child = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        wxASSERT( child != NULL );

        if (child->GetRange().Contains(pos))
        {
            wxRichTextLineList::compatibility_iterator node2 = child->GetLines().GetFirst();
            while (node2)
            {
                wxRichTextLine* line = node2->GetData();
                wxRichTextRange lineRange = line->GetAbsoluteRange();

                if (lineRange.Contains(pos))
                {
                    // If the caret is displayed at the end of the previous wrapped line,
                    // we want to return the line it's _displayed_ at (not the actual line
                    // containing the position).
                    if (lineRange.GetStart() == pos && !startOfLine && child->GetRange().GetStart() != pos)
                        return lineCount - 1;
                    else
                        return lineCount;
                }

                lineCount ++;

                node2 = node2->GetNext();
            }
            // If we didn't find it in the lines, it must be
            // the last position of the paragraph. So return the last line.
            return lineCount-1;
        }
        else
            lineCount += child->GetLines().GetCount();

        node = node->GetNext();
    }

    // Not found
    return -1;
}

/// Given a line number, get the corresponding wxRichTextLine object.
wxRichTextLine* wxRichTextParagraphLayoutBox::GetLineForVisibleLineNumber(long lineNumber) const
{
    int lineCount = 0;

    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextParagraph* child = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        wxASSERT(child != NULL);

        if (lineNumber < (int) (child->GetLines().GetCount() + lineCount))
        {
            wxRichTextLineList::compatibility_iterator node2 = child->GetLines().GetFirst();
            while (node2)
            {
                wxRichTextLine* line = node2->GetData();

                if (lineCount == lineNumber)
                    return line;

                lineCount ++;

                node2 = node2->GetNext();
            }
        }
        else
            lineCount += child->GetLines().GetCount();

        node = node->GetNext();
    }

    // Didn't find it
    return NULL;
}

/// Delete range from layout.
bool wxRichTextParagraphLayoutBox::DeleteRange(const wxRichTextRange& range)
{
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();

    wxRichTextParagraph* firstPara = NULL;
    while (node)
    {
        wxRichTextParagraph* obj = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        wxASSERT (obj != NULL);

        wxRichTextObjectList::compatibility_iterator next = node->GetNext();

        // Delete the range in each paragraph

        if (!obj->GetRange().IsOutside(range))
        {
            // Deletes the content of this object within the given range
            obj->DeleteRange(range);

            wxRichTextRange thisRange = obj->GetRange();
            wxTextAttrEx thisAttr = obj->GetAttributes();

            // If the whole paragraph is within the range to delete,
            // delete the whole thing.
            if (range.GetStart() <= thisRange.GetStart() && range.GetEnd() >= thisRange.GetEnd())
            {
                // Delete the whole object
                RemoveChild(obj, true);
                obj = NULL;
            }
            else if (!firstPara)
                firstPara = obj;

            // If the range includes the paragraph end, we need to join this
            // and the next paragraph.
            if (range.GetEnd() <= thisRange.GetEnd())
            {
                // We need to move the objects from the next paragraph
                // to this paragraph

                wxRichTextParagraph* nextParagraph = NULL;
                if ((range.GetEnd() < thisRange.GetEnd()) && obj)
                    nextParagraph = obj;
                else
                {
                    // We're ending at the end of the paragraph, so merge the _next_ paragraph.
                    if (next)
                        nextParagraph = wxDynamicCast(next->GetData(), wxRichTextParagraph);
                }

                bool applyFinalParagraphStyle = firstPara && nextParagraph && nextParagraph != firstPara;

                wxTextAttrEx nextParaAttr;
                if (applyFinalParagraphStyle)
                {
                    // Special case when deleting the end of a paragraph - use _this_ paragraph's style,
                    // not the next one.
                    if (range.GetStart() == range.GetEnd() && range.GetStart() == thisRange.GetEnd())
                        nextParaAttr = thisAttr;
                    else
                        nextParaAttr = nextParagraph->GetAttributes();
                }

                if (firstPara && nextParagraph && firstPara != nextParagraph)
                {
                    // Move the objects to the previous para
                    wxRichTextObjectList::compatibility_iterator node1 = nextParagraph->GetChildren().GetFirst();

                    while (node1)
                    {
                        wxRichTextObject* obj1 = node1->GetData();

                        firstPara->AppendChild(obj1);

                        wxRichTextObjectList::compatibility_iterator next1 = node1->GetNext();
                        nextParagraph->GetChildren().Erase(node1);

                        node1 = next1;
                    }

                    // Delete the paragraph
                    RemoveChild(nextParagraph, true);
                }

                // Avoid empty paragraphs
                if (firstPara && firstPara->GetChildren().GetCount() == 0)
                {
                    wxRichTextPlainText* text = new wxRichTextPlainText(wxEmptyString);
                    firstPara->AppendChild(text);
                }

                if (applyFinalParagraphStyle)
                    firstPara->SetAttributes(nextParaAttr);

                return true;
            }
        }

        node = next;
    }

    return true;
}

/// Get any text in this object for the given range
wxString wxRichTextParagraphLayoutBox::GetTextForRange(const wxRichTextRange& range) const
{
    int lineCount = 0;
    wxString text;
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();
        if (!child->GetRange().IsOutside(range))
        {
            wxRichTextRange childRange = range;
            childRange.LimitTo(child->GetRange());

            wxString childText = child->GetTextForRange(childRange);

            text += childText;

            if ((childRange.GetEnd() == child->GetRange().GetEnd()) && node->GetNext())
                text += wxT("\n");

            lineCount ++;
        }
        node = node->GetNext();
    }

    return text;
}

/// Get all the text
wxString wxRichTextParagraphLayoutBox::GetText() const
{
    return GetTextForRange(GetRange());
}

/// Get the paragraph by number
wxRichTextParagraph* wxRichTextParagraphLayoutBox::GetParagraphAtLine(long paragraphNumber) const
{
    if ((size_t) paragraphNumber >= GetChildCount())
        return NULL;

    return (wxRichTextParagraph*) GetChild((size_t) paragraphNumber);
}

/// Get the length of the paragraph
int wxRichTextParagraphLayoutBox::GetParagraphLength(long paragraphNumber) const
{
    wxRichTextParagraph* para = GetParagraphAtLine(paragraphNumber);
    if (para)
        return para->GetRange().GetLength() - 1; // don't include newline
    else
        return 0;
}

/// Get the text of the paragraph
wxString wxRichTextParagraphLayoutBox::GetParagraphText(long paragraphNumber) const
{
    wxRichTextParagraph* para = GetParagraphAtLine(paragraphNumber);
    if (para)
        return para->GetTextForRange(para->GetRange());
    else
        return wxEmptyString;
}

/// Convert zero-based line column and paragraph number to a position.
long wxRichTextParagraphLayoutBox::XYToPosition(long x, long y) const
{
    wxRichTextParagraph* para = GetParagraphAtLine(y);
    if (para)
    {
        return para->GetRange().GetStart() + x;
    }
    else
        return -1;
}

/// Convert zero-based position to line column and paragraph number
bool wxRichTextParagraphLayoutBox::PositionToXY(long pos, long* x, long* y) const
{
    wxRichTextParagraph* para = GetParagraphAtPosition(pos);
    if (para)
    {
        int count = 0;
        wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
        while (node)
        {
            wxRichTextObject* child = node->GetData();
            if (child == para)
                break;
            count ++;
            node = node->GetNext();
        }

        *y = count;
        *x = pos - para->GetRange().GetStart();

        return true;
    }
    else
        return false;
}

/// Get the leaf object in a paragraph at this position.
/// Given a line number, get the corresponding wxRichTextLine object.
wxRichTextObject* wxRichTextParagraphLayoutBox::GetLeafObjectAtPosition(long position) const
{
    wxRichTextParagraph* para = GetParagraphAtPosition(position);
    if (para)
    {
        wxRichTextObjectList::compatibility_iterator node = para->GetChildren().GetFirst();

        while (node)
        {
            wxRichTextObject* child = node->GetData();
            if (child->GetRange().Contains(position))
                return child;

            node = node->GetNext();
        }
        if (position == para->GetRange().GetEnd() && para->GetChildCount() > 0)
            return para->GetChildren().GetLast()->GetData();
    }
    return NULL;
}

/// Set character or paragraph text attributes: apply character styles only to immediate text nodes
bool wxRichTextParagraphLayoutBox::SetStyle(const wxRichTextRange& range, const wxTextAttr& style, int flags)
{
    bool characterStyle = false;
    bool paragraphStyle = false;

    if (style.IsCharacterStyle())
        characterStyle = true;
    if (style.IsParagraphStyle())
        paragraphStyle = true;

    bool withUndo = ((flags & wxRICHTEXT_SETSTYLE_WITH_UNDO) != 0);
    bool applyMinimal = ((flags & wxRICHTEXT_SETSTYLE_OPTIMIZE) != 0);
    bool parasOnly = ((flags & wxRICHTEXT_SETSTYLE_PARAGRAPHS_ONLY) != 0);
    bool charactersOnly = ((flags & wxRICHTEXT_SETSTYLE_CHARACTERS_ONLY) != 0);
    bool resetExistingStyle = ((flags & wxRICHTEXT_SETSTYLE_RESET) != 0);
    bool removeStyle = ((flags & wxRICHTEXT_SETSTYLE_REMOVE) != 0);

    // Apply paragraph style first, if any
    wxTextAttr wholeStyle(style);

    if (!removeStyle && wholeStyle.HasParagraphStyleName() && GetStyleSheet())
    {
        wxRichTextParagraphStyleDefinition* def = GetStyleSheet()->FindParagraphStyle(wholeStyle.GetParagraphStyleName());
        if (def)
            wxRichTextApplyStyle(wholeStyle, def->GetStyleMergedWithBase(GetStyleSheet()));
    }

    // Limit the attributes to be set to the content to only character attributes.
    wxTextAttr characterAttributes(wholeStyle);
    characterAttributes.SetFlags(characterAttributes.GetFlags() & (wxTEXT_ATTR_CHARACTER));

    if (!removeStyle && characterAttributes.HasCharacterStyleName() && GetStyleSheet())
    {
        wxRichTextCharacterStyleDefinition* def = GetStyleSheet()->FindCharacterStyle(characterAttributes.GetCharacterStyleName());
        if (def)
            wxRichTextApplyStyle(characterAttributes, def->GetStyleMergedWithBase(GetStyleSheet()));
    }

    // If we are associated with a control, make undoable; otherwise, apply immediately
    // to the data.

    bool haveControl = (GetRichTextCtrl() != NULL);

    wxRichTextAction* action = NULL;

    if (haveControl && withUndo)
    {
        action = new wxRichTextAction(NULL, _("Change Style"), wxRICHTEXT_CHANGE_STYLE, & GetRichTextCtrl()->GetBuffer(), GetRichTextCtrl());
        action->SetRange(range);
        action->SetPosition(GetRichTextCtrl()->GetCaretPosition());
    }

    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextParagraph* para = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        wxASSERT (para != NULL);

        if (para && para->GetChildCount() > 0)
        {
            // Stop searching if we're beyond the range of interest
            if (para->GetRange().GetStart() > range.GetEnd())
                break;

            if (!para->GetRange().IsOutside(range))
            {
                // We'll be using a copy of the paragraph to make style changes,
                // not updating the buffer directly.
                wxRichTextParagraph* newPara wxDUMMY_INITIALIZE(NULL);

                if (haveControl && withUndo)
                {
                    newPara = new wxRichTextParagraph(*para);
                    action->GetNewParagraphs().AppendChild(newPara);

                    // Also store the old ones for Undo
                    action->GetOldParagraphs().AppendChild(new wxRichTextParagraph(*para));
                }
                else
                    newPara = para;

                // If we're specifying paragraphs only, then we really mean character formatting
                // to be included in the paragraph style
                if ((paragraphStyle || parasOnly) && !charactersOnly)
                {
                    if (removeStyle)
                    {
                        // Removes the given style from the paragraph
                        wxRichTextRemoveStyle(newPara->GetAttributes(), style);
                    }
                    else if (resetExistingStyle)
                        newPara->GetAttributes() = wholeStyle;
                    else
                    {
                        if (applyMinimal)
                        {
                            // Only apply attributes that will make a difference to the combined
                            // style as seen on the display
                            wxTextAttr combinedAttr(para->GetCombinedAttributes());
                            wxRichTextApplyStyle(newPara->GetAttributes(), wholeStyle, & combinedAttr);
                        }
                        else
                            wxRichTextApplyStyle(newPara->GetAttributes(), wholeStyle);
                    }
                }

                // When applying paragraph styles dynamically, don't change the text objects' attributes
                // since they will computed as needed. Only apply the character styling if it's _only_
                // character styling. This policy is subject to change and might be put under user control.

                // Hm. we might well be applying a mix of paragraph and character styles, in which
                // case we _do_ want to apply character styles regardless of what para styles are set.
                // But if we're applying a paragraph style, which has some character attributes, but
                // we only want the paragraphs to hold this character style, then we _don't_ want to
                // apply the character style. So we need to be able to choose.

                if (!parasOnly && (characterStyle|charactersOnly) && range.GetStart() != newPara->GetRange().GetEnd())
                {
                    wxRichTextRange childRange(range);
                    childRange.LimitTo(newPara->GetRange());

                    // Find the starting position and if necessary split it so
                    // we can start applying a different style.
                    // TODO: check that the style actually changes or is different
                    // from style outside of range
                    wxRichTextObject* firstObject wxDUMMY_INITIALIZE(NULL);
                    wxRichTextObject* lastObject wxDUMMY_INITIALIZE(NULL);

                    if (childRange.GetStart() == newPara->GetRange().GetStart())
                        firstObject = newPara->GetChildren().GetFirst()->GetData();
                    else
                        firstObject = newPara->SplitAt(range.GetStart());

                    // Increment by 1 because we're apply the style one _after_ the split point
                    long splitPoint = childRange.GetEnd();
                    if (splitPoint != newPara->GetRange().GetEnd())
                        splitPoint ++;

                    // Find last object
                    if (splitPoint == newPara->GetRange().GetEnd())
                        lastObject = newPara->GetChildren().GetLast()->GetData();
                    else
                        // lastObject is set as a side-effect of splitting. It's
                        // returned as the object before the new object.
                        (void) newPara->SplitAt(splitPoint, & lastObject);

                    wxASSERT(firstObject != NULL);
                    wxASSERT(lastObject != NULL);

                    if (!firstObject || !lastObject)
                        continue;

                    wxRichTextObjectList::compatibility_iterator firstNode = newPara->GetChildren().Find(firstObject);
                    wxRichTextObjectList::compatibility_iterator lastNode = newPara->GetChildren().Find(lastObject);

                    wxASSERT(firstNode);
                    wxASSERT(lastNode);

                    wxRichTextObjectList::compatibility_iterator node2 = firstNode;

                    while (node2)
                    {
                        wxRichTextObject* child = node2->GetData();

                        if (removeStyle)
                        {
                            // Removes the given style from the paragraph
                            wxRichTextRemoveStyle(child->GetAttributes(), style);
                        }
                        else if (resetExistingStyle)
                            child->GetAttributes() = characterAttributes;
                        else
                        {
                            if (applyMinimal)
                            {
                                // Only apply attributes that will make a difference to the combined
                                // style as seen on the display
                                wxTextAttr combinedAttr(newPara->GetCombinedAttributes(child->GetAttributes()));
                                wxRichTextApplyStyle(child->GetAttributes(), characterAttributes, & combinedAttr);
                            }
                            else
                                wxRichTextApplyStyle(child->GetAttributes(), characterAttributes);
                        }

                        if (node2 == lastNode)
                            break;

                        node2 = node2->GetNext();
                    }
                }
            }
        }

        node = node->GetNext();
    }

    // Do action, or delay it until end of batch.
    if (haveControl && withUndo)
        GetRichTextCtrl()->GetBuffer().SubmitAction(action);

    return true;
}

void wxRichTextParagraphLayoutBox::SetImageStyle(wxRichTextImage *image, const wxRichTextImageAttr& attr, int flags)
{
    bool withUndo = flags & wxRICHTEXT_SETSTYLE_WITH_UNDO;
    bool haveControl = (GetRichTextCtrl() != NULL);
    wxRichTextParagraph* newPara wxDUMMY_INITIALIZE(NULL);
    wxRichTextParagraph* para = GetParagraphAtPosition(image->GetRange().GetStart());
    wxRichTextAction *action = NULL;
    wxRichTextImageAttr oldAttr = image->GetImageAttr();

    if (haveControl && withUndo)
    {
        action = new wxRichTextAction(NULL, _("Change Image Style"), wxRICHTEXT_CHANGE_STYLE, & GetRichTextCtrl()->GetBuffer(), GetRichTextCtrl());
        action->SetRange(image->GetRange().FromInternal());
        action->SetPosition(GetRichTextCtrl()->GetCaretPosition());
        image->SetImageAttr(attr);
        // Set the new attribute
        newPara = new wxRichTextParagraph(*para);
        action->GetNewParagraphs().AppendChild(newPara);
        // Change back to the old one
        image->SetImageAttr(oldAttr);
        action->GetOldParagraphs().AppendChild(new wxRichTextParagraph(*para));
    }
    else
        newPara = para;

    if (haveControl && withUndo)
        GetRichTextCtrl()->GetBuffer().SubmitAction(action);
}

/// Get the text attributes for this position.
bool wxRichTextParagraphLayoutBox::GetStyle(long position, wxTextAttr& style)
{
    return DoGetStyle(position, style, true);
}

bool wxRichTextParagraphLayoutBox::GetUncombinedStyle(long position, wxTextAttr& style)
{
    return DoGetStyle(position, style, false);
}

/// Implementation helper for GetStyle. If combineStyles is true, combine base, paragraph and
/// context attributes.
bool wxRichTextParagraphLayoutBox::DoGetStyle(long position, wxTextAttr& style, bool combineStyles)
{
    wxRichTextObject* obj wxDUMMY_INITIALIZE(NULL);

    if (style.IsParagraphStyle())
    {
        obj = GetParagraphAtPosition(position);
        if (obj)
        {
            if (combineStyles)
            {
                // Start with the base style
                style = GetAttributes();

                // Apply the paragraph style
                wxRichTextApplyStyle(style, obj->GetAttributes());
            }
            else
                style = obj->GetAttributes();

            return true;
        }
    }
    else
    {
        obj = GetLeafObjectAtPosition(position);
        if (obj)
        {
            if (combineStyles)
            {
                wxRichTextParagraph* para = wxDynamicCast(obj->GetParent(), wxRichTextParagraph);
                style = para ? para->GetCombinedAttributes(obj->GetAttributes()) : obj->GetAttributes();
            }
            else
                style = obj->GetAttributes();

            return true;
        }
    }
    return false;
}

static bool wxHasStyle(long flags, long style)
{
    return (flags & style) != 0;
}

/// Combines 'style' with 'currentStyle' for the purpose of summarising the attributes of a range of
/// content.
bool wxRichTextParagraphLayoutBox::CollectStyle(wxTextAttr& currentStyle, const wxTextAttr& style, long& multipleStyleAttributes, int& multipleTextEffectAttributes, int& absentStyleAttributes, int& absentTextEffectAttributes)
{
    absentStyleAttributes |= (~style.GetFlags() & wxTEXT_ATTR_ALL);
    absentTextEffectAttributes |= (~style.GetTextEffectFlags() & 0xFFFF);

    if (style.HasFont())
    {
        if (style.HasFontSize() && !wxHasStyle(multipleStyleAttributes|absentStyleAttributes, wxTEXT_ATTR_FONT_SIZE))
        {
            if (currentStyle.HasFontSize())
            {
                if (currentStyle.GetFontSize() != style.GetFontSize())
                {
                    // Clash of style - mark as such
                    multipleStyleAttributes |= wxTEXT_ATTR_FONT_SIZE;
                    currentStyle.SetFlags(currentStyle.GetFlags() & ~wxTEXT_ATTR_FONT_SIZE);
                }
            }
            else
            {
                currentStyle.SetFontSize(style.GetFontSize());
            }
        }

        if (style.HasFontItalic() && !wxHasStyle(multipleStyleAttributes|absentStyleAttributes, wxTEXT_ATTR_FONT_ITALIC))
        {
            if (currentStyle.HasFontItalic())
            {
                if (currentStyle.GetFontStyle() != style.GetFontStyle())
                {
                    // Clash of style - mark as such
                    multipleStyleAttributes |= wxTEXT_ATTR_FONT_ITALIC;
                    currentStyle.SetFlags(currentStyle.GetFlags() & ~wxTEXT_ATTR_FONT_ITALIC);
                }
            }
            else
            {
                currentStyle.SetFontStyle(style.GetFontStyle());
            }
        }

        if (style.HasFontFamily() && !wxHasStyle(multipleStyleAttributes|absentStyleAttributes, wxTEXT_ATTR_FONT_FAMILY))
        {
            if (currentStyle.HasFontFamily())
            {
                if (currentStyle.GetFontFamily() != style.GetFontFamily())
                {
                    // Clash of style - mark as such
                    multipleStyleAttributes |= wxTEXT_ATTR_FONT_FAMILY;
                    currentStyle.SetFlags(currentStyle.GetFlags() & ~wxTEXT_ATTR_FONT_FAMILY);
                }
            }
            else
            {
                currentStyle.SetFontFamily(style.GetFontFamily());
            }
        }

        if (style.HasFontWeight() && !wxHasStyle(multipleStyleAttributes|absentStyleAttributes, wxTEXT_ATTR_FONT_WEIGHT))
        {
            if (currentStyle.HasFontWeight())
            {
                if (currentStyle.GetFontWeight() != style.GetFontWeight())
                {
                    // Clash of style - mark as such
                    multipleStyleAttributes |= wxTEXT_ATTR_FONT_WEIGHT;
                    currentStyle.SetFlags(currentStyle.GetFlags() & ~wxTEXT_ATTR_FONT_WEIGHT);
                }
            }
            else
            {
                currentStyle.SetFontWeight(style.GetFontWeight());
            }
        }

        if (style.HasFontFaceName() && !wxHasStyle(multipleStyleAttributes|absentStyleAttributes, wxTEXT_ATTR_FONT_FACE))
        {
            if (currentStyle.HasFontFaceName())
            {
                wxString faceName1(currentStyle.GetFontFaceName());
                wxString faceName2(style.GetFontFaceName());

                if (faceName1 != faceName2)
                {
                    // Clash of style - mark as such
                    multipleStyleAttributes |= wxTEXT_ATTR_FONT_FACE;
                    currentStyle.SetFlags(currentStyle.GetFlags() & ~wxTEXT_ATTR_FONT_FACE);
                }
            }
            else
            {
                currentStyle.SetFontFaceName(style.GetFontFaceName());
            }
        }

        if (style.HasFontUnderlined() && !wxHasStyle(multipleStyleAttributes|absentStyleAttributes, wxTEXT_ATTR_FONT_UNDERLINE))
        {
            if (currentStyle.HasFontUnderlined())
            {
                if (currentStyle.GetFontUnderlined() != style.GetFontUnderlined())
                {
                    // Clash of style - mark as such
                    multipleStyleAttributes |= wxTEXT_ATTR_FONT_UNDERLINE;
                    currentStyle.SetFlags(currentStyle.GetFlags() & ~wxTEXT_ATTR_FONT_UNDERLINE);
                }
            }
            else
            {
                currentStyle.SetFontUnderlined(style.GetFontUnderlined());
            }
        }
    }

    if (style.HasTextColour() && !wxHasStyle(multipleStyleAttributes|absentStyleAttributes, wxTEXT_ATTR_TEXT_COLOUR))
    {
        if (currentStyle.HasTextColour())
        {
            if (currentStyle.GetTextColour() != style.GetTextColour())
            {
                // Clash of style - mark as such
                multipleStyleAttributes |= wxTEXT_ATTR_TEXT_COLOUR;
                currentStyle.SetFlags(currentStyle.GetFlags() & ~wxTEXT_ATTR_TEXT_COLOUR);
            }
        }
        else
            currentStyle.SetTextColour(style.GetTextColour());
    }

    if (style.HasBackgroundColour() && !wxHasStyle(multipleStyleAttributes|absentStyleAttributes, wxTEXT_ATTR_BACKGROUND_COLOUR))
    {
        if (currentStyle.HasBackgroundColour())
        {
            if (currentStyle.GetBackgroundColour() != style.GetBackgroundColour())
            {
                // Clash of style - mark as such
                multipleStyleAttributes |= wxTEXT_ATTR_BACKGROUND_COLOUR;
                currentStyle.SetFlags(currentStyle.GetFlags() & ~wxTEXT_ATTR_BACKGROUND_COLOUR);
            }
        }
        else
            currentStyle.SetBackgroundColour(style.GetBackgroundColour());
    }

    if (style.HasAlignment() && !wxHasStyle(multipleStyleAttributes|absentStyleAttributes, wxTEXT_ATTR_ALIGNMENT))
    {
        if (currentStyle.HasAlignment())
        {
            if (currentStyle.GetAlignment() != style.GetAlignment())
            {
                // Clash of style - mark as such
                multipleStyleAttributes |= wxTEXT_ATTR_ALIGNMENT;
                currentStyle.SetFlags(currentStyle.GetFlags() & ~wxTEXT_ATTR_ALIGNMENT);
            }
        }
        else
            currentStyle.SetAlignment(style.GetAlignment());
    }

    if (style.HasTabs() && !wxHasStyle(multipleStyleAttributes|absentStyleAttributes, wxTEXT_ATTR_TABS))
    {
        if (currentStyle.HasTabs())
        {
            if (!wxRichTextTabsEq(currentStyle.GetTabs(), style.GetTabs()))
            {
                // Clash of style - mark as such
                multipleStyleAttributes |= wxTEXT_ATTR_TABS;
                currentStyle.SetFlags(currentStyle.GetFlags() & ~wxTEXT_ATTR_TABS);
            }
        }
        else
            currentStyle.SetTabs(style.GetTabs());
    }

    if (style.HasLeftIndent() && !wxHasStyle(multipleStyleAttributes|absentStyleAttributes, wxTEXT_ATTR_LEFT_INDENT))
    {
        if (currentStyle.HasLeftIndent())
        {
            if (currentStyle.GetLeftIndent() != style.GetLeftIndent() || currentStyle.GetLeftSubIndent() != style.GetLeftSubIndent())
            {
                // Clash of style - mark as such
                multipleStyleAttributes |= wxTEXT_ATTR_LEFT_INDENT;
                currentStyle.SetFlags(currentStyle.GetFlags() & ~wxTEXT_ATTR_LEFT_INDENT);
            }
        }
        else
            currentStyle.SetLeftIndent(style.GetLeftIndent(), style.GetLeftSubIndent());
    }

    if (style.HasRightIndent() && !wxHasStyle(multipleStyleAttributes|absentStyleAttributes, wxTEXT_ATTR_RIGHT_INDENT))
    {
        if (currentStyle.HasRightIndent())
        {
            if (currentStyle.GetRightIndent() != style.GetRightIndent())
            {
                // Clash of style - mark as such
                multipleStyleAttributes |= wxTEXT_ATTR_RIGHT_INDENT;
                currentStyle.SetFlags(currentStyle.GetFlags() & ~wxTEXT_ATTR_RIGHT_INDENT);
            }
        }
        else
            currentStyle.SetRightIndent(style.GetRightIndent());
    }

    if (style.HasParagraphSpacingAfter() && !wxHasStyle(multipleStyleAttributes|absentStyleAttributes, wxTEXT_ATTR_PARA_SPACING_AFTER))
    {
        if (currentStyle.HasParagraphSpacingAfter())
        {
            if (currentStyle.GetParagraphSpacingAfter() != style.GetParagraphSpacingAfter())
            {
                // Clash of style - mark as such
                multipleStyleAttributes |= wxTEXT_ATTR_PARA_SPACING_AFTER;
                currentStyle.SetFlags(currentStyle.GetFlags() & ~wxTEXT_ATTR_PARA_SPACING_AFTER);
            }
        }
        else
            currentStyle.SetParagraphSpacingAfter(style.GetParagraphSpacingAfter());
    }

    if (style.HasParagraphSpacingBefore() && !wxHasStyle(multipleStyleAttributes|absentStyleAttributes, wxTEXT_ATTR_PARA_SPACING_BEFORE))
    {
        if (currentStyle.HasParagraphSpacingBefore())
        {
            if (currentStyle.GetParagraphSpacingBefore() != style.GetParagraphSpacingBefore())
            {
                // Clash of style - mark as such
                multipleStyleAttributes |= wxTEXT_ATTR_PARA_SPACING_BEFORE;
                currentStyle.SetFlags(currentStyle.GetFlags() & ~wxTEXT_ATTR_PARA_SPACING_BEFORE);
            }
        }
        else
            currentStyle.SetParagraphSpacingBefore(style.GetParagraphSpacingBefore());
    }

    if (style.HasLineSpacing() && !wxHasStyle(multipleStyleAttributes|absentStyleAttributes, wxTEXT_ATTR_LINE_SPACING))
    {
        if (currentStyle.HasLineSpacing())
        {
            if (currentStyle.GetLineSpacing() != style.GetLineSpacing())
            {
                // Clash of style - mark as such
                multipleStyleAttributes |= wxTEXT_ATTR_LINE_SPACING;
                currentStyle.SetFlags(currentStyle.GetFlags() & ~wxTEXT_ATTR_LINE_SPACING);
            }
        }
        else
            currentStyle.SetLineSpacing(style.GetLineSpacing());
    }

    if (style.HasCharacterStyleName() && !wxHasStyle(multipleStyleAttributes|absentStyleAttributes, wxTEXT_ATTR_CHARACTER_STYLE_NAME))
    {
        if (currentStyle.HasCharacterStyleName())
        {
            if (currentStyle.GetCharacterStyleName() != style.GetCharacterStyleName())
            {
                // Clash of style - mark as such
                multipleStyleAttributes |= wxTEXT_ATTR_CHARACTER_STYLE_NAME;
                currentStyle.SetFlags(currentStyle.GetFlags() & ~wxTEXT_ATTR_CHARACTER_STYLE_NAME);
            }
        }
        else
            currentStyle.SetCharacterStyleName(style.GetCharacterStyleName());
    }

    if (style.HasParagraphStyleName() && !wxHasStyle(multipleStyleAttributes|absentStyleAttributes, wxTEXT_ATTR_PARAGRAPH_STYLE_NAME))
    {
        if (currentStyle.HasParagraphStyleName())
        {
            if (currentStyle.GetParagraphStyleName() != style.GetParagraphStyleName())
            {
                // Clash of style - mark as such
                multipleStyleAttributes |= wxTEXT_ATTR_PARAGRAPH_STYLE_NAME;
                currentStyle.SetFlags(currentStyle.GetFlags() & ~wxTEXT_ATTR_PARAGRAPH_STYLE_NAME);
            }
        }
        else
            currentStyle.SetParagraphStyleName(style.GetParagraphStyleName());
    }

    if (style.HasListStyleName() && !wxHasStyle(multipleStyleAttributes|absentStyleAttributes, wxTEXT_ATTR_LIST_STYLE_NAME))
    {
        if (currentStyle.HasListStyleName())
        {
            if (currentStyle.GetListStyleName() != style.GetListStyleName())
            {
                // Clash of style - mark as such
                multipleStyleAttributes |= wxTEXT_ATTR_LIST_STYLE_NAME;
                currentStyle.SetFlags(currentStyle.GetFlags() & ~wxTEXT_ATTR_LIST_STYLE_NAME);
            }
        }
        else
            currentStyle.SetListStyleName(style.GetListStyleName());
    }

    if (style.HasBulletStyle() && !wxHasStyle(multipleStyleAttributes|absentStyleAttributes, wxTEXT_ATTR_BULLET_STYLE))
    {
        if (currentStyle.HasBulletStyle())
        {
            if (currentStyle.GetBulletStyle() != style.GetBulletStyle())
            {
                // Clash of style - mark as such
                multipleStyleAttributes |= wxTEXT_ATTR_BULLET_STYLE;
                currentStyle.SetFlags(currentStyle.GetFlags() & ~wxTEXT_ATTR_BULLET_STYLE);
            }
        }
        else
            currentStyle.SetBulletStyle(style.GetBulletStyle());
    }

    if (style.HasBulletNumber() && !wxHasStyle(multipleStyleAttributes|absentStyleAttributes, wxTEXT_ATTR_BULLET_NUMBER))
    {
        if (currentStyle.HasBulletNumber())
        {
            if (currentStyle.GetBulletNumber() != style.GetBulletNumber())
            {
                // Clash of style - mark as such
                multipleStyleAttributes |= wxTEXT_ATTR_BULLET_NUMBER;
                currentStyle.SetFlags(currentStyle.GetFlags() & ~wxTEXT_ATTR_BULLET_NUMBER);
            }
        }
        else
            currentStyle.SetBulletNumber(style.GetBulletNumber());
    }

    if (style.HasBulletText() && !wxHasStyle(multipleStyleAttributes|absentStyleAttributes, wxTEXT_ATTR_BULLET_TEXT))
    {
        if (currentStyle.HasBulletText())
        {
            if (currentStyle.GetBulletText() != style.GetBulletText())
            {
                // Clash of style - mark as such
                multipleStyleAttributes |= wxTEXT_ATTR_BULLET_TEXT;
                currentStyle.SetFlags(currentStyle.GetFlags() & ~wxTEXT_ATTR_BULLET_TEXT);
            }
        }
        else
        {
            currentStyle.SetBulletText(style.GetBulletText());
            currentStyle.SetBulletFont(style.GetBulletFont());
        }
    }

    if (style.HasBulletName() && !wxHasStyle(multipleStyleAttributes|absentStyleAttributes, wxTEXT_ATTR_BULLET_NAME))
    {
        if (currentStyle.HasBulletName())
        {
            if (currentStyle.GetBulletName() != style.GetBulletName())
            {
                // Clash of style - mark as such
                multipleStyleAttributes |= wxTEXT_ATTR_BULLET_NAME;
                currentStyle.SetFlags(currentStyle.GetFlags() & ~wxTEXT_ATTR_BULLET_NAME);
            }
        }
        else
        {
            currentStyle.SetBulletName(style.GetBulletName());
        }
    }

    if (style.HasURL() && !wxHasStyle(multipleStyleAttributes|absentStyleAttributes, wxTEXT_ATTR_URL))
    {
        if (currentStyle.HasURL())
        {
            if (currentStyle.GetURL() != style.GetURL())
            {
                // Clash of style - mark as such
                multipleStyleAttributes |= wxTEXT_ATTR_URL;
                currentStyle.SetFlags(currentStyle.GetFlags() & ~wxTEXT_ATTR_URL);
            }
        }
        else
        {
            currentStyle.SetURL(style.GetURL());
        }
    }

    if (style.HasTextEffects() && !wxHasStyle(multipleStyleAttributes|absentStyleAttributes, wxTEXT_ATTR_EFFECTS))
    {
        if (currentStyle.HasTextEffects())
        {
            // We need to find the bits in the new style that are different:
            // just look at those bits that are specified by the new style.

            // We need to remove the bits and flags that are not common between current style
            // and new style. In so doing we need to take account of the styles absent from one or more of the
            // previous styles.

            int currentRelevantTextEffects = currentStyle.GetTextEffects() & style.GetTextEffectFlags();
            int newRelevantTextEffects = style.GetTextEffects() & style.GetTextEffectFlags();

            if (currentRelevantTextEffects != newRelevantTextEffects)
            {
                // Find the text effects that were different, using XOR
                int differentEffects = currentRelevantTextEffects ^ newRelevantTextEffects;

                // Clash of style - mark as such
                multipleTextEffectAttributes |= differentEffects;
                currentStyle.SetTextEffectFlags(currentStyle.GetTextEffectFlags() & ~differentEffects);
            }
        }
        else
        {
            currentStyle.SetTextEffects(style.GetTextEffects());
            currentStyle.SetTextEffectFlags(style.GetTextEffectFlags());
        }

        // Mask out the flags and values that cannot be common because they were absent in one or more objecrs
        // that we've looked at so far
        currentStyle.SetTextEffects(currentStyle.GetTextEffects() & ~absentTextEffectAttributes);
        currentStyle.SetTextEffectFlags(currentStyle.GetTextEffectFlags() & ~absentTextEffectAttributes);

        if (currentStyle.GetTextEffectFlags() == 0)
            currentStyle.SetFlags(currentStyle.GetFlags() & ~wxTEXT_ATTR_EFFECTS);
    }

    if (style.HasOutlineLevel() && !wxHasStyle(multipleStyleAttributes|absentStyleAttributes, wxTEXT_ATTR_OUTLINE_LEVEL))
    {
        if (currentStyle.HasOutlineLevel())
        {
            if (currentStyle.GetOutlineLevel() != style.GetOutlineLevel())
            {
                // Clash of style - mark as such
                multipleStyleAttributes |= wxTEXT_ATTR_OUTLINE_LEVEL;
                currentStyle.SetFlags(currentStyle.GetFlags() & ~wxTEXT_ATTR_OUTLINE_LEVEL);
            }
        }
        else
            currentStyle.SetOutlineLevel(style.GetOutlineLevel());
    }

    return true;
}

/// Get the combined style for a range - if any attribute is different within the range,
/// that attribute is not present within the flags.
/// *** Note that this is not recursive, and so assumes that content inside a paragraph is not itself
/// nested.
bool wxRichTextParagraphLayoutBox::GetStyleForRange(const wxRichTextRange& range, wxTextAttr& style)
{
    style = wxTextAttr();

    // The attributes that aren't valid because of multiple styles within the range
    long multipleStyleAttributes = 0;
    int multipleTextEffectAttributes = 0;

    int absentStyleAttributesPara = 0;
    int absentStyleAttributesChar = 0;
    int absentTextEffectAttributesPara = 0;
    int absentTextEffectAttributesChar = 0;

    wxRichTextObjectList::compatibility_iterator node = GetChildren().GetFirst();
    while (node)
    {
        wxRichTextParagraph* para = (wxRichTextParagraph*) node->GetData();
        if (!(para->GetRange().GetStart() > range.GetEnd() || para->GetRange().GetEnd() < range.GetStart()))
        {
            if (para->GetChildren().GetCount() == 0)
            {
                wxTextAttr paraStyle = para->GetCombinedAttributes();

                CollectStyle(style, paraStyle, multipleStyleAttributes, multipleTextEffectAttributes, absentStyleAttributesPara, absentTextEffectAttributesPara);
            }
            else
            {
                wxRichTextRange paraRange(para->GetRange());
                paraRange.LimitTo(range);

                // First collect paragraph attributes only
                wxTextAttr paraStyle = para->GetCombinedAttributes();
                paraStyle.SetFlags(paraStyle.GetFlags() & wxTEXT_ATTR_PARAGRAPH);
                CollectStyle(style, paraStyle, multipleStyleAttributes, multipleTextEffectAttributes, absentStyleAttributesPara, absentTextEffectAttributesPara);

                wxRichTextObjectList::compatibility_iterator childNode = para->GetChildren().GetFirst();

                while (childNode)
                {
                    wxRichTextObject* child = childNode->GetData();
                    if (!(child->GetRange().GetStart() > range.GetEnd() || child->GetRange().GetEnd() < range.GetStart()))
                    {
                        wxTextAttr childStyle = para->GetCombinedAttributes(child->GetAttributes());

                        // Now collect character attributes only
                        childStyle.SetFlags(childStyle.GetFlags() & wxTEXT_ATTR_CHARACTER);

                        CollectStyle(style, childStyle, multipleStyleAttributes, multipleTextEffectAttributes, absentStyleAttributesChar, absentTextEffectAttributesChar);
                    }

                    childNode = childNode->GetNext();
                }
            }
        }
        node = node->GetNext();
    }
    return true;
}

/// Set default style
bool wxRichTextParagraphLayoutBox::SetDefaultStyle(const wxTextAttr& style)
{
    m_defaultAttributes = style;
    return true;
}

/// Test if this whole range has character attributes of the specified kind. If any
/// of the attributes are different within the range, the test fails. You
/// can use this to implement, for example, bold button updating. style must have
/// flags indicating which attributes are of interest.
bool wxRichTextParagraphLayoutBox::HasCharacterAttributes(const wxRichTextRange& range, const wxTextAttr& style) const
{
    int foundCount = 0;
    int matchingCount = 0;

    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextParagraph* para = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        wxASSERT (para != NULL);

        if (para)
        {
            // Stop searching if we're beyond the range of interest
            if (para->GetRange().GetStart() > range.GetEnd())
                return foundCount == matchingCount && foundCount != 0;

            if (!para->GetRange().IsOutside(range))
            {
                wxRichTextObjectList::compatibility_iterator node2 = para->GetChildren().GetFirst();

                while (node2)
                {
                    wxRichTextObject* child = node2->GetData();
                    // Allow for empty string if no buffer
                    wxRichTextRange childRange = child->GetRange();
                    if (childRange.GetLength() == 0 && GetRange().GetLength() == 1)
                        childRange.SetEnd(childRange.GetEnd()+1);

                    if (!childRange.IsOutside(range) && child->IsKindOf(CLASSINFO(wxRichTextPlainText)))
                    {
                        foundCount ++;
                        wxTextAttr textAttr = para->GetCombinedAttributes(child->GetAttributes());

                        if (wxTextAttrEqPartial(textAttr, style, style.GetFlags()))
                            matchingCount ++;
                    }

                    node2 = node2->GetNext();
                }
            }
        }

        node = node->GetNext();
    }

    return foundCount == matchingCount && foundCount != 0;
}

/// Test if this whole range has paragraph attributes of the specified kind. If any
/// of the attributes are different within the range, the test fails. You
/// can use this to implement, for example, centering button updating. style must have
/// flags indicating which attributes are of interest.
bool wxRichTextParagraphLayoutBox::HasParagraphAttributes(const wxRichTextRange& range, const wxTextAttr& style) const
{
    int foundCount = 0;
    int matchingCount = 0;

    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextParagraph* para = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        wxASSERT (para != NULL);

        if (para)
        {
            // Stop searching if we're beyond the range of interest
            if (para->GetRange().GetStart() > range.GetEnd())
                return foundCount == matchingCount && foundCount != 0;

            if (!para->GetRange().IsOutside(range))
            {
                wxTextAttr textAttr = GetAttributes();
                // Apply the paragraph style
                wxRichTextApplyStyle(textAttr, para->GetAttributes());

                foundCount ++;
                if (wxTextAttrEqPartial(textAttr, style, style.GetFlags()))
                    matchingCount ++;
            }
        }

        node = node->GetNext();
    }
    return foundCount == matchingCount && foundCount != 0;
}

void wxRichTextParagraphLayoutBox::Clear()
{
    DeleteChildren();
}

void wxRichTextParagraphLayoutBox::Reset()
{
    Clear();

    wxRichTextBuffer* buffer = wxDynamicCast(this, wxRichTextBuffer);
    if (buffer && GetRichTextCtrl())
    {
        wxRichTextEvent event(wxEVT_COMMAND_RICHTEXT_BUFFER_RESET, GetRichTextCtrl()->GetId());
        event.SetEventObject(GetRichTextCtrl());

        buffer->SendEvent(event, true);
    }

    AddParagraph(wxEmptyString);

    Invalidate(wxRICHTEXT_ALL);
}

/// Invalidate the buffer. With no argument, invalidates whole buffer.
void wxRichTextParagraphLayoutBox::Invalidate(const wxRichTextRange& invalidRange)
{
    SetDirty(true);

    if (invalidRange == wxRICHTEXT_ALL)
    {
        m_invalidRange = wxRICHTEXT_ALL;
        return;
    }

    // Already invalidating everything
    if (m_invalidRange == wxRICHTEXT_ALL)
        return;

    if ((invalidRange.GetStart() < m_invalidRange.GetStart()) || m_invalidRange.GetStart() == -1)
        m_invalidRange.SetStart(invalidRange.GetStart());
    if (invalidRange.GetEnd() > m_invalidRange.GetEnd())
        m_invalidRange.SetEnd(invalidRange.GetEnd());
}

/// Get invalid range, rounding to entire paragraphs if argument is true.
wxRichTextRange wxRichTextParagraphLayoutBox::GetInvalidRange(bool wholeParagraphs) const
{
    if (m_invalidRange == wxRICHTEXT_ALL || m_invalidRange == wxRICHTEXT_NONE)
        return m_invalidRange;

    wxRichTextRange range = m_invalidRange;

    if (wholeParagraphs)
    {
        wxRichTextParagraph* para1 = GetParagraphAtPosition(range.GetStart());
        wxRichTextParagraph* para2 = GetParagraphAtPosition(range.GetEnd());
        if (para1)
            range.SetStart(para1->GetRange().GetStart());
        if (para2)
            range.SetEnd(para2->GetRange().GetEnd());
    }
    return range;
}

/// Apply the style sheet to the buffer, for example if the styles have changed.
bool wxRichTextParagraphLayoutBox::ApplyStyleSheet(wxRichTextStyleSheet* styleSheet)
{
    wxASSERT(styleSheet != NULL);
    if (!styleSheet)
        return false;

    int foundCount = 0;

    wxRichTextAttr attr(GetBasicStyle());
    if (GetBasicStyle().HasParagraphStyleName())
    {
        wxRichTextParagraphStyleDefinition* paraDef = styleSheet->FindParagraphStyle(GetBasicStyle().GetParagraphStyleName());
        if (paraDef)
        {
            attr.Apply(paraDef->GetStyleMergedWithBase(styleSheet));
            SetBasicStyle(attr);
            foundCount ++;
        }
    }

    if (GetBasicStyle().HasCharacterStyleName())
    {
        wxRichTextCharacterStyleDefinition* charDef = styleSheet->FindCharacterStyle(GetBasicStyle().GetCharacterStyleName());
        if (charDef)
        {
            attr.Apply(charDef->GetStyleMergedWithBase(styleSheet));
            SetBasicStyle(attr);
            foundCount ++;
        }
    }

    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextParagraph* para = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        wxASSERT (para != NULL);

        if (para)
        {
            // Combine paragraph and list styles. If there is a list style in the original attributes,
            // the current indentation overrides anything else and is used to find the item indentation.
            // Also, for applying paragraph styles, consider having 2 modes: (1) we merge with what we have,
            // thereby taking into account all user changes, (2) reset the style completely (except for indentation/list
            // exception as above).
            // Problem: when changing from one list style to another, there's a danger that the level info will get lost.
            // So when changing a list style interactively, could retrieve level based on current style, then
            // set appropriate indent and apply new style.

            if (!para->GetAttributes().GetParagraphStyleName().IsEmpty() && !para->GetAttributes().GetListStyleName().IsEmpty())
            {
                int currentIndent = para->GetAttributes().GetLeftIndent();

                wxRichTextParagraphStyleDefinition* paraDef = styleSheet->FindParagraphStyle(para->GetAttributes().GetParagraphStyleName());
                wxRichTextListStyleDefinition* listDef = styleSheet->FindListStyle(para->GetAttributes().GetListStyleName());
                if (paraDef && !listDef)
                {
                    para->GetAttributes() = paraDef->GetStyleMergedWithBase(styleSheet);
                    foundCount ++;
                }
                else if (listDef && !paraDef)
                {
                    // Set overall style defined for the list style definition
                    para->GetAttributes() = listDef->GetStyleMergedWithBase(styleSheet);

                    // Apply the style for this level
                    wxRichTextApplyStyle(para->GetAttributes(), * listDef->GetLevelAttributes(listDef->FindLevelForIndent(currentIndent)));
                    foundCount ++;
                }
                else if (listDef && paraDef)
                {
                    // Combines overall list style, style for level, and paragraph style
                    para->GetAttributes() = listDef->CombineWithParagraphStyle(currentIndent, paraDef->GetStyleMergedWithBase(styleSheet));
                    foundCount ++;
                }
            }
            else if (para->GetAttributes().GetParagraphStyleName().IsEmpty() && !para->GetAttributes().GetListStyleName().IsEmpty())
            {
                int currentIndent = para->GetAttributes().GetLeftIndent();

                wxRichTextListStyleDefinition* listDef = styleSheet->FindListStyle(para->GetAttributes().GetListStyleName());

                // Overall list definition style
                para->GetAttributes() = listDef->GetStyleMergedWithBase(styleSheet);

                // Style for this level
                wxRichTextApplyStyle(para->GetAttributes(), * listDef->GetLevelAttributes(listDef->FindLevelForIndent(currentIndent)));

                foundCount ++;
            }
            else if (!para->GetAttributes().GetParagraphStyleName().IsEmpty() && para->GetAttributes().GetListStyleName().IsEmpty())
            {
                wxRichTextParagraphStyleDefinition* def = styleSheet->FindParagraphStyle(para->GetAttributes().GetParagraphStyleName());
                if (def)
                {
                    para->GetAttributes() = def->GetStyleMergedWithBase(styleSheet);
                    foundCount ++;
                }
            }
        }

        node = node->GetNext();
    }
    return foundCount != 0;
}

/// Set list style
bool wxRichTextParagraphLayoutBox::SetListStyle(const wxRichTextRange& range, wxRichTextListStyleDefinition* def, int flags, int startFrom, int specifiedLevel)
{
    wxRichTextStyleSheet* styleSheet = GetStyleSheet();

    bool withUndo = ((flags & wxRICHTEXT_SETSTYLE_WITH_UNDO) != 0);
    // bool applyMinimal = ((flags & wxRICHTEXT_SETSTYLE_OPTIMIZE) != 0);
    bool specifyLevel = ((flags & wxRICHTEXT_SETSTYLE_SPECIFY_LEVEL) != 0);
    bool renumber = ((flags & wxRICHTEXT_SETSTYLE_RENUMBER) != 0);

    // Current number, if numbering
    int n = startFrom;

    wxASSERT (!specifyLevel || (specifyLevel && (specifiedLevel >= 0)));

    // If we are associated with a control, make undoable; otherwise, apply immediately
    // to the data.

    bool haveControl = (GetRichTextCtrl() != NULL);

    wxRichTextAction* action = NULL;

    if (haveControl && withUndo)
    {
        action = new wxRichTextAction(NULL, _("Change List Style"), wxRICHTEXT_CHANGE_STYLE, & GetRichTextCtrl()->GetBuffer(), GetRichTextCtrl());
        action->SetRange(range);
        action->SetPosition(GetRichTextCtrl()->GetCaretPosition());
    }

    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextParagraph* para = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        wxASSERT (para != NULL);

        if (para && para->GetChildCount() > 0)
        {
            // Stop searching if we're beyond the range of interest
            if (para->GetRange().GetStart() > range.GetEnd())
                break;

            if (!para->GetRange().IsOutside(range))
            {
                // We'll be using a copy of the paragraph to make style changes,
                // not updating the buffer directly.
                wxRichTextParagraph* newPara wxDUMMY_INITIALIZE(NULL);

                if (haveControl && withUndo)
                {
                    newPara = new wxRichTextParagraph(*para);
                    action->GetNewParagraphs().AppendChild(newPara);

                    // Also store the old ones for Undo
                    action->GetOldParagraphs().AppendChild(new wxRichTextParagraph(*para));
                }
                else
                    newPara = para;

                if (def)
                {
                    int thisIndent = newPara->GetAttributes().GetLeftIndent();
                    int thisLevel = specifyLevel ? specifiedLevel : def->FindLevelForIndent(thisIndent);

                    // How is numbering going to work?
                    // If we are renumbering, or numbering for the first time, we need to keep
                    // track of the number for each level. But we might be simply applying a different
                    // list style.
                    // In Word, applying a style to several paragraphs, even if at different levels,
                    // reverts the level back to the same one. So we could do the same here.
                    // Renumbering will need to be done when we promote/demote a paragraph.

                    // Apply the overall list style, and item style for this level
                    wxTextAttr listStyle(def->GetCombinedStyleForLevel(thisLevel, styleSheet));
                    wxRichTextApplyStyle(newPara->GetAttributes(), listStyle);

                    // Now we need to do numbering
                    if (renumber)
                    {
                        newPara->GetAttributes().SetBulletNumber(n);
                    }

                    n ++;
                }
                else if (!newPara->GetAttributes().GetListStyleName().IsEmpty())
                {
                    // if def is NULL, remove list style, applying any associated paragraph style
                    // to restore the attributes

                    newPara->GetAttributes().SetListStyleName(wxEmptyString);
                    newPara->GetAttributes().SetLeftIndent(0, 0);
                    newPara->GetAttributes().SetBulletText(wxEmptyString);

                    // Eliminate the main list-related attributes
                    newPara->GetAttributes().SetFlags(newPara->GetAttributes().GetFlags() & ~wxTEXT_ATTR_LEFT_INDENT & ~wxTEXT_ATTR_BULLET_STYLE & ~wxTEXT_ATTR_BULLET_NUMBER & ~wxTEXT_ATTR_BULLET_TEXT & wxTEXT_ATTR_LIST_STYLE_NAME);

                    if (styleSheet && !newPara->GetAttributes().GetParagraphStyleName().IsEmpty())
                    {
                        wxRichTextParagraphStyleDefinition* def = styleSheet->FindParagraphStyle(newPara->GetAttributes().GetParagraphStyleName());
                        if (def)
                        {
                            newPara->GetAttributes() = def->GetStyleMergedWithBase(styleSheet);
                        }
                    }
                }
            }
        }

        node = node->GetNext();
    }

    // Do action, or delay it until end of batch.
    if (haveControl && withUndo)
        GetRichTextCtrl()->GetBuffer().SubmitAction(action);

    return true;
}

bool wxRichTextParagraphLayoutBox::SetListStyle(const wxRichTextRange& range, const wxString& defName, int flags, int startFrom, int specifiedLevel)
{
    if (GetStyleSheet())
    {
        wxRichTextListStyleDefinition* def = GetStyleSheet()->FindListStyle(defName);
        if (def)
            return SetListStyle(range, def, flags, startFrom, specifiedLevel);
    }
    return false;
}

/// Clear list for given range
bool wxRichTextParagraphLayoutBox::ClearListStyle(const wxRichTextRange& range, int flags)
{
    return SetListStyle(range, NULL, flags);
}

/// Number/renumber any list elements in the given range
bool wxRichTextParagraphLayoutBox::NumberList(const wxRichTextRange& range, wxRichTextListStyleDefinition* def, int flags, int startFrom, int specifiedLevel)
{
    return DoNumberList(range, range, 0, def, flags, startFrom, specifiedLevel);
}

/// Number/renumber any list elements in the given range. Also do promotion or demotion of items, if specified
bool wxRichTextParagraphLayoutBox::DoNumberList(const wxRichTextRange& range, const wxRichTextRange& promotionRange, int promoteBy,
                                                wxRichTextListStyleDefinition* def, int flags, int startFrom, int specifiedLevel)
{
    wxRichTextStyleSheet* styleSheet = GetStyleSheet();

    bool withUndo = ((flags & wxRICHTEXT_SETSTYLE_WITH_UNDO) != 0);
    // bool applyMinimal = ((flags & wxRICHTEXT_SETSTYLE_OPTIMIZE) != 0);
#if wxDEBUG_LEVEL
    bool specifyLevel = ((flags & wxRICHTEXT_SETSTYLE_SPECIFY_LEVEL) != 0);
#endif

    bool renumber = ((flags & wxRICHTEXT_SETSTYLE_RENUMBER) != 0);

    // Max number of levels
    const int maxLevels = 10;

    // The level we're looking at now
    int currentLevel = -1;

    // The item number for each level
    int levels[maxLevels];
    int i;

    // Reset all numbering
    for (i = 0; i < maxLevels; i++)
    {
        if (startFrom != -1)
            levels[i] = startFrom-1;
        else if (renumber) // start again
            levels[i] = 0;
        else
            levels[i] = -1; // start from the number we found, if any
    }

    wxASSERT(!specifyLevel || (specifyLevel && (specifiedLevel >= 0)));

    // If we are associated with a control, make undoable; otherwise, apply immediately
    // to the data.

    bool haveControl = (GetRichTextCtrl() != NULL);

    wxRichTextAction* action = NULL;

    if (haveControl && withUndo)
    {
        action = new wxRichTextAction(NULL, _("Renumber List"), wxRICHTEXT_CHANGE_STYLE, & GetRichTextCtrl()->GetBuffer(), GetRichTextCtrl());
        action->SetRange(range);
        action->SetPosition(GetRichTextCtrl()->GetCaretPosition());
    }

    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextParagraph* para = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        wxASSERT (para != NULL);

        if (para && para->GetChildCount() > 0)
        {
            // Stop searching if we're beyond the range of interest
            if (para->GetRange().GetStart() > range.GetEnd())
                break;

            if (!para->GetRange().IsOutside(range))
            {
                // We'll be using a copy of the paragraph to make style changes,
                // not updating the buffer directly.
                wxRichTextParagraph* newPara wxDUMMY_INITIALIZE(NULL);

                if (haveControl && withUndo)
                {
                    newPara = new wxRichTextParagraph(*para);
                    action->GetNewParagraphs().AppendChild(newPara);

                    // Also store the old ones for Undo
                    action->GetOldParagraphs().AppendChild(new wxRichTextParagraph(*para));
                }
                else
                    newPara = para;

                wxRichTextListStyleDefinition* defToUse = def;
                if (!defToUse)
                {
                    if (styleSheet && !newPara->GetAttributes().GetListStyleName().IsEmpty())
                        defToUse = styleSheet->FindListStyle(newPara->GetAttributes().GetListStyleName());
                }

                if (defToUse)
                {
                    int thisIndent = newPara->GetAttributes().GetLeftIndent();
                    int thisLevel = defToUse->FindLevelForIndent(thisIndent);

                    // If we've specified a level to apply to all, change the level.
                    if (specifiedLevel != -1)
                        thisLevel = specifiedLevel;

                    // Do promotion if specified
                    if ((promoteBy != 0) && !para->GetRange().IsOutside(promotionRange))
                    {
                        thisLevel = thisLevel - promoteBy;
                        if (thisLevel < 0)
                            thisLevel = 0;
                        if (thisLevel > 9)
                            thisLevel = 9;
                    }

                    // Apply the overall list style, and item style for this level
                    wxTextAttr listStyle(defToUse->GetCombinedStyleForLevel(thisLevel, styleSheet));
                    wxRichTextApplyStyle(newPara->GetAttributes(), listStyle);

                    // OK, we've (re)applied the style, now let's get the numbering right.

                    if (currentLevel == -1)
                        currentLevel = thisLevel;

                    // Same level as before, do nothing except increment level's number afterwards
                    if (currentLevel == thisLevel)
                    {
                    }
                    // A deeper level: start renumbering all levels after current level
                    else if (thisLevel > currentLevel)
                    {
                        for (i = currentLevel+1; i <= thisLevel; i++)
                        {
                            levels[i] = 0;
                        }
                        currentLevel = thisLevel;
                    }
                    else if (thisLevel < currentLevel)
                    {
                        currentLevel = thisLevel;
                    }

                    // Use the current numbering if -1 and we have a bullet number already
                    if (levels[currentLevel] == -1)
                    {
                        if (newPara->GetAttributes().HasBulletNumber())
                            levels[currentLevel] = newPara->GetAttributes().GetBulletNumber();
                        else
                            levels[currentLevel] = 1;
                    }
                    else
                    {
                        levels[currentLevel] ++;
                    }

                    newPara->GetAttributes().SetBulletNumber(levels[currentLevel]);

                    // Create the bullet text if an outline list
                    if (listStyle.GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_OUTLINE)
                    {
                        wxString text;
                        for (i = 0; i <= currentLevel; i++)
                        {
                            if (!text.IsEmpty())
                                text += wxT(".");
                            text += wxString::Format(wxT("%d"), levels[i]);
                        }
                        newPara->GetAttributes().SetBulletText(text);
                    }
                }
            }
        }

        node = node->GetNext();
    }

    // Do action, or delay it until end of batch.
    if (haveControl && withUndo)
        GetRichTextCtrl()->GetBuffer().SubmitAction(action);

    return true;
}

bool wxRichTextParagraphLayoutBox::NumberList(const wxRichTextRange& range, const wxString& defName, int flags, int startFrom, int specifiedLevel)
{
    if (GetStyleSheet())
    {
        wxRichTextListStyleDefinition* def = NULL;
        if (!defName.IsEmpty())
            def = GetStyleSheet()->FindListStyle(defName);
        return NumberList(range, def, flags, startFrom, specifiedLevel);
    }
    return false;
}

/// Promote the list items within the given range. promoteBy can be a positive or negative number, e.g. 1 or -1
bool wxRichTextParagraphLayoutBox::PromoteList(int promoteBy, const wxRichTextRange& range, wxRichTextListStyleDefinition* def, int flags, int specifiedLevel)
{
    // TODO
    // One strategy is to first work out the range within which renumbering must occur. Then could pass these two ranges
    // to NumberList with a flag indicating promotion is required within one of the ranges.
    // Find first and last paragraphs in range. Then for first, calculate new indentation and look back until we find
    // a paragraph that either has no list style, or has one that is different or whose indentation is less.
    // We start renumbering from the para after that different para we found. We specify that the numbering of that
    // list position will start from 1.
    // Similarly, we look after the last para in the promote range for an indentation that is less (or no list style).
    // We can end the renumbering at this point.

    // For now, only renumber within the promotion range.

    return DoNumberList(range, range, promoteBy, def, flags, 1, specifiedLevel);
}

bool wxRichTextParagraphLayoutBox::PromoteList(int promoteBy, const wxRichTextRange& range, const wxString& defName, int flags, int specifiedLevel)
{
    if (GetStyleSheet())
    {
        wxRichTextListStyleDefinition* def = NULL;
        if (!defName.IsEmpty())
            def = GetStyleSheet()->FindListStyle(defName);
        return PromoteList(promoteBy, range, def, flags, specifiedLevel);
    }
    return false;
}

/// Fills in the attributes for numbering a paragraph after previousParagraph. It also finds the
/// position of the paragraph that it had to start looking from.
bool wxRichTextParagraphLayoutBox::FindNextParagraphNumber(wxRichTextParagraph* previousParagraph, wxTextAttr& attr) const
{
    if (!previousParagraph->GetAttributes().HasFlag(wxTEXT_ATTR_BULLET_STYLE) || previousParagraph->GetAttributes().GetBulletStyle() == wxTEXT_ATTR_BULLET_STYLE_NONE)
        return false;

    wxRichTextStyleSheet* styleSheet = GetStyleSheet();
    if (styleSheet && !previousParagraph->GetAttributes().GetListStyleName().IsEmpty())
    {
        wxRichTextListStyleDefinition* def = styleSheet->FindListStyle(previousParagraph->GetAttributes().GetListStyleName());
        if (def)
        {
            // int thisIndent = previousParagraph->GetAttributes().GetLeftIndent();
            // int thisLevel = def->FindLevelForIndent(thisIndent);

            bool isOutline = (previousParagraph->GetAttributes().GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_OUTLINE) != 0;

            attr.SetFlags(previousParagraph->GetAttributes().GetFlags() & (wxTEXT_ATTR_BULLET_STYLE|wxTEXT_ATTR_BULLET_NUMBER|wxTEXT_ATTR_BULLET_TEXT|wxTEXT_ATTR_BULLET_NAME));
            if (previousParagraph->GetAttributes().HasBulletName())
                attr.SetBulletName(previousParagraph->GetAttributes().GetBulletName());
            attr.SetBulletStyle(previousParagraph->GetAttributes().GetBulletStyle());
            attr.SetListStyleName(previousParagraph->GetAttributes().GetListStyleName());

            int nextNumber = previousParagraph->GetAttributes().GetBulletNumber() + 1;
            attr.SetBulletNumber(nextNumber);

            if (isOutline)
            {
                wxString text = previousParagraph->GetAttributes().GetBulletText();
                if (!text.IsEmpty())
                {
                    int pos = text.Find(wxT('.'), true);
                    if (pos != wxNOT_FOUND)
                    {
                        text = text.Mid(0, text.Length() - pos - 1);
                    }
                    else
                        text = wxEmptyString;
                    if (!text.IsEmpty())
                        text += wxT(".");
                    text += wxString::Format(wxT("%d"), nextNumber);
                    attr.SetBulletText(text);
                }
            }

            return true;
        }
        else
            return false;
    }
    else
        return false;
}

/*!
 * wxRichTextParagraph
 * This object represents a single paragraph (or in a straight text editor, a line).
 */

IMPLEMENT_DYNAMIC_CLASS(wxRichTextParagraph, wxRichTextBox)

wxArrayInt wxRichTextParagraph::sm_defaultTabs;

wxRichTextParagraph::wxRichTextParagraph(wxRichTextObject* parent, wxTextAttr* style):
    wxRichTextBox(parent)
{
    if (style)
        SetAttributes(*style);
}

wxRichTextParagraph::wxRichTextParagraph(const wxString& text, wxRichTextObject* parent, wxTextAttr* paraStyle, wxTextAttr* charStyle):
    wxRichTextBox(parent)
{
    if (paraStyle)
        SetAttributes(*paraStyle);

    AppendChild(new wxRichTextPlainText(text, this, charStyle));
}

wxRichTextParagraph::~wxRichTextParagraph()
{
    ClearLines();
}

/// Draw the item
bool wxRichTextParagraph::Draw(wxDC& dc, const wxRichTextRange& range, const wxRichTextRange& selectionRange, const wxRect& rect, int WXUNUSED(descent), int style)
{
    wxTextAttr attr = GetCombinedAttributes();

    // Draw the bullet, if any
    if (attr.GetBulletStyle() != wxTEXT_ATTR_BULLET_STYLE_NONE)
    {
        if (attr.GetLeftSubIndent() != 0)
        {
            int spaceBeforePara = ConvertTenthsMMToPixels(dc, attr.GetParagraphSpacingBefore());
            int leftIndent = ConvertTenthsMMToPixels(dc, attr.GetLeftIndent());

            wxTextAttr bulletAttr(GetCombinedAttributes());

            // Combine with the font of the first piece of content, if one is specified
            if (GetChildren().GetCount() > 0)
            {
                wxRichTextObject* firstObj = (wxRichTextObject*) GetChildren().GetFirst()->GetData();
                if (firstObj->GetAttributes().HasFont())
                {
                    wxRichTextApplyStyle(bulletAttr, firstObj->GetAttributes());
                }
            }

            // Get line height from first line, if any
            wxRichTextLine* line = m_cachedLines.GetFirst() ? (wxRichTextLine* ) m_cachedLines.GetFirst()->GetData() : NULL;

            wxPoint linePos;
            int lineHeight wxDUMMY_INITIALIZE(0);
            if (line)
            {
                lineHeight = line->GetSize().y;
                linePos = line->GetPosition() + GetPosition();
            }
            else
            {
                wxFont font;
                if (bulletAttr.HasFont() && GetBuffer())
                    font = GetBuffer()->GetFontTable().FindFont(bulletAttr);
                else
                    font = (*wxNORMAL_FONT);

                wxCheckSetFont(dc, font);

                lineHeight = dc.GetCharHeight();
                linePos = GetPosition();
                linePos.y += spaceBeforePara;
            }

            wxRect bulletRect(GetPosition().x + leftIndent, linePos.y, linePos.x - (GetPosition().x + leftIndent), lineHeight);

            if (attr.GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_BITMAP)
            {
                if (wxRichTextBuffer::GetRenderer())
                    wxRichTextBuffer::GetRenderer()->DrawBitmapBullet(this, dc, bulletAttr, bulletRect);
            }
            else if (attr.GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_STANDARD)
            {
                if (wxRichTextBuffer::GetRenderer())
                    wxRichTextBuffer::GetRenderer()->DrawStandardBullet(this, dc, bulletAttr, bulletRect);
            }
            else
            {
                wxString bulletText = GetBulletText();

                if (!bulletText.empty() && wxRichTextBuffer::GetRenderer())
                    wxRichTextBuffer::GetRenderer()->DrawTextBullet(this, dc, bulletAttr, bulletRect, bulletText);
            }
        }
    }

    // Draw the range for each line, one object at a time.

    wxRichTextLineList::compatibility_iterator node = m_cachedLines.GetFirst();
    while (node)
    {
        wxRichTextLine* line = node->GetData();
        wxRichTextRange lineRange = line->GetAbsoluteRange();

        // Lines are specified relative to the paragraph

        wxPoint linePosition = line->GetPosition() + GetPosition();

        // Don't draw if off the screen
        if (((style & wxRICHTEXT_DRAW_IGNORE_CACHE) != 0) || ((linePosition.y + line->GetSize().y) >= rect.y && linePosition.y <= rect.y + rect.height))
        {
            wxPoint objectPosition = linePosition;
            int maxDescent = line->GetDescent();

            // Loop through objects until we get to the one within range
            wxRichTextObjectList::compatibility_iterator node2 = m_children.GetFirst();

            int i = 0;
            while (node2)
            {
                wxRichTextObject* child = node2->GetData();

                if (child->GetRange().GetLength() > 0 && !child->GetRange().IsOutside(lineRange) && !lineRange.IsOutside(range))
                {
                    // Draw this part of the line at the correct position
                    wxRichTextRange objectRange(child->GetRange());
                    objectRange.LimitTo(lineRange);

                    wxSize objectSize;
#if wxRICHTEXT_USE_OPTIMIZED_LINE_DRAWING && wxRICHTEXT_USE_PARTIAL_TEXT_EXTENTS
                    if (i < (int) line->GetObjectSizes().GetCount())
                    {
                        objectSize.x = line->GetObjectSizes()[(size_t) i];
                    }
                    else
#endif
                    {
                        int descent = 0;
                        child->GetRangeSize(objectRange, objectSize, descent, dc, wxRICHTEXT_UNFORMATTED, objectPosition);
                    }

                    // Use the child object's width, but the whole line's height
                    wxRect childRect(objectPosition, wxSize(objectSize.x, line->GetSize().y));
                    child->Draw(dc, objectRange, selectionRange, childRect, maxDescent, style);

                    objectPosition.x += objectSize.x;
                    i ++;
                }
                else if (child->GetRange().GetStart() > lineRange.GetEnd())
                    // Can break out of inner loop now since we've passed this line's range
                    break;

                node2 = node2->GetNext();
            }
        }

        node = node->GetNext();
    }

    return true;
}

// Get the range width using partial extents calculated for the whole paragraph.
static int wxRichTextGetRangeWidth(const wxRichTextParagraph& para, const wxRichTextRange& range, const wxArrayInt& partialExtents)
{
    wxASSERT(partialExtents.GetCount() >= (size_t) range.GetLength());

    if (partialExtents.GetCount() < (size_t) range.GetLength())
        return 0;

    int leftMostPos = 0;
    if (range.GetStart() - para.GetRange().GetStart() > 0)
        leftMostPos = partialExtents[range.GetStart() - para.GetRange().GetStart() - 1];

    int rightMostPos = partialExtents[range.GetEnd() - para.GetRange().GetStart()];

    int w = rightMostPos - leftMostPos;

    return w;
}

/// Lay the item out
bool wxRichTextParagraph::Layout(wxDC& dc, const wxRect& rect, int style)
{
    // Deal with floating objects firstly before the normal layout
    wxRichTextBuffer* buffer = GetBuffer();
    assert(buffer);
    wxFloatCollector* collector = buffer->GetFloatCollector();
    assert(collector);
    LayoutFloat(dc, rect, style, collector);
    collector->CollectFloat(this);

    wxTextAttr attr = GetCombinedAttributes();

    // ClearLines();

    // Increase the size of the paragraph due to spacing
    int spaceBeforePara = ConvertTenthsMMToPixels(dc, attr.GetParagraphSpacingBefore());
    int spaceAfterPara = ConvertTenthsMMToPixels(dc, attr.GetParagraphSpacingAfter());
    int leftIndent = ConvertTenthsMMToPixels(dc, attr.GetLeftIndent());
    int leftSubIndent = ConvertTenthsMMToPixels(dc, attr.GetLeftSubIndent());
    int rightIndent = ConvertTenthsMMToPixels(dc, attr.GetRightIndent());

    int lineSpacing = 0;

    // Let's assume line spacing of 10 is normal, 15 is 1.5, 20 is 2, etc.
    if (attr.GetLineSpacing() != 10 && GetBuffer())
    {
        wxFont font(GetBuffer()->GetFontTable().FindFont(attr));
        wxCheckSetFont(dc, font);
        lineSpacing = (ConvertTenthsMMToPixels(dc, dc.GetCharHeight()) * attr.GetLineSpacing())/10;
    }

    // Start position for each line relative to the paragraph
    int startPositionFirstLine = leftIndent;
    int startPositionSubsequentLines = leftIndent + leftSubIndent;
    wxRect availableRect;

    // If we have a bullet in this paragraph, the start position for the first line's text
    // is actually leftIndent + leftSubIndent.
    if (attr.GetBulletStyle() != wxTEXT_ATTR_BULLET_STYLE_NONE)
        startPositionFirstLine = startPositionSubsequentLines;

    long lastEndPos = GetRange().GetStart()-1;
    long lastCompletedEndPos = lastEndPos;

    int currentWidth = 0;
    SetPosition(rect.GetPosition());

    wxPoint currentPosition(0, spaceBeforePara); // We will calculate lines relative to paragraph
    int lineHeight = 0;
    int maxWidth = 0;
    int maxAscent = 0;
    int maxDescent = 0;
    int lineCount = 0;

    wxRichTextObjectList::compatibility_iterator node;

#if wxRICHTEXT_USE_PARTIAL_TEXT_EXTENTS
    wxUnusedVar(style);
    wxArrayInt partialExtents;

    wxSize paraSize;
    int paraDescent;

    // This calculates the partial text extents
    GetRangeSize(GetRange(), paraSize, paraDescent, dc, wxRICHTEXT_UNFORMATTED|wxRICHTEXT_CACHE_SIZE, wxPoint(0,0), & partialExtents);
#else
    node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();

        child->SetCachedSize(wxDefaultSize);
        child->Layout(dc, rect, style);

        node = node->GetNext();
    }

#endif

    // Split up lines

    // We may need to go back to a previous child, in which case create the new line,
    // find the child corresponding to the start position of the string, and
    // continue.

    node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();

        if (child->GetRange().GetLength() == 0)
        {
            node = node->GetNext();
            continue;
        }

        // If this is e.g. a composite text box, it will need to be laid out itself.
        // But if just a text fragment or image, for example, this will
        // do nothing. NB: won't we need to set the position after layout?
        // since for example if position is dependent on vertical line size, we
        // can't tell the position until the size is determined. So possibly introduce
        // another layout phase.

        // We may only be looking at part of a child, if we searched back for wrapping
        // and found a suitable point some way into the child. So get the size for the fragment
        // if necessary.

        long nextBreakPos = GetFirstLineBreakPosition(lastEndPos+1);
        long lastPosToUse = child->GetRange().GetEnd();
        bool lineBreakInThisObject = (nextBreakPos > -1 && nextBreakPos <= child->GetRange().GetEnd());

        if (lineBreakInThisObject)
            lastPosToUse = nextBreakPos;

        wxSize childSize;
        int childDescent = 0;

        if ((nextBreakPos == -1) && (lastEndPos == child->GetRange().GetStart() - 1)) // i.e. we want to get the whole thing
        {
            childSize = child->GetCachedSize();
            childDescent = child->GetDescent();
        }
        else
        {
#if wxRICHTEXT_USE_PARTIAL_TEXT_EXTENTS
            // Get height only, then the width using the partial extents
            GetRangeSize(wxRichTextRange(lastEndPos+1, lastPosToUse), childSize, childDescent, dc, wxRICHTEXT_UNFORMATTED|wxRICHTEXT_HEIGHT_ONLY);
            childSize.x = wxRichTextGetRangeWidth(*this, wxRichTextRange(lastEndPos+1, lastPosToUse), partialExtents);
#else
            GetRangeSize(wxRichTextRange(lastEndPos+1, lastPosToUse), childSize, childDescent, dc, wxRICHTEXT_UNFORMATTED, rect.GetPosition());
#endif
        }

        // Available width depends on the floating objects and the line height
        // Note: the floating objects may be placed vertically along the two side of
        //       buffer, so we may have different available line width with different
        //       [startY, endY]. So, we should can't determine how wide the available
        //       space is until we know the exact line height.
        maxDescent = wxMax(childDescent, maxDescent);
        maxAscent = wxMax(childSize.y-childDescent, maxAscent);
        lineHeight = maxDescent + maxAscent;
        availableRect = collector->GetAvailableRect(rect.y + currentPosition.y, rect.y + currentPosition.y + lineHeight);

        currentPosition.x = (lineCount == 0 ? availableRect.x + startPositionFirstLine : availableRect.x + startPositionSubsequentLines);

        // Cases:
        // 1) There was a line break BEFORE the natural break
        // 2) There was a line break AFTER the natural break
        // 3) The child still fits (carry on)

        if ((lineBreakInThisObject && (childSize.x + currentWidth <= availableRect.width)) ||
            (childSize.x + currentWidth > availableRect.width))
        {
            long wrapPosition = 0;

            int indent = lineCount == 0 ? startPositionFirstLine : startPositionSubsequentLines;
            indent += rightIndent;
            // Find a place to wrap. This may walk back to previous children,
            // for example if a word spans several objects.
            // Note: one object must contains only one wxTextAtrr, so the line height will not
            //       change inside one object. Thus, we can pass the remain line width to the
            //       FindWrapPosition function.
            if (!FindWrapPosition(wxRichTextRange(lastCompletedEndPos+1, child->GetRange().GetEnd()), dc, availableRect.width - indent, wrapPosition, & partialExtents))
            {
                // If the function failed, just cut it off at the end of this child.
                wrapPosition = child->GetRange().GetEnd();
            }

            // FindWrapPosition can still return a value that will put us in an endless wrapping loop
            if (wrapPosition <= lastCompletedEndPos)
                wrapPosition = wxMax(lastCompletedEndPos+1,child->GetRange().GetEnd());

            // wxLogDebug(wxT("Split at %ld"), wrapPosition);

            // Let's find the actual size of the current line now
            wxSize actualSize;
            wxRichTextRange actualRange(lastCompletedEndPos+1, wrapPosition);

            /// Use previous descent, not the wrapping descent we just found, since this may be too big
            /// for the fragment we're about to add.
            childDescent = maxDescent;

#if wxRICHTEXT_USE_PARTIAL_TEXT_EXTENTS
            // Get height only, then the width using the partial extents
            GetRangeSize(actualRange, actualSize, childDescent, dc, wxRICHTEXT_UNFORMATTED|wxRICHTEXT_HEIGHT_ONLY);
            actualSize.x = wxRichTextGetRangeWidth(*this, actualRange, partialExtents);
#else
            GetRangeSize(actualRange, actualSize, childDescent, dc, wxRICHTEXT_UNFORMATTED);
#endif

            currentWidth = actualSize.x;
            maxDescent = wxMax(childDescent, maxDescent);
            maxAscent = wxMax(actualSize.y-childDescent, maxAscent);
            lineHeight = maxDescent + maxAscent;

            // Add a new line
            wxRichTextLine* line = AllocateLine(lineCount);

            // Set relative range so we won't have to change line ranges when paragraphs are moved
            line->SetRange(wxRichTextRange(actualRange.GetStart() - GetRange().GetStart(), actualRange.GetEnd() - GetRange().GetStart()));
            line->SetPosition(currentPosition);
            line->SetSize(wxSize(currentWidth, lineHeight));
            line->SetDescent(maxDescent);

            // Now move down a line. TODO: add margins, spacing
            currentPosition.y += lineHeight;
            currentPosition.y += lineSpacing;
            currentWidth = 0;
            maxDescent = 0;
            maxAscent = 0;
            maxWidth = wxMax(maxWidth, currentWidth);

            lineCount ++;

            // TODO: account for zero-length objects, such as fields
            wxASSERT(wrapPosition > lastCompletedEndPos);

            lastEndPos = wrapPosition;
            lastCompletedEndPos = lastEndPos;

            lineHeight = 0;

            // May need to set the node back to a previous one, due to searching back in wrapping
            wxRichTextObject* childAfterWrapPosition = FindObjectAtPosition(wrapPosition+1);
            if (childAfterWrapPosition)
                node = m_children.Find(childAfterWrapPosition);
            else
                node = node->GetNext();
        }
        else
        {
            // We still fit, so don't add a line, and keep going
            currentWidth += childSize.x;
            maxDescent = wxMax(childDescent, maxDescent);
            maxAscent = wxMax(childSize.y-childDescent, maxAscent);
            lineHeight = maxDescent + maxAscent;

            maxWidth = wxMax(maxWidth, currentWidth);
            lastEndPos = child->GetRange().GetEnd();

            node = node->GetNext();
        }
    }

    // Add the last line - it's the current pos -> last para pos
    // Substract -1 because the last position is always the end-paragraph position.
    if (lastCompletedEndPos <= GetRange().GetEnd()-1)
    {
        currentPosition.x = (lineCount == 0 ? availableRect.x + startPositionFirstLine : availableRect.x + startPositionSubsequentLines);

        wxRichTextLine* line = AllocateLine(lineCount);

        wxRichTextRange actualRange(lastCompletedEndPos+1, GetRange().GetEnd()-1);

        // Set relative range so we won't have to change line ranges when paragraphs are moved
        line->SetRange(wxRichTextRange(actualRange.GetStart() - GetRange().GetStart(), actualRange.GetEnd() - GetRange().GetStart()));

        line->SetPosition(currentPosition);

        if (lineHeight == 0 && GetBuffer())
        {
            wxFont font(GetBuffer()->GetFontTable().FindFont(attr));
            wxCheckSetFont(dc, font);
            lineHeight = dc.GetCharHeight();
        }
        if (maxDescent == 0)
        {
            int w, h;
            dc.GetTextExtent(wxT("X"), & w, &h, & maxDescent);
        }

        line->SetSize(wxSize(currentWidth, lineHeight));
        line->SetDescent(maxDescent);
        currentPosition.y += lineHeight;
        currentPosition.y += lineSpacing;
        lineCount ++;
    }

    // Remove remaining unused line objects, if any
    ClearUnusedLines(lineCount);

    // Apply styles to wrapped lines
    ApplyParagraphStyle(attr, rect, dc);

    SetCachedSize(wxSize(maxWidth, currentPosition.y + spaceBeforePara + spaceAfterPara));

    m_dirty = false;

#if wxRICHTEXT_USE_PARTIAL_TEXT_EXTENTS
#if wxRICHTEXT_USE_OPTIMIZED_LINE_DRAWING
    // Use the text extents to calculate the size of each fragment in each line
    wxRichTextLineList::compatibility_iterator lineNode = m_cachedLines.GetFirst();
    while (lineNode)
    {
        wxRichTextLine* line = lineNode->GetData();
        wxRichTextRange lineRange = line->GetAbsoluteRange();

        // Loop through objects until we get to the one within range
        wxRichTextObjectList::compatibility_iterator node2 = m_children.GetFirst();

        while (node2)
        {
            wxRichTextObject* child = node2->GetData();

            if (child->GetRange().GetLength() > 0 && !child->GetRange().IsOutside(lineRange))
            {
                wxRichTextRange rangeToUse = lineRange;
                rangeToUse.LimitTo(child->GetRange());

                // Find the size of the child from the text extents, and store in an array
                // for drawing later
                int left = 0;
                if (rangeToUse.GetStart() > GetRange().GetStart())
                    left = partialExtents[(rangeToUse.GetStart()-1) - GetRange().GetStart()];
                int right = partialExtents[rangeToUse.GetEnd() - GetRange().GetStart()];
                int sz = right - left;
                line->GetObjectSizes().Add(sz);
            }
            else if (child->GetRange().GetStart() > lineRange.GetEnd())
                // Can break out of inner loop now since we've passed this line's range
                break;

            node2 = node2->GetNext();
        }

        lineNode = lineNode->GetNext();
    }
#endif
#endif

    return true;
}

/// Apply paragraph styles, such as centering, to wrapped lines
void wxRichTextParagraph::ApplyParagraphStyle(const wxTextAttr& attr, const wxRect& rect, wxDC& dc)
{
    if (!attr.HasAlignment())
        return;

    wxRichTextLineList::compatibility_iterator node = m_cachedLines.GetFirst();
    while (node)
    {
        wxRichTextLine* line = node->GetData();

        wxPoint pos = line->GetPosition();
        wxSize size = line->GetSize();

        // centering, right-justification
        if (attr.HasAlignment() && GetAttributes().GetAlignment() == wxTEXT_ALIGNMENT_CENTRE)
        {
            int rightIndent = ConvertTenthsMMToPixels(dc, attr.GetRightIndent());
            pos.x = (rect.GetWidth() - pos.x - rightIndent - size.x)/2 + pos.x;
            line->SetPosition(pos);
        }
        else if (attr.HasAlignment() && GetAttributes().GetAlignment() == wxTEXT_ALIGNMENT_RIGHT)
        {
            int rightIndent = ConvertTenthsMMToPixels(dc, attr.GetRightIndent());
            pos.x = rect.GetWidth() - size.x - rightIndent;
            line->SetPosition(pos);
        }

        node = node->GetNext();
    }
}

/// Insert text at the given position
bool wxRichTextParagraph::InsertText(long pos, const wxString& text)
{
    wxRichTextObject* childToUse = NULL;
    wxRichTextObjectList::compatibility_iterator nodeToUse = wxRichTextObjectList::compatibility_iterator();

    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();
        if (child->GetRange().Contains(pos) && child->GetRange().GetLength() > 0)
        {
            childToUse = child;
            nodeToUse = node;
            break;
        }

        node = node->GetNext();
    }

    if (childToUse)
    {
        wxRichTextPlainText* textObject = wxDynamicCast(childToUse, wxRichTextPlainText);
        if (textObject)
        {
            int posInString = pos - textObject->GetRange().GetStart();

            wxString newText = textObject->GetText().Mid(0, posInString) +
                               text + textObject->GetText().Mid(posInString);
            textObject->SetText(newText);

            int textLength = text.length();

            textObject->SetRange(wxRichTextRange(textObject->GetRange().GetStart(),
                                                 textObject->GetRange().GetEnd() + textLength));

            // Increment the end range of subsequent fragments in this paragraph.
            // We'll set the paragraph range itself at a higher level.

            wxRichTextObjectList::compatibility_iterator node = nodeToUse->GetNext();
            while (node)
            {
                wxRichTextObject* child = node->GetData();
                child->SetRange(wxRichTextRange(textObject->GetRange().GetStart() + textLength,
                                                 textObject->GetRange().GetEnd() + textLength));

                node = node->GetNext();
            }

            return true;
        }
        else
        {
            // TODO: if not a text object, insert at closest position, e.g. in front of it
        }
    }
    else
    {
        // Add at end.
        // Don't pass parent initially to suppress auto-setting of parent range.
        // We'll do that at a higher level.
        wxRichTextPlainText* textObject = new wxRichTextPlainText(text, this);

        AppendChild(textObject);
        return true;
    }

    return false;
}

void wxRichTextParagraph::Copy(const wxRichTextParagraph& obj)
{
    wxRichTextBox::Copy(obj);
}

/// Clear the cached lines
void wxRichTextParagraph::ClearLines()
{
    WX_CLEAR_LIST(wxRichTextLineList, m_cachedLines);
}

/// Get/set the object size for the given range. Returns false if the range
/// is invalid for this object.
bool wxRichTextParagraph::GetRangeSize(const wxRichTextRange& range, wxSize& size, int& descent, wxDC& dc, int flags, wxPoint position, wxArrayInt* partialExtents) const
{
    if (!range.IsWithin(GetRange()))
        return false;

    if (flags & wxRICHTEXT_UNFORMATTED)
    {
        // Just use unformatted data, assume no line breaks
        // TODO: take into account line breaks

        wxSize sz;

        wxArrayInt childExtents;
        wxArrayInt* p;
        if (partialExtents)
            p = & childExtents;
        else
            p = NULL;

        wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
        while (node)
        {

            wxRichTextObject* child = node->GetData();
            if (!child->GetRange().IsOutside(range))
            {
                wxSize childSize;

                wxRichTextRange rangeToUse = range;
                rangeToUse.LimitTo(child->GetRange());
                int childDescent = 0;

                // At present wxRICHTEXT_HEIGHT_ONLY is only fast if we're already cached the size,
                // but it's only going to be used after caching has taken place.
                if ((flags & wxRICHTEXT_HEIGHT_ONLY) && child->GetCachedSize().y != 0)
                {
                    childDescent = child->GetDescent();
                    childSize = child->GetCachedSize();

                    sz.y = wxMax(sz.y, childSize.y);
                    sz.x += childSize.x;
                    descent = wxMax(descent, childDescent);
                }
                else if (child->GetRangeSize(rangeToUse, childSize, childDescent, dc, flags, wxPoint(position.x + sz.x, position.y), p))
                {
                    sz.y = wxMax(sz.y, childSize.y);
                    sz.x += childSize.x;
                    descent = wxMax(descent, childDescent);

                    if ((flags & wxRICHTEXT_CACHE_SIZE) && (rangeToUse == child->GetRange()))
                    {
                        child->SetCachedSize(childSize);
                        child->SetDescent(childDescent);
                    }

                    if (partialExtents)
                    {
                        int lastSize;
                        if (partialExtents->GetCount() > 0)
                            lastSize = (*partialExtents)[partialExtents->GetCount()-1];
                        else
                            lastSize = 0;

                        size_t i;
                        for (i = 0; i < childExtents.GetCount(); i++)
                        {
                            partialExtents->Add(childExtents[i] + lastSize);
                        }
                    }
                }

                if (p)
                    p->Clear();
            }

            node = node->GetNext();
        }
        size = sz;
    }
    else
    {
        // Use formatted data, with line breaks
        wxSize sz;

        // We're going to loop through each line, and then for each line,
        // call GetRangeSize for the fragment that comprises that line.
        // Only we have to do that multiple times within the line, because
        // the line may be broken into pieces. For now ignore line break commands
        // (so we can assume that getting the unformatted size for a fragment
        // within a line is the actual size)

        wxRichTextLineList::compatibility_iterator node = m_cachedLines.GetFirst();
        while (node)
        {
            wxRichTextLine* line = node->GetData();
            wxRichTextRange lineRange = line->GetAbsoluteRange();
            if (!lineRange.IsOutside(range))
            {
                wxSize lineSize;

                wxRichTextObjectList::compatibility_iterator node2 = m_children.GetFirst();
                while (node2)
                {
                    wxRichTextObject* child = node2->GetData();

                    if (!child->GetRange().IsOutside(lineRange))
                    {
                        wxRichTextRange rangeToUse = lineRange;
                        rangeToUse.LimitTo(child->GetRange());

                        wxSize childSize;
                        int childDescent = 0;
                        if (child->GetRangeSize(rangeToUse, childSize, childDescent, dc, flags, wxPoint(position.x + sz.x, position.y)))
                        {
                            lineSize.y = wxMax(lineSize.y, childSize.y);
                            lineSize.x += childSize.x;
                        }
                        descent = wxMax(descent, childDescent);
                    }

                    node2 = node2->GetNext();
                }

                // Increase size by a line (TODO: paragraph spacing)
                sz.y += lineSize.y;
                sz.x = wxMax(sz.x, lineSize.x);
            }
            node = node->GetNext();
        }
        size = sz;
    }
    return true;
}

/// Finds the absolute position and row height for the given character position
bool wxRichTextParagraph::FindPosition(wxDC& dc, long index, wxPoint& pt, int* height, bool forceLineStart)
{
    if (index == -1)
    {
        wxRichTextLine* line = ((wxRichTextParagraphLayoutBox*)GetParent())->GetLineAtPosition(0);
        if (line)
            *height = line->GetSize().y;
        else
            *height = dc.GetCharHeight();

        // -1 means 'the start of the buffer'.
        pt = GetPosition();
        if (line)
            pt = pt + line->GetPosition();

        return true;
    }

    // The final position in a paragraph is taken to mean the position
    // at the start of the next paragraph.
    if (index == GetRange().GetEnd())
    {
        wxRichTextParagraphLayoutBox* parent = wxDynamicCast(GetParent(), wxRichTextParagraphLayoutBox);
        wxASSERT( parent != NULL );

        // Find the height at the next paragraph, if any
        wxRichTextLine* line = parent->GetLineAtPosition(index + 1);
        if (line)
        {
            *height = line->GetSize().y;
            pt = line->GetAbsolutePosition();
        }
        else
        {
            *height = dc.GetCharHeight();
            int indent = ConvertTenthsMMToPixels(dc, m_attributes.GetLeftIndent());
            pt = wxPoint(indent, GetCachedSize().y);
        }

        return true;
    }

    if (index < GetRange().GetStart() || index > GetRange().GetEnd())
        return false;

    wxRichTextLineList::compatibility_iterator node = m_cachedLines.GetFirst();
    while (node)
    {
        wxRichTextLine* line = node->GetData();
        wxRichTextRange lineRange = line->GetAbsoluteRange();
        if (index >= lineRange.GetStart() && index <= lineRange.GetEnd())
        {
            // If this is the last point in the line, and we're forcing the
            // returned value to be the start of the next line, do the required
            // thing.
            if (index == lineRange.GetEnd() && forceLineStart)
            {
                if (node->GetNext())
                {
                    wxRichTextLine* nextLine = node->GetNext()->GetData();
                    *height = nextLine->GetSize().y;
                    pt = nextLine->GetAbsolutePosition();
                    return true;
                }
            }

            pt.y = line->GetPosition().y + GetPosition().y;

            wxRichTextRange r(lineRange.GetStart(), index);
            wxSize rangeSize;
            int descent = 0;

            // We find the size of the line up to this point,
            // then we can add this size to the line start position and
            // paragraph start position to find the actual position.

            if (GetRangeSize(r, rangeSize, descent, dc, wxRICHTEXT_UNFORMATTED, line->GetPosition()+ GetPosition()))
            {
                pt.x = line->GetPosition().x + GetPosition().x + rangeSize.x;
                *height = line->GetSize().y;

                return true;
            }

        }

        node = node->GetNext();
    }

    return false;
}

/// Hit-testing: returns a flag indicating hit test details, plus
/// information about position
int wxRichTextParagraph::HitTest(wxDC& dc, const wxPoint& pt, long& textPosition)
{
    wxPoint paraPos = GetPosition();

    wxRichTextLineList::compatibility_iterator node = m_cachedLines.GetFirst();
    while (node)
    {
        wxRichTextLine* line = node->GetData();
        wxPoint linePos = paraPos + line->GetPosition();
        wxSize lineSize = line->GetSize();
        wxRichTextRange lineRange = line->GetAbsoluteRange();

        if (pt.y <= linePos.y + lineSize.y)
        {
            if (pt.x < linePos.x)
            {
                textPosition = lineRange.GetStart();
                return wxRICHTEXT_HITTEST_BEFORE|wxRICHTEXT_HITTEST_OUTSIDE;
            }
            else if (pt.x >= (linePos.x + lineSize.x))
            {
                textPosition = lineRange.GetEnd();
                return wxRICHTEXT_HITTEST_AFTER|wxRICHTEXT_HITTEST_OUTSIDE;
            }
            else
            {
#if wxRICHTEXT_USE_PARTIAL_TEXT_EXTENTS
                wxArrayInt partialExtents;

                wxSize paraSize;
                int paraDescent;

                // This calculates the partial text extents
                GetRangeSize(lineRange, paraSize, paraDescent, dc, wxRICHTEXT_UNFORMATTED, wxPoint(0,0), & partialExtents);

                int lastX = linePos.x;
                size_t i;
                for (i = 0; i < partialExtents.GetCount(); i++)
                {
                    int nextX = partialExtents[i] + linePos.x;

                    if (pt.x >= lastX && pt.x <= nextX)
                    {
                        textPosition = i + lineRange.GetStart(); // minus 1?

                        // So now we know it's between i-1 and i.
                        // Let's see if we can be more precise about
                        // which side of the position it's on.

                        int midPoint = (nextX + lastX)/2;
                        if (pt.x >= midPoint)
                            return wxRICHTEXT_HITTEST_AFTER;
                        else
                            return wxRICHTEXT_HITTEST_BEFORE;
                    }

                    lastX = nextX;
                }
#else
                long i;
                int lastX = linePos.x;
                for (i = lineRange.GetStart(); i <= lineRange.GetEnd(); i++)
                {
                    wxSize childSize;
                    int descent = 0;

                    wxRichTextRange rangeToUse(lineRange.GetStart(), i);

                    GetRangeSize(rangeToUse, childSize, descent, dc, wxRICHTEXT_UNFORMATTED, linePos);

                    int nextX = childSize.x + linePos.x;

                    if (pt.x >= lastX && pt.x <= nextX)
                    {
                        textPosition = i;

                        // So now we know it's between i-1 and i.
                        // Let's see if we can be more precise about
                        // which side of the position it's on.

                        int midPoint = (nextX + lastX)/2;
                        if (pt.x >= midPoint)
                            return wxRICHTEXT_HITTEST_AFTER;
                        else
                            return wxRICHTEXT_HITTEST_BEFORE;
                    }
                    else
                    {
                        lastX = nextX;
                    }
                }
#endif
            }
        }

        node = node->GetNext();
    }

    return wxRICHTEXT_HITTEST_NONE;
}

/// Split an object at this position if necessary, and return
/// the previous object, or NULL if inserting at beginning.
wxRichTextObject* wxRichTextParagraph::SplitAt(long pos, wxRichTextObject** previousObject)
{
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* child = node->GetData();

        if (pos == child->GetRange().GetStart())
        {
            if (previousObject)
            {
                if (node->GetPrevious())
                    *previousObject = node->GetPrevious()->GetData();
                else
                    *previousObject = NULL;
            }

            return child;
        }

        if (child->GetRange().Contains(pos))
        {
            // This should create a new object, transferring part of
            // the content to the old object and the rest to the new object.
            wxRichTextObject* newObject = child->DoSplit(pos);

            // If we couldn't split this object, just insert in front of it.
            if (!newObject)
            {
                // Maybe this is an empty string, try the next one
                // return child;
            }
            else
            {
                // Insert the new object after 'child'
                if (node->GetNext())
                    m_children.Insert(node->GetNext(), newObject);
                else
                    m_children.Append(newObject);
                newObject->SetParent(this);

                if (previousObject)
                    *previousObject = child;

                return newObject;
            }
        }

        node = node->GetNext();
    }
    if (previousObject)
        *previousObject = NULL;
    return NULL;
}

/// Move content to a list from obj on
void wxRichTextParagraph::MoveToList(wxRichTextObject* obj, wxList& list)
{
    wxRichTextObjectList::compatibility_iterator node = m_children.Find(obj);
    while (node)
    {
        wxRichTextObject* child = node->GetData();
        list.Append(child);

        wxRichTextObjectList::compatibility_iterator oldNode = node;

        node = node->GetNext();

        m_children.DeleteNode(oldNode);
    }
}

/// Add content back from list
void wxRichTextParagraph::MoveFromList(wxList& list)
{
    for (wxList::compatibility_iterator node = list.GetFirst(); node; node = node->GetNext())
    {
        AppendChild((wxRichTextObject*) node->GetData());
    }
}

/// Calculate range
void wxRichTextParagraph::CalculateRange(long start, long& end)
{
    wxRichTextCompositeObject::CalculateRange(start, end);

    // Add one for end of paragraph
    end ++;

    m_range.SetRange(start, end);
}

/// Find the object at the given position
wxRichTextObject* wxRichTextParagraph::FindObjectAtPosition(long position)
{
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* obj = node->GetData();
        if (obj->GetRange().Contains(position))
            return obj;

        node = node->GetNext();
    }
    return NULL;
}

/// Get the plain text searching from the start or end of the range.
/// The resulting string may be shorter than the range given.
bool wxRichTextParagraph::GetContiguousPlainText(wxString& text, const wxRichTextRange& range, bool fromStart)
{
    text = wxEmptyString;

    if (fromStart)
    {
        wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
        while (node)
        {
            wxRichTextObject* obj = node->GetData();
            if (!obj->GetRange().IsOutside(range))
            {
                wxRichTextPlainText* textObj = wxDynamicCast(obj, wxRichTextPlainText);
                if (textObj)
                {
                    text += textObj->GetTextForRange(range);
                }
                else
                {
                    text += wxT(" ");
                }
            }

            node = node->GetNext();
        }
    }
    else
    {
        wxRichTextObjectList::compatibility_iterator node = m_children.GetLast();
        while (node)
        {
            wxRichTextObject* obj = node->GetData();
            if (!obj->GetRange().IsOutside(range))
            {
                wxRichTextPlainText* textObj = wxDynamicCast(obj, wxRichTextPlainText);
                if (textObj)
                {
                    text = textObj->GetTextForRange(range) + text;
                }
                else
                {
                    text = wxT(" ") + text;
                }
            }

            node = node->GetPrevious();
        }
    }

    return true;
}

/// Find a suitable wrap position.
bool wxRichTextParagraph::FindWrapPosition(const wxRichTextRange& range, wxDC& dc, int availableSpace, long& wrapPosition, wxArrayInt* partialExtents)
{
    if (range.GetLength() <= 0)
        return false;

    // Find the first position where the line exceeds the available space.
    wxSize sz;
    long breakPosition = range.GetEnd();

#if wxRICHTEXT_USE_PARTIAL_TEXT_EXTENTS
    if (partialExtents && partialExtents->GetCount() >= (size_t) (GetRange().GetLength()-1)) // the final position in a paragraph is the newline
    {
        int widthBefore;

        if (range.GetStart() > GetRange().GetStart())
            widthBefore = (*partialExtents)[range.GetStart() - GetRange().GetStart() - 1];
        else
            widthBefore = 0;

        size_t i;
        for (i = (size_t) range.GetStart(); i <= (size_t) range.GetEnd(); i++)
        {
            int widthFromStartOfThisRange = (*partialExtents)[i - GetRange().GetStart()] - widthBefore;

            if (widthFromStartOfThisRange > availableSpace)
            {
                breakPosition = i-1;
                break;
            }
        }
    }
    else
#endif
    {
        // Binary chop for speed
        long minPos = range.GetStart();
        long maxPos = range.GetEnd();
        while (true)
        {
            if (minPos == maxPos)
            {
                int descent = 0;
                GetRangeSize(wxRichTextRange(range.GetStart(), minPos), sz, descent, dc, wxRICHTEXT_UNFORMATTED);

                if (sz.x > availableSpace)
                    breakPosition = minPos - 1;
                break;
            }
            else if ((maxPos - minPos) == 1)
            {
                int descent = 0;
                GetRangeSize(wxRichTextRange(range.GetStart(), minPos), sz, descent, dc, wxRICHTEXT_UNFORMATTED);

                if (sz.x > availableSpace)
                    breakPosition = minPos - 1;
                else
                {
                    GetRangeSize(wxRichTextRange(range.GetStart(), maxPos), sz, descent, dc, wxRICHTEXT_UNFORMATTED);
                    if (sz.x > availableSpace)
                        breakPosition = maxPos-1;
                }
                break;
            }
            else
            {
                long nextPos = minPos + ((maxPos - minPos) / 2);

                int descent = 0;
                GetRangeSize(wxRichTextRange(range.GetStart(), nextPos), sz, descent, dc, wxRICHTEXT_UNFORMATTED);

                if (sz.x > availableSpace)
                {
                    maxPos = nextPos;
                }
                else
                {
                    minPos = nextPos;
                }
            }
        }
    }

    // Now we know the last position on the line.
    // Let's try to find a word break.

    wxString plainText;
    if (GetContiguousPlainText(plainText, wxRichTextRange(range.GetStart(), breakPosition), false))
    {
        int newLinePos = plainText.Find(wxRichTextLineBreakChar);
        if (newLinePos != wxNOT_FOUND)
        {
            breakPosition = wxMax(0, range.GetStart() + newLinePos);
        }
        else
        {
            int spacePos = plainText.Find(wxT(' '), true);
            int tabPos = plainText.Find(wxT('\t'), true);
            int pos = wxMax(spacePos, tabPos);
            if (pos != wxNOT_FOUND)
            {
                int positionsFromEndOfString = plainText.length() - pos - 1;
                breakPosition = breakPosition - positionsFromEndOfString;
            }
        }
    }

    wrapPosition = breakPosition;

    return true;
}

/// Get the bullet text for this paragraph.
wxString wxRichTextParagraph::GetBulletText()
{
    if (GetAttributes().GetBulletStyle() == wxTEXT_ATTR_BULLET_STYLE_NONE ||
        (GetAttributes().GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_BITMAP))
        return wxEmptyString;

    int number = GetAttributes().GetBulletNumber();

    wxString text;
    if ((GetAttributes().GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_ARABIC) || (GetAttributes().GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_OUTLINE))
    {
        text.Printf(wxT("%d"), number);
    }
    else if (GetAttributes().GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_LETTERS_UPPER)
    {
        // TODO: Unicode, and also check if number > 26
        text.Printf(wxT("%c"), (wxChar) (number+64));
    }
    else if (GetAttributes().GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_LETTERS_LOWER)
    {
        // TODO: Unicode, and also check if number > 26
        text.Printf(wxT("%c"), (wxChar) (number+96));
    }
    else if (GetAttributes().GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_ROMAN_UPPER)
    {
        text = wxRichTextDecimalToRoman(number);
    }
    else if (GetAttributes().GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_ROMAN_LOWER)
    {
        text = wxRichTextDecimalToRoman(number);
        text.MakeLower();
    }
    else if (GetAttributes().GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_SYMBOL)
    {
        text = GetAttributes().GetBulletText();
    }

    if (GetAttributes().GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_OUTLINE)
    {
        // The outline style relies on the text being computed statically,
        // since it depends on other levels points (e.g. 1.2.1.1). So normally the bullet text
        // should be stored in the attributes; if not, just use the number for this
        // level, as previously computed.
        if (!GetAttributes().GetBulletText().IsEmpty())
            text = GetAttributes().GetBulletText();
    }

    if (GetAttributes().GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_PARENTHESES)
    {
        text = wxT("(") + text + wxT(")");
    }
    else if (GetAttributes().GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_RIGHT_PARENTHESIS)
    {
        text = text + wxT(")");
    }

    if (GetAttributes().GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_PERIOD)
    {
        text += wxT(".");
    }

    return text;
}

/// Allocate or reuse a line object
wxRichTextLine* wxRichTextParagraph::AllocateLine(int pos)
{
    if (pos < (int) m_cachedLines.GetCount())
    {
        wxRichTextLine* line = m_cachedLines.Item(pos)->GetData();
        line->Init(this);
        return line;
    }
    else
    {
        wxRichTextLine* line = new wxRichTextLine(this);
        m_cachedLines.Append(line);
        return line;
    }
}

/// Clear remaining unused line objects, if any
bool wxRichTextParagraph::ClearUnusedLines(int lineCount)
{
    int cachedLineCount = m_cachedLines.GetCount();
    if ((int) cachedLineCount > lineCount)
    {
        for (int i = 0; i < (int) (cachedLineCount - lineCount); i ++)
        {
            wxRichTextLineList::compatibility_iterator node = m_cachedLines.GetLast();
            wxRichTextLine* line = node->GetData();
            m_cachedLines.Erase(node);
            delete line;
        }
    }
    return true;
}

/// Get combined attributes of the base style, paragraph style and character style. We use this to dynamically
/// retrieve the actual style.
wxTextAttr wxRichTextParagraph::GetCombinedAttributes(const wxTextAttr& contentStyle) const
{
    wxTextAttr attr;
    wxRichTextBuffer* buf = wxDynamicCast(GetParent(), wxRichTextBuffer);
    if (buf)
    {
        attr = buf->GetBasicStyle();
        wxRichTextApplyStyle(attr, GetAttributes());
    }
    else
        attr = GetAttributes();

    wxRichTextApplyStyle(attr, contentStyle);
    return attr;
}

/// Get combined attributes of the base style and paragraph style.
wxTextAttr wxRichTextParagraph::GetCombinedAttributes() const
{
    wxTextAttr attr;
    wxRichTextBuffer* buf = wxDynamicCast(GetParent(), wxRichTextBuffer);
    if (buf)
    {
        attr = buf->GetBasicStyle();
        wxRichTextApplyStyle(attr, GetAttributes());
    }
    else
        attr = GetAttributes();

    return attr;
}

/// Create default tabstop array
void wxRichTextParagraph::InitDefaultTabs()
{
    // create a default tab list at 10 mm each.
    for (int i = 0; i < 20; ++i)
    {
        sm_defaultTabs.Add(i*100);
    }
}

/// Clear default tabstop array
void wxRichTextParagraph::ClearDefaultTabs()
{
    sm_defaultTabs.Clear();
}

void wxRichTextParagraph::CollectFloat()
{
    m_anchoredObjects.Clear();
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* obj = node->GetData();
        if (obj->IsPlaceHolding())
        {
            m_anchoredObjects.Append(obj);
        }
    }
}

void wxRichTextParagraph::LayoutFloat(wxDC& WXUNUSED(dc), const wxRect& rect, int WXUNUSED(style), const wxFloatCollector* floatCollector)
{
    
}

/// Get the first position from pos that has a line break character.
long wxRichTextParagraph::GetFirstLineBreakPosition(long pos)
{
    wxRichTextObjectList::compatibility_iterator node = m_children.GetFirst();
    while (node)
    {
        wxRichTextObject* obj = node->GetData();
        if (pos >= obj->GetRange().GetStart() && pos <= obj->GetRange().GetEnd())
        {
            wxRichTextPlainText* textObj = wxDynamicCast(obj, wxRichTextPlainText);
            if (textObj)
            {
                long breakPos = textObj->GetFirstLineBreakPosition(pos);
                if (breakPos > -1)
                    return breakPos;
            }
        }
        node = node->GetNext();
    }
    return -1;
}

/*!
 * wxRichTextLine
 * This object represents a line in a paragraph, and stores
 * offsets from the start of the paragraph representing the
 * start and end positions of the line.
 */

wxRichTextLine::wxRichTextLine(wxRichTextParagraph* parent)
{
    Init(parent);
}

/// Initialisation
void wxRichTextLine::Init(wxRichTextParagraph* parent)
{
    m_parent = parent;
    m_range.SetRange(-1, -1);
    m_pos = wxPoint(0, 0);
    m_size = wxSize(0, 0);
    m_descent = 0;
#if wxRICHTEXT_USE_OPTIMIZED_LINE_DRAWING
    m_objectSizes.Clear();
#endif
}

/// Copy
void wxRichTextLine::Copy(const wxRichTextLine& obj)
{
    m_range = obj.m_range;
#if wxRICHTEXT_USE_OPTIMIZED_LINE_DRAWING
    m_objectSizes = obj.m_objectSizes;
#endif
}

/// Get the absolute object position
wxPoint wxRichTextLine::GetAbsolutePosition() const
{
    return m_parent->GetPosition() + m_pos;
}

/// Get the absolute range
wxRichTextRange wxRichTextLine::GetAbsoluteRange() const
{
    wxRichTextRange range(m_range.GetStart() + m_parent->GetRange().GetStart(), 0);
    range.SetEnd(range.GetStart() + m_range.GetLength()-1);
    return range;
}

/*!
 * wxRichTextPlainText
 * This object represents a single piece of text.
 */

IMPLEMENT_DYNAMIC_CLASS(wxRichTextPlainText, wxRichTextObject)

wxRichTextPlainText::wxRichTextPlainText(const wxString& text, wxRichTextObject* parent, wxTextAttr* style):
    wxRichTextObject(parent)
{
    if (style)
        SetAttributes(*style);

    m_text = text;
}

#define USE_KERNING_FIX 1

// If insufficient tabs are defined, this is the tab width used
#define WIDTH_FOR_DEFAULT_TABS 50

/// Draw the item
bool wxRichTextPlainText::Draw(wxDC& dc, const wxRichTextRange& range, const wxRichTextRange& selectionRange, const wxRect& rect, int descent, int WXUNUSED(style))
{
    wxRichTextParagraph* para = wxDynamicCast(GetParent(), wxRichTextParagraph);
    wxASSERT (para != NULL);

    wxTextAttr textAttr(para ? para->GetCombinedAttributes(GetAttributes()) : GetAttributes());

    int offset = GetRange().GetStart();

    // Replace line break characters with spaces
    wxString str = m_text;
    wxString toRemove = wxRichTextLineBreakChar;
    str.Replace(toRemove, wxT(" "));
    if (textAttr.HasTextEffects() && (textAttr.GetTextEffects() & wxTEXT_ATTR_EFFECT_CAPITALS))
        str.MakeUpper();

    long len = range.GetLength();
    wxString stringChunk = str.Mid(range.GetStart() - offset, (size_t) len);

    // Test for the optimized situations where all is selected, or none
    // is selected.

    wxFont textFont(GetBuffer()->GetFontTable().FindFont(textAttr));
    wxCheckSetFont(dc, textFont);
    int charHeight = dc.GetCharHeight();

    int x, y;
    if ( textFont.Ok() )
    {
        if ( textAttr.HasTextEffects() && (textAttr.GetTextEffects() & wxTEXT_ATTR_EFFECT_SUPERSCRIPT) )
        {
            double size = static_cast<double>(textFont.GetPointSize()) / wxSCRIPT_MUL_FACTOR;
            textFont.SetPointSize( static_cast<int>(size) );
            x = rect.x;
            y = rect.y;
            wxCheckSetFont(dc, textFont);
        }
        else if ( textAttr.HasTextEffects() && (textAttr.GetTextEffects() & wxTEXT_ATTR_EFFECT_SUBSCRIPT) )
        {
            double size = static_cast<double>(textFont.GetPointSize()) / wxSCRIPT_MUL_FACTOR;
            textFont.SetPointSize( static_cast<int>(size) );
            x = rect.x;
            int sub_height = static_cast<int>( static_cast<double>(charHeight) / wxSCRIPT_MUL_FACTOR);
            y = rect.y + (rect.height - sub_height + (descent - m_descent));
            wxCheckSetFont(dc, textFont);
        }
        else
        {
            x = rect.x;
            y = rect.y + (rect.height - charHeight - (descent - m_descent));
        }
    }
    else
    {
        x = rect.x;
        y = rect.y + (rect.height - charHeight - (descent - m_descent));
    }

    // (a) All selected.
    if (selectionRange.GetStart() <= range.GetStart() && selectionRange.GetEnd() >= range.GetEnd())
    {
        DrawTabbedString(dc, textAttr, rect, stringChunk, x, y, true);
    }
    // (b) None selected.
    else if (selectionRange.GetEnd() < range.GetStart() || selectionRange.GetStart() > range.GetEnd())
    {
        // Draw all unselected
        DrawTabbedString(dc, textAttr, rect, stringChunk, x, y, false);
    }
    else
    {
        // (c) Part selected, part not
        // Let's draw unselected chunk, selected chunk, then unselected chunk.

        dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);

        // 1. Initial unselected chunk, if any, up until start of selection.
        if (selectionRange.GetStart() > range.GetStart() && selectionRange.GetStart() <= range.GetEnd())
        {
            int r1 = range.GetStart();
            int s1 = selectionRange.GetStart()-1;
            int fragmentLen = s1 - r1 + 1;
            if (fragmentLen < 0)
            {
                wxLogDebug(wxT("Mid(%d, %d"), (int)(r1 - offset), (int)fragmentLen);
            }
            wxString stringFragment = str.Mid(r1 - offset, fragmentLen);

            DrawTabbedString(dc, textAttr, rect, stringFragment, x, y, false);

#if USE_KERNING_FIX
            if (stringChunk.Find(wxT("\t")) == wxNOT_FOUND)
            {
                // Compensate for kerning difference
                wxString stringFragment2(str.Mid(r1 - offset, fragmentLen+1));
                wxString stringFragment3(str.Mid(r1 - offset + fragmentLen, 1));

                wxCoord w1, h1, w2, h2, w3, h3;
                dc.GetTextExtent(stringFragment,  & w1, & h1);
                dc.GetTextExtent(stringFragment2, & w2, & h2);
                dc.GetTextExtent(stringFragment3, & w3, & h3);

                int kerningDiff = (w1 + w3) - w2;
                x = x - kerningDiff;
            }
#endif
        }

        // 2. Selected chunk, if any.
        if (selectionRange.GetEnd() >= range.GetStart())
        {
            int s1 = wxMax(selectionRange.GetStart(), range.GetStart());
            int s2 = wxMin(selectionRange.GetEnd(), range.GetEnd());

            int fragmentLen = s2 - s1 + 1;
            if (fragmentLen < 0)
            {
                wxLogDebug(wxT("Mid(%d, %d"), (int)(s1 - offset), (int)fragmentLen);
            }
            wxString stringFragment = str.Mid(s1 - offset, fragmentLen);

            DrawTabbedString(dc, textAttr, rect, stringFragment, x, y, true);

#if USE_KERNING_FIX
            if (stringChunk.Find(wxT("\t")) == wxNOT_FOUND)
            {
                // Compensate for kerning difference
                wxString stringFragment2(str.Mid(s1 - offset, fragmentLen+1));
                wxString stringFragment3(str.Mid(s1 - offset + fragmentLen, 1));

                wxCoord w1, h1, w2, h2, w3, h3;
                dc.GetTextExtent(stringFragment,  & w1, & h1);
                dc.GetTextExtent(stringFragment2, & w2, & h2);
                dc.GetTextExtent(stringFragment3, & w3, & h3);

                int kerningDiff = (w1 + w3) - w2;
                x = x - kerningDiff;
            }
#endif
        }

        // 3. Remaining unselected chunk, if any
        if (selectionRange.GetEnd() < range.GetEnd())
        {
            int s2 = wxMin(selectionRange.GetEnd()+1, range.GetEnd());
            int r2 = range.GetEnd();

            int fragmentLen = r2 - s2 + 1;
            if (fragmentLen < 0)
            {
                wxLogDebug(wxT("Mid(%d, %d"), (int)(s2 - offset), (int)fragmentLen);
            }
            wxString stringFragment = str.Mid(s2 - offset, fragmentLen);

            DrawTabbedString(dc, textAttr, rect, stringFragment, x, y, false);
        }
    }

    return true;
}

bool wxRichTextPlainText::DrawTabbedString(wxDC& dc, const wxTextAttr& attr, const wxRect& rect,wxString& str, wxCoord& x, wxCoord& y, bool selected)
{
    bool hasTabs = (str.Find(wxT('\t')) != wxNOT_FOUND);

    wxArrayInt tabArray;
    int tabCount;
    if (hasTabs)
    {
        if (attr.GetTabs().IsEmpty())
            tabArray = wxRichTextParagraph::GetDefaultTabs();
        else
            tabArray = attr.GetTabs();
        tabCount = tabArray.GetCount();

        for (int i = 0; i < tabCount; ++i)
        {
            int pos = tabArray[i];
            pos = ConvertTenthsMMToPixels(dc, pos);
            tabArray[i] = pos;
        }
    }
    else
        tabCount = 0;

    int nextTabPos = -1;
    int tabPos = -1;
    wxCoord w, h;

    if (selected)
    {
        wxColour highlightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
        wxColour highlightTextColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));

        wxCheckSetBrush(dc, wxBrush(highlightColour));
        wxCheckSetPen(dc, wxPen(highlightColour));
        dc.SetTextForeground(highlightTextColour);
        dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
    }
    else
    {
        dc.SetTextForeground(attr.GetTextColour());

        if (attr.HasFlag(wxTEXT_ATTR_BACKGROUND_COLOUR) && attr.GetBackgroundColour().IsOk())
        {
            dc.SetBackgroundMode(wxBRUSHSTYLE_SOLID);
            dc.SetTextBackground(attr.GetBackgroundColour());
        }
        else
            dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
    }

    wxCoord x_orig = x;
    while (hasTabs)
    {
        // the string has a tab
        // break up the string at the Tab
        wxString stringChunk = str.BeforeFirst(wxT('\t'));
        str = str.AfterFirst(wxT('\t'));
        dc.GetTextExtent(stringChunk, & w, & h);
        tabPos = x + w;
        bool not_found = true;
        for (int i = 0; i < tabCount && not_found; ++i)
        {
            nextTabPos = tabArray.Item(i) + x_orig;

            // Find the next tab position.
            // Even if we're at the end of the tab array, we must still draw the chunk.

            if (nextTabPos > tabPos || (i == (tabCount - 1)))
            {
                if (nextTabPos <= tabPos)
                {
                    int defaultTabWidth = ConvertTenthsMMToPixels(dc, WIDTH_FOR_DEFAULT_TABS);
                    nextTabPos = tabPos + defaultTabWidth;
                }

                not_found = false;
                if (selected)
                {
                    w = nextTabPos - x;
                    wxRect selRect(x, rect.y, w, rect.GetHeight());
                    dc.DrawRectangle(selRect);
                }
                dc.DrawText(stringChunk, x, y);

                if (attr.HasTextEffects() && (attr.GetTextEffects() & wxTEXT_ATTR_EFFECT_STRIKETHROUGH))
                {
                    wxPen oldPen = dc.GetPen();
                    wxCheckSetPen(dc, wxPen(attr.GetTextColour(), 1));
                    dc.DrawLine(x, (int) (y+(h/2)+0.5), x+w, (int) (y+(h/2)+0.5));
                    wxCheckSetPen(dc, oldPen);
                }

                x = nextTabPos;
            }
        }
        hasTabs = (str.Find(wxT('\t')) != wxNOT_FOUND);
    }

    if (!str.IsEmpty())
    {
        dc.GetTextExtent(str, & w, & h);
        if (selected)
        {
            wxRect selRect(x, rect.y, w, rect.GetHeight());
            dc.DrawRectangle(selRect);
        }
        dc.DrawText(str, x, y);

        if (attr.HasTextEffects() && (attr.GetTextEffects() & wxTEXT_ATTR_EFFECT_STRIKETHROUGH))
        {
            wxPen oldPen = dc.GetPen();
            wxCheckSetPen(dc, wxPen(attr.GetTextColour(), 1));
            dc.DrawLine(x, (int) (y+(h/2)+0.5), x+w, (int) (y+(h/2)+0.5));
            wxCheckSetPen(dc, oldPen);
        }

        x += w;
    }
    return true;

}

/// Lay the item out
bool wxRichTextPlainText::Layout(wxDC& dc, const wxRect& WXUNUSED(rect), int WXUNUSED(style))
{
    // Only lay out if we haven't already cached the size
    if (m_size.x == -1)
        GetRangeSize(GetRange(), m_size, m_descent, dc, 0, wxPoint(0, 0));

    return true;
}

/// Copy
void wxRichTextPlainText::Copy(const wxRichTextPlainText& obj)
{
    wxRichTextObject::Copy(obj);

    m_text = obj.m_text;
}

/// Get/set the object size for the given range. Returns false if the range
/// is invalid for this object.
bool wxRichTextPlainText::GetRangeSize(const wxRichTextRange& range, wxSize& size, int& descent, wxDC& dc, int WXUNUSED(flags), wxPoint position, wxArrayInt* partialExtents) const
{
    if (!range.IsWithin(GetRange()))
        return false;

    wxRichTextParagraph* para = wxDynamicCast(GetParent(), wxRichTextParagraph);
    wxASSERT (para != NULL);

    wxTextAttr textAttr(para ? para->GetCombinedAttributes(GetAttributes()) : GetAttributes());

    // Always assume unformatted text, since at this level we have no knowledge
    // of line breaks - and we don't need it, since we'll calculate size within
    // formatted text by doing it in chunks according to the line ranges

    bool bScript(false);
    wxFont font(GetBuffer()->GetFontTable().FindFont(textAttr));
    if (font.Ok())
    {
        if ( textAttr.HasTextEffects() && ( (textAttr.GetTextEffects() & wxTEXT_ATTR_EFFECT_SUPERSCRIPT)
            || (textAttr.GetTextEffects() & wxTEXT_ATTR_EFFECT_SUBSCRIPT) ) )
        {
            wxFont textFont = font;
            double size = static_cast<double>(textFont.GetPointSize()) / wxSCRIPT_MUL_FACTOR;
            textFont.SetPointSize( static_cast<int>(size) );
            wxCheckSetFont(dc, textFont);
            bScript = true;
        }
        else
        {
            wxCheckSetFont(dc, font);
        }
    }

    bool haveDescent = false;
    int startPos = range.GetStart() - GetRange().GetStart();
    long len = range.GetLength();

    wxString str(m_text);
    wxString toReplace = wxRichTextLineBreakChar;
    str.Replace(toReplace, wxT(" "));

    wxString stringChunk = str.Mid(startPos, (size_t) len);

    if (textAttr.HasTextEffects() && (textAttr.GetTextEffects() & wxTEXT_ATTR_EFFECT_CAPITALS))
        stringChunk.MakeUpper();

    wxCoord w, h;
    int width = 0;
    if (stringChunk.Find(wxT('\t')) != wxNOT_FOUND)
    {
        // the string has a tab
        wxArrayInt tabArray;
        if (textAttr.GetTabs().IsEmpty())
            tabArray = wxRichTextParagraph::GetDefaultTabs();
        else
            tabArray = textAttr.GetTabs();

        int tabCount = tabArray.GetCount();

        for (int i = 0; i < tabCount; ++i)
        {
            int pos = tabArray[i];
            pos = ((wxRichTextPlainText*) this)->ConvertTenthsMMToPixels(dc, pos);
            tabArray[i] = pos;
        }

        int nextTabPos = -1;

        while (stringChunk.Find(wxT('\t')) >= 0)
        {
            int absoluteWidth = 0;

            // the string has a tab
            // break up the string at the Tab
            wxString stringFragment = stringChunk.BeforeFirst(wxT('\t'));
            stringChunk = stringChunk.AfterFirst(wxT('\t'));

            if (partialExtents)
            {
                int oldWidth;
                if (partialExtents->GetCount() > 0)
                    oldWidth = (*partialExtents)[partialExtents->GetCount()-1];
                else
                    oldWidth = 0;

                // Add these partial extents
                wxArrayInt p;
                dc.GetPartialTextExtents(stringFragment, p);
                size_t j;
                for (j = 0; j < p.GetCount(); j++)
                    partialExtents->Add(oldWidth + p[j]);

                if (partialExtents->GetCount() > 0)
                    absoluteWidth = (*partialExtents)[(*partialExtents).GetCount()-1] + position.x;
                else
                    absoluteWidth = position.x;
            }
            else
            {
                dc.GetTextExtent(stringFragment, & w, & h);
                width += w;
                absoluteWidth = width + position.x;
                haveDescent = true;
            }

            bool notFound = true;
            for (int i = 0; i < tabCount && notFound; ++i)
            {
                nextTabPos = tabArray.Item(i);

                // Find the next tab position.
                // Even if we're at the end of the tab array, we must still process the chunk.

                if (nextTabPos > absoluteWidth || (i == (tabCount - 1)))
                {
                    if (nextTabPos <= absoluteWidth)
                    {
                        int defaultTabWidth = ((wxRichTextPlainText*) this)->ConvertTenthsMMToPixels(dc, WIDTH_FOR_DEFAULT_TABS);
                        nextTabPos = absoluteWidth + defaultTabWidth;
                    }

                    notFound = false;
                    width = nextTabPos - position.x;

                    if (partialExtents)
                        partialExtents->Add(width);
                }
            }
        }
    }

    if (!stringChunk.IsEmpty())
    {
        if (partialExtents)
        {
            int oldWidth;
            if (partialExtents->GetCount() > 0)
                oldWidth = (*partialExtents)[partialExtents->GetCount()-1];
            else
                oldWidth = 0;

            // Add these partial extents
            wxArrayInt p;
            dc.GetPartialTextExtents(stringChunk, p);
            size_t j;
            for (j = 0; j < p.GetCount(); j++)
                partialExtents->Add(oldWidth + p[j]);
        }
        else
        {
            dc.GetTextExtent(stringChunk, & w, & h, & descent);
            width += w;
            haveDescent = true;
        }
    }

    if (partialExtents)
    {
        int charHeight = dc.GetCharHeight();
        if ((*partialExtents).GetCount() > 0)
            w = (*partialExtents)[partialExtents->GetCount()-1];
        else
            w = 0;
        size = wxSize(w, charHeight);
    }
    else
    {
        size = wxSize(width, dc.GetCharHeight());
    }

    if (!haveDescent)
        dc.GetTextExtent(wxT("X"), & w, & h, & descent);

    if ( bScript )
        dc.SetFont(font);

    return true;
}

/// Do a split, returning an object containing the second part, and setting
/// the first part in 'this'.
wxRichTextObject* wxRichTextPlainText::DoSplit(long pos)
{
    long index = pos - GetRange().GetStart();

    if (index < 0 || index >= (int) m_text.length())
        return NULL;

    wxString firstPart = m_text.Mid(0, index);
    wxString secondPart = m_text.Mid(index);

    m_text = firstPart;

    wxRichTextPlainText* newObject = new wxRichTextPlainText(secondPart);
    newObject->SetAttributes(GetAttributes());

    newObject->SetRange(wxRichTextRange(pos, GetRange().GetEnd()));
    GetRange().SetEnd(pos-1);

    return newObject;
}

/// Calculate range
void wxRichTextPlainText::CalculateRange(long start, long& end)
{
    end = start + m_text.length() - 1;
    m_range.SetRange(start, end);
}

/// Delete range
bool wxRichTextPlainText::DeleteRange(const wxRichTextRange& range)
{
    wxRichTextRange r = range;

    r.LimitTo(GetRange());

    if (r.GetStart() == GetRange().GetStart() && r.GetEnd() == GetRange().GetEnd())
    {
        m_text.Empty();
        return true;
    }

    long startIndex = r.GetStart() - GetRange().GetStart();
    long len = r.GetLength();

    m_text = m_text.Mid(0, startIndex) + m_text.Mid(startIndex+len);
    return true;
}

/// Get text for the given range.
wxString wxRichTextPlainText::GetTextForRange(const wxRichTextRange& range) const
{
    wxRichTextRange r = range;

    r.LimitTo(GetRange());

    long startIndex = r.GetStart() - GetRange().GetStart();
    long len = r.GetLength();

    return m_text.Mid(startIndex, len);
}

/// Returns true if this object can merge itself with the given one.
bool wxRichTextPlainText::CanMerge(wxRichTextObject* object) const
{
    return object->GetClassInfo() == CLASSINFO(wxRichTextPlainText) &&
        (m_text.empty() || wxTextAttrEq(GetAttributes(), object->GetAttributes()));
}

/// Returns true if this object merged itself with the given one.
/// The calling code will then delete the given object.
bool wxRichTextPlainText::Merge(wxRichTextObject* object)
{
    wxRichTextPlainText* textObject = wxDynamicCast(object, wxRichTextPlainText);
    wxASSERT( textObject != NULL );

    if (textObject)
    {
        m_text += textObject->GetText();
        wxRichTextApplyStyle(m_attributes, textObject->GetAttributes());
        return true;
    }
    else
        return false;
}

/// Dump to output stream for debugging
void wxRichTextPlainText::Dump(wxTextOutputStream& stream)
{
    wxRichTextObject::Dump(stream);
    stream << m_text << wxT("\n");
}

/// Get the first position from pos that has a line break character.
long wxRichTextPlainText::GetFirstLineBreakPosition(long pos)
{
    int i;
    int len = m_text.length();
    int startPos = pos - m_range.GetStart();
    for (i = startPos; i < len; i++)
    {
        wxChar ch = m_text[i];
        if (ch == wxRichTextLineBreakChar)
        {
            return i + m_range.GetStart();
        }
    }
    return -1;
}

/*!
 * wxRichTextBuffer
 * This is a kind of box, used to represent the whole buffer
 */

IMPLEMENT_DYNAMIC_CLASS(wxRichTextBuffer, wxRichTextParagraphLayoutBox)

wxList                  wxRichTextBuffer::sm_handlers;
wxRichTextRenderer*     wxRichTextBuffer::sm_renderer = NULL;
int                     wxRichTextBuffer::sm_bulletRightMargin = 20;
float                   wxRichTextBuffer::sm_bulletProportion = (float) 0.3;

/// Initialisation
void wxRichTextBuffer::Init()
{
    m_commandProcessor = new wxCommandProcessor;
    m_styleSheet = NULL;
    m_modified = false;
    m_batchedCommandDepth = 0;
    m_batchedCommand = NULL;
    m_suppressUndo = 0;
    m_handlerFlags = 0;
    m_scale = 1.0;
}

/// Initialisation
wxRichTextBuffer::~wxRichTextBuffer()
{
    delete m_commandProcessor;
    delete m_batchedCommand;

    ClearStyleStack();
    ClearEventHandlers();
}

void wxRichTextBuffer::ResetAndClearCommands()
{
    Reset();

    GetCommandProcessor()->ClearCommands();

    Modify(false);
    Invalidate(wxRICHTEXT_ALL);
}

void wxRichTextBuffer::Copy(const wxRichTextBuffer& obj)
{
    wxRichTextParagraphLayoutBox::Copy(obj);

    m_styleSheet = obj.m_styleSheet;
    m_modified = obj.m_modified;
    m_batchedCommandDepth = obj.m_batchedCommandDepth;
    m_batchedCommand = obj.m_batchedCommand;
    m_suppressUndo = obj.m_suppressUndo;
}

/// Push style sheet to top of stack
bool wxRichTextBuffer::PushStyleSheet(wxRichTextStyleSheet* styleSheet)
{
    if (m_styleSheet)
        styleSheet->InsertSheet(m_styleSheet);

    SetStyleSheet(styleSheet);

    return true;
}

/// Pop style sheet from top of stack
wxRichTextStyleSheet* wxRichTextBuffer::PopStyleSheet()
{
    if (m_styleSheet)
    {
        wxRichTextStyleSheet* oldSheet = m_styleSheet;
        m_styleSheet = oldSheet->GetNextSheet();
        oldSheet->Unlink();

        return oldSheet;
    }
    else
        return NULL;
}

/// Submit command to insert paragraphs
bool wxRichTextBuffer::InsertParagraphsWithUndo(long pos, const wxRichTextParagraphLayoutBox& paragraphs, wxRichTextCtrl* ctrl, int flags)
{
    wxRichTextAction* action = new wxRichTextAction(NULL, _("Insert Text"), wxRICHTEXT_INSERT, this, ctrl, false);

    wxTextAttr attr(GetDefaultStyle());

    wxTextAttr* p = NULL;
    wxTextAttr paraAttr;
    if (flags & wxRICHTEXT_INSERT_WITH_PREVIOUS_PARAGRAPH_STYLE)
    {
        paraAttr = GetStyleForNewParagraph(pos);
        if (!paraAttr.IsDefault())
            p = & paraAttr;
    }
    else
        p = & attr;

    action->GetNewParagraphs() = paragraphs;

    if (p && !p->IsDefault())
    {
        for (wxRichTextObjectList::compatibility_iterator node = action->GetNewParagraphs().GetChildren().GetFirst(); node; node = node->GetNext())
        {
            wxRichTextObject* child = node->GetData();
            child->SetAttributes(*p);
        }
    }

    action->SetPosition(pos);

    wxRichTextRange range = wxRichTextRange(pos, pos + paragraphs.GetRange().GetEnd() - 1);
    if (!paragraphs.GetPartialParagraph())
        range.SetEnd(range.GetEnd()+1);

    // Set the range we'll need to delete in Undo
    action->SetRange(range);

    SubmitAction(action);

    return true;
}

/// Submit command to insert the given text
bool wxRichTextBuffer::InsertTextWithUndo(long pos, const wxString& text, wxRichTextCtrl* ctrl, int flags)
{
    wxRichTextAction* action = new wxRichTextAction(NULL, _("Insert Text"), wxRICHTEXT_INSERT, this, ctrl, false);

    wxTextAttr* p = NULL;
    wxTextAttr paraAttr;
    if (flags & wxRICHTEXT_INSERT_WITH_PREVIOUS_PARAGRAPH_STYLE)
    {
        // Get appropriate paragraph style
        paraAttr = GetStyleForNewParagraph(pos, false, false);
        if (!paraAttr.IsDefault())
            p = & paraAttr;
    }

    action->GetNewParagraphs().AddParagraphs(text, p);

    int length = action->GetNewParagraphs().GetRange().GetLength();

    if (text.length() > 0 && text.Last() != wxT('\n'))
    {
        // Don't count the newline when undoing
        length --;
        action->GetNewParagraphs().SetPartialParagraph(true);
    }
    else if (text.length() > 0 && text.Last() == wxT('\n'))
        length --;

    action->SetPosition(pos);

    // Set the range we'll need to delete in Undo
    action->SetRange(wxRichTextRange(pos, pos + length - 1));

    SubmitAction(action);

    return true;
}

/// Submit command to insert the given text
bool wxRichTextBuffer::InsertNewlineWithUndo(long pos, wxRichTextCtrl* ctrl, int flags)
{
    wxRichTextAction* action = new wxRichTextAction(NULL, _("Insert Text"), wxRICHTEXT_INSERT, this, ctrl, false);

    wxTextAttr* p = NULL;
    wxTextAttr paraAttr;
    if (flags & wxRICHTEXT_INSERT_WITH_PREVIOUS_PARAGRAPH_STYLE)
    {
        paraAttr = GetStyleForNewParagraph(pos, false, true /* look for next paragraph style */);
        if (!paraAttr.IsDefault())
            p = & paraAttr;
    }

    wxTextAttr attr(GetDefaultStyle());

    wxRichTextParagraph* newPara = new wxRichTextParagraph(wxEmptyString, this, & attr);
    action->GetNewParagraphs().AppendChild(newPara);
    action->GetNewParagraphs().UpdateRanges();
    action->GetNewParagraphs().SetPartialParagraph(false);
    wxRichTextParagraph* para = GetParagraphAtPosition(pos, false);
    long pos1 = pos;

    if (p)
        newPara->SetAttributes(*p);

    if (flags & wxRICHTEXT_INSERT_INTERACTIVE)
    {
        if (para && para->GetRange().GetEnd() == pos)
            pos1 ++;

        // Now see if we need to number the paragraph.
        if (newPara->GetAttributes().HasBulletNumber())
        {
            wxRichTextAttr numberingAttr;
            if (FindNextParagraphNumber(para, numberingAttr))
                wxRichTextApplyStyle(newPara->GetAttributes(), (const wxRichTextAttr&) numberingAttr);
        }
    }

    action->SetPosition(pos);

    // Use the default character style
    // Use the default character style
    if (!GetDefaultStyle().IsDefault() && newPara->GetChildren().GetFirst())
    {
        // Check whether the default style merely reflects the paragraph/basic style,
        // in which case don't apply it.
        wxTextAttrEx defaultStyle(GetDefaultStyle());
        wxTextAttrEx toApply;
        if (para)
        {
            wxRichTextAttr combinedAttr = para->GetCombinedAttributes();
            wxTextAttrEx newAttr;
            // This filters out attributes that are accounted for by the current
            // paragraph/basic style
            wxRichTextApplyStyle(toApply, defaultStyle, & combinedAttr);
        }
        else
            toApply = defaultStyle;

        if (!toApply.IsDefault())
            newPara->GetChildren().GetFirst()->GetData()->SetAttributes(toApply);
    }

    // Set the range we'll need to delete in Undo
    action->SetRange(wxRichTextRange(pos1, pos1));

    SubmitAction(action);

    return true;
}

/// Submit command to insert the given image
bool wxRichTextBuffer::InsertImageWithUndo(long pos, const wxRichTextImageBlock& imageBlock, wxRichTextCtrl* ctrl, int flags, const wxRichTextAnchoredObjectAttr& imgAttr)
{
    wxRichTextAction* action = new wxRichTextAction(NULL, _("Insert Image"), wxRICHTEXT_INSERT, this, ctrl, false);

    wxTextAttr* p = NULL;
    wxTextAttr paraAttr;
    if (flags & wxRICHTEXT_INSERT_WITH_PREVIOUS_PARAGRAPH_STYLE)
    {
        paraAttr = GetStyleForNewParagraph(pos);
        if (!paraAttr.IsDefault())
            p = & paraAttr;
    }

    wxTextAttr attr(GetDefaultStyle());

    wxRichTextParagraph* newPara = new wxRichTextParagraph(this, & attr);
    if (p)
        newPara->SetAttributes(*p);

    wxRichTextImage* imageObject = new wxRichTextImage(imageBlock, newPara, NULL, imgAttr);
    newPara->AppendChild(imageObject);
    action->GetNewParagraphs().AppendChild(newPara);
    action->GetNewParagraphs().UpdateRanges();

    action->GetNewParagraphs().SetPartialParagraph(true);

    action->SetPosition(pos);

    // Set the range we'll need to delete in Undo
    action->SetRange(wxRichTextRange(pos, pos));

    SubmitAction(action);

    return true;
}

/// Get the style that is appropriate for a new paragraph at this position.
/// If the previous paragraph has a paragraph style name, look up the next-paragraph
/// style.
wxTextAttr wxRichTextBuffer::GetStyleForNewParagraph(long pos, bool caretPosition, bool lookUpNewParaStyle) const
{
    wxRichTextParagraph* para = GetParagraphAtPosition(pos, caretPosition);
    if (para)
    {
        wxTextAttr attr;
        bool foundAttributes = false;

        // Look for a matching paragraph style
        if (lookUpNewParaStyle && !para->GetAttributes().GetParagraphStyleName().IsEmpty() && GetStyleSheet())
        {
            wxRichTextParagraphStyleDefinition* paraDef = GetStyleSheet()->FindParagraphStyle(para->GetAttributes().GetParagraphStyleName());
            if (paraDef)
            {
                // If we're not at the end of the paragraph, then we apply THIS style, and not the designated next style.
                if (para->GetRange().GetEnd() == pos && !paraDef->GetNextStyle().IsEmpty())
                {
                    wxRichTextParagraphStyleDefinition* nextParaDef = GetStyleSheet()->FindParagraphStyle(paraDef->GetNextStyle());
                    if (nextParaDef)
                    {
                        foundAttributes = true;
                        attr = nextParaDef->GetStyleMergedWithBase(GetStyleSheet());
                    }
                }

                // If we didn't find the 'next style', use this style instead.
                if (!foundAttributes)
                {
                    foundAttributes = true;
                    attr = paraDef->GetStyleMergedWithBase(GetStyleSheet());
                }
            }
        }

        // Also apply list style if present
        if (lookUpNewParaStyle && !para->GetAttributes().GetListStyleName().IsEmpty() && GetStyleSheet())
        {
            wxRichTextListStyleDefinition* listDef = GetStyleSheet()->FindListStyle(para->GetAttributes().GetListStyleName());
            if (listDef)
            {
                int thisIndent = para->GetAttributes().GetLeftIndent();
                int thisLevel = para->GetAttributes().HasOutlineLevel() ? para->GetAttributes().GetOutlineLevel() : listDef->FindLevelForIndent(thisIndent);

                // Apply the overall list style, and item style for this level
                wxRichTextAttr listStyle(listDef->GetCombinedStyleForLevel(thisLevel, GetStyleSheet()));
                wxRichTextApplyStyle(attr, listStyle);
                attr.SetOutlineLevel(thisLevel);
                if (para->GetAttributes().HasBulletNumber())
                    attr.SetBulletNumber(para->GetAttributes().GetBulletNumber());
            }
        }

        if (!foundAttributes)
        {
            attr = para->GetAttributes();
            int flags = attr.GetFlags();

            // Eliminate character styles
            flags &= ( (~ wxTEXT_ATTR_FONT) |
                    (~ wxTEXT_ATTR_TEXT_COLOUR) |
                    (~ wxTEXT_ATTR_BACKGROUND_COLOUR) );
            attr.SetFlags(flags);
        }

        return attr;
    }
    else
        return wxTextAttr();
}

/// Submit command to delete this range
bool wxRichTextBuffer::DeleteRangeWithUndo(const wxRichTextRange& range, wxRichTextCtrl* ctrl)
{
    wxRichTextAction* action = new wxRichTextAction(NULL, _("Delete"), wxRICHTEXT_DELETE, this, ctrl);

    action->SetPosition(ctrl->GetCaretPosition());

    // Set the range to delete
    action->SetRange(range);

    // Copy the fragment that we'll need to restore in Undo
    CopyFragment(range, action->GetOldParagraphs());

    // See if we're deleting a paragraph marker, in which case we need to
    // make a note not to copy the attributes from the 2nd paragraph to the 1st.
    if (range.GetStart() == range.GetEnd())
    {
        wxRichTextParagraph* para = GetParagraphAtPosition(range.GetStart());
        if (para && para->GetRange().GetEnd() == range.GetEnd())
        {
            wxRichTextParagraph* nextPara = GetParagraphAtPosition(range.GetStart()+1);
            if (nextPara && nextPara != para)
            {
                action->GetOldParagraphs().GetChildren().GetFirst()->GetData()->SetAttributes(nextPara->GetAttributes());
                action->GetOldParagraphs().GetAttributes().SetFlags(action->GetOldParagraphs().GetAttributes().GetFlags() | wxTEXT_ATTR_KEEP_FIRST_PARA_STYLE);
            }
        }
    }

    SubmitAction(action);

    return true;
}

/// Collapse undo/redo commands
bool wxRichTextBuffer::BeginBatchUndo(const wxString& cmdName)
{
    if (m_batchedCommandDepth == 0)
    {
        wxASSERT(m_batchedCommand == NULL);
        if (m_batchedCommand)
        {
            GetCommandProcessor()->Store(m_batchedCommand);
        }
        m_batchedCommand = new wxRichTextCommand(cmdName);
    }

    m_batchedCommandDepth ++;

    return true;
}

/// Collapse undo/redo commands
bool wxRichTextBuffer::EndBatchUndo()
{
    m_batchedCommandDepth --;

    wxASSERT(m_batchedCommandDepth >= 0);
    wxASSERT(m_batchedCommand != NULL);

    if (m_batchedCommandDepth == 0)
    {
        GetCommandProcessor()->Store(m_batchedCommand);
        m_batchedCommand = NULL;
    }

    return true;
}

/// Submit immediately, or delay according to whether collapsing is on
bool wxRichTextBuffer::SubmitAction(wxRichTextAction* action)
{
    if (BatchingUndo() && m_batchedCommand && !SuppressingUndo())
    {
        wxRichTextCommand* cmd = new wxRichTextCommand(action->GetName());
        cmd->AddAction(action);
        cmd->Do();
        cmd->GetActions().Clear();
        delete cmd;

        m_batchedCommand->AddAction(action);
    }
    else
    {
        wxRichTextCommand* cmd = new wxRichTextCommand(action->GetName());
        cmd->AddAction(action);

        // Only store it if we're not suppressing undo.
        return GetCommandProcessor()->Submit(cmd, !SuppressingUndo());
    }

    return true;
}

/// Begin suppressing undo/redo commands.
bool wxRichTextBuffer::BeginSuppressUndo()
{
    m_suppressUndo ++;

    return true;
}

/// End suppressing undo/redo commands.
bool wxRichTextBuffer::EndSuppressUndo()
{
    m_suppressUndo --;

    return true;
}

/// Begin using a style
bool wxRichTextBuffer::BeginStyle(const wxTextAttr& style)
{
    wxTextAttr newStyle(GetDefaultStyle());

    // Save the old default style
    m_attributeStack.Append((wxObject*) new wxTextAttr(GetDefaultStyle()));

    wxRichTextApplyStyle(newStyle, style);
    newStyle.SetFlags(style.GetFlags()|newStyle.GetFlags());

    SetDefaultStyle(newStyle);

    return true;
}

/// End the style
bool wxRichTextBuffer::EndStyle()
{
    if (!m_attributeStack.GetFirst())
    {
        wxLogDebug(_("Too many EndStyle calls!"));
        return false;
    }

    wxList::compatibility_iterator node = m_attributeStack.GetLast();
    wxTextAttr* attr = (wxTextAttr*)node->GetData();
    m_attributeStack.Erase(node);

    SetDefaultStyle(*attr);

    delete attr;
    return true;
}

/// End all styles
bool wxRichTextBuffer::EndAllStyles()
{
    while (m_attributeStack.GetCount() != 0)
        EndStyle();
    return true;
}

/// Clear the style stack
void wxRichTextBuffer::ClearStyleStack()
{
    for (wxList::compatibility_iterator node = m_attributeStack.GetFirst(); node; node = node->GetNext())
        delete (wxTextAttr*) node->GetData();
    m_attributeStack.Clear();
}

/// Begin using bold
bool wxRichTextBuffer::BeginBold()
{
    wxTextAttr attr;
    attr.SetFontWeight(wxFONTWEIGHT_BOLD);

    return BeginStyle(attr);
}

/// Begin using italic
bool wxRichTextBuffer::BeginItalic()
{
    wxTextAttr attr;
    attr.SetFontStyle(wxFONTSTYLE_ITALIC);

    return BeginStyle(attr);
}

/// Begin using underline
bool wxRichTextBuffer::BeginUnderline()
{
    wxTextAttr attr;
    attr.SetFontUnderlined(true);

    return BeginStyle(attr);
}

/// Begin using point size
bool wxRichTextBuffer::BeginFontSize(int pointSize)
{
    wxTextAttr attr;
    attr.SetFontSize(pointSize);

    return BeginStyle(attr);
}

/// Begin using this font
bool wxRichTextBuffer::BeginFont(const wxFont& font)
{
    wxTextAttr attr;
    attr.SetFont(font);

    return BeginStyle(attr);
}

/// Begin using this colour
bool wxRichTextBuffer::BeginTextColour(const wxColour& colour)
{
    wxTextAttr attr;
    attr.SetFlags(wxTEXT_ATTR_TEXT_COLOUR);
    attr.SetTextColour(colour);

    return BeginStyle(attr);
}

/// Begin using alignment
bool wxRichTextBuffer::BeginAlignment(wxTextAttrAlignment alignment)
{
    wxTextAttr attr;
    attr.SetFlags(wxTEXT_ATTR_ALIGNMENT);
    attr.SetAlignment(alignment);

    return BeginStyle(attr);
}

/// Begin left indent
bool wxRichTextBuffer::BeginLeftIndent(int leftIndent, int leftSubIndent)
{
    wxTextAttr attr;
    attr.SetFlags(wxTEXT_ATTR_LEFT_INDENT);
    attr.SetLeftIndent(leftIndent, leftSubIndent);

    return BeginStyle(attr);
}

/// Begin right indent
bool wxRichTextBuffer::BeginRightIndent(int rightIndent)
{
    wxTextAttr attr;
    attr.SetFlags(wxTEXT_ATTR_RIGHT_INDENT);
    attr.SetRightIndent(rightIndent);

    return BeginStyle(attr);
}

/// Begin paragraph spacing
bool wxRichTextBuffer::BeginParagraphSpacing(int before, int after)
{
    long flags = 0;
    if (before != 0)
        flags |= wxTEXT_ATTR_PARA_SPACING_BEFORE;
    if (after != 0)
        flags |= wxTEXT_ATTR_PARA_SPACING_AFTER;

    wxTextAttr attr;
    attr.SetFlags(flags);
    attr.SetParagraphSpacingBefore(before);
    attr.SetParagraphSpacingAfter(after);

    return BeginStyle(attr);
}

/// Begin line spacing
bool wxRichTextBuffer::BeginLineSpacing(int lineSpacing)
{
    wxTextAttr attr;
    attr.SetFlags(wxTEXT_ATTR_LINE_SPACING);
    attr.SetLineSpacing(lineSpacing);

    return BeginStyle(attr);
}

/// Begin numbered bullet
bool wxRichTextBuffer::BeginNumberedBullet(int bulletNumber, int leftIndent, int leftSubIndent, int bulletStyle)
{
    wxTextAttr attr;
    attr.SetFlags(wxTEXT_ATTR_BULLET_STYLE|wxTEXT_ATTR_LEFT_INDENT);
    attr.SetBulletStyle(bulletStyle);
    attr.SetBulletNumber(bulletNumber);
    attr.SetLeftIndent(leftIndent, leftSubIndent);

    return BeginStyle(attr);
}

/// Begin symbol bullet
bool wxRichTextBuffer::BeginSymbolBullet(const wxString& symbol, int leftIndent, int leftSubIndent, int bulletStyle)
{
    wxTextAttr attr;
    attr.SetFlags(wxTEXT_ATTR_BULLET_STYLE|wxTEXT_ATTR_LEFT_INDENT);
    attr.SetBulletStyle(bulletStyle);
    attr.SetLeftIndent(leftIndent, leftSubIndent);
    attr.SetBulletText(symbol);

    return BeginStyle(attr);
}

/// Begin standard bullet
bool wxRichTextBuffer::BeginStandardBullet(const wxString& bulletName, int leftIndent, int leftSubIndent, int bulletStyle)
{
    wxTextAttr attr;
    attr.SetFlags(wxTEXT_ATTR_BULLET_STYLE|wxTEXT_ATTR_LEFT_INDENT);
    attr.SetBulletStyle(bulletStyle);
    attr.SetLeftIndent(leftIndent, leftSubIndent);
    attr.SetBulletName(bulletName);

    return BeginStyle(attr);
}

/// Begin named character style
bool wxRichTextBuffer::BeginCharacterStyle(const wxString& characterStyle)
{
    if (GetStyleSheet())
    {
        wxRichTextCharacterStyleDefinition* def = GetStyleSheet()->FindCharacterStyle(characterStyle);
        if (def)
        {
            wxTextAttr attr = def->GetStyleMergedWithBase(GetStyleSheet());
            return BeginStyle(attr);
        }
    }
    return false;
}

/// Begin named paragraph style
bool wxRichTextBuffer::BeginParagraphStyle(const wxString& paragraphStyle)
{
    if (GetStyleSheet())
    {
        wxRichTextParagraphStyleDefinition* def = GetStyleSheet()->FindParagraphStyle(paragraphStyle);
        if (def)
        {
            wxTextAttr attr = def->GetStyleMergedWithBase(GetStyleSheet());
            return BeginStyle(attr);
        }
    }
    return false;
}

/// Begin named list style
bool wxRichTextBuffer::BeginListStyle(const wxString& listStyle, int level, int number)
{
    if (GetStyleSheet())
    {
        wxRichTextListStyleDefinition* def = GetStyleSheet()->FindListStyle(listStyle);
        if (def)
        {
            wxTextAttr attr(def->GetCombinedStyleForLevel(level));

            attr.SetBulletNumber(number);

            return BeginStyle(attr);
        }
    }
    return false;
}

/// Begin URL
bool wxRichTextBuffer::BeginURL(const wxString& url, const wxString& characterStyle)
{
    wxTextAttr attr;

    if (!characterStyle.IsEmpty() && GetStyleSheet())
    {
        wxRichTextCharacterStyleDefinition* def = GetStyleSheet()->FindCharacterStyle(characterStyle);
        if (def)
        {
            attr = def->GetStyleMergedWithBase(GetStyleSheet());
        }
    }
    attr.SetURL(url);

    return BeginStyle(attr);
}

/// Adds a handler to the end
void wxRichTextBuffer::AddHandler(wxRichTextFileHandler *handler)
{
    sm_handlers.Append(handler);
}

/// Inserts a handler at the front
void wxRichTextBuffer::InsertHandler(wxRichTextFileHandler *handler)
{
    sm_handlers.Insert( handler );
}

/// Removes a handler
bool wxRichTextBuffer::RemoveHandler(const wxString& name)
{
    wxRichTextFileHandler *handler = FindHandler(name);
    if (handler)
    {
        sm_handlers.DeleteObject(handler);
        delete handler;
        return true;
    }
    else
        return false;
}

/// Finds a handler by filename or, if supplied, type
wxRichTextFileHandler *wxRichTextBuffer::FindHandlerFilenameOrType(const wxString& filename,
                                                                   wxRichTextFileType imageType)
{
    if (imageType != wxRICHTEXT_TYPE_ANY)
        return FindHandler(imageType);
    else if (!filename.IsEmpty())
    {
        wxString path, file, ext;
        wxFileName::SplitPath(filename, & path, & file, & ext);
        return FindHandler(ext, imageType);
    }
    else
        return NULL;
}


/// Finds a handler by name
wxRichTextFileHandler* wxRichTextBuffer::FindHandler(const wxString& name)
{
    wxList::compatibility_iterator node = sm_handlers.GetFirst();
    while (node)
    {
        wxRichTextFileHandler *handler = (wxRichTextFileHandler*)node->GetData();
        if (handler->GetName().Lower() == name.Lower()) return handler;

        node = node->GetNext();
    }
    return NULL;
}

/// Finds a handler by extension and type
wxRichTextFileHandler* wxRichTextBuffer::FindHandler(const wxString& extension, wxRichTextFileType type)
{
    wxList::compatibility_iterator node = sm_handlers.GetFirst();
    while (node)
    {
        wxRichTextFileHandler *handler = (wxRichTextFileHandler*)node->GetData();
        if ( handler->GetExtension().Lower() == extension.Lower() &&
            (type == wxRICHTEXT_TYPE_ANY || handler->GetType() == type) )
            return handler;
        node = node->GetNext();
    }
    return 0;
}

/// Finds a handler by type
wxRichTextFileHandler* wxRichTextBuffer::FindHandler(wxRichTextFileType type)
{
    wxList::compatibility_iterator node = sm_handlers.GetFirst();
    while (node)
    {
        wxRichTextFileHandler *handler = (wxRichTextFileHandler *)node->GetData();
        if (handler->GetType() == type) return handler;
        node = node->GetNext();
    }
    return NULL;
}

void wxRichTextBuffer::InitStandardHandlers()
{
    if (!FindHandler(wxRICHTEXT_TYPE_TEXT))
        AddHandler(new wxRichTextPlainTextHandler);
}

void wxRichTextBuffer::CleanUpHandlers()
{
    wxList::compatibility_iterator node = sm_handlers.GetFirst();
    while (node)
    {
        wxRichTextFileHandler* handler = (wxRichTextFileHandler*)node->GetData();
        wxList::compatibility_iterator next = node->GetNext();
        delete handler;
        node = next;
    }

    sm_handlers.Clear();
}

wxString wxRichTextBuffer::GetExtWildcard(bool combine, bool save, wxArrayInt* types)
{
    if (types)
        types->Clear();

    wxString wildcard;

    wxList::compatibility_iterator node = GetHandlers().GetFirst();
    int count = 0;
    while (node)
    {
        wxRichTextFileHandler* handler = (wxRichTextFileHandler*) node->GetData();
        if (handler->IsVisible() && ((save && handler->CanSave()) || (!save && handler->CanLoad())))
        {
            if (combine)
            {
                if (count > 0)
                    wildcard += wxT(";");
                wildcard += wxT("*.") + handler->GetExtension();
            }
            else
            {
                if (count > 0)
                    wildcard += wxT("|");
                wildcard += handler->GetName();
                wildcard += wxT(" ");
                wildcard += _("files");
                wildcard += wxT(" (*.");
                wildcard += handler->GetExtension();
                wildcard += wxT(")|*.");
                wildcard += handler->GetExtension();
                if (types)
                    types->Add(handler->GetType());
            }
            count ++;
        }

        node = node->GetNext();
    }

    if (combine)
        wildcard = wxT("(") + wildcard + wxT(")|") + wildcard;
    return wildcard;
}

/// Load a file
bool wxRichTextBuffer::LoadFile(const wxString& filename, wxRichTextFileType type)
{
    wxRichTextFileHandler* handler = FindHandlerFilenameOrType(filename, type);
    if (handler)
    {
        SetDefaultStyle(wxTextAttr());
        handler->SetFlags(GetHandlerFlags());
        bool success = handler->LoadFile(this, filename);
        Invalidate(wxRICHTEXT_ALL);
        return success;
    }
    else
        return false;
}

/// Save a file
bool wxRichTextBuffer::SaveFile(const wxString& filename, wxRichTextFileType type)
{
    wxRichTextFileHandler* handler = FindHandlerFilenameOrType(filename, type);
    if (handler)
    {
        handler->SetFlags(GetHandlerFlags());
        return handler->SaveFile(this, filename);
    }
    else
        return false;
}

/// Load from a stream
bool wxRichTextBuffer::LoadFile(wxInputStream& stream, wxRichTextFileType type)
{
    wxRichTextFileHandler* handler = FindHandler(type);
    if (handler)
    {
        SetDefaultStyle(wxTextAttr());
        handler->SetFlags(GetHandlerFlags());
        bool success = handler->LoadFile(this, stream);
        Invalidate(wxRICHTEXT_ALL);
        return success;
    }
    else
        return false;
}

/// Save to a stream
bool wxRichTextBuffer::SaveFile(wxOutputStream& stream, wxRichTextFileType type)
{
    wxRichTextFileHandler* handler = FindHandler(type);
    if (handler)
    {
        handler->SetFlags(GetHandlerFlags());
        return handler->SaveFile(this, stream);
    }
    else
        return false;
}

/// Copy the range to the clipboard
bool wxRichTextBuffer::CopyToClipboard(const wxRichTextRange& range)
{
    bool success = false;
#if wxUSE_CLIPBOARD && wxUSE_DATAOBJ

    if (!wxTheClipboard->IsOpened() && wxTheClipboard->Open())
    {
        wxTheClipboard->Clear();

        // Add composite object

        wxDataObjectComposite* compositeObject = new wxDataObjectComposite();

        {
            wxString text = GetTextForRange(range);

#ifdef __WXMSW__
            text = wxTextFile::Translate(text, wxTextFileType_Dos);
#endif

            compositeObject->Add(new wxTextDataObject(text), false /* not preferred */);
        }

        // Add rich text buffer data object. This needs the XML handler to be present.

        if (FindHandler(wxRICHTEXT_TYPE_XML))
        {
            wxRichTextBuffer* richTextBuf = new wxRichTextBuffer;
            CopyFragment(range, *richTextBuf);

            compositeObject->Add(new wxRichTextBufferDataObject(richTextBuf), true /* preferred */);
        }

        if (wxTheClipboard->SetData(compositeObject))
            success = true;

        wxTheClipboard->Close();
    }

#else
    wxUnusedVar(range);
#endif
    return success;
}

/// Paste the clipboard content to the buffer
bool wxRichTextBuffer::PasteFromClipboard(long position)
{
    bool success = false;
#if wxUSE_CLIPBOARD && wxUSE_DATAOBJ
    if (CanPasteFromClipboard())
    {
        if (wxTheClipboard->Open())
        {
            if (wxTheClipboard->IsSupported(wxDataFormat(wxRichTextBufferDataObject::GetRichTextBufferFormatId())))
            {
                wxRichTextBufferDataObject data;
                wxTheClipboard->GetData(data);
                wxRichTextBuffer* richTextBuffer = data.GetRichTextBuffer();
                if (richTextBuffer)
                {
                    InsertParagraphsWithUndo(position+1, *richTextBuffer, GetRichTextCtrl(), 0);
                    if (GetRichTextCtrl())
                        GetRichTextCtrl()->ShowPosition(position + richTextBuffer->GetRange().GetEnd());
                    delete richTextBuffer;
                }
            }
            else if (wxTheClipboard->IsSupported(wxDF_TEXT) || wxTheClipboard->IsSupported(wxDF_UNICODETEXT))
            {
                wxTextDataObject data;
                wxTheClipboard->GetData(data);
                wxString text(data.GetText());
#ifdef __WXMSW__
                wxString text2;
                text2.Alloc(text.Length()+1);
                size_t i;
                for (i = 0; i < text.Length(); i++)
                {
                    wxChar ch = text[i];
                    if (ch != wxT('\r'))
                        text2 += ch;
                }
#else
                wxString text2 = text;
#endif
                InsertTextWithUndo(position+1, text2, GetRichTextCtrl(), wxRICHTEXT_INSERT_WITH_PREVIOUS_PARAGRAPH_STYLE);

                if (GetRichTextCtrl())
                    GetRichTextCtrl()->ShowPosition(position + text2.Length());

                success = true;
            }
            else if (wxTheClipboard->IsSupported(wxDF_BITMAP))
            {
                wxBitmapDataObject data;
                wxTheClipboard->GetData(data);
                wxBitmap bitmap(data.GetBitmap());
                wxImage image(bitmap.ConvertToImage());

                wxRichTextAction* action = new wxRichTextAction(NULL, _("Insert Image"), wxRICHTEXT_INSERT, this, GetRichTextCtrl(), false);

                action->GetNewParagraphs().AddImage(image);

                if (action->GetNewParagraphs().GetChildCount() == 1)
                    action->GetNewParagraphs().SetPartialParagraph(true);

                action->SetPosition(position+1);

                // Set the range we'll need to delete in Undo
                action->SetRange(wxRichTextRange(position+1, position+1));

                SubmitAction(action);

                success = true;
            }
            wxTheClipboard->Close();
        }
    }
#else
    wxUnusedVar(position);
#endif
    return success;
}

/// Can we paste from the clipboard?
bool wxRichTextBuffer::CanPasteFromClipboard() const
{
    bool canPaste = false;
#if wxUSE_CLIPBOARD && wxUSE_DATAOBJ
    if (!wxTheClipboard->IsOpened() && wxTheClipboard->Open())
    {
        if (wxTheClipboard->IsSupported(wxDF_TEXT) || wxTheClipboard->IsSupported(wxDF_UNICODETEXT) ||
            wxTheClipboard->IsSupported(wxDataFormat(wxRichTextBufferDataObject::GetRichTextBufferFormatId())) ||
            wxTheClipboard->IsSupported(wxDF_BITMAP))
        {
            canPaste = true;
        }
        wxTheClipboard->Close();
    }
#endif
    return canPaste;
}

/// Dumps contents of buffer for debugging purposes
void wxRichTextBuffer::Dump()
{
    wxString text;
    {
        wxStringOutputStream stream(& text);
        wxTextOutputStream textStream(stream);
        Dump(textStream);
    }

    wxLogDebug(text);
}

/// Add an event handler
bool wxRichTextBuffer::AddEventHandler(wxEvtHandler* handler)
{
    m_eventHandlers.Append(handler);
    return true;
}

/// Remove an event handler
bool wxRichTextBuffer::RemoveEventHandler(wxEvtHandler* handler, bool deleteHandler)
{
    wxList::compatibility_iterator node = m_eventHandlers.Find(handler);
    if (node)
    {
        m_eventHandlers.Erase(node);
        if (deleteHandler)
            delete handler;

        return true;
    }
    else
        return false;
}

/// Clear event handlers
void wxRichTextBuffer::ClearEventHandlers()
{
    m_eventHandlers.Clear();
}

/// Send event to event handlers. If sendToAll is true, will send to all event handlers,
/// otherwise will stop at the first successful one.
bool wxRichTextBuffer::SendEvent(wxEvent& event, bool sendToAll)
{
    bool success = false;
    for (wxList::compatibility_iterator node = m_eventHandlers.GetFirst(); node; node = node->GetNext())
    {
        wxEvtHandler* handler = (wxEvtHandler*) node->GetData();
        if (handler->ProcessEvent(event))
        {
            success = true;
            if (!sendToAll)
                return true;
        }
    }
    return success;
}

/// Set style sheet and notify of the change
bool wxRichTextBuffer::SetStyleSheetAndNotify(wxRichTextStyleSheet* sheet)
{
    wxRichTextStyleSheet* oldSheet = GetStyleSheet();

    wxWindowID id = wxID_ANY;
    if (GetRichTextCtrl())
        id = GetRichTextCtrl()->GetId();

    wxRichTextEvent event(wxEVT_COMMAND_RICHTEXT_STYLESHEET_REPLACING, id);
    event.SetEventObject(GetRichTextCtrl());
    event.SetOldStyleSheet(oldSheet);
    event.SetNewStyleSheet(sheet);
    event.Allow();

    if (SendEvent(event) && !event.IsAllowed())
    {
        if (sheet != oldSheet)
            delete sheet;

        return false;
    }

    if (oldSheet && oldSheet != sheet)
        delete oldSheet;

    SetStyleSheet(sheet);

    event.SetEventType(wxEVT_COMMAND_RICHTEXT_STYLESHEET_REPLACED);
    event.SetOldStyleSheet(NULL);
    event.Allow();

    return SendEvent(event);
}

/// Set renderer, deleting old one
void wxRichTextBuffer::SetRenderer(wxRichTextRenderer* renderer)
{
    if (sm_renderer)
        delete sm_renderer;
    sm_renderer = renderer;
}

bool wxRichTextStdRenderer::DrawStandardBullet(wxRichTextParagraph* paragraph, wxDC& dc, const wxTextAttr& bulletAttr, const wxRect& rect)
{
    if (bulletAttr.GetTextColour().Ok())
    {
        wxCheckSetPen(dc, wxPen(bulletAttr.GetTextColour()));
        wxCheckSetBrush(dc, wxBrush(bulletAttr.GetTextColour()));
    }
    else
    {
        wxCheckSetPen(dc, *wxBLACK_PEN);
        wxCheckSetBrush(dc, *wxBLACK_BRUSH);
    }

    wxFont font;
    if (bulletAttr.HasFont())
    {
        font = paragraph->GetBuffer()->GetFontTable().FindFont(bulletAttr);
    }
    else
        font = (*wxNORMAL_FONT);

    wxCheckSetFont(dc, font);

    int charHeight = dc.GetCharHeight();

    int bulletWidth = (int) (((float) charHeight) * wxRichTextBuffer::GetBulletProportion());
    int bulletHeight = bulletWidth;

    int x = rect.x;

    // Calculate the top position of the character (as opposed to the whole line height)
    int y = rect.y + (rect.height - charHeight);

    // Calculate where the bullet should be positioned
    y = y + (charHeight+1)/2 - (bulletHeight+1)/2;

    // The margin between a bullet and text.
    int margin = paragraph->ConvertTenthsMMToPixels(dc, wxRichTextBuffer::GetBulletRightMargin());

    if (bulletAttr.GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_ALIGN_RIGHT)
        x = rect.x + rect.width - bulletWidth - margin;
    else if (bulletAttr.GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_ALIGN_CENTRE)
        x = x + (rect.width)/2 - bulletWidth/2;

    if (bulletAttr.GetBulletName() == wxT("standard/square"))
    {
        dc.DrawRectangle(x, y, bulletWidth, bulletHeight);
    }
    else if (bulletAttr.GetBulletName() == wxT("standard/diamond"))
    {
        wxPoint pts[5];
        pts[0].x = x;                   pts[0].y = y + bulletHeight/2;
        pts[1].x = x + bulletWidth/2;   pts[1].y = y;
        pts[2].x = x + bulletWidth;     pts[2].y = y + bulletHeight/2;
        pts[3].x = x + bulletWidth/2;   pts[3].y = y + bulletHeight;

        dc.DrawPolygon(4, pts);
    }
    else if (bulletAttr.GetBulletName() == wxT("standard/triangle"))
    {
        wxPoint pts[3];
        pts[0].x = x;                   pts[0].y = y;
        pts[1].x = x + bulletWidth;     pts[1].y = y + bulletHeight/2;
        pts[2].x = x;                   pts[2].y = y + bulletHeight;

        dc.DrawPolygon(3, pts);
    }
    else // "standard/circle", and catch-all
    {
        dc.DrawEllipse(x, y, bulletWidth, bulletHeight);
    }

    return true;
}

bool wxRichTextStdRenderer::DrawTextBullet(wxRichTextParagraph* paragraph, wxDC& dc, const wxTextAttr& attr, const wxRect& rect, const wxString& text)
{
    if (!text.empty())
    {
        wxFont font;
        if ((attr.GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_SYMBOL) && !attr.GetBulletFont().IsEmpty() && attr.HasFont())
        {
            wxTextAttr fontAttr;
            fontAttr.SetFontSize(attr.GetFontSize());
            fontAttr.SetFontStyle(attr.GetFontStyle());
            fontAttr.SetFontWeight(attr.GetFontWeight());
            fontAttr.SetFontUnderlined(attr.GetFontUnderlined());
            fontAttr.SetFontFaceName(attr.GetBulletFont());
            font = paragraph->GetBuffer()->GetFontTable().FindFont(fontAttr);
        }
        else if (attr.HasFont())
            font = paragraph->GetBuffer()->GetFontTable().FindFont(attr);
        else
            font = (*wxNORMAL_FONT);

        wxCheckSetFont(dc, font);

        if (attr.GetTextColour().Ok())
            dc.SetTextForeground(attr.GetTextColour());

        dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);

        int charHeight = dc.GetCharHeight();
        wxCoord tw, th;
        dc.GetTextExtent(text, & tw, & th);

        int x = rect.x;

        // Calculate the top position of the character (as opposed to the whole line height)
        int y = rect.y + (rect.height - charHeight);

        // The margin between a bullet and text.
        int margin = paragraph->ConvertTenthsMMToPixels(dc, wxRichTextBuffer::GetBulletRightMargin());

        if (attr.GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_ALIGN_RIGHT)
            x = (rect.x + rect.width) - tw - margin;
        else if (attr.GetBulletStyle() & wxTEXT_ATTR_BULLET_STYLE_ALIGN_CENTRE)
            x = x + (rect.width)/2 - tw/2;

        dc.DrawText(text, x, y);

        return true;
    }
    else
        return false;
}

bool wxRichTextStdRenderer::DrawBitmapBullet(wxRichTextParagraph* WXUNUSED(paragraph), wxDC& WXUNUSED(dc), const wxTextAttr& WXUNUSED(attr), const wxRect& WXUNUSED(rect))
{
    // Currently unimplemented. The intention is to store bitmaps by name in a media store associated
    // with the buffer. The store will allow retrieval from memory, disk or other means.
    return false;
}

/// Enumerate the standard bullet names currently supported
bool wxRichTextStdRenderer::EnumerateStandardBulletNames(wxArrayString& bulletNames)
{
    bulletNames.Add(wxTRANSLATE("standard/circle"));
    bulletNames.Add(wxTRANSLATE("standard/square"));
    bulletNames.Add(wxTRANSLATE("standard/diamond"));
    bulletNames.Add(wxTRANSLATE("standard/triangle"));

    return true;
}

/*
 * Module to initialise and clean up handlers
 */

class wxRichTextModule: public wxModule
{
DECLARE_DYNAMIC_CLASS(wxRichTextModule)
public:
    wxRichTextModule() {}
    bool OnInit()
    {
        wxRichTextBuffer::SetRenderer(new wxRichTextStdRenderer);
        wxRichTextBuffer::InitStandardHandlers();
        wxRichTextParagraph::InitDefaultTabs();
        return true;
    }
    void OnExit()
    {
        wxRichTextBuffer::CleanUpHandlers();
        wxRichTextDecimalToRoman(-1);
        wxRichTextParagraph::ClearDefaultTabs();
        wxRichTextCtrl::ClearAvailableFontNames();
        wxRichTextBuffer::SetRenderer(NULL);
    }
};

IMPLEMENT_DYNAMIC_CLASS(wxRichTextModule, wxModule)


// If the richtext lib is dynamically loaded after the app has already started
// (such as from wxPython) then the built-in module system will not init this
// module.  Provide this function to do it manually.
void wxRichTextModuleInit()
{
    wxModule* module = new wxRichTextModule;
    module->Init();
    wxModule::RegisterModule(module);
}


/*!
 * Commands for undo/redo
 *
 */

wxRichTextCommand::wxRichTextCommand(const wxString& name, wxRichTextCommandId id, wxRichTextBuffer* buffer,
                                     wxRichTextCtrl* ctrl, bool ignoreFirstTime): wxCommand(true, name)
{
    /* wxRichTextAction* action = */ new wxRichTextAction(this, name, id, buffer, ctrl, ignoreFirstTime);
}

wxRichTextCommand::wxRichTextCommand(const wxString& name): wxCommand(true, name)
{
}

wxRichTextCommand::~wxRichTextCommand()
{
    ClearActions();
}

void wxRichTextCommand::AddAction(wxRichTextAction* action)
{
    if (!m_actions.Member(action))
        m_actions.Append(action);
}

bool wxRichTextCommand::Do()
{
    for (wxList::compatibility_iterator node = m_actions.GetFirst(); node; node = node->GetNext())
    {
        wxRichTextAction* action = (wxRichTextAction*) node->GetData();
        action->Do();
    }

    return true;
}

bool wxRichTextCommand::Undo()
{
    for (wxList::compatibility_iterator node = m_actions.GetLast(); node; node = node->GetPrevious())
    {
        wxRichTextAction* action = (wxRichTextAction*) node->GetData();
        action->Undo();
    }

    return true;
}

void wxRichTextCommand::ClearActions()
{
    WX_CLEAR_LIST(wxList, m_actions);
}

/*!
 * Individual action
 *
 */

wxRichTextAction::wxRichTextAction(wxRichTextCommand* cmd, const wxString& name, wxRichTextCommandId id, wxRichTextBuffer* buffer,
                                     wxRichTextCtrl* ctrl, bool ignoreFirstTime)
{
    m_buffer = buffer;
    m_ignoreThis = ignoreFirstTime;
    m_cmdId = id;
    m_position = -1;
    m_ctrl = ctrl;
    m_name = name;
    m_newParagraphs.SetDefaultStyle(buffer->GetDefaultStyle());
    m_newParagraphs.SetBasicStyle(buffer->GetBasicStyle());
    if (cmd)
        cmd->AddAction(this);
}

wxRichTextAction::~wxRichTextAction()
{
}

void wxRichTextAction::CalculateRefreshOptimizations(wxArrayInt& optimizationLineCharPositions, wxArrayInt& optimizationLineYPositions)
{
    // Store a list of line start character and y positions so we can figure out which area
    // we need to refresh

#if wxRICHTEXT_USE_OPTIMIZED_DRAWING
    // NOTE: we're assuming that the buffer is laid out correctly at this point.
    // If we had several actions, which only invalidate and leave layout until the
    // paint handler is called, then this might not be true. So we may need to switch
    // optimisation on only when we're simply adding text and not simultaneously
    // deleting a selection, for example. Or, we make sure the buffer is laid out correctly
    // first, but of course this means we'll be doing it twice.
    if (!m_buffer->GetDirty() && m_ctrl) // can only do optimisation if the buffer is already laid out correctly
    {
        wxSize clientSize = m_ctrl->GetClientSize();
        wxPoint firstVisiblePt = m_ctrl->GetFirstVisiblePoint();
        int lastY = firstVisiblePt.y + clientSize.y;

        wxRichTextParagraph* para = m_buffer->GetParagraphAtPosition(GetRange().GetStart());
        wxRichTextObjectList::compatibility_iterator node = m_buffer->GetChildren().Find(para);
        while (node)
        {
            wxRichTextParagraph* child = (wxRichTextParagraph*) node->GetData();
            wxRichTextLineList::compatibility_iterator node2 = child->GetLines().GetFirst();
            while (node2)
            {
                wxRichTextLine* line = node2->GetData();
                wxPoint pt = line->GetAbsolutePosition();
                wxRichTextRange range = line->GetAbsoluteRange();

                if (pt.y > lastY)
                {
                    node2 = wxRichTextLineList::compatibility_iterator();
                    node = wxRichTextObjectList::compatibility_iterator();
                }
                else if (range.GetStart() > GetPosition() && pt.y >= firstVisiblePt.y)
                {
                    optimizationLineCharPositions.Add(range.GetStart());
                    optimizationLineYPositions.Add(pt.y);
                }

                if (node2)
                    node2 = node2->GetNext();
            }

            if (node)
                node = node->GetNext();
        }
    }
#endif
}

bool wxRichTextAction::Do()
{
    m_buffer->Modify(true);

    switch (m_cmdId)
    {
    case wxRICHTEXT_INSERT:
        {
            // Store a list of line start character and y positions so we can figure out which area
            // we need to refresh
            wxArrayInt optimizationLineCharPositions;
            wxArrayInt optimizationLineYPositions;

#if wxRICHTEXT_USE_OPTIMIZED_DRAWING
            CalculateRefreshOptimizations(optimizationLineCharPositions, optimizationLineYPositions);
#endif

            m_buffer->InsertFragment(GetRange().GetStart(), m_newParagraphs);
            m_buffer->UpdateRanges();
            m_buffer->Invalidate(wxRichTextRange(wxMax(0, GetRange().GetStart()-1), GetRange().GetEnd()));

            long newCaretPosition = GetPosition() + m_newParagraphs.GetRange().GetLength();

            // Character position to caret position
            newCaretPosition --;

            // Don't take into account the last newline
            if (m_newParagraphs.GetPartialParagraph())
                newCaretPosition --;
            else
                if (m_newParagraphs.GetChildren().GetCount() > 1)
                {
                    wxRichTextObject* p = (wxRichTextObject*) m_newParagraphs.GetChildren().GetLast()->GetData();
                    if (p->GetRange().GetLength() == 1)
                        newCaretPosition --;
                }

            newCaretPosition = wxMin(newCaretPosition, (m_buffer->GetRange().GetEnd()-1));

            UpdateAppearance(newCaretPosition, true /* send update event */, & optimizationLineCharPositions, & optimizationLineYPositions, true /* do */);

            wxRichTextEvent cmdEvent(
                wxEVT_COMMAND_RICHTEXT_CONTENT_INSERTED,
                m_ctrl ? m_ctrl->GetId() : -1);
            cmdEvent.SetEventObject(m_ctrl ? (wxObject*) m_ctrl : (wxObject*) m_buffer);
            cmdEvent.SetRange(GetRange());
            cmdEvent.SetPosition(GetRange().GetStart());

            m_buffer->SendEvent(cmdEvent);

            break;
        }
    case wxRICHTEXT_DELETE:
        {
            wxArrayInt optimizationLineCharPositions;
            wxArrayInt optimizationLineYPositions;

#if wxRICHTEXT_USE_OPTIMIZED_DRAWING
            CalculateRefreshOptimizations(optimizationLineCharPositions, optimizationLineYPositions);
#endif

            m_buffer->DeleteRange(GetRange());
            m_buffer->UpdateRanges();
            m_buffer->Invalidate(wxRichTextRange(GetRange().GetStart(), GetRange().GetStart()));

            long caretPos = GetRange().GetStart()-1;
            if (caretPos >= m_buffer->GetRange().GetEnd())
                caretPos --;

            UpdateAppearance(caretPos, true /* send update event */, & optimizationLineCharPositions, & optimizationLineYPositions, true /* do */);

            wxRichTextEvent cmdEvent(
                wxEVT_COMMAND_RICHTEXT_CONTENT_DELETED,
                m_ctrl ? m_ctrl->GetId() : -1);
            cmdEvent.SetEventObject(m_ctrl ? (wxObject*) m_ctrl : (wxObject*) m_buffer);
            cmdEvent.SetRange(GetRange());
            cmdEvent.SetPosition(GetRange().GetStart());

            m_buffer->SendEvent(cmdEvent);

            break;
        }
    case wxRICHTEXT_CHANGE_STYLE:
        {
            ApplyParagraphs(GetNewParagraphs());
            m_buffer->Invalidate(GetRange());

            UpdateAppearance(GetPosition());

            wxRichTextEvent cmdEvent(
                wxEVT_COMMAND_RICHTEXT_STYLE_CHANGED,
                m_ctrl ? m_ctrl->GetId() : -1);
            cmdEvent.SetEventObject(m_ctrl ? (wxObject*) m_ctrl : (wxObject*) m_buffer);
            cmdEvent.SetRange(GetRange());
            cmdEvent.SetPosition(GetRange().GetStart());

            m_buffer->SendEvent(cmdEvent);

            break;
        }
    default:
        break;
    }

    return true;
}

bool wxRichTextAction::Undo()
{
    m_buffer->Modify(true);

    switch (m_cmdId)
    {
    case wxRICHTEXT_INSERT:
        {
            wxArrayInt optimizationLineCharPositions;
            wxArrayInt optimizationLineYPositions;

#if wxRICHTEXT_USE_OPTIMIZED_DRAWING
            CalculateRefreshOptimizations(optimizationLineCharPositions, optimizationLineYPositions);
#endif

            m_buffer->DeleteRange(GetRange());
            m_buffer->UpdateRanges();
            m_buffer->Invalidate(wxRichTextRange(GetRange().GetStart(), GetRange().GetStart()));

            long newCaretPosition = GetPosition() - 1;

            UpdateAppearance(newCaretPosition, true, /* send update event */ & optimizationLineCharPositions, & optimizationLineYPositions, false /* undo */);

            wxRichTextEvent cmdEvent(
                wxEVT_COMMAND_RICHTEXT_CONTENT_DELETED,
                m_ctrl ? m_ctrl->GetId() : -1);
            cmdEvent.SetEventObject(m_ctrl ? (wxObject*) m_ctrl : (wxObject*) m_buffer);
            cmdEvent.SetRange(GetRange());
            cmdEvent.SetPosition(GetRange().GetStart());

            m_buffer->SendEvent(cmdEvent);

            break;
        }
    case wxRICHTEXT_DELETE:
        {
            wxArrayInt optimizationLineCharPositions;
            wxArrayInt optimizationLineYPositions;

#if wxRICHTEXT_USE_OPTIMIZED_DRAWING
            CalculateRefreshOptimizations(optimizationLineCharPositions, optimizationLineYPositions);
#endif

            m_buffer->InsertFragment(GetRange().GetStart(), m_oldParagraphs);
            m_buffer->UpdateRanges();
            m_buffer->Invalidate(GetRange());

            UpdateAppearance(GetPosition(), true, /* send update event */ & optimizationLineCharPositions, & optimizationLineYPositions, false /* undo */);

            wxRichTextEvent cmdEvent(
                wxEVT_COMMAND_RICHTEXT_CONTENT_INSERTED,
                m_ctrl ? m_ctrl->GetId() : -1);
            cmdEvent.SetEventObject(m_ctrl ? (wxObject*) m_ctrl : (wxObject*) m_buffer);
            cmdEvent.SetRange(GetRange());
            cmdEvent.SetPosition(GetRange().GetStart());

            m_buffer->SendEvent(cmdEvent);

            break;
        }
    case wxRICHTEXT_CHANGE_STYLE:
        {
            ApplyParagraphs(GetOldParagraphs());
            m_buffer->Invalidate(GetRange());

            UpdateAppearance(GetPosition());

            wxRichTextEvent cmdEvent(
                wxEVT_COMMAND_RICHTEXT_STYLE_CHANGED,
                m_ctrl ? m_ctrl->GetId() : -1);
            cmdEvent.SetEventObject(m_ctrl ? (wxObject*) m_ctrl : (wxObject*) m_buffer);
            cmdEvent.SetRange(GetRange());
            cmdEvent.SetPosition(GetRange().GetStart());

            m_buffer->SendEvent(cmdEvent);

            break;
        }
    default:
        break;
    }

    return true;
}

/// Update the control appearance
void wxRichTextAction::UpdateAppearance(long caretPosition, bool sendUpdateEvent, wxArrayInt* optimizationLineCharPositions, wxArrayInt* optimizationLineYPositions, bool isDoCmd)
{
    if (m_ctrl)
    {
        m_ctrl->SetCaretPosition(caretPosition);
        if (!m_ctrl->IsFrozen())
        {
            m_ctrl->LayoutContent();

#if wxRICHTEXT_USE_OPTIMIZED_DRAWING
            // Find refresh rectangle if we are in a position to optimise refresh
            if ((m_cmdId == wxRICHTEXT_INSERT || m_cmdId == wxRICHTEXT_DELETE) && optimizationLineCharPositions)
            {
                size_t i;

                wxSize clientSize = m_ctrl->GetClientSize();
                wxPoint firstVisiblePt = m_ctrl->GetFirstVisiblePoint();

                // Start/end positions
                int firstY = 0;
                int lastY = firstVisiblePt.y + clientSize.y;

                bool foundEnd = false;

                // position offset - how many characters were inserted
                int positionOffset = GetRange().GetLength();

                // Determine whether this is Do or Undo, and adjust positionOffset accordingly
                if ((m_cmdId == wxRICHTEXT_DELETE && isDoCmd) || (m_cmdId == wxRICHTEXT_INSERT && !isDoCmd))
                    positionOffset = - positionOffset;

                // find the first line which is being drawn at the same position as it was
                // before. Since we're talking about a simple insertion, we can assume
                // that the rest of the window does not need to be redrawn.

                wxRichTextParagraph* para = m_buffer->GetParagraphAtPosition(GetPosition());
                if (para)
                {
                    // Find line containing GetPosition().
                    wxRichTextLine* line = NULL;
                    wxRichTextLineList::compatibility_iterator node2 = para->GetLines().GetFirst();
                    while (node2)
                    {
                        wxRichTextLine* l = node2->GetData();
                        wxRichTextRange range = l->GetAbsoluteRange();
                        if (range.Contains(GetRange().GetStart()-1))
                        {
                            line = l;
                            break;
                        }
                        node2 = node2->GetNext();
                    }

                    if (line)
                    {
                        // Step back a couple of lines to where we can be sure of reformatting correctly
                        wxRichTextLineList::compatibility_iterator lineNode = para->GetLines().Find(line);
                        if (lineNode)
                        {
                            lineNode = lineNode->GetPrevious();
                            if (lineNode)
                            {
                                line = (wxRichTextLine*) lineNode->GetData();
                                lineNode = lineNode->GetPrevious();
                                if (lineNode)
                                    line = (wxRichTextLine*) lineNode->GetData();
                            }
                        }

                        firstY = line->GetAbsolutePosition().y;
                    }
                }

                wxRichTextObjectList::compatibility_iterator node = m_buffer->GetChildren().Find(para);
                while (node)
                {
                    wxRichTextParagraph* child = (wxRichTextParagraph*) node->GetData();
                    wxRichTextLineList::compatibility_iterator node2 = child->GetLines().GetFirst();
                    while (node2)
                    {
                        wxRichTextLine* line = node2->GetData();
                        wxPoint pt = line->GetAbsolutePosition();
                        wxRichTextRange range = line->GetAbsoluteRange();

                        // we want to find the first line that is in the same position
                        // as before. This will mean we're at the end of the changed text.

                        if (pt.y > lastY) // going past the end of the window, no more info
                        {
                            node2 = wxRichTextLineList::compatibility_iterator();
                            node = wxRichTextObjectList::compatibility_iterator();
                        }
                        // Detect last line in the buffer
                        else if (!node2->GetNext() && para->GetRange().Contains(m_buffer->GetRange().GetEnd()))
                        {
                            // If deleting text, make sure we refresh below as well as above
                            if (positionOffset >= 0)
                            {
                                foundEnd = true;
                                lastY = pt.y + line->GetSize().y;
                            }

                            node2 = wxRichTextLineList::compatibility_iterator();
                            node = wxRichTextObjectList::compatibility_iterator();

                            break;
                        }
                        else
                        {
                            // search for this line being at the same position as before
                            for (i = 0; i < optimizationLineCharPositions->GetCount(); i++)
                            {
                                if (((*optimizationLineCharPositions)[i] + positionOffset == range.GetStart()) &&
                                    ((*optimizationLineYPositions)[i] == pt.y))
                                {
                                    // Stop, we're now the same as we were
                                    foundEnd = true;

                                    lastY = pt.y;

                                    node2 = wxRichTextLineList::compatibility_iterator();
                                    node = wxRichTextObjectList::compatibility_iterator();

                                    break;
                                }
                            }
                        }

                        if (node2)
                            node2 = node2->GetNext();
                    }

                    if (node)
                        node = node->GetNext();
                }

                firstY = wxMax(firstVisiblePt.y, firstY);
                if (!foundEnd)
                    lastY = firstVisiblePt.y + clientSize.y;

                // Convert to device coordinates
                wxRect rect(m_ctrl->GetPhysicalPoint(wxPoint(firstVisiblePt.x, firstY)), wxSize(clientSize.x, lastY - firstY));
                m_ctrl->RefreshRect(rect);
            }
            else
#endif
                m_ctrl->Refresh(false);

#if wxRICHTEXT_USE_OWN_CARET
            m_ctrl->PositionCaret();
#endif
            if (sendUpdateEvent)
                wxTextCtrl::SendTextUpdatedEvent(m_ctrl);
        }
    }
}

/// Replace the buffer paragraphs with the new ones.
void wxRichTextAction::ApplyParagraphs(const wxRichTextParagraphLayoutBox& fragment)
{
    wxRichTextObjectList::compatibility_iterator node = fragment.GetChildren().GetFirst();
    while (node)
    {
        wxRichTextParagraph* para = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        wxASSERT (para != NULL);

        // We'll replace the existing paragraph by finding the paragraph at this position,
        // delete its node data, and setting a copy as the new node data.
        // TODO: make more efficient by simply swapping old and new paragraph objects.

        wxRichTextParagraph* existingPara = m_buffer->GetParagraphAtPosition(para->GetRange().GetStart());
        if (existingPara)
        {
            wxRichTextObjectList::compatibility_iterator bufferParaNode = m_buffer->GetChildren().Find(existingPara);
            if (bufferParaNode)
            {
                wxRichTextParagraph* newPara = new wxRichTextParagraph(*para);
                newPara->SetParent(m_buffer);

                bufferParaNode->SetData(newPara);

                delete existingPara;
            }
        }

        node = node->GetNext();
    }
}


/*!
 * wxRichTextRange
 * This stores beginning and end positions for a range of data.
 */

/// Limit this range to be within 'range'
bool wxRichTextRange::LimitTo(const wxRichTextRange& range)
{
    if (m_start < range.m_start)
        m_start = range.m_start;

    if (m_end > range.m_end)
        m_end = range.m_end;

    return true;
}

/*!
 * wxRichTextPlaceHoldingObject implementation
 */

IMPLEMENT_DYNAMIC_CLASS(wxRichTextPlaceHoldingObject, wxRichTextObject)

wxRichTextPlaceHoldingObject::wxRichTextPlaceHoldingObject(wxRichTextObject *parent, wxRichTextObject *real)
                            : wxRichTextObject(parent), m_real(real)
{
}

wxRichTextPlaceHoldingObject::~wxRichTextPlaceHoldingObject()
{
}

bool wxRichTextPlaceHoldingObject::Draw(wxDC& dc, const wxRichTextRange& range, const wxRichTextRange& selectionrange, const wxRect& rect, int descent, int style)
{
    return true;
}

bool wxRichTextPlaceHoldingObject::Layout(wxDC& dc, const wxRect& rect, int style)
{
    SetCachedSize(wxSize(0, 0));
    return true;
}

bool wxRichTextPlaceHoldingObject::GetRangeSize(const wxRichTextRange& range, wxSize& size, int& descent, wxDC& dc, int flags, wxPoint position, wxArrayInt* partialExtents) const
{
    size.x = size.y = 0;
    return true;
}

/*!
 * wxRichTextAnchoredObject implementation
 */
IMPLEMENT_CLASS(wxRichTextAnchoredObject, wxRichTextObject)

wxRichTextAnchoredObject::wxRichTextAnchoredObject(wxRichTextObject* parent, const wxRichTextAnchoredObjectAttr& attr):
    wxRichTextObject(parent), m_attr(attr)
{
}

wxRichTextAnchoredObjectAttr wxRichTextAnchoredObject::GetAnchoredAttr()
{
    return m_attr;
}

void wxRichTextAnchoredObject::SetAnchoredAttr(const wxRichTextAnchoredObjectAttr& attr)
{
    m_attr = attr;
}

/*!
 * wxRichTextImage implementation
 * This object represents an image.
 */

IMPLEMENT_DYNAMIC_CLASS(wxRichTextImage, wxRichTextAnchoredObject)

wxRichTextImage::wxRichTextImage(const wxImage& image, wxRichTextObject* parent, wxTextAttr* charStyle, const wxRichTextAnchoredObjectAttr& attr):
    wxRichTextAnchoredObject(parent, attr)
{
    m_attrInit = false;
    m_image = image;
    if (charStyle)
        SetAttributes(*charStyle);
}

wxRichTextImage::wxRichTextImage(const wxRichTextImageBlock& imageBlock, wxRichTextObject* parent, wxTextAttr* charStyle, const wxRichTextAnchoredObjectAttr& attr):
    wxRichTextAnchoredObject(parent, attr)
{
    m_attrInit = false;
    m_imageBlock = imageBlock;
    m_imageBlock.Load(m_image);
    if (charStyle)
        SetAttributes(*charStyle);
}

/// Load wxImage from the block
bool wxRichTextImage::LoadFromBlock()
{
    m_imageBlock.Load(m_image);
    return m_imageBlock.Ok();
}

/// Make block from the wxImage
bool wxRichTextImage::MakeBlock()
{
    wxBitmapType type = m_imageBlock.GetImageType();
    if ( type == wxBITMAP_TYPE_ANY || type == wxBITMAP_TYPE_INVALID )
        m_imageBlock.SetImageType(type = wxBITMAP_TYPE_PNG);

    m_imageBlock.MakeImageBlock(m_image, type);
    return m_imageBlock.Ok();
}


/// Draw the item
bool wxRichTextImage::Draw(wxDC& dc, const wxRichTextRange& range, const wxRichTextRange& selectionRange, const wxRect& rect, int WXUNUSED(descent), int WXUNUSED(style))
{
    wxSize size = GetCachedSize();
    wxImage img;

    if (!m_image.Ok() && m_imageBlock.Ok())
        LoadFromBlock();

    if (!m_image.Ok())
        return false;

    img = m_image;
    img.Rescale(size.x, size.y);
    wxBitmap bitmap(img);

    int y = rect.y + (rect.height - img.GetHeight());

    if (bitmap.Ok())
        dc.DrawBitmap(bitmap, rect.x, y, true);

    if (selectionRange.Contains(range.GetStart()))
    {
        wxCheckSetBrush(dc, *wxBLACK_BRUSH);
        wxCheckSetPen(dc, *wxBLACK_PEN);
        dc.SetLogicalFunction(wxINVERT);
        dc.DrawRectangle(rect);
        dc.SetLogicalFunction(wxCOPY);
    }

    return true;
}

void wxRichTextImage::UpdateImageSize(wxDC& dc, int& width, int& height) const
{
    if (m_attrInit == false)
    {
        width = m_image.GetWidth();
        height = m_image.GetHeight();
        return;
    }

    // Calculate the user specified length
    if (m_attr.m_scaleW == wxRICHTEXT_MM)
    {
        width = ConvertTenthsMMToPixels(dc, m_attr.m_width);
    }
    else
    {
        width = m_attr.m_width;
    }
    if (m_attr.m_scaleH == wxRICHTEXT_MM)
    {
        height = ConvertTenthsMMToPixels(dc, m_attr.m_height);
    }
    else
    {
        height = m_attr.m_height;
    }
}

/// Lay the item out
bool wxRichTextImage::Layout(wxDC& dc, const wxRect& rect, int WXUNUSED(style))
{
    if (!m_image.Ok())
        LoadFromBlock();

    if (m_image.Ok())
    {
        int width, height;
        InitializeAttribute();
        UpdateImageSize(dc, width, height);
        SetCachedSize(wxSize(width, height));
        SetPosition(rect.GetPosition());
    }

    return true;
}

/// Get/set the object size for the given range. Returns false if the range
/// is invalid for this object.
bool wxRichTextImage::GetRangeSize(const wxRichTextRange& range, wxSize& size, int& WXUNUSED(descent), wxDC& dc, int WXUNUSED(flags), wxPoint WXUNUSED(position), wxArrayInt* partialExtents) const
{
    int width, height;

    if (!range.IsWithin(GetRange()))
        return false;

    if (!m_image.Ok())
        ((wxRichTextImage*) this)->LoadFromBlock();

    UpdateImageSize(dc, width, height);

    if (partialExtents)
    {
        if (m_image.Ok())
            partialExtents->Add(width);
        else
            partialExtents->Add(0);
    }

    if (!m_image.Ok())
        return false;

    size.x = width;
    size.y = height;

    return true;
}

/// Copy
void wxRichTextImage::Copy(const wxRichTextImage& obj)
{
    wxRichTextObject::Copy(obj);

    m_image = obj.m_image;
    m_imageBlock = obj.m_imageBlock;
    m_attr = obj.m_attr;
    m_attrInit = obj.m_attrInit;
}

wxRichTextImageAttr wxRichTextImage::GetImageAttr()
{
    InitializeAttribute();
    return m_attr;
}

void wxRichTextImage::SetImageAttr(const wxRichTextImageAttr& attr)
{
    m_attr = attr;
}

void wxRichTextImage::InitializeAttribute()
{
    if (m_attrInit)
        return;

    wxRichTextAnchoredObjectAttr& attr = m_attr;
    attr = GetAnchoredAttr();
    m_attr.m_scaleW = m_attr.m_scaleH = wxRICHTEXT_PX;
    m_attr.m_width = m_image.GetWidth();
    m_attr.m_height = m_image.GetHeight();
    m_attrInit = true;
}

/*!
 * Utilities
 *
 */

/// Compare two attribute objects
bool wxTextAttrEq(const wxTextAttr& attr1, const wxTextAttr& attr2)
{
    return (attr1 == attr2);
}

// Partial equality test taking flags into account
bool wxTextAttrEqPartial(const wxTextAttr& attr1, const wxTextAttr& attr2, int flags)
{
    return attr1.EqPartial(attr2, flags);
}

/// Compare tabs
bool wxRichTextTabsEq(const wxArrayInt& tabs1, const wxArrayInt& tabs2)
{
    if (tabs1.GetCount() != tabs2.GetCount())
        return false;

    size_t i;
    for (i = 0; i < tabs1.GetCount(); i++)
    {
        if (tabs1[i] != tabs2[i])
            return false;
    }
    return true;
}

bool wxRichTextApplyStyle(wxTextAttr& destStyle, const wxTextAttr& style, wxTextAttr* compareWith)
{
    return destStyle.Apply(style, compareWith);
}

// Remove attributes
bool wxRichTextRemoveStyle(wxTextAttr& destStyle, const wxTextAttr& style)
{
    return wxTextAttr::RemoveStyle(destStyle, style);
}

/// Combine two bitlists, specifying the bits of interest with separate flags.
bool wxRichTextCombineBitlists(int& valueA, int valueB, int& flagsA, int flagsB)
{
    return wxTextAttr::CombineBitlists(valueA, valueB, flagsA, flagsB);
}

/// Compare two bitlists
bool wxRichTextBitlistsEqPartial(int valueA, int valueB, int flags)
{
    return wxTextAttr::BitlistsEqPartial(valueA, valueB, flags);
}

/// Split into paragraph and character styles
bool wxRichTextSplitParaCharStyles(const wxTextAttr& style, wxTextAttr& parStyle, wxTextAttr& charStyle)
{
    return wxTextAttr::SplitParaCharStyles(style, parStyle, charStyle);
}

/// Convert a decimal to Roman numerals
wxString wxRichTextDecimalToRoman(long n)
{
    static wxArrayInt decimalNumbers;
    static wxArrayString romanNumbers;

    // Clean up arrays
    if (n == -1)
    {
        decimalNumbers.Clear();
        romanNumbers.Clear();
        return wxEmptyString;
    }

    if (decimalNumbers.GetCount() == 0)
    {
        #define wxRichTextAddDecRom(n, r) decimalNumbers.Add(n); romanNumbers.Add(r);

        wxRichTextAddDecRom(1000, wxT("M"));
        wxRichTextAddDecRom(900, wxT("CM"));
        wxRichTextAddDecRom(500, wxT("D"));
        wxRichTextAddDecRom(400, wxT("CD"));
        wxRichTextAddDecRom(100, wxT("C"));
        wxRichTextAddDecRom(90, wxT("XC"));
        wxRichTextAddDecRom(50, wxT("L"));
        wxRichTextAddDecRom(40, wxT("XL"));
        wxRichTextAddDecRom(10, wxT("X"));
        wxRichTextAddDecRom(9, wxT("IX"));
        wxRichTextAddDecRom(5, wxT("V"));
        wxRichTextAddDecRom(4, wxT("IV"));
        wxRichTextAddDecRom(1, wxT("I"));
    }

    int i = 0;
    wxString roman;

    while (n > 0 && i < 13)
    {
        if (n >= decimalNumbers[i])
        {
            n -= decimalNumbers[i];
            roman += romanNumbers[i];
        }
        else
        {
            i ++;
        }
    }
    if (roman.IsEmpty())
        roman = wxT("0");
    return roman;
}

/*!
 * wxRichTextFileHandler
 * Base class for file handlers
 */

IMPLEMENT_CLASS(wxRichTextFileHandler, wxObject)

#if wxUSE_FFILE && wxUSE_STREAMS
bool wxRichTextFileHandler::LoadFile(wxRichTextBuffer *buffer, const wxString& filename)
{
    wxFFileInputStream stream(filename);
    if (stream.Ok())
        return LoadFile(buffer, stream);

    return false;
}

bool wxRichTextFileHandler::SaveFile(wxRichTextBuffer *buffer, const wxString& filename)
{
    wxFFileOutputStream stream(filename);
    if (stream.Ok())
        return SaveFile(buffer, stream);

    return false;
}
#endif // wxUSE_FFILE && wxUSE_STREAMS

/// Can we handle this filename (if using files)? By default, checks the extension.
bool wxRichTextFileHandler::CanHandle(const wxString& filename) const
{
    wxString path, file, ext;
    wxFileName::SplitPath(filename, & path, & file, & ext);

    return (ext.Lower() == GetExtension());
}

/*!
 * wxRichTextTextHandler
 * Plain text handler
 */

IMPLEMENT_CLASS(wxRichTextPlainTextHandler, wxRichTextFileHandler)

#if wxUSE_STREAMS
bool wxRichTextPlainTextHandler::DoLoadFile(wxRichTextBuffer *buffer, wxInputStream& stream)
{
    if (!stream.IsOk())
        return false;

    wxString str;
    int lastCh = 0;

    while (!stream.Eof())
    {
        int ch = stream.GetC();

        if (!stream.Eof())
        {
            if (ch == 10 && lastCh != 13)
                str += wxT('\n');

            if (ch > 0 && ch != 10)
                str += wxChar(ch);

            lastCh = ch;
        }
    }

    buffer->ResetAndClearCommands();
    buffer->Clear();
    buffer->AddParagraphs(str);
    buffer->UpdateRanges();

    return true;
}

bool wxRichTextPlainTextHandler::DoSaveFile(wxRichTextBuffer *buffer, wxOutputStream& stream)
{
    if (!stream.IsOk())
        return false;

    wxString text = buffer->GetText();

    wxString newLine = wxRichTextLineBreakChar;
    text.Replace(newLine, wxT("\n"));

    wxCharBuffer buf = text.ToAscii();

    stream.Write((const char*) buf, text.length());
    return true;
}
#endif // wxUSE_STREAMS

/*
 * Stores information about an image, in binary in-memory form
 */

wxRichTextImageBlock::wxRichTextImageBlock()
{
    Init();
}

wxRichTextImageBlock::wxRichTextImageBlock(const wxRichTextImageBlock& block):wxObject()
{
    Init();
    Copy(block);
}

wxRichTextImageBlock::~wxRichTextImageBlock()
{
    if (m_data)
    {
        delete[] m_data;
        m_data = NULL;
    }
}

void wxRichTextImageBlock::Init()
{
    m_data = NULL;
    m_dataSize = 0;
    m_imageType = wxBITMAP_TYPE_INVALID;
}

void wxRichTextImageBlock::Clear()
{
    delete[] m_data;
    m_data = NULL;
    m_dataSize = 0;
    m_imageType = wxBITMAP_TYPE_INVALID;
}


// Load the original image into a memory block.
// If the image is not a JPEG, we must convert it into a JPEG
// to conserve space.
// If it's not a JPEG we can make use of 'image', already scaled, so we don't have to
// load the image a 2nd time.

bool wxRichTextImageBlock::MakeImageBlock(const wxString& filename, wxBitmapType imageType,
                                          wxImage& image, bool convertToJPEG)
{
    m_imageType = imageType;

    wxString filenameToRead(filename);
    bool removeFile = false;

    if (imageType == wxBITMAP_TYPE_INVALID)
        return false; // Could not determine image type

    if ((imageType != wxBITMAP_TYPE_JPEG) && convertToJPEG)
    {
        wxString tempFile =
            wxFileName::CreateTempFileName(_("image"));

        wxASSERT(!tempFile.IsEmpty());

        image.SaveFile(tempFile, wxBITMAP_TYPE_JPEG);
        filenameToRead = tempFile;
        removeFile = true;

        m_imageType = wxBITMAP_TYPE_JPEG;
    }
    wxFile file;
    if (!file.Open(filenameToRead))
        return false;

    m_dataSize = (size_t) file.Length();
    file.Close();

    if (m_data)
        delete[] m_data;
    m_data = ReadBlock(filenameToRead, m_dataSize);

    if (removeFile)
        wxRemoveFile(filenameToRead);

    return (m_data != NULL);
}

// Make an image block from the wxImage in the given
// format.
bool wxRichTextImageBlock::MakeImageBlock(wxImage& image, wxBitmapType imageType, int quality)
{
    m_imageType = imageType;
    image.SetOption(wxT("quality"), quality);

    if (imageType == wxBITMAP_TYPE_INVALID)
        return false; // Could not determine image type

    wxString tempFile = wxFileName::CreateTempFileName(_("image")) ;
    wxASSERT(!tempFile.IsEmpty());

    if (!image.SaveFile(tempFile, m_imageType))
    {
        if (wxFileExists(tempFile))
            wxRemoveFile(tempFile);
        return false;
    }

    wxFile file;
    if (!file.Open(tempFile))
        return false;

    m_dataSize = (size_t) file.Length();
    file.Close();

    if (m_data)
        delete[] m_data;
    m_data = ReadBlock(tempFile, m_dataSize);

    wxRemoveFile(tempFile);

    return (m_data != NULL);
}


// Write to a file
bool wxRichTextImageBlock::Write(const wxString& filename)
{
    return WriteBlock(filename, m_data, m_dataSize);
}

void wxRichTextImageBlock::Copy(const wxRichTextImageBlock& block)
{
    m_imageType = block.m_imageType;
    if (m_data)
    {
        delete[] m_data;
        m_data = NULL;
    }
    m_dataSize = block.m_dataSize;
    if (m_dataSize == 0)
        return;

    m_data = new unsigned char[m_dataSize];
    unsigned int i;
    for (i = 0; i < m_dataSize; i++)
        m_data[i] = block.m_data[i];
}

//// Operators
void wxRichTextImageBlock::operator=(const wxRichTextImageBlock& block)
{
    Copy(block);
}

// Load a wxImage from the block
bool wxRichTextImageBlock::Load(wxImage& image)
{
    if (!m_data)
        return false;

    // Read in the image.
#if wxUSE_STREAMS
    wxMemoryInputStream mstream(m_data, m_dataSize);
    bool success = image.LoadFile(mstream, GetImageType());
#else
    wxString tempFile = wxFileName::CreateTempFileName(_("image"));
    wxASSERT(!tempFile.IsEmpty());

    if (!WriteBlock(tempFile, m_data, m_dataSize))
    {
        return false;
    }
    success = image.LoadFile(tempFile, GetImageType());
    wxRemoveFile(tempFile);
#endif

    return success;
}

// Write data in hex to a stream
bool wxRichTextImageBlock::WriteHex(wxOutputStream& stream)
{
    const int bufSize = 512;
    char buf[bufSize+1];

    int left = m_dataSize;
    int n, i, j;
    j = 0;
    while (left > 0)
    {
        if (left*2 > bufSize)
        {
            n = bufSize; left -= (bufSize/2);
        }
        else
        {
            n = left*2; left = 0;
        }

        char* b = buf;
        for (i = 0; i < (n/2); i++)
        {
            wxDecToHex(m_data[j], b, b+1);
            b += 2; j ++;
        }

        buf[n] = 0;
        stream.Write((const char*) buf, n);
    }
    return true;
}

// Read data in hex from a stream
bool wxRichTextImageBlock::ReadHex(wxInputStream& stream, int length, wxBitmapType imageType)
{
    int dataSize = length/2;

    if (m_data)
        delete[] m_data;

    // create a null terminated temporary string:
    char str[3];
    str[2] = '\0';

    m_data = new unsigned char[dataSize];
    int i;
    for (i = 0; i < dataSize; i ++)
    {
        str[0] = (char)stream.GetC();
        str[1] = (char)stream.GetC();

        m_data[i] = (unsigned char)wxHexToDec(str);
    }

    m_dataSize = dataSize;
    m_imageType = imageType;

    return true;
}

// Allocate and read from stream as a block of memory
unsigned char* wxRichTextImageBlock::ReadBlock(wxInputStream& stream, size_t size)
{
    unsigned char* block = new unsigned char[size];
    if (!block)
        return NULL;

    stream.Read(block, size);

    return block;
}

unsigned char* wxRichTextImageBlock::ReadBlock(const wxString& filename, size_t size)
{
    wxFileInputStream stream(filename);
    if (!stream.Ok())
        return NULL;

    return ReadBlock(stream, size);
}

// Write memory block to stream
bool wxRichTextImageBlock::WriteBlock(wxOutputStream& stream, unsigned char* block, size_t size)
{
    stream.Write((void*) block, size);
    return stream.IsOk();

}

// Write memory block to file
bool wxRichTextImageBlock::WriteBlock(const wxString& filename, unsigned char* block, size_t size)
{
    wxFileOutputStream outStream(filename);
    if (!outStream.Ok())
        return false;

    return WriteBlock(outStream, block, size);
}

// Gets the extension for the block's type
wxString wxRichTextImageBlock::GetExtension() const
{
    wxImageHandler* handler = wxImage::FindHandler(GetImageType());
    if (handler)
        return handler->GetExtension();
    else
        return wxEmptyString;
}

#if wxUSE_DATAOBJ

/*!
 * The data object for a wxRichTextBuffer
 */

const wxChar *wxRichTextBufferDataObject::ms_richTextBufferFormatId = wxT("wxShape");

wxRichTextBufferDataObject::wxRichTextBufferDataObject(wxRichTextBuffer* richTextBuffer)
{
    m_richTextBuffer = richTextBuffer;

    // this string should uniquely identify our format, but is otherwise
    // arbitrary
    m_formatRichTextBuffer.SetId(GetRichTextBufferFormatId());

    SetFormat(m_formatRichTextBuffer);
}

wxRichTextBufferDataObject::~wxRichTextBufferDataObject()
{
    delete m_richTextBuffer;
}

// after a call to this function, the richTextBuffer is owned by the caller and it
// is responsible for deleting it!
wxRichTextBuffer* wxRichTextBufferDataObject::GetRichTextBuffer()
{
    wxRichTextBuffer* richTextBuffer = m_richTextBuffer;
    m_richTextBuffer = NULL;

    return richTextBuffer;
}

wxDataFormat wxRichTextBufferDataObject::GetPreferredFormat(Direction WXUNUSED(dir)) const
{
    return m_formatRichTextBuffer;
}

size_t wxRichTextBufferDataObject::GetDataSize() const
{
    if (!m_richTextBuffer)
        return 0;

    wxString bufXML;

    {
        wxStringOutputStream stream(& bufXML);
        if (!m_richTextBuffer->SaveFile(stream, wxRICHTEXT_TYPE_XML))
        {
            wxLogError(wxT("Could not write the buffer to an XML stream.\nYou may have forgotten to add the XML file handler."));
            return 0;
        }
    }

#if wxUSE_UNICODE
    wxCharBuffer buffer = bufXML.mb_str(wxConvUTF8);
    return strlen(buffer) + 1;
#else
    return bufXML.Length()+1;
#endif
}

bool wxRichTextBufferDataObject::GetDataHere(void *pBuf) const
{
    if (!pBuf || !m_richTextBuffer)
        return false;

    wxString bufXML;

    {
        wxStringOutputStream stream(& bufXML);
        if (!m_richTextBuffer->SaveFile(stream, wxRICHTEXT_TYPE_XML))
        {
            wxLogError(wxT("Could not write the buffer to an XML stream.\nYou may have forgotten to add the XML file handler."));
            return 0;
        }
    }

#if wxUSE_UNICODE
    wxCharBuffer buffer = bufXML.mb_str(wxConvUTF8);
    size_t len = strlen(buffer);
    memcpy((char*) pBuf, (const char*) buffer, len);
    ((char*) pBuf)[len] = 0;
#else
    size_t len = bufXML.Length();
    memcpy((char*) pBuf, (const char*) bufXML.c_str(), len);
    ((char*) pBuf)[len] = 0;
#endif

    return true;
}

bool wxRichTextBufferDataObject::SetData(size_t WXUNUSED(len), const void *buf)
{
    delete m_richTextBuffer;
    m_richTextBuffer = NULL;

    wxString bufXML((const char*) buf, wxConvUTF8);

    m_richTextBuffer = new wxRichTextBuffer;

    wxStringInputStream stream(bufXML);
    if (!m_richTextBuffer->LoadFile(stream, wxRICHTEXT_TYPE_XML))
    {
        wxLogError(wxT("Could not read the buffer from an XML stream.\nYou may have forgotten to add the XML file handler."));

        delete m_richTextBuffer;
        m_richTextBuffer = NULL;

        return false;
    }
    return true;
}

#endif
    // wxUSE_DATAOBJ


/*
 * wxRichTextFontTable
 * Manages quick access to a pool of fonts for rendering rich text
 */

WX_DECLARE_STRING_HASH_MAP_WITH_DECL(wxFont, wxRichTextFontTableHashMap, class WXDLLIMPEXP_RICHTEXT);

class wxRichTextFontTableData: public wxObjectRefData
{
public:
    wxRichTextFontTableData() {}

    wxFont FindFont(const wxTextAttr& fontSpec);

    wxRichTextFontTableHashMap  m_hashMap;
};

wxFont wxRichTextFontTableData::FindFont(const wxTextAttr& fontSpec)
{
    wxString facename(fontSpec.GetFontFaceName());
    wxString spec(wxString::Format(wxT("%d-%d-%d-%d-%s-%d"), fontSpec.GetFontSize(), fontSpec.GetFontStyle(), fontSpec.GetFontWeight(), (int) fontSpec.GetFontUnderlined(), facename.c_str(), (int) fontSpec.GetFontEncoding()));
    wxRichTextFontTableHashMap::iterator entry = m_hashMap.find(spec);

    if ( entry == m_hashMap.end() )
    {
        wxFont font(fontSpec.GetFontSize(), wxDEFAULT, fontSpec.GetFontStyle(), fontSpec.GetFontWeight(), fontSpec.GetFontUnderlined(), facename.c_str());
        m_hashMap[spec] = font;
        return font;
    }
    else
    {
        return entry->second;
    }
}

IMPLEMENT_DYNAMIC_CLASS(wxRichTextFontTable, wxObject)

wxRichTextFontTable::wxRichTextFontTable()
{
    m_refData = new wxRichTextFontTableData;
}

wxRichTextFontTable::wxRichTextFontTable(const wxRichTextFontTable& table)
    : wxObject()
{
    (*this) = table;
}

wxRichTextFontTable::~wxRichTextFontTable()
{
    UnRef();
}

bool wxRichTextFontTable::operator == (const wxRichTextFontTable& table) const
{
    return (m_refData == table.m_refData);
}

void wxRichTextFontTable::operator= (const wxRichTextFontTable& table)
{
    Ref(table);
}

wxFont wxRichTextFontTable::FindFont(const wxTextAttr& fontSpec)
{
    wxRichTextFontTableData* data = (wxRichTextFontTableData*) m_refData;
    if (data)
        return data->FindFont(fontSpec);
    else
        return wxFont();
}

void wxRichTextFontTable::Clear()
{
    wxRichTextFontTableData* data = (wxRichTextFontTableData*) m_refData;
    if (data)
        data->m_hashMap.clear();
}

#endif
    // wxUSE_RICHTEXT
