
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wx import _rename
from wxPython.lib.mixins import grid
_rename(globals(), grid.__dict__, modulename='lib.mixins.grid')
del grid
del _rename
