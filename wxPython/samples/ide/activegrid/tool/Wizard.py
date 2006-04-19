#----------------------------------------------------------------------------
# Name:         Wizard.py
# Purpose:      
#
# Author:       Peter Yared
#
# Created:      10/28/04
# CVS-ID:       $Id$
# Copyright:    (c) 2004-2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------
import wx
import wx.xrc as xrc
import wx.wizard


#----------------------------------------------------------------------------
# Classes
#----------------------------------------------------------------------------

class BaseWizard(wx.wizard.Wizard):


    def __init__(self, parent, title, pos=(-1,-1)):
        wizardBitMap = getWizardBitmap()
        wx.wizard.Wizard.__init__(self, parent, wx.NewId(), title, wizardBitMap, pos=pos)

    def GetDocument(self):
        if self.GetParent() and hasattr(self.GetParent(), 'GetDocument'):
            return self.GetParent().GetDocument()
        else:
            return None

    def SetPrevNext(self, prev, next):
        prev.SetNext(next)
        next.SetPrev(prev)


class TitledWizardPage(wx.wizard.PyWizardPage):


    def __init__(self, parent, title=None):
        self._prev = None
        self._prevFunc = None
        self._next = None
        self._nextFunc = None
        wx.wizard.PyWizardPage.__init__(self, parent)
        self.SetSizer(wx.BoxSizer(wx.VERTICAL))
        self.MakePageTitle(title)

    def SetTitle(self, title):
        if not title: title = ""
        self.title.SetLabel(title)
        
    def MakePageTitle(self, title):
        sizer = wx.BoxSizer(wx.VERTICAL)
        if not title: title = ""
        self.title = wx.StaticText(self, -1, title)
        self.title.SetFont(wx.Font(18, wx.SWISS, wx.NORMAL, wx.BOLD))
        # the code below used to add a 5 pixel border in all directions
        # but I found that the left margin was not aligned properly because
        # only a few of the wizards made sure that pages themselves added
        # the 5 pixel left border. If we still want to inset 5 more pixels,
        # we should add a wx.HORIZONTAL sizer here to take care of it.
        sizer.Add(self.title, 0, wx.ALIGN_LEFT | wx.TOP | wx.BOTTOM, 5)
        sizer.Add(wx.StaticLine(self, -1), 0, wx.EXPAND | wx.TOP | wx.BOTTOM, 5)
        self.GetSizer().Add(sizer)


    def GetPrev(self):
        if self._prevFunc:
            self._prev = self._prevFunc()
        return self._prev
        

    def SetPrev(self, prev):
        self._prev = prev
        self._prevFunc = None


    def GetPrevFunc(self):
        return self._prevFunc


    def SetPrevFunc(self, prevFunc):
        self._prevFunc = prevFunc
        self._prev = None
        

    def GetNext(self):
        if self._nextFunc:
            self._next = self._nextFunc()
        return self._next


    def SetNext(self, next):
        self._next = next
        self._nextFunc = None
        

    def GetNextFunc(self):
        return self._nextFunc


    def SetNextFunc(self, nextFunc):
        self._nextFunc = nextFunc
        self._next = None
        

    def SetPrevNext(self, prev, next):
        self._prev = prev
        self._next = next
        self._nextFunc = None
        self._prevFunc = None



#----------------------------------------------------------------------------
# Menu Bitmaps - generated by encode_bitmaps.py
#----------------------------------------------------------------------------
from wx import ImageFromStream, BitmapFromImage
import cStringIO


