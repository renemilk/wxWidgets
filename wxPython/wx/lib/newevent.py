
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib import newevent
_rename(globals(), newevent.__dict__, modulename='lib.newevent')
del newevent
del _rename
