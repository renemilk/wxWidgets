#!/usr/bin/env python
#----------------------------------------------------------------------

import sys, os, string
from distutils.core import setup, Extension
from distutils.file_util import copy_file
from distutils.dir_util import mkpath

from my_distutils import run_swig, contrib_copy_tree

#----------------------------------------------------------------------
# flags that affect this script
#----------------------------------------------------------------------

VERSION          = "2.2.2"
DESCRIPTION      = "Cross platform GUI toolkit for Python"
AUTHOR           = "Robin Dunn"
AUTHOR_EMAIL     = "robin@alldunn.com"
URL              = "http://wxPython.org/"
LICENCE          = "wxWindows (LGPL derivative)"
LONG_DESCRIPTION = """\
wxPython is a GUI toolkit for Python that is a wrapper around the
wxWindows C++ GUI library.  wxPython provides a large variety of
window types and controls, all imlemented with a native look and
feel (and runtime speed) on the platforms it is supported on.
"""


BUILD_GLCANVAS = 1 # If true, build the contrib/glcanvas extension module
BUILD_OGL = 1      # If true, build the contrib/ogl extension module
BUILD_STC = 1      # If true, build the contrib/stc extension module


USE_SWIG = 0       # Should we actually execute SWIG, or just use the
                   # files already in the distribution?

IN_CVS_TREE = 0    # Set to true if building in a full wxWindows CVS
                   # tree, otherwise will assume all needed files are
                   # available in the wxPython source distribution


#----------------------------------------------------------------------
# Some globals
#----------------------------------------------------------------------

PKGDIR = 'wxPython'
wxpExtensions = []

force = '--force' in sys.argv or '-f' in sys.argv
debug = '--debug' in sys.argv or '-g' in sys.argv

#----------------------------------------------------------------------
# Setup some platform specific stuff
#----------------------------------------------------------------------

if os.name == 'nt':
    # Set compile flags and such for MSVC.  These values are derived
    # from the wxWindows makefiles for MSVC, others will probably
    # vary...
    WXDIR = os.environ['WXWIN']
    WXPLAT = '__WXMSW__'
    GENDIR = 'msw'


    #------------------------------------------------------------------
    # Some MSW build settings

    FINAL = 0          # Mirrors use of same flag in wx makefiles,
                       # should probably autodetect...

    HYBRID = 1         # If set and not debug or FINAL, then build a
                       # hybrid extension that can be used by the
                       # non-debug version of python, but contains
                       # debugging symbols for wxWindows and wxPython.
                       # wxWindows must hav ebeen built with /MD, not /MDd

    WXDLLVER = '22_1'  # Version part of DLL name


    if debug:
        FINAL=0
        HYBRID = 0


    includes = ['src',
                os.path.join(WXDIR, 'include'),
                ##os.path.join(WXDIR, 'contrib/include'),
                ]

    defines = [ ('WIN32', None),        # Some of these are no longer
                ('__WIN32__', None),    # necessary.  Anybody know which?
                ('_WINDOWS', None),
                ('__WINDOWS__', None),
                ('WINVER', '0x0400'),
                ('__WIN95__', None),
                ('STRICT', None),

                (WXPLAT, None),
                ('WXUSINGDLL', '1'),

                ('SWIG_GLOBAL', None),
                ('HAVE_CONFIG_H', None),
                ('WXP_USE_THREAD', '1'),
                ]

    if not FINAL or HYBRID:
        defines.append( ('__WXDEBUG__', None) )

    libdirs = [os.path.join(WXDIR, 'lib'), 'build\\ilib']

    if FINAL:
        wxdll = 'wx' + WXDLLVER
    else:
        wxdll = 'wx' + WXDLLVER + 'd'
    libs = [wxdll, 'kernel32', 'user32', 'gdi32', 'comdlg32',
            'winspool', 'winmm', 'shell32', 'oldnames', 'comctl32',
            'ctl3d32', 'odbc32', 'ole32', 'oleaut32', 'uuid', 'rpcrt4',
            'advapi32', 'wsock32']

    cflags = None
    lflags = None

    if not FINAL and HYBRID:
        cflags = ['/Z7', '/Od']
        lflags = ['/DEBUG']


elif os.name == 'posix':
    # Set flags for Unix type platforms

    WXDIR = '..'              # assumes IN_CVS_TREE
    WXPLAT = '__WXGTK__'      # and assumes GTK...
    GENDIR = 'gtk'            # Need to allow for Motif eventually too

    includes = ['src']
    defines = [('SWIG_GLOBAL', None),
               ('HAVE_CONFIG_H', None),
               ('WXP_USE_THREAD', '1'),
               ]
    libdirs = []
    libs = []

    cflags = os.popen('wx-config --cflags', 'r').read()[:-1] + ' ' + \
             os.popen('gtk-config --cflags', 'r').read()[:-1]
    cflags = string.split(cflags)

    lflags = os.popen('wx-config --libs', 'r').read()[:-1] + ' ' + \
             os.popen('gtk-config --libs', 'r').read()[:-1]
    lflags = string.split(lflags)