def getWizardData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00t\x00\x00\x01\x04\x08\x06\
\x00\x00\x00\xf9\xcf\x10R\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x0f\xdfIDATx\x9c\xed]K\xb6\xe3(\x0cU\xf5\xa9MU\x86\xd9\xd6\xcb\xd0\xd9\
\x96\x87\xaeeU\x0f\xfc\t\x06\x01\x12\x16\x1f+\xba\xe7\xf4\xe9\xd4\xb3-d\xf0E\
 \t\xf8\xb5,\xcb\xbf\xc7c\x06\xc3\x05L\x00\xf0\xfa\xd9~\xbf\xcf\xbf\x81p\r\
\xbb\xaf\xf0\xdao\xb1\x97\xfa6L\xdb\xff_?\x00/\xb8\xd6@\xdc\x0f q\xed?\x89w\
\xfb\x0e\xfc\xac\x8d8m\xbf\x07\x8514\x8b\x9dU\xefj\xac\x92\xecr\x8d\xa1\x01~\
\xe0`\xe3\xc0L\x8c\xc1\x18z\xc2O5\xe6$\xaf\t\xb2\xfd\xd7:\xca\xfd\xc3\x7fw\
\xc3\x90\xd8\x18\xfa\xee\xabE\x17\x10\xd8X\xc2\x9c\xa3.\x192\xa5\xd8\x0eoc\
\xa86|\x19C\xa5F\x9f\x992\xc8\xacz\xe7\x9fs\xef#80\x8c\xa1\xca\xa0\x9f\xa1\
\xaeG\xe7\xca\xe83\xc9J\x80\x13\xfb\xb3\xb6\x17\xa9o\x9f}\xae^~\xd9\x13Du6\
\x86*\x83b\x86\xd6\x9a\x1f\x12\xd8\x95\x1d\x01#\xe00t\x7f?c\xa8~\xe8bh\xec+\
\xdf\xff\xdd\xcb\xc7J\xaa_\xc2(\x97\xf0\x9c1T\x19\x140\xb4\xd4\xff\x1a{ga\
\x1fkvt\xbc\x81mC\x11\xbd\xcd\x86\xea\xc3\x8d\x19\xca\xf4\xbf\x96\xbec\x8a9~\
\xd9\xa5\xd7\x04\xed\xfeM\xc3g?\xf1\x97E_\xfcBQ~z\xc9!s\xc0\xc1\xd7=\x03\xdc\
?\xe8_\'\x00\x80\xe9\r\xcb\xf3y8\x87\xd6,\x03\x81"_ #\xa7\x01~\xdd+\xeb/1\
\x00\xda\x1a\xf31;\xef"\xda\x08\xa5\x83/\xc25A\xb6\xdf\x88\xa183\xa3\xb8\t\
\xa3\xa4q\x13\x1b\x9a\xff\xb2wv.\xcf\'\x00\x00<@\xb0\xd7\x99\x00\xf2Ib\x8e\
\x9e\x1dm\xe8\r\x1a\x94\xc9\xcc*\xe5gF\xc8Go\xb0\xdb\xec\xb7\xf7\xf7\x8a\xf0\
\xca\x1b\xdf\x86\xba\xe1/\x00\xba\r\x95\xaaL\xd2\xb4\xa5dJT\xc7I\xa1\xc6\xb1\
\xb0,\x7f\xcf]\xae\x82w*\xc1\xc0\x8e\x05\xaa=\xdau\x7fz\xcfK\xbc\x13\x819Ruw\
\xd9\xf5\xb7B\rC\xc3.\xb7\xb7\xed\xed\x83a\x18:M\x00/w\xa4\x08\x90\x19\xe5\
\xe5$^x\xa7\t\xf0\x8f\xe3\x02s\xf2(\r\x9fyX\x96\xe5\x1f\xc0\xbf\xae\xff-\xcb\
\xf2oY\x96\x7f0M\x97d\xc04}d]\xd4g\x97\xd5\xbbn\xb8\xff\xf5g\xe8\x04p\xb2\
\x7f\xc1\xc8\x8e\xcaL\x01\x1b\xba13\xfc\xbb\x90\xa3?S\xb6\x1e\x1b\xba\xbd\
\xc01B\x9dg\xbe\r\x14\xb0\xa1\xfbHy\xd7\xa5H\x8f\xce\xe8\xcfP\x00\x80\x17\
\xc0\xb2<\xcf\x95\t\xc0\x9a\x87\x85\x9e"\xe6;M\x00\x8f9\xf2Q]\x9es\xd2\xca\
\xd7\xc3\xd0\x1d\x17\x98Z<\x0f\xf5\x98\xbd<\x9f\xb7\x9e\xc3\x8e\xe1)r\xbcAh\
\xb7\x07\x90\xffJ\x83\x91)\xad\xe8\xbdg8}D\x9b.\xe2\xb1\xd5$dF\xb9\xbf\x0fa=\
\xe1T\xd2\xe3\xf1\x07`\xc2>\xb0\x9c\x8e\xfeKg\xee\xdf\x98\xe9be\xa6\xc3N\xbf\
w\xa8mO\xa3\xe5\xd1\xcb\xed\xcc\xd0\xb87(\xe8B\xe79mC}\xa6e\xde)\xb0\xd9\xd8\
\x9c\xb3\t37\xa8\xb4\xa1>\x1c\x16\xe5F\xafd\x1b\xca\x90yG\xf4c(i\x11\xd1\xe7\
\xbe,S\x8964`\xe6c\xae\x18Q\xe1@\xc6\x86v\xecr\t\xe1\xa3\xedE\xd0F\xf0@\xear\
\xa3\x1f\x07!5\xe4&]n\x9f\x14\x94)\x7f\x8b\xfbU\xfa\x8d\xb3,O\xa2\x8cOy\xcb\
\xe2\x0f\x80\xe6S\x0f\x90\xc4\x8d\xd2Y\xfad, \xc1\xe9\xe37\x00\xfeUb\xce\x87\
\xe9\xd3\xfd\xce\x1b;\x1f\xf3\x1c\xb4Q\xe0t8%\x92%\xc2t\xe4@\x80\x00\x92\td\
\xeciKK\x10\x06 \x91\n<\xec\x1d\xac\r\xe8\xde\xb7Fjf\xf4\xf1\xd9i@\xb6y\xb9\
\x11;\x01z\xd8P,\xa5\x84i\xab>\xa16F\xb1\xd3&\x9a\xb1\x1a\xba\xcd`\xc8\xd3c/\
{\xd7\x85iC\x1b3T\xa6\x82\xb8\x8d\x19}\x86c\x87o\x82\xb6\x0c\x1dbz\xe0"7\xd2\
n\xac\x8b\xdc\xb4eP\xc7\x82\x81\x8d\xba\xe13g\xde\xe7\x8eN\x01\x00u\xbb\x91\
\xaf!\xdd\xe7n#\x97\xe7\xf3\x18\x04\xf9\xdd\xec>u\xc1d^\xd1K\xa4\x87\xbb\x83\
\xeboY\xfe\x02@\xbc\xc2J\xaf\xa1\xfaR\xc2`Xx\x0e\xe2\x8dG\xd6k \xd7aU\x86\
\xce3\xc0\x13K\xe9\xb8\x0c\x9a\xbeG(n\xfb\xc2\'\xa85\x0b\x11J\x195\x1b\xfa\
\x01\x1a\x9d\x81\xfb\x07\xac\xb9h\x93\x82\x92\xdd\x9dD\xa2\x90xO\xe03uE\xca[\
\xd5!%\xe7\x0e6\xb4)\\\x1b\xba\xc5S\x01\x10;9\x90\xbd\xab\x81\x06\x0c\xa5,i\
\xe0I\xc4<E\xae\xbf\xf6\x01ox<\xe0\x94\x96\xe9^\xcb\xef2f\x0cm\x03/8=\x01\
\xc0k\xab\x04\xd7\x86\xba\xef\x13\x1di+ej}\x86\xa6|\xa5\x00\xac\xaf/\x98\x96\
\xcc3\xec\xba\xef)\x98\xeb\xdf\\g<\x04\xc1\xef5R\x93H\xe8\xee\xe2\x90\xff\
\xa6Q.%m\xc4\xb3\xa1>\xfcl\xc2C\x962\xa66\xf0\xe5\xa6l(u>\xc9\xcbX\x88\xbd\
\x0f\xe6):\x1a\xb5e\xec\x13\xc3W\xd8\xd0\xc2$\xb1\xd4\xfbhgj]\x86\xe6\x12\
\xc1R_\x1ef\xf7Rz\xba\xf7g\xf6\x84\x8fOi\xb2oT\x11w\xc8XH\xae"K+\x9aL\x1b\
\xc9\xdc\x9f\xdc\x01ez\xc3\xe3\xf1\x13\xb0?\xfb\\m\xa4\xd2`\x86H\x12\xcb\x06\
\x8f#\x8d\x99J\xe8\x92\x04:pb&\x9f\r\x88z\x0c\xcd%\x82E\x1a\x14\x9f\x9a\xe4q\
\xb0,w\xbf\xa3\x8b\xcb\xd4\xf3\xca\xb7\x0eL\x15J\x83\x19g\'\xb1V\xcc\xf4\xa1\
\x8c\xa9\xf5\x18\xca\\\x89}5h\xcc\xb1\xa1\xbe.\x8f\xc7\x0f,\xcb\xfa\xa7X\x9a\
hu\x0coCSp\x15\x8c1\xb31\xa2]\xf5\xcd\x98Zq\xdaB\x0bO\x1d\xe9\x95p1\x9d\x834\
mI\x05\x08V\xbd\\\xe7\x03u\xe5\x9b\x0c\x84RIG\xd8\xe9c\xdaw\x1c\x11\xda\x05\
\xe5\xb2N\xae>7\xdb\r\xa5\x9es>u8\x8d\xf7\xb5\xbbY\xef\xe5\xba\xb8\xddv|J\
\xb4\x16\x98\xb1U/\x00X\xce\x8f\xe2ArA|\x85\xeb\x8f\x83\x8cs\xbe\x04\xa8Gi\
\xf0\xba\xaa\x18>\xeb\x99\xe2\x11\x93\xcf\x0b\xb2?\x1e\xe1\x80\xad\x1eS\xbf)\
|F\x00\xd59_*\x1b\xe0\x1e\xe9,}l(\xd4\xb8\x96\xd8\x8dl\x7f.\x1adO\xb1\xe3<\
\xe2=\xbf\xa3SoW\xde\x87\xabW\xe2\x9a\x1a\x86\xd6\xb0\xa1>\xee\x90\xce\xa2\
\x86\xa1\'O\x910C\xb3\xbe\xdf\xacW\xcc\x18\xcaFM\x1b\x1a+\x0b`\xbc y\xa7Q.\
\x88_;G[\xfe\xc8\xf4\x04\x91\xb2\xe3\xbe\xdf\xb7HOp\xe5\x9a\x1a\x86\xb6\xb0\
\xa1>Fdj=_nO\x1b\xeaV\xaa\xa0\r\xc5\xae%\xa74fC\xcb\xd1\xd2\x86\xc6\xca\x06\
\x88l3\xd7\x10j<E\xb1D\xeb@\'\x00\x99\xa8\x86[\xb6\xe3Q:MiX~XQOQ\xe3.\xb7\
\x82\x83\x9b\x92\x97Kv\xce\x97\x82\x9b\xa9\xe8\xeb&\xa0\xd78)(\x1a\x80T|\xeb\
t\x96\xee\x01n10\xf2r\xa5\xbb\\\x1fh\x86~\xb6\x8ee\xf4\xea\xc4\xd0\xfe\x13\
\xf0\x9a\x88-\xd5h\xc1\xd4\x8e\xd3\x16\xd9\xe2H6\x14\xd3\x0b@\xce\x86":\x01\
\x103\xf4\xa5\xf4\xbaSz\x059m\xa4\xb7.\x88^\xad\x0e\xf7i\xb2OQ|\x92]\xa7\xd8\
\xf4\xfb\xf0l\x95\x9f\xc4\x06\xc0\xdf\x8a\'\x94\xf9F\xb6\xaa\x13\x9d\xb6\x98\
c!\x8a)\\\x01\'\xb1\x91V-\xa7C\xe5\x15\xdc97\x9ad\xb9\x84$1\x80\xa66\xd4\xc7\
\xb1\xcd\xab{*\xaf\xb4^_mC\xb74M\xea}w\xa8\xab\xbe{,t\xb4\xa1k\xf7\xb7\xfe\
\xe5\x93\xf8\x95\xef5p\xfb\'\x01\xb3\xa1\'\xb0l\xe8Vq\xc1\xfaP\xe2\xca\xef\
\x11\x02\xd91\xdcj\x17\x94\x14h\xcey8<J\xeb3\xfe`\xe5\x0f\xc4\xea\xe2t\xd0]\
\x95\xdeE\x86\xa1\xf5\x8f\xca:\xb2\xd1\xbd2\xc4\xbfr\xf7\xa5#\xb2\xa7L\xc5\
\xb8\x83\xb7\xa4\xfcD\x19W\x10\xad#zY\xb7\xdcI\x0c\x83\xbf\x93X\x80\x083\xf7\
\x7f\x1f\xbf\x01\x00\xab\x0f<1\xec\xba\xde\xae~\x12\xa3\xdc6\x87\xd9\xb9_^5\
\xa6\xfeu~\x9fe\x9e\xec\x9fS\xee\x03>\x19\x07\x07",]7\xb0\xf2\xbbr\xc1zs\x1b\
-U_\x194\xdas\x9e\x90J!\x19\x8b\xf4\xd6\x9f\x02D\x96\xfa;i$\xd4\xc8\x08\xd9g\
\xcc\xc6]l\xe8\x8e\x143EX\x1a\xda\xd0\x143\xddc%\xc3#.S\xfa|\x18\xbd,\xc2^)\
\x01\x1b\xda\xf0T\x88:\xe9\x1f;\x02\xe6 \xe7\x9c\x9dYuf\x84\x9f\xec\x15\xed1\
\xbc\xac\x84u\x03\xc9K\xaa\x1fr%z\xafv\r\xda!\xfd\x03 \x11d\xc6\xc2{\xae\xdf\
\x96\xb0\xc9\xd5!_dpt\xb7\x007\xe1\xc8\xe5\x9ae\xb3?\xda\x94>\x03\x1f\x9f\
\xd5\xf8\xa8\xacL\xae\xea\x85n7v\x96v|\xf0\x12\xa6\xc8\xe0\x03\xa7H\x81N\x97\
>\xcf\xf3\xf5nWe\x92\x984K\x93\xcc\x0c?\x1e\xea&W\xbb\xec\x1d\xcf\xe78\xfb\
\x1au8\x106\x97\x01^\xc8\xd2\xa2\x14J<\xbcwrR\x10\xa60\xf4\xf22\xba\xdc\xca\
\x86\x92!0\x85!\xdb\xcc|E\xf9K\xf2]\xf4;\x8c>\x8e>G6WH \xdbSE\xd8\xd3\x08,x\
\xe0\xd9\xd3\x94L?\x11\xacx\xc0d\xbb\xa0T\x86\x9fz\x92r~\x9c\xa6;}\xeb\xb2\
\xcdA<\x01RI\xd8\xd0$\x1d$\xd0\x05c\xc7\xc9\xcfK\xac\xa3b\xdd-\xc0]\x1fN |\
\x9d\x9a\xe0,EW\x9fu\n\x82wb\xe8\x8e6\xa15\xb2\x1eA\xd9\x9f\xbf\xafS\x13\\\
\x9f\xfd(\x913\x98uj6\xb4\r\xc2HL>M\xa5\xe7\x8ec\x9d\x19\n\xd0$\xb4F\xd5\x03\
);\x9c\x00\xa4\xea\xea3\xc5Y\x96\xbf\xcci\x8d\xd9\xd0f\x08#1\xf1\x14\x97p\
\xba\xd3\xd6\x96\xf6\x99\x87\xfa\xc8Eb\x00\xfa\xd8Rdn\nP+\x12c\x0cm\x0b\xc6\
\xbc\xf4\xc4\xe8\xc6\x0c\x1d\xc0\x86\xee\xc8DbZ\xd8\xd2\x84\x07\x8b\x93$v$p?\
\x9f0S\x13\xb3m\x94\xdb\x01\xce\xbc\x94\xe5=j\xc8\xd4\x81\x18\xbac\x00/R,9\
\x1c\x08\xe9\xa2\xde}\xc7\xefl\x1d\x9b\r\xed\x02\xf4\xf0\xf6\xad\x01\xa6\xe9\
\xbd\xeen\x8e\x06\xd9\xdb\xd4\xf1\x80\x0c\x05\xc8/C\x84\xcaL\x8d{\xb0\xfc\
\x13\x9c\x0e\xf6m\xd1\x9ex^Sf&q\xbb$1\x0er+\xbf\x8f\x7f7\xc8\xf8\xc7\xca\x06\
b\x16>+\xa4\xa66\xc0\r\xf4/\xb2j\xdaG\xa2\x12\t\x03\x9c\xe3\x98\xaf\xc6\tec2\
tG\xac\x1b\xda\xff-\x90\\\x96F\xa6l\x7f$\xbb\xab\\\xd2\x88*\x93\xc4|tgj\xe6C\
y\xfd\xc0<\xcf\'6\xd6Y\x0cL\xc7\xd8\x0c\xdd\x91\xfaz\x01\xea2\x95T\xb6D\xb9\
\x9am\xa8\x0f\x16S\x85\'\xf0\xd4\x01M\xea\xda\x04\x1f\xdd\xdc\xdf\xfb\xbf\
\xb3:\xd3\x9f\xbb\x07CwP6U\xae\xe5\xcc\'m\xe8\xec2\x89\xe0\xca\x0ct\xberm\
\xfdm\x8e\x05e8;\x16Ze\x7fK-\x1b 1\xe0bY~\xb9{y"\xac\x8a\xe9\\~\xcd\x18\xaa\
\x0c+C\'\xfa(\xaa*\xd8l*`\x87T"\xb4 \xab$\xd9n\x0cU\x86\xb1\x18\x8a\x813m\
\xe0\xb0#%7\xbb\xb7\xd2\x85k\xa9\xa30I\xf3P\xb3\xa1_\x85\x0fC\xfd/\x96\xb2\
\x82\xd9\xbf\xe7\x85\xfc\x8d\xf2\x1c\x05W\xd28\xf6\x7f\xc7\xae\x1d\x0c \x04\
\xd7\xd9\xd7\xa8\xbd\xdf\x05O\x913\xfa6\x86*C\x18\xe0\xe6\xec-\x801\xb4DN\
\xee~\xca\xf3\x01\xc6\x9a\x1f\x92X\xc6\x1d\x99#\xe9\xaf\xc6Pe\xa0\x8drs_H\
\xea\xbe\xda\xde\'N6\x80\xc4\xfc\x90\xc2\x96\xa2\xb90\xc7\x86\xc6\xdf\xc7\
\x18\xaa\x0cc2\xb4D\x0e\'\xc4&bC\x99\xf21\x99\xfe\x98#\xcb\xe8\xfc(|e(g;\x97\
\xda\xc8M\xf8K\x9f\xc5d\x89\xb9\xe4<B\x90\xd6\xeap\xc2mo\\&"\x7f\x9c\x007%\
\xa1*w\xcf\x11\x04&\x96\'\x96\xe1\xf0\xe3\x94\xfd\x93\xb9\x17y\x8e\x12\xe0&\
\xe27\xa3\xf4q\x90s`\xb0\xf0n4mI\\\x13t1\x86\x0c\xad\x99\xe4\x94b\x18\xb7\
\x81r\xb2X\xf2\xde\xb4\x1e\xe2\x0683\xf4\xe5\xfd\x1f\xe0:\x13nWIu\x98\x93\
\xbc&\xc8\xf6|\x97\xcb\x19\xa4P\x1b/\xd6eN\x0c\x19~\x991y\x1c\xbdNpG\x97o\
\xb8\xcb\x11\x99\xf7\xb4\xa1]\xb07p\xa1\x93"u\xcd\x02\xdc\x86\x18\xae\xad>+\
\xedrc\xcf\x97\xca\xa1\xc8ma\xcbK\x03\xe3\x82l7\x86*C_\x86\xc6\xe4\\\x91\x95\
\x92)!wp\x18C\x95a\x0c\x86b\xb2\xae\xca\x8b\xc9\x94\x90;0\x8c\xa1\xca06C\xaf\
\xca\xac)wP\x18C\x95a\x1c\x86b\xf2F\x97; \x8c\xa1\xca0\x16C1\x995\xe5J\xc9\
\x1e\x08\xc6Pe\xf8\x1e\x86\xd6\x96=\x08\xc6\xc9)JA*\xdd\xa4G\xdepc\x8c\xd7\
\xa0\xca\x18\xd3\x1a\xe35h\x0c5\x99\xa4\x88\xa5c6h\xedD5\xc5\x18\xb3A{0F\tK\
\xc7lP\xe5,\xaa\x891\x1b\xb46\x14\x7f0\xe6XP\x86\xf1\x1c\x0b1\xd9w\x93\xdf\t\
\xc6Pe\x18\x97\xa1\x98|\xc92\x8c\xa1\x86;\xc0\x18ZCvG\x18C\x95\xc1\x18ZCvG\
\x18C\x95a\xfc\xdd8[B\x01C\xd7\x06}\xddc1\xab!\x0fc\xa8\x0b\x05\x0c\xfdN\xe7\
|\x0c\nBh[\x97[\xf8\xb4\x8dr\x87\x831\xd4\x85\x02\x86\xde\xabAk3\xc8\x18Z\
\x11\n\xd8\xd2\x03\xe36\xa8\xa1\x08\xd6\xa0\xca`\xae?e\x18\xd79\xdfz\xca")\
\xbf#\x8c\xa1\xca`\x0c\x95\x96\xdd\x19\xc6Pe\x18\x93\xa15m\x9cbv\x02\x18C\
\xd5\xe1>\x0c\xad%[\x11;\x01\x8c\xa1\xea0\x1eCmt{\t\xc6Pe\xf8\x1e\x86*\xf6\
\x0e\xb90\x86*\xc3X\x0cm\xc9Ne\xcc\xdc1N\xf8\xacV@\xfb\xcb\x02\xe5\xe34(\x86\
Z\xbbp*\xc6\xd8\rZ\x0bJ\xbb[\x80Q\x1a\xd4\xf6\x9b\x17\xc3\x18\rj\x10C\xff\
\x06\xada\xe7Fd\xe7~\xf0\xeb\xe9\xe0\xd8\x9f\xf0wp\x8d\x87\xbe\x87\xd9\xd5\
\xa8\xf8a\x1a\xd3;\x16\xeb\x05\x02\x87\xd9\xe5\xa7\x97\xe6XP\x86\xcf\xea\xb3\
\x92/\xf8\x8acA\x9aI\xa9\xee\xa9*;\x079\x10v{Gc\xa82\x9c\xd7\x87^=.\x92\xfa\
\xbc$;\x9b3\x93z \xec\xfb\xfc\xcc\xe9\x1a\xac\xd7\x83\xe3)\x1d\xf9\xe0\\\xc3\
\x8e\xd0\xb6\x03a\xbf\x03\xe3\xac\xe0\x96\xb0\xbf%\xb2(eQl!:\x02\xc5X\xec1\
\xd7gm\xecZt\x9cs\xd6\xc5\x18\xaa\x0cc0tHv\x96\x8cV==\x83\xe7"e\xc4X\xee\xcb\
\xc0\x9e\xf7\xf42\x86*\x03\x9d\xa1\x943O\xb0\xd1\x18\x17\xa5g\xabH2\x13 2\
\x92\x8d<\x120)g;\xbd\xe7r\x0c>d@\xa2\x97X\x7f\x1bC\x95\xa1\x1eC\xb1\xeb\x92\
\x10\x9fcF\x18\xc0\xb5\xef\xcd=E\xe7\xb63\x86*\x03\x9e$F\xf5\xe4\xc4\x18\xca\
\xf1\xdepX\\\xc3\xf3\x83\xb2\x8a9\xeaGmhLF\xc1\x88\xd9/\xcb}\xce\xbboe\xe8<\
\xd3\x074\xa9\xfb^\x8cgs\xe5\xe4\x9e\x15A\xda\xd1\xcd\x92S\xd4Ug\xa6-\xb1g\
\x0e=\xc3\xe7>\xf1Pj\xe5\xa6\xee\xcb\xc9(\xb1\xa9\xb5"%SD\xb6DyU\xa3;o8\x1au\
\x87\xf3.\x1f\x86\x8e\x86\xea\x01\xe9RVyp\xbb@\x00\x9e\x9c\xec\xb4\x85\xfa\
\xdc\xa7\x8b\xef\x9f\x82\xe2\xe3\x05m\x1a\x13+\xb7\x9b\x1c&b>]\x18\x89\xa1\
\xcdRD\xa8\x8eu\x02\xb2\xae9\xa2.\xc5z\x84\x8e\x8f\xf3n\x9c\xad\x12\x93\x87J\
\xa5\x1c \xd2T\x0c\xdf\x9e\x1e\xbe\\\x8f\xa1\xb5\x1a\xb6wC^\xb1w(\x84\xa6;\
\xa5\t\x06\xae\x0c\x00\x0bpk\xc4\xb5\xd5gw\xc2e{\xe7B\xca\x0e_\xb5\xa1\xa1.\
\xc6Pe\xf8\x0e\x86J\xb8\xf7\x0ePSCJ\xf4*\x94\x03fC\xd5\xe2\x0b\x18\x9a\xf6}\
\xb2Q\x93\xed\x02z\x19C\x95\xe1\x0b\x19z\xf1]\xc5\xec\xde\xa6\x9b1\xd4\x90\
\x82r\x86\nET0\x99\x83\xb2\xdd\x18\xaa\x0c\xba\x19*\xea\x1d\x82l\xfa\x07\x1f\
\x826t\x93g\x0cU\x06\xdd\x0c\xbd\x1a\r\xc1\xe4\x01\x0cjCWyx\xf8L\x0b\xbe\xae\
\xcb\x1d1\x05E\n\xe2;\x85"\xe9&\x03B1C\x89\xebR\xa8`\xe5\xde2eJ\xe9\x08\x9a\
\x19Z\x1dc\x8e;\xf42\x14M7a\xae!\xe1\xaeQ)\xbd&\xb1\x0b\xcav\xcd\x18\xaa\x0c\
\xd6\xa0\xca`\x8e\x05eP\xecX\xa8=\xca\xbd(\xcf\x95)8\xca5\x86*\x83^\x86\xfa\
\xa3\\\t\xb7_\xd5P\x1c\x80D;\x18C\x95\xe17\x00\xc0\xd4{\x9f"\x83\x18\xfe\x07\
\xd2\x8b\x12\xe7\xc3\x8c\xd4\xb6\x00\x00\x00\x00IEND\xaeB`\x82' 

def getWizardBitmap():
    return BitmapFromImage(getWizardImage())


def getWizardImage():
    stream = cStringIO.StringIO(getWizardData()) # NOTE: This reverts us to the bitmap Peter likes.
    return ImageFromStream(stream)

