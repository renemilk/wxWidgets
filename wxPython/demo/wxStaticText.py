
from wxPython.wx import *

#---------------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent):
        wxPanel.__init__(self, parent, -1)

        wxStaticText(self, -1, "This is an example of static text",
                     wxPoint(20, 10))

        wxStaticText(self, -1, "using the wxStaticText Control.",
                     wxPoint(20, 30))

        wxStaticText(self, -1, "Is this yellow?",
                     wxPoint(20, 70)).SetBackgroundColour('Yellow')

        str = "This is a different font."
        text = wxStaticText(self, -1, str, wxPoint(20, 100))
        font = wxFont(20, wxSWISS, wxNORMAL, wxNORMAL, false, "Arial")
        w, h, d, e = self.GetFullTextExtent(str, font)
        text.SetFont(font)
        text.SetSize(wxSize(w, h))


#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    panel = TestPanel(nb)
    return panel


#---------------------------------------------------------------------------








overview = '''\
A static text control displays one or more lines of read-only text.

'''

#---------------------------------------------------------------------------
