from wxPython.wx import *
from wxPython.grid import *
from wxPython.lib.mixins.grid import wxGridAutoEditMixin

#---------------------------------------------------------------------------

class SimpleGrid(wxGrid): ##, wxGridAutoEditMixin):
    def __init__(self, parent, log):
        wxGrid.__init__(self, parent, -1)
        ##wxGridAutoEditMixin.__init__(self)
        self.log = log
        self.moveTo = None

        EVT_IDLE(self, self.OnIdle)

        self.CreateGrid(25, 25)
        ##self.EnableEditing(false)

        # simple cell formatting
        self.SetColSize(3, 200)
        self.SetRowSize(4, 45)
        self.SetCellValue(0, 0, "First cell")
        self.SetCellValue(1, 1, "Another cell")
        self.SetCellValue(2, 2, "Yet another cell")
        self.SetCellValue(3, 3, "This cell is read-only")
        self.SetCellFont(0, 0, wxFont(12, wxROMAN, wxITALIC, wxNORMAL))
        self.SetCellTextColour(1, 1, wxRED)
        self.SetCellBackgroundColour(2, 2, wxCYAN)
        self.SetReadOnly(3, 3, true)

        self.SetCellEditor(5, 0, wxGridCellNumberEditor())
        self.SetCellValue(5, 0, "123")
        self.SetCellEditor(6, 0, wxGridCellFloatEditor())
        self.SetCellValue(6, 0, "123.34")
        self.SetCellEditor(7, 0, wxGridCellNumberEditor())

        self.SetCellValue(6, 3, "You can veto editing this cell")


        # attribute objects let you keep a set of formatting values
        # in one spot, and reuse them if needed
        attr = wxGridCellAttr()
        attr.SetTextColour(wxBLACK)
        attr.SetBackgroundColour(wxRED)
        attr.SetFont(wxFont(10, wxSWISS, wxNORMAL, wxBOLD))

        # you can set cell attributes for the whole row (or column)
        self.SetRowAttr(5, attr)

        self.SetColLabelValue(0, "Custom")
        self.SetColLabelValue(1, "column")
        self.SetColLabelValue(2, "labels")

        self.SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_BOTTOM)

        # overflow cells
        self.SetCellValue( 9, 1, "This default cell will overflow into neighboring cells, but not if you turn overflow off.");
        self.SetCellSize(11, 1, 3, 3);
        self.SetCellAlignment(11, 1, wxALIGN_CENTRE, wxALIGN_CENTRE);
        self.SetCellValue(11, 1, "This cell is set to span 3 rows and 3 columns");



        # test all the events
        EVT_GRID_CELL_LEFT_CLICK(self, self.OnCellLeftClick)
        EVT_GRID_CELL_RIGHT_CLICK(self, self.OnCellRightClick)
        EVT_GRID_CELL_LEFT_DCLICK(self, self.OnCellLeftDClick)
        EVT_GRID_CELL_RIGHT_DCLICK(self, self.OnCellRightDClick)

        EVT_GRID_LABEL_LEFT_CLICK(self, self.OnLabelLeftClick)
        EVT_GRID_LABEL_RIGHT_CLICK(self, self.OnLabelRightClick)
        EVT_GRID_LABEL_LEFT_DCLICK(self, self.OnLabelLeftDClick)
        EVT_GRID_LABEL_RIGHT_DCLICK(self, self.OnLabelRightDClick)

        EVT_GRID_ROW_SIZE(self, self.OnRowSize)
        EVT_GRID_COL_SIZE(self, self.OnColSize)

        EVT_GRID_RANGE_SELECT(self, self.OnRangeSelect)
        EVT_GRID_CELL_CHANGE(self, self.OnCellChange)
        EVT_GRID_SELECT_CELL(self, self.OnSelectCell)

        EVT_GRID_EDITOR_SHOWN(self, self.OnEditorShown)
        EVT_GRID_EDITOR_HIDDEN(self, self.OnEditorHidden)
        EVT_GRID_EDITOR_CREATED(self, self.OnEditorCreated)



    def OnCellLeftClick(self, evt):
        self.log.write("OnCellLeftClick: (%d,%d) %s\n" %
                       (evt.GetRow(), evt.GetCol(), evt.GetPosition()))
        evt.Skip()

    def OnCellRightClick(self, evt):
        self.log.write("OnCellRightClick: (%d,%d) %s\n" %
                       (evt.GetRow(), evt.GetCol(), evt.GetPosition()))
        evt.Skip()

    def OnCellLeftDClick(self, evt):
        self.log.write("OnCellLeftDClick: (%d,%d) %s\n" %
                       (evt.GetRow(), evt.GetCol(), evt.GetPosition()))
        evt.Skip()

    def OnCellRightDClick(self, evt):
        self.log.write("OnCellRightDClick: (%d,%d) %s\n" %
                       (evt.GetRow(), evt.GetCol(), evt.GetPosition()))
        evt.Skip()

    def OnLabelLeftClick(self, evt):
        self.log.write("OnLabelLeftClick: (%d,%d) %s\n" %
                       (evt.GetRow(), evt.GetCol(), evt.GetPosition()))
        evt.Skip()

    def OnLabelRightClick(self, evt):
        self.log.write("OnLabelRightClick: (%d,%d) %s\n" %
                       (evt.GetRow(), evt.GetCol(), evt.GetPosition()))
        evt.Skip()

    def OnLabelLeftDClick(self, evt):
        self.log.write("OnLabelLeftDClick: (%d,%d) %s\n" %
                       (evt.GetRow(), evt.GetCol(), evt.GetPosition()))
        evt.Skip()

    def OnLabelRightDClick(self, evt):
        self.log.write("OnLabelRightDClick: (%d,%d) %s\n" %
                       (evt.GetRow(), evt.GetCol(), evt.GetPosition()))
        evt.Skip()


    def OnRowSize(self, evt):
        self.log.write("OnRowSize: row %d, %s\n" %
                       (evt.GetRowOrCol(), evt.GetPosition()))
        evt.Skip()

    def OnColSize(self, evt):
        self.log.write("OnColSize: col %d, %s\n" %
                       (evt.GetRowOrCol(), evt.GetPosition()))
        evt.Skip()

    def OnRangeSelect(self, evt):
        if evt.Selecting():
            self.log.write("OnRangeSelect: top-left %s, bottom-right %s\n" %
                           (evt.GetTopLeftCoords(), evt.GetBottomRightCoords()))
        evt.Skip()


    def OnCellChange(self, evt):
        self.log.write("OnCellChange: (%d,%d) %s\n" %
                       (evt.GetRow(), evt.GetCol(), evt.GetPosition()))

        # Show how to stay in a cell that has bad data.  We can't just
        # call SetGridCursor here since we are nested inside one so it
        # won't have any effect.  Instead, set coordinants to move to in
        # idle time.
        value = self.GetCellValue(evt.GetRow(), evt.GetCol())
        if value == 'no good':
            self.moveTo = evt.GetRow(), evt.GetCol()


    def OnIdle(self, evt):
        if self.moveTo != None:
            self.SetGridCursor(self.moveTo[0], self.moveTo[1])
            self.moveTo = None
        evt.Skip()


    def OnSelectCell(self, evt):
        self.log.write("OnSelectCell: (%d,%d) %s\n" %
                       (evt.GetRow(), evt.GetCol(), evt.GetPosition()))

        # Another way to stay in a cell that has a bad value...
        row = self.GetGridCursorRow()
        col = self.GetGridCursorCol()
        if self.IsCellEditControlEnabled():
            self.HideCellEditControl()
            self.DisableCellEditControl()
        value = self.GetCellValue(row, col)
        if value == 'no good 2':
            return  # cancels the cell selection
        evt.Skip()


    def OnEditorShown(self, evt):
        if evt.GetRow() == 6 and evt.GetCol() == 3 and \
           wxMessageBox("Are you sure you wish to edit this cell?",
                        "Checking", wxYES_NO) == wxNO:
            evt.Veto()
            return

        self.log.write("OnEditorShown: (%d,%d) %s\n" %
                       (evt.GetRow(), evt.GetCol(), evt.GetPosition()))
        evt.Skip()


    def OnEditorHidden(self, evt):
        if evt.GetRow() == 6 and evt.GetCol() == 3 and \
           wxMessageBox("Are you sure you wish to  finish editing this cell?",
                        "Checking", wxYES_NO) == wxNO:
            evt.Veto()
            return

        self.log.write("OnEditorHidden: (%d,%d) %s\n" %
                       (evt.GetRow(), evt.GetCol(), evt.GetPosition()))
        evt.Skip()


    def OnEditorCreated(self, evt):
        self.log.write("OnEditorCreated: (%d, %d) %s\n" %
                       (evt.GetRow(), evt.GetCol(), evt.GetControl()))



#---------------------------------------------------------------------------

class TestFrame(wxFrame):
    def __init__(self, parent, log):
        wxFrame.__init__(self, parent, -1, "Simple Grid Demo", size=(640,480))
        grid = SimpleGrid(self, log)



#---------------------------------------------------------------------------

if __name__ == '__main__':
    import sys
    app = wxPySimpleApp()
    frame = TestFrame(None, sys.stdout)
    frame.Show(true)
    app.MainLoop()


#---------------------------------------------------------------------------


