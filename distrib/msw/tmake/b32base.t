#!#############################################################################
#! File:    baseb32.t
#! Purpose: tmake template file from which makebase.b32 is generated by running
#!          tmake -t b32 wxwin.pro
#! Author:  Michael Fielding
#! Created: 6-08-02
#! Version: $Id$
#!#############################################################################

#${
    #! include the code which parses filelist.txt file and initializes
    #! %wxCommon, %wxGeneric and %wxMSW hashes.
    IncludeTemplate("filelist.t");

    #! now transform these hashes into $project tags
    foreach $file (sort keys %wxCommon) {
        next if $wxCommon{$file} !~ /\bB\b/;

        $isCFile = $file =~ /\.c$/;
        $file =~ s/cp?p?$/obj/;
        $obj = "\$(MSWDIR)\\" . $file . " ";
        $project{"WXCOMMONOBJS"} .= $obj;
        $project{"WXCOBJS"} .= $obj if $isCFile;
    }

    foreach $file (sort keys %wxMSW) {
        next if $wxMSW{$file} !~ /\bB\b/;

        $isCFile = $file =~ /\.c$/;

        my $isOleObj = $wxMSW{$file} =~ /\bO\b/;
        $file =~ s/cp?p?$/obj/;
        my $obj = "\$(MSWDIR)\\" . $file . " ";

        $project{"WXMSWOBJS"} .= $obj;
        if ( $isOleObj ) {
            #! remember that this file is in ole subdir
            $project{"WXOLEOBJS"} .= $obj;
        }
        $project{"WXCOBJS"} .= $obj if $isCFile;
    }

    foreach $file (sort keys %wxBase) {
        $isCFile = $file =~ /\.c$/;
        $file =~ s/cp?p?$/obj/;
        $project{"WXCOMMONOBJS"} .= "\$(MSWDIR)\\" . $file . " ";
        $project{"WXCOBJS"} .= $obj if $isCFile;
    }
#$}



# This file was automatically generated by tmake
# DO NOT CHANGE THIS FILE, YOUR CHANGES WILL BE LOST! CHANGE B32.T!

#
# File:     makebase.b32
# Author:   Julian Smart
# Created:  1998
# Updated:  Michael Fielding, 2002
# Copyright:
#
# "%W% %G%"
#
# Makefile : Builds wxWindows library wxbase.lib for MS Windows,
# and Borland C++ (32-bit).

# BCCDIR now defined in ../makeb32.env

!if "$(WXWIN)" == ""
!error You must define the WXWIN variable in autoexec.bat, e.g. WXWIN=c:\wx
!endif

WXDIR = $(WXWIN)
THISDIR = $(WXDIR)\src\msw

# Set all these to 1 if you want to build a dynamic library
!if "$(DLL)" == "1"
WXMAKINGDLL=1
WXBUILDDLL=1
!endif

wxUSE_GUI=0

!include $(WXDIR)\src\makeb32.env

PERIPH_LIBS=
PERIPH_TARGET=zlib regex $(PERIPH_TARGET)
PERIPH_CLEAN_TARGET=clean_zlib clean_regex $(PERIPH_CLEAN_TARGET)

!if "$(DLL)" == "0"
DUMMY=dummy
!else
DUMMY=dummydll
LIBS= cw32mti import32 ole2w32 odbc32 zlib winpng jpeg tiff regex
!endif

LIBTARGET=$(WXLIB)

GENDIR=..\generic
COMMDIR=..\common
HTMLDIR=..\html
OLEDIR=.\ole
MSWDIR=.

DOCDIR = $(WXDIR)\docs

COMMONOBJS = \
    #$ ExpandList("WXCOMMONOBJS");

MSWOBJS = #$ ExpandList("WXMSWOBJS");

OBJECTS = $(COMMONOBJS) $(MSWOBJS)

default:    wx

wx:    $(ARCHINCDIR)\wx makesetuph makearchsetuph $(CFG) $(DUMMY).obj $(OBJECTS) $(PERIPH_TARGET) $(LIBTARGET)

all:    wx

# Copy the in-CVS setup0.h to setup.h if necessary
makesetuph:
     cd $(WXDIR)\include\wx\msw
     if not exist setup.h copy setup0.h setup.h
     cd $(WXDIR)\src\msw

# Copy include\wx\msw\setup.h to the architecture-specific location
makearchsetuph:
     copy $(WXDIR)\include\wx\msw\setup.h $(ARCHSETUPH)
     cd $(WXDIR)\src\msw

$(ARCHINCDIR)\wx:
    -mkdir $(ARCHINCDIR)
    -mkdir $(ARCHINCDIR)\wx
    -$(RM) $(CFG)

!if "$(DLL)" == "0"

$(LIBTARGET): $(DUMMY).obj $(OBJECTS)
    -$(RM) $(WXLIB)
    tlib "$(WXLIB)" /P1024 @&&!
+$(OBJECTS:.obj =.obj +) +$(PERIPH_LIBS:.lib =.lib +)
!

!else

$(LIBTARGET): $(DUMMY).obj $(OBJECTS)
    -$(RM) $(WXLIB)
    -$(RM) $(WXDLL)
        $(LINK) $(LINK_FLAGS) /L$(WXLIBDIR);$(BCCDIR)\lib;$(BCCDIR)\lib\psdk /v @&&!
