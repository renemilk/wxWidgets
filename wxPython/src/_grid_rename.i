// A bunch of %rename directives generated by BuildRenamers in config.py
// in order to remove the wx prefix from all global scope names.

#ifndef BUILDING_RENAMERS

%rename(GRID_VALUE_STRING)                  wxGRID_VALUE_STRING;
%rename(GRID_VALUE_BOOL)                    wxGRID_VALUE_BOOL;
%rename(GRID_VALUE_NUMBER)                  wxGRID_VALUE_NUMBER;
%rename(GRID_VALUE_FLOAT)                   wxGRID_VALUE_FLOAT;
%rename(GRID_VALUE_CHOICE)                  wxGRID_VALUE_CHOICE;
%rename(GRID_VALUE_TEXT)                    wxGRID_VALUE_TEXT;
%rename(GRID_VALUE_LONG)                    wxGRID_VALUE_LONG;
%rename(GRID_VALUE_CHOICEINT)               wxGRID_VALUE_CHOICEINT;
%rename(GRID_VALUE_DATETIME)                wxGRID_VALUE_DATETIME;
%rename(GridNoCellCoords)                   wxGridNoCellCoords;
%rename(GridNoCellRect)                     wxGridNoCellRect;
%rename(GRID_DEFAULT_NUMBER_ROWS)           wxGRID_DEFAULT_NUMBER_ROWS;
%rename(GRID_DEFAULT_NUMBER_COLS)           wxGRID_DEFAULT_NUMBER_COLS;
%rename(GRID_DEFAULT_ROW_HEIGHT)            wxGRID_DEFAULT_ROW_HEIGHT;
%rename(GRID_DEFAULT_COL_WIDTH)             wxGRID_DEFAULT_COL_WIDTH;
%rename(GRID_DEFAULT_COL_LABEL_HEIGHT)      wxGRID_DEFAULT_COL_LABEL_HEIGHT;
%rename(GRID_DEFAULT_ROW_LABEL_WIDTH)       wxGRID_DEFAULT_ROW_LABEL_WIDTH;
%rename(GRID_LABEL_EDGE_ZONE)               wxGRID_LABEL_EDGE_ZONE;
%rename(GRID_MIN_ROW_HEIGHT)                wxGRID_MIN_ROW_HEIGHT;
%rename(GRID_MIN_COL_WIDTH)                 wxGRID_MIN_COL_WIDTH;
%rename(GRID_DEFAULT_SCROLLBAR_WIDTH)       wxGRID_DEFAULT_SCROLLBAR_WIDTH;
%rename(GridCellRenderer)                   wxGridCellRenderer;
%rename(PyGridCellRenderer)                 wxPyGridCellRenderer;
%rename(GridCellStringRenderer)             wxGridCellStringRenderer;
%rename(GridCellNumberRenderer)             wxGridCellNumberRenderer;
%rename(GridCellFloatRenderer)              wxGridCellFloatRenderer;
%rename(GridCellBoolRenderer)               wxGridCellBoolRenderer;
%rename(GridCellDateTimeRenderer)           wxGridCellDateTimeRenderer;
%rename(GridCellEnumRenderer)               wxGridCellEnumRenderer;
%rename(GridCellAutoWrapStringRenderer)     wxGridCellAutoWrapStringRenderer;
%rename(GridCellEditor)                     wxGridCellEditor;
%rename(PyGridCellEditor)                   wxPyGridCellEditor;
%rename(GridCellTextEditor)                 wxGridCellTextEditor;
%rename(GridCellNumberEditor)               wxGridCellNumberEditor;
%rename(GridCellFloatEditor)                wxGridCellFloatEditor;
%rename(GridCellBoolEditor)                 wxGridCellBoolEditor;
%rename(GridCellChoiceEditor)               wxGridCellChoiceEditor;
%rename(GridCellEnumEditor)                 wxGridCellEnumEditor;
%rename(GridCellAutoWrapStringEditor)       wxGridCellAutoWrapStringEditor;
%rename(GridCellAttr)                       wxGridCellAttr;
%rename(GridCellAttrProvider)               wxGridCellAttrProvider;
%rename(PyGridCellAttrProvider)             wxPyGridCellAttrProvider;
%rename(GridTableBase)                      wxGridTableBase;
%rename(PyGridTableBase)                    wxPyGridTableBase;
%rename(GridStringTable)                    wxGridStringTable;
%rename(GRIDTABLE_REQUEST_VIEW_GET_VALUES)  wxGRIDTABLE_REQUEST_VIEW_GET_VALUES;
%rename(GRIDTABLE_REQUEST_VIEW_SEND_VALUES)  wxGRIDTABLE_REQUEST_VIEW_SEND_VALUES;
%rename(GRIDTABLE_NOTIFY_ROWS_INSERTED)     wxGRIDTABLE_NOTIFY_ROWS_INSERTED;
%rename(GRIDTABLE_NOTIFY_ROWS_APPENDED)     wxGRIDTABLE_NOTIFY_ROWS_APPENDED;
%rename(GRIDTABLE_NOTIFY_ROWS_DELETED)      wxGRIDTABLE_NOTIFY_ROWS_DELETED;
%rename(GRIDTABLE_NOTIFY_COLS_INSERTED)     wxGRIDTABLE_NOTIFY_COLS_INSERTED;
%rename(GRIDTABLE_NOTIFY_COLS_APPENDED)     wxGRIDTABLE_NOTIFY_COLS_APPENDED;
%rename(GRIDTABLE_NOTIFY_COLS_DELETED)      wxGRIDTABLE_NOTIFY_COLS_DELETED;
%rename(GridTableMessage)                   wxGridTableMessage;
%rename(GridCellCoords)                     wxGridCellCoords;
%rename(Grid)                               wxGrid;
%rename(GridEvent)                          wxGridEvent;
%rename(GridSizeEvent)                      wxGridSizeEvent;
%rename(GridRangeSelectEvent)               wxGridRangeSelectEvent;
%rename(GridEditorCreatedEvent)             wxGridEditorCreatedEvent;

#endif