else:
    raise 'Sorry Charlie...'



#----------------------------------------------------------------------
# Define the CORE extension module
#----------------------------------------------------------------------

print 'Preparing CORE...'
swig_force = force
swig_args = ['-c++', '-shadow', '-python', '-keyword', '-dnone',
             '-I./src', '-D'+WXPLAT]

swig_files = [ 'wx.i', 'windows.i', 'windows2.i', 'windows3.i', 'events.i',
               'misc.i', 'misc2.i', 'utils.i', 'gdi.i', 'mdi.i', 'controls.i',
               'controls2.i', 'cmndlgs.i', 'stattool.i', 'frames.i', 'image.i',
               'printfw.i', 'sizers.i', 'clip_dnd.i', 'grid.i', 'html.i',
               'htmlhelp.i', 'calendar.i',
               ]

swig_sources = run_swig(swig_files, 'src', GENDIR, PKGDIR,
                        USE_SWIG, swig_force, swig_args)

copy_file('src/__init__.py', PKGDIR, update=1, verbose=0)
copy_file('src/__version__.py', PKGDIR, update=1, verbose=0)


if IN_CVS_TREE:   # update the licence files
    mkpath('licence')
    for file in ['preamble.txt', 'licence.txt', 'licendoc.txt', 'lgpl.txt']:
        copy_file(WXDIR+'/docs/'+file, 'licence/'+file, update=1, verbose=0)


if os.name == 'nt':
    rc_file = ['src/wxc.rc']
else:
    rc_file = []


ext = Extension('wxc', ['src/helpers.cpp',
                        'src/libpy.c',
                        ] + rc_file + swig_sources,

                include_dirs = includes,
                define_macros = defines,

                library_dirs = libdirs,
                libraries = libs,

                extra_compile_args = cflags,
                extra_link_args = lflags,
                )


wxpExtensions.append(ext)

if os.name == 'nt':
    libs = libs[:]
    if debug:
        libs.insert(0, 'wxc_d')
    else:
        libs.insert(0, 'wxc')

#----------------------------------------------------------------------
# Define the GLCanvas extension module
#----------------------------------------------------------------------

if BUILD_GLCANVAS:
    print 'Preparing GLCANVAS...'
    location = 'contrib/glcanvas'
    swig_files = ['glcanvas.i']

    swig_sources = run_swig(swig_files, location, GENDIR, PKGDIR,
                            USE_SWIG, swig_force, swig_args)

    gl_libs = []
    if os.name == 'posix':
        if '-D__WXDEBUG__' in cflags:
            gl_libs = ['wx_gtkd_gl', 'GL', 'GLU']
        else:
            gl_libs = ['wx_gtk_gl', 'GL', 'GLU']

    ext = Extension('glcanvasc',
                    swig_sources,

                    include_dirs = includes,
                    define_macros = defines,

                    library_dirs = libdirs,
                    libraries = libs + gl_libs,

                    extra_compile_args = cflags,
                    extra_link_args = lflags,
                    )

    wxpExtensions.append(ext)

#----------------------------------------------------------------------
# Define the OGL extension module
#----------------------------------------------------------------------

if BUILD_OGL:
    print 'Preparing OGL...'
    location = 'contrib/ogl'
    OGLLOC = location + '/contrib/src/ogl'
    OGLINC = location + '/contrib/include'

    swig_files = ['ogl.i', 'oglbasic.i', 'oglshapes.i', 'oglshapes2.i',
                  'oglcanvas.i']

    swig_sources = run_swig(swig_files, location, '', PKGDIR,
                            USE_SWIG, swig_force, swig_args)

    # make sure local copy of contrib files are up to date
    if IN_CVS_TREE:
        contrib_copy_tree(WXDIR + '/contrib/include/wx/ogl', OGLINC+'/wx/ogl')
        contrib_copy_tree(WXDIR + '/contrib/src/ogl', OGLLOC)

    ext = Extension('oglc', [location + '/oglhelpers.cpp',
                             '%s/basic.cpp' % OGLLOC,
                             '%s/bmpshape.cpp' % OGLLOC,
                             '%s/composit.cpp' % OGLLOC,
                             '%s/divided.cpp' % OGLLOC,
                             '%s/lines.cpp' % OGLLOC,
                             '%s/misc.cpp' % OGLLOC,
                             '%s/basic2.cpp' % OGLLOC,
                             '%s/canvas.cpp' % OGLLOC,
                             '%s/constrnt.cpp' % OGLLOC,
                             '%s/drawn.cpp' % OGLLOC,
                             '%s/mfutils.cpp' % OGLLOC,
                             '%s/ogldiag.cpp' % OGLLOC,
                             ] + swig_sources,

                    include_dirs =  [OGLINC] + includes,
                    define_macros = defines,

                    library_dirs = libdirs,
                    libraries = libs,

                    extra_compile_args = cflags,
                    extra_link_args = lflags,
                    )

    wxpExtensions.append(ext)

