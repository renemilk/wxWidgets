#----------------------------------------------------------------------
# Name:        wx.lib.buttons
# Purpose:     Various kinds of generic buttons, (not native controls but
#              self-drawn.)
#
# Author:      Robin Dunn
#
# Created:     9-Dec-1999
# RCS-ID:      $Id$
# Copyright:   (c) 1999 by Total Control Software
# Licence:     wxWindows license
#----------------------------------------------------------------------
# 11/30/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
# o Tested with updated demo
# 

"""
This module implements various forms of generic buttons, meaning that
they are not built on native controls but are self-drawn.

The GenButton is the base.  It acts like a normal button but you
are able to better control how it looks, bevel width, colours, etc.

GenBitmapButton is a button with one or more bitmaps that show
the various states the button can be in.

GenToggleButton stays depressed when clicked, until clicked again.

GenBitmapToggleButton the same but with bitmaps.

"""

import wx
import imageutils


#----------------------------------------------------------------------

class GenButtonEvent(wx.PyCommandEvent):
    def __init__(self, eventType, ID):
        wx.PyCommandEvent.__init__(self, eventType, ID)
        self.isDown = False
        self.theButton = None

    def SetIsDown(self, isDown):
        self.isDown = isDown

    def GetIsDown(self):
        return self.isDown

    def SetButtonObj(self, btn):
        self.theButton = btn

    def GetButtonObj(self):
        return self.theButton


#----------------------------------------------------------------------

