"""
wxPyColourChooser
Copyright (C) 2002 Michael Gilfix

This file is part of wxPyColourChooser.

You should have received a file COPYING containing license terms
along with this program; if not, write to Michael Gilfix
(mgilfix@eecs.tufts.edu) for a copy.

This version of wxPyColourChooser is open source; you can redistribute it and/or
modify it under the terms listed in the file COPYING.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
"""

import canvas
import colorsys
import cStringIO, zlib
from wxPython.wx import *

# Bitmap functions generated from img2py
def getData():
    return zlib.decompress(
'x\xdaUV{<\x93{\x18\x1f:\xe3\xc8Xj\xa3a\xe6R\x9db*B.3\x93\xeb\xe9\x1c\xb7\
\xe4\xb2r9\x8c\xe5\x12e\xc9\xda\x88\x9a\xb4\x83\xcet\xa1N\xae\x85\xe3\xb2\
\xe1,\xb7\xc2\x8a\xd0\x99D\xc8t\xdc\x86r\x1f\x96\\\x16k:\xebC\xe78\xef\xe7\
\xf3\xbe\xbf?\xde\xf7\xf9=\xcf\xf3\xbd<\xbf\xf7WgG;\x90,L\x16\x00\x00\x80\
\x1c\xec\xad]\xc5+[|7\xc8\x00\xc5O\xccv\xddE\xf1\xb2\x8d`\xe5\xe0&#\xbebdpT\
\x00`\xbb\x94\x835\xc6\x8d\x989\x97\xd5N\xca\xb3\x05\xdb\xdc\x9c[\xd0\xf4\
\xd2\xe6\x7f\xb9\xd3\xaa\xe9\x85\xd3\xd5\xd9\xbe]\xa7\xf2\xf1\xf2\x1b\xef\
\x1b\xe6\xb7\tF\xca\t`\xef\xf9\xb5\xa2\x87\xe1\x8d\xd3\x8ft\xef\x06z\xa5\x8f\
\xd7\x04\xf6y\xaf\xfbT\x0e\xad~\xce\xf1E\xa7\xc7\xe9\r\x8c\xab\xb7\xad_\xe4/\
r\xf9\xb1\xbe\x97[\xb3\x88\xfcx\xd6\\\xdf\x15\x88\xca\xcc\xc0\xb0\x10\xef\
\x81B\x9a\xf3Xzq\x1c\xb9\xa2E\xb2\xd3v\x80\xfd90@Ju[\xc3H\xe0\xea\xca\xdc\
\xfbr\xde\xbb\xf0U\xcd\xf4\xd7\xfe\xabc\xbaN\xc2\xe3\xcc>T\xa6)(\xaa\x9f\xe7\
\n\xaa\xe9\x00gcz\xa8Y\xb4\x8a\xc4\x0fr\x9a\x89\xe4\x9eJ\xe7\xa6z\xaf\xf7\
\xcc\xc4\x11YI\xbfx\n@y\x04\x01\x00\nL\xd8\xb3#\xfa\x0e\'\xb7\xc1n\x98\xd3h\
\xd5\x14\xd50\xeaA\x0bd\x8a\x16\x07\x96\xa0#\x13/!\xbf\xf1\xe5\x94)\xd5\xc3\
\xfe\x9ej\x0c<\x8b<8\x89\x98\xcb\x9ek\x18X^\xf9\xa9\xffJ\r/\xd93\xda\xc2\xfd\
\xfa\tt\xb9\xa3\x07\xe8u1\xc5\\O\xe9w\x94\xd6\n\x1e\xe8\xb1Q#@Z\xe2\x10\xd1\
\xcb\xc7\x17\xbd\x9e\x94\x14~\xf9\xd6\xe0\xb1\xb3\xe8\xd8\x07E\xe4\xa6F\x87\
\xbe6:K\xf8\xfanrn\x8b\xa4=e\xb3\x98\xf3\xc8\xde|\x0b\xf0\xb5\xb7\xfb\x0e\
\xf5\x83:m3\xcbs\xfe*T\xc7\x89\xe1\x1a\xc8X=\xbfZ\xc4#\xd0\xee\x93d\xb6lL\
\x15\xe4\xe3\xf7\xd8w\r\x1eK\xe0\xce\xad-`\x19g\xe6\x1f\xc8}\xcc}\xee\xecF\
\x19_\xa6PUq\xee\xa2\x80\x1d\xbc\xf5\x14g\xe04\xa4\xc0EZB\xe6[\x1al\xb2!,\
\xac\xa4\xf3O\x83\xa5y\x96(\xa7\xdd\xc61\x1aX\xa4X\xa8\x96\xdf2\x93\x067\x1c\
\xf0\xed\x10\xaa\xe3\xe9\x96\xac\'l\x14\xb7\xc1\x0ed\x85\x96\xb1\x84m&d\x872\
\x1f)\xe6wt\xd6\tV\xbe\xd4\xbe\xeaE\xcf\xaca\x92f4*Z\xce\xf8\xa0\xd9\x97\xa2\
+\xcc\x90$\xbb\xac8\x0b\xf7\x93\xfa\xb6\xf2\x92\x9d\xa0M\xec\xc6\xaa<\x17\
\xad\x8d\'\xc29\xd1uQjcU}\x8a\x1c\xbf\x9fg\x12\x9c\x1f\\4RZ8\xe2\xf5s@J\t\
\xc5\rU\x92\xab\xf6B\xb3\x97\x8f^\xb8\xa5_\xd0\x98tQ\xa6KA\x12\x0c\x14gl\xc0\
\x00\xe4f*=\xa3\x1ef\x0c\rU<\xb1sO\xaa\x0c\x82;\r)\xc8\x9c\xc0\x1f9CMch\xd4\
\x9fn\xde/g\xc3C\xb0\xb0\x8cuS6\xf3;\x8d2q\x7fG7\x88;\xb4~9\xd5e\xa1}\x7f\
\xd5\xa9Z\x88\x99\xaft+\xeca?\xa2qh\xaf\x8af\xbf\x82\xfe%\xac\xf6\nC_\xc6D\
\xc6Ry\xb3\xb7N\x11"\xcfU\xbb@m\x86AY\na\xfa;\xa1\x93\x96\xf2i\xd0\x04S\x90~\
\x1b\xb8z[1C\xde\x15w\xed\x0b\xd8\xd0\xbe\\\x19\x84\x84\xfe\x1bE\re\xda\x1az\
6\xe6\xbe!+\x86V\xbd\xfb\xc3\xfb\xd9[\xff\xc0\x8c\xc5\nH<\x82U#2S\t\xc8\x1en\
\xa7\xb5E\xde\x14W\xd2w\xe3\xf0\x18\x02\xa0\xf7\xad\xb6\xb2\x96\xfb\xbbH\x02\
\x0b\x85P\xb7\xe4\x02!\xe6a\xc4d\xe8]\x17\xa4\x8bk2G\xaf\xa2p\\\x8e\xd0\x8e\
\t\nf\xce\xf0\x10}O\xc1\x95\x9e\x80\xa6\x91\x8d\xae0\xcf\xa0\xc7\x97-h\xc3\
\x1f\xb8"l\x14\xcaz9\xffYl\x15.l|\xc4\x94\xdd&\x9c\x9f\xf8\xb8f7\x08cA\x1e\
\x85\x11,\xb0\xba\xf1\x87639\xfbJ\xef~e\xf3\xdbK\xd4\xe7\xec\xa5\x92\x17\xf7\
\x1aO\xe3*\xd5\xf3\xcbA\xef\xf4\xa4[\x1d\xaf\xd7>M-\xf1\xb9@\xea\x96x\xd9(\
\x06Z\xec+J\xed\xe5\xd45\x95\xe1\xba\xeb\xf4\xa4\xa3i\xbb\x82}\xd0\xf6Lh\xe8\
E4;0\x9aPk\x0emo~G\x04\xb6&u\xb31\x80\xdao\x01\xf5P\x1d\xd8\x05\x92qmV\xf6R\
\x17\x89\x1a-\xf4\x15P*\xf9?|\xcea\xa9\x8f\x99~6\x92^\xf8\x03s\x11;v\xe2D\
\xc4^\x1c>Q\xbfY\xd0n\xbaE\xc0b=\x91\x0c=[U\x86\xfb\x06\xb52\x92\x19M1uz<\
\xb1\xa4r~4\x83E\xe2y\x08\x0f\nQ\x84\xe8\xfa\xfa\xea\x13\x0e\xd4\x95\x91\\\
\x7f\xf6)\x08`\xb6\x89\xc5\x95^\xff\xe2\\\x03X\xe4\xbe\x88x\x8f\xe34\xb7\xe6\
\xbe\t\xf8\\w\x9c\xd3\x1a\xee\x98\xeeW\x92\xad\x83\x99\xb6B\xcb\x8f\xbdD\x86\
\xc6\xe3\xab\x1b\xb2\xdf\x08:9\xbc\x9e\xf3\xf9h\xd1\xec\xd98\xc8%\x0b\x87c\
\xb8\xbc\x0f\xad\x89\xca\xa5\x94J\xa9\x88j\x1ddo\x91u\x84\xf3\xcd\xea\xc5\
\xf6\x99\xab\xe0\xd7-\x92\xff:\xe6)T\x07\x0bd\x1b\xa9+9\xa4\x86\xec2F\xa1\
\xa6\x7f\xbc\xd1C\x9e\xf4=D\x12\xa2\x07\x94\t\xb1\xe8\xb5\xfb\x94\x14q~R\xa1\
\xe6Y<\xad\xcb\x94j\xbc\xb3##\x0f\xd0 \xbf\xc9\x01\xf8\xad\xb8V\x82sdO7\xbd\
\xbe\xd5Bd\x9f\xc0m#\xd4h<j\xf5*\x84\x86VKt\x0c4\xc1QkD\xbd\xee\xd0\xdc\xcc\
\xad\xc5bN\'\x8f\x1b\x92\x95\x8e\xdf,\xb1\xfa\xe0\xc7f\xd5\xc7\x95\xd1&\xe06\
\xcb\xb4t/\xa0KTj\xd0\xe2\xfb\xd2\xc3!\xf1\xcb?\r7_\x14K$xs(\xfa}~\xe2\xd9\
\xe5gP\xd4\xfaq\x97\xb1\x0b\xd2]\xe5|\x19o2(Vm\xfe>\xe5\x13jZ\xdan\x98\xf3\
\xe4:\x1ep\x93B\xec6?\tO\x0eaB\x99>\xc6Zkr\xcf!\x1b\x84|\xb5\xdb\x8b*O\xb4\
\xe7\x14Ko\xa0\x93\xecmq\xd7\xf0\xbd\x12\x07\x8d\x95\xd7\x7f\xf5&\xb8bmj\xda\
/&`>e\xeb\xfc\x14a\x19\x94S\x7f\xd2\xb5:\x8c\x04\x8f\x91\x03\xc2Q\x0e\xff{\
\x93\xc7\xea\xd6\xbb\x1b\x0e7\xe7E\xa6\xae\x9d\xc6\x85%\x9e\xfbnc\xe3\xff\
\xd4\xa2`9\x13\xa3\x97\x9e\xa4\x9b\x06\xa5\x9f\xec\x9f\x1a\x0c\xf0\xfe\xcd\
\x021\x9b\x0cM\xc06\xfd u:\xe7:g\x02\xc1r\x926\x9b\x7f\xe2\xf9\xe3\xed\xf1qU\
\xbe\xbf\xe8\x91\t\x0c0\xfb-\xe5%d\xef\x19v\x966\xec\xaaB\xe2`N[\x8c\xda\x98\
\xf4\xb4\x83\x13\xcc\x8a\x83\x81\xa3\x91%\xdb\xad\xab\xff\x87\xe1\xba\xda\
\xb5\xdb\xf0\x17\xfd\xf4F\x18KTNH\xf5J\xbc\x97\xdfB:\xa7\x96\xdf/k\x1c\xeaF_\
\x8c\xfc\xdfap\x1e\x99\xae8\x94b\xa1t<\xb54.3c\xd9\xe8y=u9FM;D\xa6\xc0\xea~\
\x0f"O\xee\x81\xdc\xa3\xb2\x1a\xa0\xa7\x02\xb9\x7f>\xfdg\x974\xc8\x0b\xbaU6P\
\xe7\x14\xd8\xd5 \x90\xbc\x0b\xf0\xb7\xc4\x7f\x08\xfaPl\xf5\xa7\x96\xac\xc2\
\x0f*\x05\xf3\x83\xe8\xce\xa7\xc4\x8c\xdbX\xa4U\x9b\xeeW\xe9\xf1\xbf\xa4Q#\
\xcbDQ\x18h\x02\xca\'\xca\xee),1"\x8d\xfb<\x15\xafl\xb8\xb3z\x18\x18\xaf\xb6\
I$\xa2\xbc\xe5\xe5J\xbe\x00\r\x08&<\x0eK\x98\x0e.[\xd1\xea`\xa9\xe2\x96\xae-\
d9%\xc0 \x85\xc5{c_\x03x\xaf\x8f\x98P= 0\x8e\xff\xaa\xf5>7\xfeO\x7f\x1b\xcbm\
\xb1W\xa7\t\x9b\xe1w\x02\xc5\xb5\x9fM/\x8d\xab\xe4u\r\x06\xa0\xd6\xc9\xb5\
\xf2\xb7J\x01\xda<\r\x9f\xd1\x06\x03\xea>\xab\x9d\xe6\xde\xb4\xbb\xb1\xc6\
\xa3EP\x1e\x17\x16\xf2\x1c\xa7\x823\xa7\xcc~\xd1\xdb\xb2\xcb\xbd\xe1\xdb\xf0\
W(,\xe9XD)3I\xc4\x15z_\x98\x8b\xb2v\xdb\x13\xd0\xb8\xf3U"\xb713\xaf\xa0\x1dC\
j\x0b\xb0\xf9\xfd\xef\x0ex\xd7l\xa5\xc1\xf7Z\xd2\x12\x1f\xbe\r\x87Shjm\xe3\
\x1c\x92\xbc\xc7^\x9e\xe5\x84\xf9\xb8\xcb\x88+\x12\xb4M\xee\xb0\xbb\xcd\xc4\
\x9c\xc7V\x1f\xde\x1b\x02\xb0!\x0c\xbeY&\xf6\xe9\xdd[u:0\x0f)(\xc5g\n\xd5\
\xb6\xcc\xf0st\n\x113\x81Q\xcc\xef\xaa\x1b\x9a#\xad|\x12\x98\xd8\xf7"\xa2\
\xa2\xd7\xdbwz+\x08\xb8\x0c\x9d&mZ\\-<\xbaG6j\x9cy@\x8ah\x10@\x8e\xd9\x12\
\x9dK\x00\xf3\xabdo\x1f\x8b\x80\x9c\xf7i^#\xc1X\x7f-\x13R](\x17\xa0\x89p\x9c\
L\x1a\xc5\x9c\x88U\xde\xc7\x9d\xd04\x04\x8en\x11\x8112\xbd\xf6J\x96wP\x06\
\xcd\xa9x\x08\xf7\xc3\xfc\x0e\xceE\xef\xf9j\xd50\x19~\x02~\xf7C\x13\xae\xd8\
\xea\x8e\xc9\x8c\xafpU\xc8\x8d\xaa\xe5\x88Q\xfan\x93\xf7\xf0(\xb0\x93\xf5NH\
\x1f\xae\xc5\xf8\xaa\x97F4\x19;\x19\xe4=\x89\xe0\xae\x15\xc9\xb6\xfe\xe2\xce\
\x1e\xca\xe6\x1a\n<\t\xa9].x\x03\xfd\x1c\x86Fd?\xbd\x17|z\x03\xa8\xafX[N"|\
\x16\xa3#\x0e\x92\xf0h{^+K\x04/!\x8f\xac\xf4\xe4\xbbH\xa9.\x85q\xdd\x93\xc7\
\xbb`\x96\xbb\xb5\xefQ\xdc\x9ch+G\xf8\xbf\xf6b\xdc\xfbww\xcf\xc7\x85\xf7\n@\
\x8d[\xdc\x1b\x8e\xd5\x85\x1c\xf0@JG\x08\xc9;\n\xfb\x9dX\xc5\x8e\t\\\xb3g#\
\xa0\xa2\xb7`\n\x96\x116?\xda\x83\xea\xa1\x7f.Y\x9f\xcb\xda_\x8c\xe9\x01s\
\x0f\xf6\x03\xb7:\xa0\xc6\x94\xaat\xc4\x96r\x1c\x12\x06\x1dZ\xf7\x10V\xd5\
\x088\x02N\xc6\xcc\x05y\xd7\xc0T\x07,c\xea\xb2\xcf\xc7=>y\x87M_\x9a\x86\x12\
\xa5\x92\x83\n_"\x84\xff\x8b7\x95\xfeu\x02\x9c\xf7\xe4\xfacQyo\xda\xbb\t\xed\
\xdeS\xd3\xb7\x04/j\xdb\x96\xae\xec\xd3\x01\xb8P\x8ap\x8c7\x88\xc2\xa8\xfd\
\x1d\xd5\xd1=\xab$*\x8c\x9dd\xacu\x07\xe3\xa6X\xed\x1d\xb9eHd@\x8f\xb7\xd4V\
\xdc\x95\x0f\xa91\xba\xe3s?\n\x12\xf2\x97\xefh\xf4\x1d\x89\x04\xccC)\x8f\x83\
\xbf\x84\xd5\xe0A\xb7\xccC\xf9\xc3fGA\x92\xe4\x12\x89\x03\x14bb\xdfe\xd9\x7f\
\x0f\x86\xc6R\xf9wC\x114\xe0\xdd\xae9\xc9ef\x92\xb6\x12\x1eU\'ZW\xa2\xe9\xa7\
4\x15\xfdb\nr\x17\xf1\xe15IkA\xe5\x12aM[&\x93T\x16\xa5\x92x\xf8\xc3\xd4\xca\
\xd8[\x96]wPNO\t!5\xaf&\xfarlLC\xdd\x00\xdd\x8e\x13qc\xea&]nAb\x8b1>)9\x047\
\xc5\x8e\x1a\xd5\x84\x8b\x7f\x8f\x01\x0e6\x8e\xd6eV~W\xff\x01[x\x1b=' )