#----------------------------------------------------------------------
# Define the STC extension module
#----------------------------------------------------------------------

if BUILD_STC:
    print 'Preparing STC...'
    location = 'contrib/stc'
    STCLOC = location + '/contrib/src/stc'
    STCINC = location + '/contrib/include'
    STC_H =  location + '/contrib/include/wx/stc'

    # make sure local copy of contrib files are up to date
    if IN_CVS_TREE:
        contrib_copy_tree(WXDIR + '/contrib/include/wx/stc', STCINC+'/wx/stc')
        contrib_copy_tree(WXDIR + '/contrib/src/stc', STCLOC)


    swig_files = ['stc_.i']
    swig_sources = run_swig(swig_files, location, '', PKGDIR,
                            USE_SWIG, swig_force,
                            swig_args + ['-I'+STC_H, '-I'+location],
                            [STC_H+'/stc.h'])

    # copy a projext specific py module to the main package dir
    copy_file(location+'/stc.py', PKGDIR, update=1, verbose=1)

    # add some include dirs to the standard set
    stc_includes = includes[:]
    stc_includes.append('%s/scintilla/include' % STCLOC)
    stc_includes.append('%s/scintilla/src' % STCLOC)
    stc_includes.append(STCINC)

    # and some macro definitions
    stc_defines = defines[:]
    stc_defines.append( ('__WX__', None) )
    stc_defines.append( ('SCI_LEXER', None) )


    ext = Extension('stc_c',
                    ['%s/scintilla/src/AutoComplete.cxx' % STCLOC,
                     '%s/scintilla/src/CallTip.cxx' % STCLOC,
                     '%s/scintilla/src/CellBuffer.cxx' % STCLOC,
                     '%s/scintilla/src/ContractionState.cxx' % STCLOC,
                     '%s/scintilla/src/Document.cxx' % STCLOC,
                     '%s/scintilla/src/Editor.cxx' % STCLOC,
                     '%s/scintilla/src/Indicator.cxx' % STCLOC,
                     '%s/scintilla/src/KeyMap.cxx' % STCLOC,
                     '%s/scintilla/src/KeyWords.cxx' % STCLOC,
                     '%s/scintilla/src/LineMarker.cxx' % STCLOC,
                     '%s/scintilla/src/PropSet.cxx' % STCLOC,
                     '%s/scintilla/src/ScintillaBase.cxx' % STCLOC,
                     '%s/scintilla/src/Style.cxx' % STCLOC,
                     '%s/scintilla/src/ViewStyle.cxx' % STCLOC,
                     '%s/scintilla/src/LexCPP.cxx' % STCLOC,
                     '%s/scintilla/src/LexHTML.cxx' % STCLOC,
                     '%s/scintilla/src/LexLua.cxx' % STCLOC,
                     '%s/scintilla/src/LexOthers.cxx' % STCLOC,
                     '%s/scintilla/src/LexPerl.cxx' % STCLOC,
                     '%s/scintilla/src/LexPython.cxx' % STCLOC,
                     '%s/scintilla/src/LexSQL.cxx' % STCLOC,
                     '%s/scintilla/src/LexVB.cxx' % STCLOC,
                     '%s/scintilla/src/DocumentAccessor.cxx' % STCLOC,
                     '%s/scintilla/src/UniConversion.cxx' % STCLOC,
                     '%s/scintilla/src/WindowAccessor.cxx' % STCLOC,
                     '%s/scintilla/src/PosRegExp.cxx' % STCLOC,

                     '%s/PlatWX.cpp' % STCLOC,
                     '%s/ScintillaWX.cpp' % STCLOC,
                     '%s/stc.cpp' % STCLOC,
                     ] + swig_sources,

                    include_dirs = stc_includes,
                    define_macros = stc_defines,

                    library_dirs = libdirs,
                    libraries = libs,

                    extra_compile_args = cflags,
                    extra_link_args = lflags,
                    )

    wxpExtensions.append(ext)


#----------------------------------------------------------------------
# Do the Setup/Build/Install/Whatever
#----------------------------------------------------------------------

setup(name             = PKGDIR,
      version          = VERSION,
      description      = DESCRIPTION,
      long_description = LONG_DESCRIPTION,
      author           = AUTHOR,
      author_email     = AUTHOR_EMAIL,
      url              = URL,
      licence          = LICENCE,

      packages = [PKGDIR,
                  PKGDIR+'.lib',
                  PKGDIR+'.lib.editor',
                  PKGDIR+'.lib.sizers'
                  ],

      ext_package = PKGDIR,
      ext_modules = wxpExtensions,

      )


