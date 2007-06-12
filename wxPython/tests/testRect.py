import unittest
import wx

"""
This file contains classes and methods for unit testing the API of wx.Rect
        
Methods yet to test:
__add__, __del__, __eq__, __getitem__, __iadd__, __len__, __ne__, __nonzero__,
__reduce__, __repr__, __setitem__, __str__, CenterIn, CentreIn, Contains, ContainsRect,
ContainsXY, Deflate, GetBottom, GetBottomLeft, GetBottomRight, GetHeight, GetLeft,
GetPosition, GetRight, GetSize, GetTop, GetTopLeft, GetTopRight, GetWidth, GetX, GetY,
Inflate, Inside, InsideRect, InsideXY, Intersect, Intersects, IsEmpty, Offset, OffsetXY,
Set, SetBottom, SetBottomLeft, SetBottomRight, SetHeight, SetLeft, SetPosition,
SetRight, SetSize, SetTop, SetTopLeft, SetTopRight, SetWidth, SetX, SetY, Union

asTuple -> Use Get instead.
"""

def getRectData():
    # TODO: find out if 32767 is some Windows limit, or a hard one.
    #       Can we get rid of this magic number?
    # NOTE: a wx.Rect can be created with values of greater than 32767,
    #       but when returned from wx.Window, that's the max.
    return (
                wx.Rect(1,1,1,1), wx.Rect(1,1,1000,1000),
                wx.Rect(1000,1000,1,1), wx.Rect(1000,1000,1000,1000),
                wx.Rect(10,200,3000,1), wx.Rect(1,999,111,9999),
                wx.Rect(32767,32767,32767,32767), wx.Rect()
        )
        
def getRectTuples():
    return (
                (1,1,1,1), (1,1,1000,1000), (1000,1000,1,1), (1000,1000,1000,1000),
                (10,200,3000,1), (1,999,111,9999), (32767,32767,32767,32767),
                (9,99,999,9999), (314,159,265,358), (-1,-1,-1,-1), (-9,99,-999,9999),
                (-32767,-32767,-32767,-32767), (-32767,32767,-32767,32767)
        )

# -----------------------------------------------------------

class RectTest(unittest.TestCase):
    #####################
    ## Fixture Methods ##
    #####################
    def setUp(self):
        self.app = wx.PySimpleApp()
        
    def tearDown(self):
        self.app.Destroy()
        
    ##################
    ## Test Methods ##
    ##################
    
    def testGet(self):
        """__init__, Get"""
        for x,y,w,h in getRectTuples():
            rect = wx.Rect(x,y,w,h)
            self.assertEquals((x,y,w,h), rect.Get())
        rect = wx.Rect()
        self.assertEquals((0,0,0,0), rect.Get())
    


def suite():
    suite = unittest.makeSuite(RectTest)
    return suite
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
