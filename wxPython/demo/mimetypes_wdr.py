#-----------------------------------------------------------------------------
# Python source generated by wxDesigner from file: mimetypes.wdr
# Do not modify this file, all changes will be lost!
#-----------------------------------------------------------------------------

# Include wxWindows' modules
from wxPython.wx import *

# Window functions

ID_INPUT_TEXT = 10000
ID_EXTENSION_Btn = 10001
ID_MIME_BTN = 10002
ID_LOOKUP_BTN = 10003
ID_LINE = 10004
ID_TEXT = 10005
ID_ICON_BMP = 10006
ID_ICON_FILE_TXT = 10007
ID_ICON_INDEX_TXT = 10008
ID_MIME_TYPE_TXT = 10009
ID_MIME_TYPES_TXT = 10010
ID_EXTENSIONS_TXT = 10011
ID_DESCRIPTION_TXT = 10012
ID_OPEN_CMD_TXT = 10013
ID_PRINT_CMD_TXT = 10014
ID_ALL_CMDS_TXT = 10015
ID_LISTBOX = 10016

def MakeMimeTypesTestPanel( parent, call_fit = True, set_sizer = True ):
    item0 = wxBoxSizer( wxVERTICAL )

    item1 = wxBoxSizer( wxHORIZONTAL )

    item2 = wxTextCtrl( parent, ID_INPUT_TEXT, "", wxDefaultPosition, wxSize(100,-1), 0 )
    item1.AddWindow( item2, 0, wxALIGN_CENTRE|wxALL, 5 )

    item3 = wxRadioButton( parent, ID_EXTENSION_Btn, "By extension", wxDefaultPosition, wxDefaultSize, wxRB_GROUP )
    item3.SetValue( True )
    item1.AddWindow( item3, 0, wxALIGN_CENTRE|wxALL, 5 )

    item4 = wxRadioButton( parent, ID_MIME_BTN, "By mime type", wxDefaultPosition, wxDefaultSize, 0 )
    item1.AddWindow( item4, 0, wxALIGN_CENTRE|wxALL, 5 )

    item5 = wxButton( parent, ID_LOOKUP_BTN, "Lookup", wxDefaultPosition, wxDefaultSize, 0 )
    item5.SetDefault()
    item1.AddWindow( item5, 0, wxALIGN_CENTRE|wxALL, 5 )

    item0.AddSizer( item1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 )

    item6 = wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL )
    item0.AddWindow( item6, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 )

    item7 = wxBoxSizer( wxHORIZONTAL )

    item8 = wxFlexGridSizer( 0, 2, 0, 0 )
    item8.AddGrowableCol( 1 )
    item8.AddGrowableRow( 8 )

    item9 = wxStaticText( parent, ID_TEXT, "wxFileType:", wxDefaultPosition, wxDefaultSize, 0 )
    item9.SetFont( wxFont( 14, wxSWISS, wxNORMAL, wxBOLD ) )
    item8.AddWindow( item9, 0, wxALL, 5 )

    item8.AddSpacer( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 )

    item10 = wxStaticText( parent, ID_TEXT, "GetIconInfo:", wxDefaultPosition, wxDefaultSize, 0 )
    item8.AddWindow( item10, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 )

    item11 = wxBoxSizer( wxHORIZONTAL )

    item12 = wxStaticBitmap( parent, ID_ICON_BMP, MyBitmapsFunc( 0 ), wxDefaultPosition, wxDefaultSize )
    item11.AddWindow( item12, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 )

    item13 = wxTextCtrl( parent, ID_ICON_FILE_TXT, "", wxDefaultPosition, wxSize(120,-1), wxTE_READONLY )
    item11.AddWindow( item13, 1, wxALIGN_CENTRE|wxRIGHT|wxTOP|wxBOTTOM, 5 )

    item14 = wxTextCtrl( parent, ID_ICON_INDEX_TXT, "", wxDefaultPosition, wxSize(30,-1), wxTE_READONLY )
    item11.AddWindow( item14, 0, wxALIGN_CENTRE|wxRIGHT|wxTOP|wxBOTTOM, 5 )

    item8.AddSizer( item11, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 )

    item15 = wxStaticText( parent, ID_TEXT, "GetMimeType:", wxDefaultPosition, wxDefaultSize, 0 )
    item8.AddWindow( item15, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5 )

    item16 = wxTextCtrl( parent, ID_MIME_TYPE_TXT, "", wxDefaultPosition, wxSize(310,-1), wxTE_READONLY )
    item8.AddWindow( item16, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5 )

    item17 = wxStaticText( parent, ID_TEXT, "GetMimeTypes:", wxDefaultPosition, wxDefaultSize, 0 )
    item8.AddWindow( item17, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5 )

    item18 = wxTextCtrl( parent, ID_MIME_TYPES_TXT, "", wxDefaultPosition, wxSize(80,-1), wxTE_READONLY )
    item8.AddWindow( item18, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5 )

    item19 = wxStaticText( parent, ID_TEXT, "GetExtensions:", wxDefaultPosition, wxDefaultSize, 0 )
    item8.AddWindow( item19, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5 )

    item20 = wxTextCtrl( parent, ID_EXTENSIONS_TXT, "", wxDefaultPosition, wxSize(80,-1), wxTE_READONLY )
    item8.AddWindow( item20, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5 )

    item21 = wxStaticText( parent, ID_TEXT, "GetDescription:", wxDefaultPosition, wxDefaultSize, 0 )
    item8.AddWindow( item21, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5 )

    item22 = wxTextCtrl( parent, ID_DESCRIPTION_TXT, "", wxDefaultPosition, wxSize(80,-1), wxTE_READONLY )
    item8.AddWindow( item22, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5 )

    item23 = wxStaticText( parent, ID_TEXT, "GetOpenCommand:", wxDefaultPosition, wxDefaultSize, 0 )
    item8.AddWindow( item23, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5 )

    item24 = wxTextCtrl( parent, ID_OPEN_CMD_TXT, "", wxDefaultPosition, wxSize(80,-1), wxTE_READONLY )
    item8.AddWindow( item24, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5 )

    item25 = wxStaticText( parent, ID_TEXT, "GetPrintCommand:", wxDefaultPosition, wxDefaultSize, 0 )
    item8.AddWindow( item25, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5 )

    item26 = wxTextCtrl( parent, ID_PRINT_CMD_TXT, "", wxDefaultPosition, wxSize(80,-1), wxTE_READONLY )
    item8.AddWindow( item26, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5 )

    item27 = wxStaticText( parent, ID_TEXT, "GetAllCommands:", wxDefaultPosition, wxDefaultSize, 0 )
    item8.AddWindow( item27, 0, wxALIGN_RIGHT|wxLEFT|wxRIGHT|wxBOTTOM, 5 )

    item28 = wxTextCtrl( parent, ID_ALL_CMDS_TXT, "", wxDefaultPosition, wxSize(80,100), wxTE_MULTILINE|wxTE_READONLY|wxHSCROLL )
    item8.AddWindow( item28, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5 )

    item7.AddSizer( item8, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 )

    item29 = wxBoxSizer( wxVERTICAL )

    item30 = wxStaticText( parent, ID_TEXT, "Known mime types:", wxDefaultPosition, wxDefaultSize, 0 )
    item29.AddWindow( item30, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM, 5 )

    item31 = wxListBox( parent, ID_LISTBOX, wxDefaultPosition, wxSize(150,100), [], wxLB_SINGLE|wxLB_SORT|wxLB_HSCROLL )
    item29.AddWindow( item31, 1, wxALIGN_CENTRE|wxRIGHT|wxBOTTOM, 5 )

    item7.AddSizer( item29, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxRIGHT|wxTOP|wxBOTTOM, 5 )

    item0.AddSizer( item7, 1, wxGROW|wxALIGN_CENTER_VERTICAL, 5 )

    if set_sizer == True:
        parent.SetAutoLayout( True )
        parent.SetSizer( item0 )
        if call_fit == True:
            item0.Fit( parent )
            item0.SetSizeHints( parent )

    return item0

# Menu bar functions

# Bitmap functions

def MyBitmapsFunc( index ):
    if index == 0:
        return wxImage( "mimetypes_wdr/MyBitmapsFunc_0.png", wxBITMAP_TYPE_PNG ).ConvertToBitmap()
    return wxNullBitmap

# End of generated file
