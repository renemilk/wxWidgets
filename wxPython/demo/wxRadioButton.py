# 11/21/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
# 

import  wx

#----------------------------------------------------------------------

class TestPanel( wx.Panel ):
    def __init__( self, parent, log ):

        wx.Panel.__init__( self, parent, -1 )
        self.log = log
        panel = wx.Panel( self, -1 )

        # 1st group of controls:
        self.group1_ctrls = []
        radio1 = wx.RadioButton( panel, -1, " Radio1 ", style = wx.RB_GROUP )
        text1 = wx.TextCtrl( panel, -1, "" )
        radio2 = wx.RadioButton( panel, -1, " Radio2 " )
        text2 = wx.TextCtrl( panel, -1, "" )
        radio3 = wx.RadioButton( panel, -1, " Radio3 " )
        text3 = wx.TextCtrl( panel, -1, "" )
        self.group1_ctrls.append((radio1, text1))
        self.group1_ctrls.append((radio2, text2))
        self.group1_ctrls.append((radio3, text3))

        # 2nd group of controls:
        self.group2_ctrls = []
        radio4 = wx.RadioButton( panel, -1, " Radio1 ", style = wx.RB_GROUP )
        text4 = wx.TextCtrl( panel, -1, "" )
        radio5 = wx.RadioButton( panel, -1, " Radio2 " )
        text5 = wx.TextCtrl( panel, -1, "" )
        radio6 = wx.RadioButton( panel, -1, " Radio3 " )
        text6 = wx.TextCtrl( panel, -1, "" )
        self.group2_ctrls.append((radio4, text4))
        self.group2_ctrls.append((radio5, text5))
        self.group2_ctrls.append((radio6, text6))

        # Layout controls on panel:
        vs = wx.BoxSizer( wx.VERTICAL )

        box1_title = wx.StaticBox( panel, -1, "Group 1" )
        box1 = wx.StaticBoxSizer( box1_title, wx.VERTICAL )
        grid1 = wx.FlexGridSizer( 0, 2, 0, 0 )

        for radio, text in self.group1_ctrls:
            grid1.AddWindow( radio, 0, wx.ALIGN_CENTRE|wx.LEFT|wx.RIGHT|wx.TOP, 5 )
            grid1.AddWindow( text, 0, wx.ALIGN_CENTRE|wx.LEFT|wx.RIGHT|wx.TOP, 5 )

        box1.AddSizer( grid1, 0, wx.ALIGN_CENTRE|wx.ALL, 5 )
        vs.AddSizer( box1, 0, wx.ALIGN_CENTRE|wx.ALL, 5 )

        box2_title = wx.StaticBox( panel, -1, "Group 2" )
        box2 = wx.StaticBoxSizer( box2_title, wx.VERTICAL )
        grid2 = wx.FlexGridSizer( 0, 2, 0, 0 )

        for radio, text in self.group2_ctrls:
            grid2.AddWindow( radio, 0, wx.ALIGN_CENTRE|wx.LEFT|wx.RIGHT|wx.TOP, 5 )
            grid2.AddWindow( text, 0, wx.ALIGN_CENTRE|wx.LEFT|wx.RIGHT|wx.TOP, 5 )

        box2.AddSizer( grid2, 0, wx.ALIGN_CENTRE|wx.ALL, 5 )
        vs.AddSizer( box2, 0, wx.ALIGN_CENTRE|wx.ALL, 5 )

        panel.SetSizer( vs )
        vs.Fit( panel )
        panel.Move( (50,50) )
        self.panel = panel

        # Setup event handling and initial state for controls:
        for radio, text in self.group1_ctrls:
            self.Bind(wx.EVT_RADIOBUTTON, self.OnGroup1Select, radio )

        for radio, text in self.group2_ctrls:
            self.Bind(wx.EVT_RADIOBUTTON, self.OnGroup2Select, radio )

        for radio, text in self.group1_ctrls + self.group2_ctrls:
            radio.SetValue(0)
            text.Enable(False)

    def OnGroup1Select( self, event ):
        radio_selected = event.GetEventObject()
        self.log.write('Group1 %s selected\n' % radio_selected.GetLabel() )

        for radio, text in self.group1_ctrls:
            if radio is radio_selected:
                text.Enable(True)
            else:
                text.Enable(False)

    def OnGroup2Select( self, event ):
        radio_selected = event.GetEventObject()
        self.log.write('Group2 %s selected\n' % radio_selected.GetLabel() )

        for radio, text in self.group2_ctrls:
            if radio is radio_selected:
                text.Enable(True)
            else:
                text.Enable(False)

#----------------------------------------------------------------------

def runTest( frame, nb, log ):
    win = TestPanel( nb, log )
    return win

#----------------------------------------------------------------------


overview = """\
<html><body>
<P>
This demo shows how individual radio buttons can be used to build
more complicated selection mechanisms...
<P>
It uses 2 groups of wxRadioButtons, where the groups are defined by
instantiation.  When a wxRadioButton is created with the <I>wxRB_GROUP</I>
style, all subsequent wxRadioButtons created without it are implicitly
added to that group by the framework.
</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