def getBitmap():
    return wxBitmapFromImage(getImage())

def getImage():
    stream = cStringIO.StringIO(getData())
    return wxImageFromStream(stream)

class PyPalette(canvas.Canvas):
    """The Pure-Python Palette

    The PyPalette is a pure python implementation of a colour palette. The
    palette implementation here imitates the palette layout used by MS
    Windows and Adobe Photoshop.

    The actual palette image has been embedded as an XPM for speed. The
    actual reverse-engineered drawing algorithm is provided in the
    GeneratePaletteBMP() method. The algorithm is tweakable by supplying
    the granularity factor to improve speed at the cost of display
    beauty. Since the generator isn't used in real time, no one will
    likely care :) But if you need it for some sort of unforeseen realtime
    application, it's there.
    """

    HORIZONTAL_STEP = 2
    VERTICAL_STEP   = 4

    def __init__(self, parent, id):
        """Creates a palette object."""
        # Load the pre-generated palette XPM
        self.palette = getBitmap ()
        canvas.Canvas.__init__ (self, parent, id, size=wxSize(200, 192))

    def GetValue(self, x, y):
        """Returns a colour value at a specific x, y coordinate pair. This
        is useful for determining the colour found a specific mouse click
        in an external event handler."""
        return self.buffer.GetPixel(x, y)

    def DrawBuffer(self):
        """Draws the palette XPM into the memory buffer."""
        #self.GeneratePaletteBMP ("foo.bmp")
        self.buffer.DrawBitmap(self.palette, 0, 0, 0)

    def HighlightPoint(self, x, y):
        """Highlights an area of the palette with a little circle around
        the coordinate point"""
        colour = wxColour(0, 0, 0)
        self.buffer.SetPen(wxPen(colour, 1, wxSOLID))
        self.buffer.SetBrush(wxBrush(colour, wxTRANSPARENT))
        self.buffer.DrawCircle(x, y, 3)
        self.Refresh()

    def GeneratePaletteBMP(self, file_name, granularity=1):
        """The actual palette drawing algorithm.

        This used to be 100% reverse engineered by looking at the
        values on the MS map, but has since been redone Correctly(tm)
        according to the HSV (hue, saturation, value) colour model by
        Charl P. Botha <http://cpbotha.net/>.

        Speed is tweakable by changing the granularity factor, but
        that affects how nice the output looks (makes the vertical
        blocks bigger. This method was used to generate the embedded
        XPM data."""
        self.vertical_step = self.VERTICAL_STEP * granularity
        width, height = self.GetSize ()

        # simply iterate over hue (horizontal) and saturation (vertical)
        value = 1.0
        for y in range(0, height, self.vertical_step):
            saturation = 1.0 - float(y) / float(height)
            for x in range(0, width, self.HORIZONTAL_STEP):
                hue = float(x) / float(width)
                r,g,b = colorsys.hsv_to_rgb(hue, saturation, value)
                colour = wxColour(r * 255.0, g * 255.0, b * 255.0)
                self.buffer.SetPen(wxPen(colour, 1, wxSOLID))
                self.buffer.SetBrush(wxBrush(colour, wxSOLID))
                self.buffer.DrawRectangle(x, y,
                                          self.HORIZONTAL_STEP,
                                          self.vertical_step)

        # this code is now simpler (and works)
        bitmap = self.buffer.GetBitmap()
        image = wxImageFromBitmap(bitmap)
        image.SaveFile (file_name, wxBITMAP_TYPE_XPM)
