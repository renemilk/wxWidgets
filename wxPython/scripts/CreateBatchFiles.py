#----------------------------------------------------------------------
# Name:         CreateBatchFiles.py
# Purpose:	Run by the InnoSetup installer to create a DOS batch
#               file for each of the wxPython tool scripts.
#
# Author:       Robin Dunn
#
# Created:      8-Aug-2002
# Copyright:    (c) 2002 by Total Control Software
# Licence:      wxWindows license
#----------------------------------------------------------------------

import sys, os

python = sys.executable
pythonw = 'start ' + os.path.join(os.path.split(python)[0], 'pythonw.exe')
scriptdir = os.getcwd()

scripts = [ ("img2png", 0),
            ("img2py",  0),
            ("img2xpm", 0),
            ("xrced",   1),
            ("pyshell", 1),
            ("pycrust", 1),
            ]

template = """\
@echo off

%s %s\\%s %%1 %%2 %%3 %%4 %%5 %%6 %%7 %%8 %%9
"""

for script, usegui in scripts:
    batfile = os.path.join(scriptdir, script + '.bat')
    print "Creating", batfile
    f = open(batfile, 'w')
    if usegui:
        f.write(template % (pythonw, scriptdir, script))
    else:
        f.write(template % (python, scriptdir, script))
    f.close()