class GenButton(wx.PyControl):
    labelDelta = 1

    def __init__(self, parent, ID, label,
                 pos = wx.DefaultPosition, size = wx.DefaultSize,
                 style = 0, validator = wx.DefaultValidator,
                 name = "genbutton"):
        if style == 0:
            style = wx.NO_BORDER
        wx.PyControl.__init__(self, parent, ID, pos, size, style, validator, name)

        self.up = True
        self.bezelWidth = 2
        self.hasFocus = False
        self.useFocusInd = True

        self.SetLabel(label)
        self.SetPosition(pos)
        font = parent.GetFont()
        if not font.Ok():
            font = wx.SystemSettings.GetSystemFont(wx.SYS_DEFAULT_GUI_FONT)
        self.SetFont(font)
        self.SetBestSize(size)
        self.InitColours()

        self.Bind(wx.EVT_LEFT_DOWN,        self.OnLeftDown)
        self.Bind(wx.EVT_LEFT_UP,          self.OnLeftUp)
        if wx.Platform == '__WXMSW__':
            self.Bind(wx.EVT_LEFT_DCLICK,  self.OnLeftDown)
        self.Bind(wx.EVT_MOTION,           self.OnMotion)
        self.Bind(wx.EVT_SET_FOCUS,        self.OnGainFocus)
        self.Bind(wx.EVT_KILL_FOCUS,       self.OnLoseFocus)
        self.Bind(wx.EVT_KEY_DOWN,         self.OnKeyDown)
        self.Bind(wx.EVT_KEY_UP,           self.OnKeyUp)
        self.Bind(wx.EVT_ERASE_BACKGROUND, self.OnEraseBackground)
        self.Bind(wx.EVT_PAINT,            self.OnPaint)


    def SetBestSize(self, size=None):
        """
        Given the current font and bezel width settings, calculate
        and set a good size.
        """
        if size is None:
            size = wx.Size(-1,-1)
        if type(size) == type(()):
            size = wx.Size(size[0], size[1])
        size = wx.Size(size.width, size.height)  # make a copy

        best = self.GetBestSize()
        if size.width == -1:
            size.width = best.width
        if size.height == -1:
            size.height = best.height

        self.SetSize(size)


    def DoGetBestSize(self):
        """Overridden base class virtual.  Determines the best size of the
        button based on the label and bezel size."""
        w, h, useMin = self._GetLabelSize()
        defSize = wx.Button.GetDefaultSize()
        width = 12 + w
        if useMin and width < defSize.width:
           width = defSize.width
        height = 11 + h
        if useMin and height < defSize.height:
            height = defSize.height
        width = width + self.bezelWidth - 1
        height = height + self.bezelWidth - 1
        return (width, height)


    def AcceptsFocus(self):
        """Overridden base class virtual."""
        return self.IsShown() and self.IsEnabled()


    def Enable(self, enable=True):
        wx.PyControl.Enable(self, enable)
        self.Refresh()


    def SetBezelWidth(self, width):
        """Set the width of the 3D effect"""
        self.bezelWidth = width

    def GetBezelWidth(self):
        """Return the width of the 3D effect"""
        return self.bezelWidth

    def SetUseFocusIndicator(self, flag):
        """Specifiy if a focus indicator (dotted line) should be used"""
        self.useFocusInd = flag

    def GetUseFocusIndicator(self):
        """Return focus indicator flag"""
        return self.useFocusInd


    def InitColours(self):
        faceClr      = wx.SystemSettings.GetColour(wx.SYS_COLOUR_BTNFACE)
        textClr      = wx.SystemSettings.GetColour(wx.SYS_COLOUR_BTNTEXT)
        self.faceDnClr = faceClr
        self.SetBackgroundColour(faceClr)
        self.SetForegroundColour(textClr)

        shadowClr    = wx.SystemSettings.GetColour(wx.SYS_COLOUR_BTNSHADOW)
        highlightClr = wx.SystemSettings.GetColour(wx.SYS_COLOUR_BTNHIGHLIGHT)
        self.shadowPen    = wx.Pen(shadowClr, 1, wx.SOLID)
        self.highlightPen = wx.Pen(highlightClr, 1, wx.SOLID)
        if wx.Platform == "__WXMAC__":
            self.focusIndPen = wx.Pen(textClr, 1, wx.SOLID)
        else:
            self.focusIndPen  = wx.Pen(textClr, 1, wx.USER_DASH)
            self.focusIndPen.SetDashes([1,1])
            self.focusIndPen.SetCap(wx.CAP_BUTT)
        self.focusClr = highlightClr


    def SetBackgroundColour(self, colour):
        wx.PyControl.SetBackgroundColour(self, colour)
        colour = self.GetBackgroundColour()

        # Calculate a new set of highlight and shadow colours based on
        # the new background colour.  Works okay if the colour is dark...
        r, g, b = colour.Get()
        fr, fg, fb = min(255,r+32), min(255,g+32), min(255,b+32)
        self.faceDnClr = wx.Colour(fr, fg, fb)
        sr, sg, sb = max(0,r-32), max(0,g-32), max(0,b-32)
        self.shadowPen = wx.Pen(wx.Colour(sr,sg,sb), 1, wx.SOLID)
        hr, hg, hb = min(255,r+64), min(255,g+64), min(255,b+64)
        self.highlightPen = wx.Pen(wx.Colour(hr,hg,hb), 1, wx.SOLID)
        self.focusClr = wx.Colour(hr, hg, hb)


    def _GetLabelSize(self):
        """ used internally """
        w, h = self.GetTextExtent(self.GetLabel())
        return w, h, True


    def Notify(self):
        evt = GenButtonEvent(wx.wxEVT_COMMAND_BUTTON_CLICKED, self.GetId())
        evt.SetIsDown(not self.up)
        evt.SetButtonObj(self)
        evt.SetEventObject(self)
        self.GetEventHandler().ProcessEvent(evt)


    def DrawBezel(self, dc, x1, y1, x2, y2):
        # draw the upper left sides
        if self.up:
            dc.SetPen(self.highlightPen)
        else:
            dc.SetPen(self.shadowPen)
        for i in range(self.bezelWidth):
            dc.DrawLine((x1+i, y1), (x1+i, y2-i))
            dc.DrawLine((x1, y1+i), (x2-i, y1+i))

        # draw the lower right sides
        if self.up:
            dc.SetPen(self.shadowPen)
        else:
            dc.SetPen(self.highlightPen)
        for i in range(self.bezelWidth):
            dc.DrawLine((x1+i, y2-i), (x2+1, y2-i))
            dc.DrawLine((x2-i, y1+i), (x2-i, y2))


    def DrawLabel(self, dc, width, height, dw=0, dy=0):
        dc.SetFont(self.GetFont())
        if self.IsEnabled():
            dc.SetTextForeground(self.GetForegroundColour())
        else:
            dc.SetTextForeground(wx.SystemSettings.GetColour(wx.SYS_COLOUR_GRAYTEXT))
        label = self.GetLabel()
        tw, th = dc.GetTextExtent(label)
        if not self.up:
            dw = dy = self.labelDelta
        dc.DrawText(label, ((width-tw)/2+dw, (height-th)/2+dy))


    def DrawFocusIndicator(self, dc, w, h):
        bw = self.bezelWidth