c0d32.obj $(OBJECTS)
$(WXLIBDIR)\$(WXLIBNAME)
nul
$(PERIPH_LIBS) $(LIBS)
wxb32
!
        implib -c $(LIBTARGET) $(WXLIBDIR)\$(WXLIBNAME).dll
!endif

dummy.obj: dummy.$(SRCSUFF) $(LOCALHEADERS) $(BASEHEADERS) $(WXDIR)\include\wx\wx.h
dummydll.obj: dummydll.$(SRCSUFF) $(LOCALHEADERS) $(BASEHEADERS) $(WXDIR)\include\wx\wx.h version.res

version.res:
    brc32 -r -i$(WXDIR)\include\ $(MSWDIR)\version.rc

#${
    $_ = $project{"WXMSWOBJS"};
    my @objs = split;
    foreach (@objs) {
        $text .= $_ . ": ";
        if ( $project{"WXOLEOBJS"} =~ /\Q$_/ ) { s/MSWDIR/OLEDIR/; }
        $suffix = $project{"WXCOBJS"} =~ /\Q$_/ ? "c" : '$(SRCSUFF)';
        s/obj$/$suffix/;
        $text .= $_ . "\n\n";
    }
#$}

########################################################
# Common objects (always compiled)

#${
    $_ = $project{"WXCOMMONOBJS"};
    my @objs = split;
    foreach (@objs) {
        $text .= $_ . ": ";
        $suffix = $project{"WXCOBJS"} =~ /\Q$_/ ? "c" : '$(SRCSUFF)';
        s/MSWDIR/COMMDIR/;
        s/obj$/$suffix/;
        $text .= $_ . "\n\n";
    }
#$}


all_utils:
    cd $(WXDIR)\utils
    ${MAKE} -f makefile.b32
    cd $(WXDIR)\src\msw

all_samples:
    cd $(WXDIR)\samples
    ${MAKE} -f makefile.b32
    cd $(WXDIR)\src\msw

all_execs:
    cd $(WXDIR)\utils
    ${MAKE} -f makefile.b32 all_execs
    cd $(WXDIR)\src\msw

png:    $(CFG)
        cd $(WXDIR)\src\png
        ${MAKE} -f makefile.b32 wxUSE_GUI=0
        cd $(WXDIR)\src\msw

clean_png:
        cd $(WXDIR)\src\png
        ${MAKE} -f makefile.b32 clean
        cd $(WXDIR)\src\msw

zlib:   $(CFG)
        cd $(WXDIR)\src\zlib
        ${MAKE} -f makefile.b32 wxUSE_GUI=0 lib
        cd $(WXDIR)\src\msw

clean_zlib:
        cd $(WXDIR)\src\zlib
        ${MAKE} -f makefile.b32 clean
        cd $(WXDIR)\src\msw

jpeg:    $(CFG)
        cd $(WXDIR)\src\jpeg
        ${MAKE} -f makefile.b32 wxUSE_GUI=0
        cd $(WXDIR)\src\msw

clean_jpeg:
        cd $(WXDIR)\src\jpeg
        ${MAKE} -f makefile.b32 clean
        cd $(WXDIR)\src\msw

regex:   $(CFG)
        cd $(WXDIR)\src\regex
        ${MAKE} -f makefile.b32 wxUSE_GUI=0 lib
        cd $(WXDIR)\src\msw

clean_regex:
        cd $(WXDIR)\src\regex
        ${MAKE} -f makefile.b32 clean
        cd $(WXDIR)\src\msw

tiff:   $(CFG)
        cd $(WXDIR)\src\tiff
        ${MAKE} -f makefile.b32 wxUSE_GUI=0 lib
        cd $(WXDIR)\src\msw

clean_tiff:
        cd $(WXDIR)\src\tiff
        ${MAKE} -f makefile.b32 clean
        cd $(WXDIR)\src\msw

$(CFG): makebase.b32
    copy &&!
-Hc
-H=$(WXDIR)\src\msw\wx32.csm
-3
-d
-a1 # byte alignment
-R-
-X
-w-par
-w-aus
-w-hid # virtual function A hides virtual function B
-tWM
-tWC

-I$(ARCHINCDIR);$(WXINC);$(BCCDIR)\include;$(WXDIR)/src/generic;$(WXDIR)/src/png;$(WXDIR)/src/jpeg;$(WXDIR)/src/zlib;$(WXDIR)/src/tiff;$(THISDIR)
-I$(WXDIR)\include\wx\msw\gnuwin32

-L$(BCCDIR)\lib;$(BCCDIR)\lib\psdk
-D__WXWIN__
-D__WXMSW__
-D__WINDOWS__
-DWIN32
$(OPT)
$(DEBUG_FLAGS)
$(WIN95FLAG)
! $(CFG)

clean: $(PERIPH_CLEAN_TARGET)
    -$(RM) $(WXLIBDIR)\wx.tds
    -$(RM) $(WXLIBDIR)\wx.il?
    -$(RM) *.obj
    -$(RM) *.pch
    -$(RM) *.csm
    -$(RM) "wx32.#??"

cleanall: clean

self:
    cd $(WXWIN)\distrib\msw\tmake
    tmake -t $(MFTYPE) wxwin.pro -o makefile.$(MFTYPE)
    copy makefile.$(MFTYPE) $(WXWIN)\src\msw
