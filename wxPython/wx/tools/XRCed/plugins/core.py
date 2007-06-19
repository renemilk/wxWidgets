# Name:         core.py
# Purpose:      Core componenets
# Author:       Roman Rolinsky <rolinsky@femagsoft.com>
# Created:      31.05.2007
# RCS-ID:       $Id$

from component import *
import images

# Test

print 'creating core components'

### wxFrame

c = Container('wxFrame', ['frame','window','top_level'], 
              ['pos', 'size', 'title', 'centered'],
              image=images.getTreeFrameImage())
c.addStyles('wxDEFAULT_FRAME_STYLE', 'wxDEFAULT_DIALOG_STYLE', 'wxCAPTION', 
            'wxSTAY_ON_TOP', 'wxSYSTEM_MENU', 'wxTHICK_FRAME',
            'wxRESIZE_BORDER', 'wxRESIZE_BOX', 'wxCLOSE_BOX',
            'wxMAXIMIZE_BOX', 'wxMINIMIZE_BOX',
            'wxFRAME_NO_TASKBAR', 'wxFRAME_SHAPED', 'wxFRAME_TOOL_WINDOW',
            'wxFRAME_FLOAT_ON_PARENT',
            'wxNO_3D', 'wxTAB_TRAVERSAL')
c.addExStyles('wxWS_EX_VALIDATE_RECURSIVELY', 'wxFRAME_EX_METAL')
Manager.register(c)
Manager.setMenu(c, 'root', 'Frame', 'Frame window', 10)

### wxDialog

# !!! TODO

### wxPanel

c = Container('wxPanel', ['window','top_level','control'], 
              ['pos', 'size'],
              image=images.getTreePanelImage())
c.addStyles('wxNO_3D', 'wxTAB_TRAVERSAL')
c.addExStyles('wxWS_EX_VALIDATE_RECURSIVELY')
Manager.register(c)
Manager.setMenu(c, 'root', 'Panel', 'Panel window', 30)
Manager.setMenu(c, 'container', 'Panel', 'Panel window', 10)

### wxBoxSizer

c = Sizer('wxBoxSizer', ['sizer'],
          ['orient'], {'orient': 'wxVERTICAL'})
Manager.register(c)
Manager.setMenu(c, 'sizer', 'BoxSizer', 'Box sizer', 10)

### wxStaticBoxSizer

c = Sizer('wxStaticBoxSizer', ['sizer'],
          ['label', 'orient'], {'orient': 'wxVERTICAL'})
Manager.register(c)
Manager.setMenu(c, 'sizer', 'StaticBoxSizer', 'StaticBox sizer', 20)

### wxGridSizer

c = Sizer('wxGridSizer', ['sizer'],
          ['cols', 'rows', 'vhap', 'hgap'], {'cols': '2', 'rows': '2'})
Manager.register(c)
Manager.setMenu(c, 'sizer', 'GridSizer', 'Grid sizer', 30)

### wxFlexGridSizer

c = Sizer('wxFlexGridSizer', ['sizer'],
          ['cols', 'rows', 'vhap', 'hgap', 'growablecols', 'growablerows'],
          {'cols': '2', 'rows': '2'})
Manager.register(c)
Manager.setMenu(c, 'sizer', 'FlexGridSizer', 'FlexGrid sizer', 40)

### spacer

c = Component('spacer', ['spacer'], ['size', 'option', 'flag', 'border'])
c.hasName = False
c.windowAttributes = []
Manager.register(c)
Manager.setMenu(c, 'sizer', 'spacer', 'spacer', 60)

### wxStaticText

c = Component('wxStaticText', ['control','tool'],
              ['pos', 'size', 'label'], {'label': 'LABEL'})
c.addStyles('wxALIGN_LEFT', 'wxALIGN_RIGHT', 'wxALIGN_CENTRE', 'wxST_NO_AUTORESIZE')
Manager.register(c)
Manager.setMenu(c, 'control', 'Label', 'Label', 10)

### wxTextCtrl

c = Component('wxTextCtrl', ['control','tool'],
              ['pos', 'size', 'value'])
c.addStyles('wxTE_NO_VSCROLL',
            'wxTE_AUTO_SCROLL',
            'wxTE_PROCESS_ENTER',
            'wxTE_PROCESS_TAB',
            'wxTE_MULTILINE',
            'wxTE_PASSWORD',
            'wxTE_READONLY',
            'wxHSCROLL',
            'wxTE_RICH',
            'wxTE_RICH2',
            'wxTE_AUTO_URL',
            'wxTE_NOHIDESEL',
            'wxTE_LEFT',
            'wxTE_CENTRE',
            'wxTE_RIGHT',
            'wxTE_DONTWRAP',
            'wxTE_LINEWRAP',
            'wxTE_WORDWRAP')
Manager.register(c)
Manager.setMenu(c, 'control', 'TextCtrl', 'Text field', 20)

### wxChoice

c = Component('wxChoice', ['control','tool'],
              ['pos', 'size', 'content', 'selection'])
c.addStyles('wxCB_SORT')
Manager.register(c)
Manager.setMenu(c, 'control', 'Choice', 'Choice control', 30)