##         if self.hasFocus:
##             self.focusIndPen.SetColour(self.GetForegroundColour())
##         else:
##             #self.focusIndPen.SetColour(self.GetBackgroundColour())
##             self.focusIndPen.SetColour(self.GetForegroundColour())
        self.focusIndPen.SetColour(self.focusClr)
        dc.SetLogicalFunction(wx.INVERT)
        dc.SetPen(self.focusIndPen)
        dc.SetBrush(wx.TRANSPARENT_BRUSH)
        dc.DrawRectangle((bw+2,bw+2), (w-bw*2-4, h-bw*2-4))
        dc.SetLogicalFunction(wx.COPY)


    def OnPaint(self, event):
        (width, height) = self.GetClientSizeTuple()
        x1 = y1 = 0
        x2 = width-1
        y2 = height-1
        dc = wx.BufferedPaintDC(self)
        if self.up:
            dc.SetBackground(wx.Brush(self.GetBackgroundColour(), wx.SOLID))
        else:
            dc.SetBackground(wx.Brush(self.faceDnClr, wx.SOLID))
        dc.Clear()
        self.DrawBezel(dc, x1, y1, x2, y2)
        self.DrawLabel(dc, width, height)
        if self.hasFocus and self.useFocusInd:
            self.DrawFocusIndicator(dc, width, height)


    def OnEraseBackground(self, event):
        pass


    def OnLeftDown(self, event):
        if not self.IsEnabled():
            return
        self.up = False
        self.CaptureMouse()
        self.SetFocus()
        self.Refresh()
        event.Skip()


    def OnLeftUp(self, event):
        if not self.IsEnabled() or not self.HasCapture():
            return
        if self.HasCapture():
            self.ReleaseMouse()
            if not self.up:    # if the button was down when the mouse was released...
                self.Notify()
            self.up = True
            self.Refresh()
            event.Skip()


    def OnMotion(self, event):
        if not self.IsEnabled() or not self.HasCapture():
            return
        if event.LeftIsDown() and self.HasCapture():
            x,y = event.GetPositionTuple()
            w,h = self.GetClientSizeTuple()
            if self.up and x<w and x>=0 and y<h and y>=0:
                self.up = False
                self.Refresh()
                return
            if not self.up and (x<0 or y<0 or x>=w or y>=h):
                self.up = True
                self.Refresh()
                return
        event.Skip()


    def OnGainFocus(self, event):
        self.hasFocus = True
        dc = wx.ClientDC(self)
        w, h = self.GetClientSizeTuple()
        if self.useFocusInd:
            self.DrawFocusIndicator(dc, w, h)


    def OnLoseFocus(self, event):
        self.hasFocus = False
        dc = wx.ClientDC(self)
        w, h = self.GetClientSizeTuple()
        if self.useFocusInd:
            self.DrawFocusIndicator(dc, w, h)


    def OnKeyDown(self, event):
        if self.hasFocus and event.KeyCode() == ord(" "):
            self.up = False
            self.Refresh()
        event.Skip()


    def OnKeyUp(self, event):
        if self.hasFocus and event.KeyCode() == ord(" "):
            self.up = True
            self.Notify()
            self.Refresh()
        event.Skip()


#----------------------------------------------------------------------

class GenBitmapButton(GenButton):
    def __init__(self, parent, ID, bitmap,
                 pos = wx.DefaultPosition, size = wx.DefaultSize,
                 style = 0, validator = wx.DefaultValidator,
                 name = "genbutton"):
        self.bmpDisabled = None
        self.bmpFocus = None
        self.bmpSelected = None
        self.SetBitmapLabel(bitmap)
        GenButton.__init__(self, parent, ID, "", pos, size, style, validator, name)


    def GetBitmapLabel(self):
        return self.bmpLabel
    def GetBitmapDisabled(self):
        return self.bmpDisabled
    def GetBitmapFocus(self):
        return self.bmpFocus
    def GetBitmapSelected(self):
        return self.bmpSelected


    def SetBitmapDisabled(self, bitmap):
        """Set bitmap to display when the button is disabled"""
        self.bmpDisabled = bitmap

    def SetBitmapFocus(self, bitmap):
        """Set bitmap to display when the button has the focus"""
        self.bmpFocus = bitmap
        self.SetUseFocusIndicator(False)

    def SetBitmapSelected(self, bitmap):
        """Set bitmap to display when the button is selected (pressed down)"""
        self.bmpSelected = bitmap

    def SetBitmapLabel(self, bitmap, createOthers=True):
        """
        Set the bitmap to display normally.
        This is the only one that is required. If
        createOthers is True, then the other bitmaps
        will be generated on the fly.  Currently,
        only the disabled bitmap is generated.
        """
        self.bmpLabel = bitmap
        if bitmap is not None and createOthers:
            image = wx.ImageFromBitmap(bitmap)
            imageutils.grayOut(image)
            self.SetBitmapDisabled(wx.BitmapFromImage(image))


    def _GetLabelSize(self):
        """ used internally """
        if not self.bmpLabel:
            return -1, -1, False
        return self.bmpLabel.GetWidth()+2, self.bmpLabel.GetHeight()+2, False

    def DrawLabel(self, dc, width, height, dw=0, dy=0):
        bmp = self.bmpLabel
        if self.bmpDisabled and not self.IsEnabled():
            bmp = self.bmpDisabled
        if self.bmpFocus and self.hasFocus:
            bmp = self.bmpFocus
        if self.bmpSelected and not self.up:
            bmp = self.bmpSelected
        bw,bh = bmp.GetWidth(), bmp.GetHeight()
        if not self.up:
            dw = dy = self.labelDelta
        hasMask = bmp.GetMask() != None
        dc.DrawBitmap(bmp, ((width-bw)/2+dw, (height-bh)/2+dy), hasMask)


