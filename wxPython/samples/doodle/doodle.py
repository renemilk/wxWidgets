# doodle.py

"""
This module contains the DoodleWindow class which is a window that you
can do simple drawings upon.
"""


from wxPython.wx import *

#----------------------------------------------------------------------

class DoodleWindow(wxWindow):
    menuColours = { 100 : 'Black',
                    101 : 'Yellow',
                    102 : 'Red',
                    103 : 'Green',
                    104 : 'Blue',
                    105 : 'Purple',
                    106 : 'Brown',
                    107 : 'Aquamarine',
                    108 : 'Forest Green',
                    109 : 'Light Blue',
                    110 : 'Goldenrod',
                    111 : 'Cyan',
                    112 : 'Orange',
                    113 : 'Navy',
                    114 : 'Dark Grey',
                    115 : 'Light Grey',
                    }
    maxThickness = 16


    def __init__(self, parent, ID):
        wxWindow.__init__(self, parent, ID, style=wxNO_FULL_REPAINT_ON_RESIZE)
        self.SetBackgroundColour("WHITE")
        self.listeners = []
        self.thickness = 1
        self.SetColour("Black")
        self.lines = []
        self.x = self.y = 0
        self.MakeMenu()

        self.InitBuffer()

        # hook some mouse events
        EVT_LEFT_DOWN(self, self.OnLeftDown)
        EVT_LEFT_UP(self, self.OnLeftUp)
        EVT_RIGHT_UP(self, self.OnRightUp)
        EVT_MOTION(self, self.OnMotion)

        # the window resize event and idle events for managing the buffer
        EVT_SIZE(self, self.OnSize)
        EVT_IDLE(self, self.OnIdle)

        # and the refresh event
        EVT_PAINT(self, self.OnPaint)

        # When the window is destroyed, clean up resources.
        EVT_WINDOW_DESTROY(self, self.Cleanup)


    def Cleanup(self, evt):
        if hasattr(self, "menu"):
            self.menu.Destroy()
            del self.menu


    def InitBuffer(self):
        """Initialize the bitmap used for buffering the display."""
        size = self.GetClientSize()
        self.buffer = wxEmptyBitmap(size.width, size.height)
        dc = wxBufferedDC(None, self.buffer)
        dc.SetBackground(wxBrush(self.GetBackgroundColour()))
        dc.Clear()
        self.DrawLines(dc)
        self.reInitBuffer = False


    def SetColour(self, colour):
        """Set a new colour and make a matching pen"""
        self.colour = colour
        self.pen = wxPen(wxNamedColour(self.colour), self.thickness, wxSOLID)
        self.Notify()


    def SetThickness(self, num):
        """Set a new line thickness and make a matching pen"""
        self.thickness = num
        self.pen = wxPen(wxNamedColour(self.colour), self.thickness, wxSOLID)
        self.Notify()


    def GetLinesData(self):
        return self.lines[:]


    def SetLinesData(self, lines):
        self.lines = lines[:]
        self.InitBuffer()
        self.Refresh()


    def MakeMenu(self):
        """Make a menu that can be popped up later"""
        menu = wxMenu()
        keys = self.menuColours.keys()
        keys.sort()
        for k in keys:
            text = self.menuColours[k]
            menu.Append(k, text, kind=wxITEM_CHECK)
        EVT_MENU_RANGE(self, 100, 200, self.OnMenuSetColour)
        EVT_UPDATE_UI_RANGE(self, 100, 200, self.OnCheckMenuColours)
        menu.Break()

        for x in range(1, self.maxThickness+1):
            menu.Append(x, str(x), kind=wxITEM_CHECK)
        EVT_MENU_RANGE(self, 1, self.maxThickness, self.OnMenuSetThickness)
        EVT_UPDATE_UI_RANGE(self, 1, self.maxThickness, self.OnCheckMenuThickness)
        self.menu = menu


    # These two event handlers are called before the menu is displayed
    # to determine which items should be checked.
    def OnCheckMenuColours(self, event):
        text = self.menuColours[event.GetId()]
        if text == self.colour:
            event.Check(True)
        else:
            event.Check(False)
    def OnCheckMenuThickness(self, event):
        if event.GetId() == self.thickness:
            event.Check(True)
        else:
            event.Check(False)


    def OnLeftDown(self, event):
        """called when the left mouse button is pressed"""
        self.curLine = []
        self.x, self.y = event.GetPositionTuple()
        self.CaptureMouse()


    def OnLeftUp(self, event):
        """called when the left mouse button is released"""
        if self.HasCapture():
            self.lines.append( (self.colour, self.thickness, self.curLine) )
            self.curLine = []
            self.ReleaseMouse()


    def OnRightUp(self, event):
        """called when the right mouse button is released, will popup the menu"""
        pt = event.GetPosition()
        self.PopupMenu(self.menu, pt)



    def OnMotion(self, event):
        """
        Called when the mouse is in motion.  If the left button is
        dragging then draw a line from the last event position to the
        current one.  Save the coordinants for redraws.
        """
        if event.Dragging() and event.LeftIsDown():
            dc = wxBufferedDC(wxClientDC(self), self.buffer)
            dc.BeginDrawing()
            dc.SetPen(self.pen)
            pos = event.GetPositionTuple()
            coords = (self.x, self.y) + pos
            self.curLine.append(coords)
            dc.DrawLine(self.x, self.y, pos[0], pos[1])
            self.x, self.y = pos
            dc.EndDrawing()


    def OnSize(self, event):
        """
        Called when the window is resized.  We set a flag so the idle
        handler will resize the buffer.
        """
        self.reInitBuffer = True


    def OnIdle(self, event):
        """
        If the size was changed then resize the bitmap used for double
        buffering to match the window size.  We do it in Idle time so
        there is only one refresh after resizing is done, not lots while
        it is happening.
        """
        if self.reInitBuffer:
            self.InitBuffer()
            self.Refresh(False)


    def OnPaint(self, event):
        """
        Called when the window is exposed.
        """
        # Create a buffered paint DC.  It will create the real
        # wxPaintDC and then blit the bitmap to it when dc is
        # deleted.  Since we don't need to draw anything else
        # here that's all there is to it.
        dc = wxBufferedPaintDC(self, self.buffer)


    def DrawLines(self, dc):
        """
        Redraws all the lines that have been drawn already.
        """
        dc.BeginDrawing()
        for colour, thickness, line in self.lines:
            pen = wxPen(wxNamedColour(colour), thickness, wxSOLID)
            dc.SetPen(pen)
            for coords in line:
                apply(dc.DrawLine, coords)
        dc.EndDrawing()


    # Event handlers for the popup menu, uses the event ID to determine
    # the colour or the thickness to set.
    def OnMenuSetColour(self, event):
        self.SetColour(self.menuColours[event.GetId()])

    def OnMenuSetThickness(self, event):
        self.SetThickness(event.GetId())


    # Observer pattern.  Listeners are registered and then notified
    # whenever doodle settings change.
    def AddListener(self, listener):
        self.listeners.append(listener)

    def Notify(self):
        for other in self.listeners:
            other.Update(self.colour, self.thickness)


#----------------------------------------------------------------------

class DoodleFrame(wxFrame):
    def __init__(self, parent):
        wxFrame.__init__(self, parent, -1, "Doodle Frame", size=(800,600),
                         style=wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE)
        doodle = DoodleWindow(self, -1)

#----------------------------------------------------------------------

if __name__ == '__main__':
    app = wxPySimpleApp()
    frame = DoodleFrame(None)
    frame.Show(True)
    app.MainLoop()

