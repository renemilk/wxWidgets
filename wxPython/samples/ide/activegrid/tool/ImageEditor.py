#----------------------------------------------------------------------------
# Name:         ImageEditor.py
# Purpose:      Image Editor for pydocview
#
# Author:       Morgan Hua
#
# Created:      12/24/04
# Copyright:    (c) 2004-2005 ActiveGrid, Inc.
# CVS-ID:       $Id$
# License:      wxWindows License
#----------------------------------------------------------------------------
import wx
import wx.lib.docview
import sys
_ = wx.GetTranslation


class ImageDocument(wx.lib.docview.Document):
    pass


class ImageView(wx.lib.docview.View):


    #----------------------------------------------------------------------------
    # Overridden methods
    #----------------------------------------------------------------------------

    def __init__(self):
        wx.lib.docview.View.__init__(self)
        self._ctrl = None


    def OnCreate(self, doc, flags):
        if len(doc.GetFilename()) == 0:
            wx.MessageBox(_("Cannot create a new image file.\n%s has no paint capability.") % wx.GetApp().GetAppName(),
                                _("New Image File"),
                                wx.OK | wx.ICON_EXCLAMATION)
            return False
            
        try:
            self._bitmap = wx.Image(doc.GetFilename()).ConvertToBitmap()
        except:
            wx.MessageBox(_("Error loading '%s'. %s") % (doc.GetPrintableName(), sys.exc_value),
                                _("Open Image File"),
                                wx.OK | wx.ICON_EXCLAMATION)
            return False

        frame = wx.GetApp().CreateDocumentFrame(self, doc, flags)
        panel = wx.Panel(frame, -1)            
        self._ctrl = wx.StaticBitmap(panel, -1, self._bitmap, (0,0), (self._bitmap.GetWidth(), self._bitmap.GetHeight()))
        wx.EVT_LEFT_DOWN(self._ctrl, self.OnFocus)
        wx.EVT_LEFT_DCLICK(self._ctrl, self.OnFocus)
        wx.EVT_RIGHT_DOWN(self._ctrl, self.OnFocus)
        wx.EVT_RIGHT_DCLICK(self._ctrl, self.OnFocus)
        wx.EVT_MIDDLE_DOWN(self._ctrl, self.OnFocus)
        wx.EVT_MIDDLE_DCLICK(self._ctrl, self.OnFocus)
        panel.SetClientSize(self._bitmap.GetSize())
        frame.SetClientSize(panel.GetSize())
        self.Activate()
        return True


    def OnFocus(self, event):
        self._ctrl.SetFocus()
        event.Skip()


    def OnClose(self, deleteWindow = True):
        statusC = wx.GetApp().CloseChildDocuments(self.GetDocument())
        statusP = wx.lib.docview.View.OnClose(self, deleteWindow = deleteWindow)
        if not (statusC and statusP):
            return False
        self.Activate(False)
        if deleteWindow:
            self.GetFrame().Destroy()
        return True


    def OnDraw(self, dc):
        """ for Print Preview and Print """
        dc.BeginDrawing()
        dc.DrawBitmap(self._bitmap, 10, 10, True)
        dc.EndDrawing()
        

#----------------------------------------------------------------------------
# Icon Bitmaps - generated by encode_bitmaps.py
#----------------------------------------------------------------------------
from wx import ImageFromStream, BitmapFromImage
import cStringIO


def getImageData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x017IDAT8\x8dcddbfh\x9f\xb3\xf3?\x03\x0e\xf0\xfc\xc5[\x0c\xb1\xfb\x0f\
\x9f10000l\x9e]\xc2\xc8\x02\x13,Ot\xc6e\x06V\xe0\x9f1\x81\x81\x81\x81\x81\
\x81\x05Y\xf0\xdc\x0b\x88C\x1e|\xfc\xc2\xf0\xe3\xf7\x01\x86{\x1fO20000(\xf1\
\x9b3p\xb0:\xc0\xd5\x05\xa9\xf3\xc2\xd9L\xe8&\xa3kf```8\xfah-\xc3\xb57\x9d\
\x0c?~\x1f\xc0p\t\x86\x01\xe8\x9aa\xe0\xf9\xc7\'X\xc5Q\x0c\x90\xe1c\xc4P\xf4\
\xfc\xe3\x13\x14\xb6\x02?\x0f~\x17(\xf1\x9bc\xd5\xcc\xc0\xc0\xc0 \xc9/\x03g_\
\xbbx\ta\x00,\xaa\xc4\xb8\x18\x18\x8c%\xd5\xb0j\x86\x19n$\xc1\x88"\xc6\x82\
\xaeH]X\x9d!T#\x96\xe1\xec\xf3[\x0cG\x1f\xade````\xb0\x96\x0bf0\x96TcP\x17V\
\xc70\x14\xc3\x00\x98!\xea\xc2\xea\x0c\xb7\x1a\x1a\x19\x18\x18\x18\x18\xa2\
\xd6,\xc0\xa6\x0c\xe1\x05J\x00\x0b\x03\x03"iz,\xd3A\x91|l\xf2\x8a\x81\x81\
\x81\x81au\x87\x18\x8a\xf8\xd5\x8aW\xc4\xb9@VN\x0c\x9f4v\x03\x94\x05LH2\x04%\
\x10wD]!h#N\x03`\xb9\x0b\x06`\t\x85\x10\x00\x00\xe4\x0ecz\x94h\xf0\x8e\x00\
\x00\x00\x00IEND\xaeB`\x82' 


def getImageBitmap():
    return BitmapFromImage(getImageImage())

def getImageImage():
    stream = cStringIO.StringIO(getImageData())
    return ImageFromStream(stream)

def getImageIcon():
    return wx.IconFromBitmap(getImageBitmap())