#----------------------------------------------------------------------


class GenBitmapTextButton(GenBitmapButton):     # generic bitmapped button with Text Label
    def __init__(self, parent, ID, bitmap, label,
                 pos = wx.DefaultPosition, size = wx.DefaultSize,
                 style = 0, validator = wx.DefaultValidator,
                 name = "genbutton"):
        GenBitmapButton.__init__(self, parent, ID, bitmap, pos, size, style, validator, name)
        self.SetLabel(label)


    def _GetLabelSize(self):
        """ used internally """
        w, h = self.GetTextExtent(self.GetLabel())
        if not self.bmpLabel:
            return w, h, True       # if there isn't a bitmap use the size of the text

        w_bmp = self.bmpLabel.GetWidth()+2
        h_bmp = self.bmpLabel.GetHeight()+2
        width = w + w_bmp
        if h_bmp > h:
            height = h_bmp
        else:
            height = h
        return width, height, True


    def DrawLabel(self, dc, width, height, dw=0, dy=0):
        bmp = self.bmpLabel
        if bmp != None:     # if the bitmap is used
            if self.bmpDisabled and not self.IsEnabled():
                bmp = self.bmpDisabled
            if self.bmpFocus and self.hasFocus:
                bmp = self.bmpFocus
            if self.bmpSelected and not self.up:
                bmp = self.bmpSelected
            bw,bh = bmp.GetWidth(), bmp.GetHeight()
            if not self.up:
                dw = dy = self.labelDelta
            hasMask = bmp.GetMask() != None
        else:
            bw = bh = 0     # no bitmap -> size is zero

        dc.SetFont(self.GetFont())
        if self.IsEnabled():
            dc.SetTextForeground(self.GetForegroundColour())
        else:
            dc.SetTextForeground(wx.SystemSettings.GetColour(wx.SYS_COLOUR_GRAYTEXT))

        label = self.GetLabel()
        tw, th = dc.GetTextExtent(label)        # size of text
        if not self.up:
            dw = dy = self.labelDelta

        pos_x = (width-bw-tw)/2+dw      # adjust for bitmap and text to centre
        if bmp !=None:
            dc.DrawBitmap(bmp, (pos_x, (height-bh)/2+dy), hasMask) # draw bitmap if available
            pos_x = pos_x + 2   # extra spacing from bitmap

        dc.DrawText(label, (pos_x + dw+bw, (height-th)/2+dy))      # draw the text


#----------------------------------------------------------------------


class __ToggleMixin:
    def SetToggle(self, flag):
        self.up = not flag
        self.Refresh()
    SetValue = SetToggle

    def GetToggle(self):
        return not self.up
    GetValue = GetToggle

    def OnLeftDown(self, event):
        if not self.IsEnabled():
            return
        self.saveUp = self.up
        self.up = not self.up
        self.CaptureMouse()
        self.SetFocus()
        self.Refresh()

    def OnLeftUp(self, event):
        if not self.IsEnabled() or not self.HasCapture():
            return
        if self.HasCapture():
            if self.up != self.saveUp:
                self.Notify()
            self.ReleaseMouse()
            self.Refresh()

    def OnKeyDown(self, event):
        event.Skip()

    def OnMotion(self, event):
        if not self.IsEnabled():
            return
        if event.LeftIsDown() and self.HasCapture():
            x,y = event.GetPositionTuple()
            w,h = self.GetClientSizeTuple()
            if x<w and x>=0 and y<h and y>=0:
                self.up = not self.saveUp
                self.Refresh()
                return
            if (x<0 or y<0 or x>=w or y>=h):
                self.up = self.saveUp
                self.Refresh()
                return
        event.Skip()

    def OnKeyUp(self, event):
        if self.hasFocus and event.KeyCode() == ord(" "):
            self.up = not self.up
            self.Notify()
            self.Refresh()
        event.Skip()




class GenToggleButton(__ToggleMixin, GenButton):
    pass

class GenBitmapToggleButton(__ToggleMixin, GenBitmapButton):
    pass

class GenBitmapTextToggleButton(__ToggleMixin, GenBitmapTextButton):
    pass

#----------------------------------------------------------------------


