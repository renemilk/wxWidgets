
# This file was automatically generated by tmake at 00:12, 1999/07/29
# DO NOT CHANGE THIS FILE, YOUR CHANGES WILL BE LOST! CHANGE GNU.T!

#
# File:     makefile.gnu
# Author:   Julian Smart, Robert Roebling, Vadim Zeitlin
# Created:  1993
# Updated:  1999
# Copyright:(c) 1993, AIAI, University of Edinburgh,
# Copyright:(c) 1999, Vadim Zeitlin
# Copyright:(c) 1999, Robert Roebling
#
# Makefile for libwx_gtk.a

# GUI
GUI = gtk

# Replace this with your own path if necessary
WXDIR = ../..

# C++ compiler
CC          = gcc

# C compiler
CCC         = gcc

# Compiler for lex/yacc .c programs
CCLEX       = $(CCC)

LEX         = lex
YACC        = yacc
# Note: if using bison, use -y option for yacc compatibility
# YACC      = bison -y
MAKE        = make
AROPTIONS   = ruv
RANLIB      = ranlib

# Debug/trace mode. 1 or more for debugging.
DEBUG       = 1

# Misc options
CPPOPTIONS  = __WXGTK__ -D__WXDEBUG__ -fno-rtti -fno-exceptions
COPTIONS    = __WXGTK__ -D__WXDEBUG__
CPPWARN     = -Wall
CWARN       = -Wall
OPTIMISE    = -O2

# object file suffix
OBJSUFF     =o

# no changes below this line

GUIINCLUDES  = `gtk-config --cflags`

WXINC       = -I$(WXDIR)/include
WXLIB       = $(WXDIR)/lib/libwx_$(GUI).a
INCLUDES    = $(WXINC) $(GUIINCLUDES) -I$(WXDIR)/src/png -I$(WXDIR)/src/jpeg -I$(WXDIR)/src/zlib

CPPFLAGS    = $(INCLUDES) $(CPPOPTIONS) $(CPPWARN) $(OPTIMISE)
CFLAGS      = $(INCLUDES) $(COPTIONS) $(CWARN) $(OPTIMISE)

# Clears all default suffixes
.SUFFIXES:	.o .cpp .c

.c.o :
	$(CCC) -c $(CFLAGS) -o $@ $<

.cpp.o :
	$(CC) -c $(CPPFLAGS) -o $@ $<

.C.o :
	$(CC) -c $(CPPFLAGS) -o $@ $<

# Subordinate library possibilities

GENDIR  = $(WXDIR)/src/generic
COMMDIR = $(WXDIR)/src/common
HTMLDIR = $(WXDIR)/src/html
UNIXDIR = $(WXDIR)/src/unix
PNGDIR  = $(WXDIR)/src/png
JPEGDIR = $(WXDIR)/src/jpeg
ZLIBDIR = $(WXDIR)/src/zlib
GTKDIR  = $(WXDIR)/src/gtk

DOCDIR = $(WXDIR)\docs

GENERICOBJS = \
                $(GENDIR)/busyinfo.$(OBJSUFF) \
		$(GENDIR)/caret.$(OBJSUFF) \
		$(GENDIR)/choicdgg.$(OBJSUFF) \
		$(GENDIR)/colrdlgg.$(OBJSUFF) \
		$(GENDIR)/dirdlgg.$(OBJSUFF) \
		$(GENDIR)/extdlgg.$(OBJSUFF) \
		$(GENDIR)/fontdlgg.$(OBJSUFF) \
		$(GENDIR)/gridg.$(OBJSUFF) \
		$(GENDIR)/helpxlp.$(OBJSUFF) \
		$(GENDIR)/imaglist.$(OBJSUFF) \
		$(GENDIR)/laywin.$(OBJSUFF) \
		$(GENDIR)/listctrl.$(OBJSUFF) \
		$(GENDIR)/msgdlgg.$(OBJSUFF) \
		$(GENDIR)/notebook.$(OBJSUFF) \
		$(GENDIR)/numdlgg.$(OBJSUFF) \
		$(GENDIR)/panelg.$(OBJSUFF) \
		$(GENDIR)/printps.$(OBJSUFF) \
		$(GENDIR)/prntdlgg.$(OBJSUFF) \
		$(GENDIR)/progdlgg.$(OBJSUFF) \
		$(GENDIR)/prop.$(OBJSUFF) \
		$(GENDIR)/propform.$(OBJSUFF) \
		$(GENDIR)/proplist.$(OBJSUFF) \
		$(GENDIR)/sashwin.$(OBJSUFF) \
		$(GENDIR)/scrolwin.$(OBJSUFF) \
		$(GENDIR)/splitter.$(OBJSUFF) \
		$(GENDIR)/statusbr.$(OBJSUFF) \
		$(GENDIR)/tabg.$(OBJSUFF) \
		$(GENDIR)/textdlgg.$(OBJSUFF) \
		$(GENDIR)/tipdlg.$(OBJSUFF) \
		$(GENDIR)/treectrl.$(OBJSUFF)

COMMONOBJS  = \
		$(COMMDIR)/y_tab.$(OBJSUFF) \
		$(COMMDIR)/cmndata.$(OBJSUFF) \
		$(COMMDIR)/config.$(OBJSUFF) \
		$(COMMDIR)/date.$(OBJSUFF) \
		$(COMMDIR)/datstrm.$(OBJSUFF) \
		$(COMMDIR)/db.$(OBJSUFF) \
		$(COMMDIR)/dbtable.$(OBJSUFF) \
		$(COMMDIR)/dcbase.$(OBJSUFF) \
		$(COMMDIR)/dlgcmn.$(OBJSUFF) \
		$(COMMDIR)/docmdi.$(OBJSUFF) \
		$(COMMDIR)/docview.$(OBJSUFF) \
		$(COMMDIR)/dynarray.$(OBJSUFF) \
		$(COMMDIR)/dynlib.$(OBJSUFF) \
		$(COMMDIR)/event.$(OBJSUFF) \
		$(COMMDIR)/extended.$(OBJSUFF) \
		$(COMMDIR)/ffile.$(OBJSUFF) \
		$(COMMDIR)/file.$(OBJSUFF) \
		$(COMMDIR)/fileconf.$(OBJSUFF) \
		$(COMMDIR)/filefn.$(OBJSUFF) \
		$(COMMDIR)/filesys.$(OBJSUFF) \
		$(COMMDIR)/framecmn.$(OBJSUFF) \
		$(COMMDIR)/fs_inet.$(OBJSUFF) \
		$(COMMDIR)/fs_zip.$(OBJSUFF) \
		$(COMMDIR)/ftp.$(OBJSUFF) \
		$(COMMDIR)/gdicmn.$(OBJSUFF) \
		$(COMMDIR)/hash.$(OBJSUFF) \
		$(COMMDIR)/helpbase.$(OBJSUFF) \
		$(COMMDIR)/http.$(OBJSUFF) \
		$(COMMDIR)/imagbmp.$(OBJSUFF) \
		$(COMMDIR)/image.$(OBJSUFF) \
		$(COMMDIR)/imaggif.$(OBJSUFF) \
		$(COMMDIR)/imagjpeg.$(OBJSUFF) \
		$(COMMDIR)/imagpng.$(OBJSUFF) \
		$(COMMDIR)/intl.$(OBJSUFF) \
		$(COMMDIR)/ipcbase.$(OBJSUFF) \
		$(COMMDIR)/layout.$(OBJSUFF) \
		$(COMMDIR)/list.$(OBJSUFF) \
		$(COMMDIR)/log.$(OBJSUFF) \
		$(COMMDIR)/memory.$(OBJSUFF) \
		$(COMMDIR)/mimetype.$(OBJSUFF) \
		$(COMMDIR)/module.$(OBJSUFF) \
		$(COMMDIR)/mstream.$(OBJSUFF) \
		$(COMMDIR)/object.$(OBJSUFF) \
		$(COMMDIR)/objstrm.$(OBJSUFF) \
		$(COMMDIR)/paper.$(OBJSUFF) \
		$(COMMDIR)/prntbase.$(OBJSUFF) \
		$(COMMDIR)/process.$(OBJSUFF) \
		$(COMMDIR)/protocol.$(OBJSUFF) \
		$(COMMDIR)/resource.$(OBJSUFF) \
		$(COMMDIR)/sckaddr.$(OBJSUFF) \
		$(COMMDIR)/sckfile.$(OBJSUFF) \
		$(COMMDIR)/sckipc.$(OBJSUFF) \
		$(COMMDIR)/sckstrm.$(OBJSUFF) \
		$(COMMDIR)/socket.$(OBJSUFF) \
		$(COMMDIR)/stream.$(OBJSUFF) \
		$(COMMDIR)/string.$(OBJSUFF) \
		$(COMMDIR)/tbarbase.$(OBJSUFF) \
		$(COMMDIR)/tbarsmpl.$(OBJSUFF) \
		$(COMMDIR)/textcmn.$(OBJSUFF) \
		$(COMMDIR)/textfile.$(OBJSUFF) \
		$(COMMDIR)/time.$(OBJSUFF) \
		$(COMMDIR)/timercmn.$(OBJSUFF) \
		$(COMMDIR)/tokenzr.$(OBJSUFF) \
		$(COMMDIR)/txtstrm.$(OBJSUFF) \
		$(COMMDIR)/unzip.$(OBJSUFF) \
		$(COMMDIR)/url.$(OBJSUFF) \
		$(COMMDIR)/utilscmn.$(OBJSUFF) \
		$(COMMDIR)/valgen.$(OBJSUFF) \
		$(COMMDIR)/validate.$(OBJSUFF) \
		$(COMMDIR)/valtext.$(OBJSUFF) \
		$(COMMDIR)/variant.$(OBJSUFF) \
		$(COMMDIR)/wfstream.$(OBJSUFF) \
		$(COMMDIR)/wincmn.$(OBJSUFF) \
		$(COMMDIR)/wxchar.$(OBJSUFF) \
		$(COMMDIR)/wxexpr.$(OBJSUFF) \
		$(COMMDIR)/zipstream.$(OBJSUFF) \
		$(COMMDIR)/zstream.$(OBJSUFF)

HTMLOBJS = \
                $(HTMLDIR)/htmlcell.$(OBJSUFF) \
		$(HTMLDIR)/htmlfilter.$(OBJSUFF) \
		$(HTMLDIR)/htmlhelp.$(OBJSUFF) \
		$(HTMLDIR)/htmlhelp_io.$(OBJSUFF) \
		$(HTMLDIR)/htmlparser.$(OBJSUFF) \
		$(HTMLDIR)/htmltag.$(OBJSUFF) \
		$(HTMLDIR)/htmlwin.$(OBJSUFF) \
		$(HTMLDIR)/htmlwinparser.$(OBJSUFF) \
		$(HTMLDIR)/mod_fonts.$(OBJSUFF) \
		$(HTMLDIR)/mod_hline.$(OBJSUFF) \
		$(HTMLDIR)/mod_image.$(OBJSUFF) \
		$(HTMLDIR)/mod_layout.$(OBJSUFF) \
		$(HTMLDIR)/mod_links.cpp \
		$(HTMLDIR)/mod_list.$(OBJSUFF) \
		$(HTMLDIR)/mod_pre.$(OBJSUFF) \
		$(HTMLDIR)/mod_tables.$(OBJSUFF) \
		$(HTMLDIR)/search.$(OBJSUFF)

GTKOBJS     = \
		$(GTKDIR)/accel.$(OBJSUFF) \
		$(GTKDIR)/app.$(OBJSUFF) \
		$(GTKDIR)/bitmap.$(OBJSUFF) \
		$(GTKDIR)/bmpbuttn.$(OBJSUFF) \
		$(GTKDIR)/brush.$(OBJSUFF) \
		$(GTKDIR)/button.$(OBJSUFF) \
		$(GTKDIR)/checkbox.$(OBJSUFF) \
		$(GTKDIR)/checklst.$(OBJSUFF) \
		$(GTKDIR)/choicdlg.$(OBJSUFF) \
		$(GTKDIR)/choice.$(OBJSUFF) \
		$(GTKDIR)/clipbrd.$(OBJSUFF) \
		$(GTKDIR)/colour.$(OBJSUFF) \
		$(GTKDIR)/combobox.$(OBJSUFF) \
		$(GTKDIR)/control.$(OBJSUFF) \
		$(GTKDIR)/cursor.$(OBJSUFF) \
		$(GTKDIR)/data.$(OBJSUFF) \
		$(GTKDIR)/dataobj.$(OBJSUFF) \
		$(GTKDIR)/dc.$(OBJSUFF) \
		$(GTKDIR)/dcclient.$(OBJSUFF) \
		$(GTKDIR)/dcmemory.cpp \
		$(GTKDIR)/dcscreen.$(OBJSUFF) \
		$(GTKDIR)/dialog.$(OBJSUFF) \
		$(GTKDIR)/dnd.$(OBJSUFF) \
		$(GTKDIR)/filedlg.$(OBJSUFF) \
		$(GTKDIR)/font.$(OBJSUFF) \
		$(GTKDIR)/frame.$(OBJSUFF) \
		$(GTKDIR)/gauge.$(OBJSUFF) \
		$(GTKDIR)/gdiobj.$(OBJSUFF) \
		$(GTKDIR)/gsockgtk.$(OBJSUFF) \
		$(GTKDIR)/icon.$(OBJSUFF) \
		$(GTKDIR)/listbox.$(OBJSUFF) \
		$(GTKDIR)/main.$(OBJSUFF) \
		$(GTKDIR)/mdi.$(OBJSUFF) \
		$(GTKDIR)/menu.$(OBJSUFF) \
		$(GTKDIR)/minifram.$(OBJSUFF) \
		$(GTKDIR)/msgdlg.$(OBJSUFF) \
		$(GTKDIR)/notebook.$(OBJSUFF) \
		$(GTKDIR)/palette.$(OBJSUFF) \
		$(GTKDIR)/pen.$(OBJSUFF) \
		$(GTKDIR)/radiobox.$(OBJSUFF) \
		$(GTKDIR)/radiobut.$(OBJSUFF) \
		$(GTKDIR)/region.$(OBJSUFF) \
		$(GTKDIR)/scrolbar.$(OBJSUFF) \
		$(GTKDIR)/settings.$(OBJSUFF) \
		$(GTKDIR)/slider.$(OBJSUFF) \
		$(GTKDIR)/spinbutt.$(OBJSUFF) \
		$(GTKDIR)/statbmp.$(OBJSUFF) \
		$(GTKDIR)/statbox.$(OBJSUFF) \
		$(GTKDIR)/statline.$(OBJSUFF) \
		$(GTKDIR)/stattext.$(OBJSUFF) \
		$(GTKDIR)/tbargtk.$(OBJSUFF) \
		$(GTKDIR)/textctrl.$(OBJSUFF) \
		$(GTKDIR)/textdlg.$(OBJSUFF) \
		$(GTKDIR)/timer.$(OBJSUFF) \
		$(GTKDIR)/tooltip.$(OBJSUFF) \
		$(GTKDIR)/utilsgtk.$(OBJSUFF) \
		$(GTKDIR)/utilsres.$(OBJSUFF) \
		$(GTKDIR)/wave.$(OBJSUFF) \
		$(GTKDIR)/win_gtk.$(OBJSUFF) \
		$(GTKDIR)/window.$(OBJSUFF)

UINXOBJS     = \
		$(UNIXDIR)/gsocket.$(OBJSUFF) \
		$(UNIXDIR)/threadpsx.$(OBJSUFF) \
		$(UNIXDIR)/utilsunx.$(OBJSUFF)

ZLIBOBJS    = \
		$(ZLIBDIR)/adler32.$(OBJSUFF) \
		$(ZLIBDIR)/compress.$(OBJSUFF) \
		$(ZLIBDIR)/crc32.$(OBJSUFF) \
		$(ZLIBDIR)/gzio.$(OBJSUFF) \
		$(ZLIBDIR)/uncompr.$(OBJSUFF) \
		$(ZLIBDIR)/deflate.$(OBJSUFF) \
		$(ZLIBDIR)/trees.$(OBJSUFF) \
		$(ZLIBDIR)/zutil.$(OBJSUFF) \
		$(ZLIBDIR)/inflate.$(OBJSUFF) \
		$(ZLIBDIR)/infblock.$(OBJSUFF) \
		$(ZLIBDIR)/inftrees.$(OBJSUFF) \
		$(ZLIBDIR)/infcodes.$(OBJSUFF) \
		$(ZLIBDIR)/infutil.$(OBJSUFF) \
		$(ZLIBDIR)/inffast.$(OBJSUFF)

PNGOBJS     = \
		$(PNGDIR)/png.$(OBJSUFF) \
		$(PNGDIR)/pngread.$(OBJSUFF) \
		$(PNGDIR)/pngrtran.$(OBJSUFF) \
		$(PNGDIR)/pngrutil.$(OBJSUFF) \
		$(PNGDIR)/pngpread.$(OBJSUFF) \
		$(PNGDIR)/pngtrans.$(OBJSUFF) \
		$(PNGDIR)/pngwrite.$(OBJSUFF) \
		$(PNGDIR)/pngwtran.$(OBJSUFF) \
		$(PNGDIR)/pngwutil.$(OBJSUFF) \
		$(PNGDIR)/pngerror.$(OBJSUFF) \
		$(PNGDIR)/pngmem.$(OBJSUFF) \
		$(PNGDIR)/pngwio.$(OBJSUFF) \
		$(PNGDIR)/pngrio.$(OBJSUFF) \
		$(PNGDIR)/pngget.$(OBJSUFF) \
		$(PNGDIR)/pngset.$(OBJSUFF)


JPEGOBJS    = \
		$(JPEGDIR)/jcomapi.$(OBJSUFF) \
		$(JPEGDIR)/jutils.$(OBJSUFF) \
		$(JPEGDIR)/jerror.$(OBJSUFF) \
		$(JPEGDIR)/jmemmgr.$(OBJSUFF) \
		$(JPEGDIR)/jmemnobs.$(OBJSUFF) \
		$(JPEGDIR)/jcapimin.$(OBJSUFF) \
		$(JPEGDIR)/jcapistd.$(OBJSUFF) \
		$(JPEGDIR)/jctrans.$(OBJSUFF) \
		$(JPEGDIR)/jcparam.$(OBJSUFF) \
		$(JPEGDIR)/jdatadst.$(OBJSUFF) \
		$(JPEGDIR)/jcinit.$(OBJSUFF) \
		$(JPEGDIR)/jcmaster.$(OBJSUFF) \
		$(JPEGDIR)/jcmarker.$(OBJSUFF) \
		$(JPEGDIR)/jcmainct.$(OBJSUFF) \
		$(JPEGDIR)/jcprepct.$(OBJSUFF) \
		$(JPEGDIR)/jccoefct.$(OBJSUFF) \
		$(JPEGDIR)/jccolor.$(OBJSUFF) \
		$(JPEGDIR)/jcsample.$(OBJSUFF) \
		$(JPEGDIR)/jchuff.$(OBJSUFF) \
		$(JPEGDIR)/jcphuff.$(OBJSUFF) \
		$(JPEGDIR)/jcdctmgr.$(OBJSUFF) \
		$(JPEGDIR)/jfdctfst.$(OBJSUFF) \
		$(JPEGDIR)/jfdctflt.$(OBJSUFF) \
		$(JPEGDIR)/jfdctint.$(OBJSUFF) \
		$(JPEGDIR)/jdapimin.$(OBJSUFF) \
		$(JPEGDIR)/jdapistd.$(OBJSUFF) \
		$(JPEGDIR)/jdtrans.$(OBJSUFF) \
		$(JPEGDIR)/jdatasrc.$(OBJSUFF) \
		$(JPEGDIR)/jdmaster.$(OBJSUFF) \
		$(JPEGDIR)/jdinput.$(OBJSUFF) \
		$(JPEGDIR)/jdmarker.$(OBJSUFF) \
		$(JPEGDIR)/jdhuff.$(OBJSUFF) \
		$(JPEGDIR)/jdphuff.$(OBJSUFF) \
		$(JPEGDIR)/jdmainct.$(OBJSUFF) \
		$(JPEGDIR)/jdcoefct.$(OBJSUFF) \
		$(JPEGDIR)/jdpostct.$(OBJSUFF) \
		$(JPEGDIR)/jddctmgr.$(OBJSUFF) \
		$(JPEGDIR)/jidctfst.$(OBJSUFF) \
		$(JPEGDIR)/jidctflt.$(OBJSUFF) \
		$(JPEGDIR)/jidctint.$(OBJSUFF) \
		$(JPEGDIR)/jidctred.$(OBJSUFF) \
		$(JPEGDIR)/jdsample.$(OBJSUFF) \
		$(JPEGDIR)/jdcolor.$(OBJSUFF) \
		$(JPEGDIR)/jquant1.$(OBJSUFF) \
		$(JPEGDIR)/jquant2.$(OBJSUFF) \
		$(JPEGDIR)/jdmerge.$(OBJSUFF)

OBJECTS = $(GTKOBJS) $(COMMONOBJS) $(GENERICOBJS) $(HTMLOBJ) $(UNIXOBJS) \
	  $(JPEGOBJS) $(PNGOBJS) $(ZLIBOBJS)

all:    $(OBJECTS) $(WXLIB)

$(WXLIB): $(OBJECTS) $(EXTRAOBJS)
	ar $(AROPTIONS) $@ $(EXTRAOBJS) $(OBJECTS)
	$(RANLIB) $@

$(OBJECTS):	$(WXINC)/wx/defs.h $(WXINC)/wx/object.h $(WXINC)/wx/setup.h

$(COMMDIR)/y_tab.$(OBJSUFF):    $(COMMDIR)/y_tab.c $(COMMDIR)/lex_yy.c
	$(CCLEX) -c $(CFLAGS) -DNO_CONFIGURE -o $@ $(COMMDIR)/y_tab.c


$(COMMDIR)/lex_yy.c:	$(COMMDIR)/lexer.l
	$(LEX) -o $(COMMDIR)/lex.yy.c $(COMMDIR)/lexer.l 
	sed -e "s/BUFSIZ/5000/g" < $(COMMDIR)/lex.yy.c | \
	sed -e "s/yyoutput(c)/void yyoutput(c)/g" | \
        sed -e "s/YYLMAX 200/YYLMAX 5000/g" > $(COMMDIR)/lex_yy.c
	rm -f $(COMMDIR)/lex.yy.c

# Replace yacc with bison if you run into compilation
# problems with y_tab.c.
$(COMMDIR)/y_tab.c:	$(COMMDIR)/parser.y
	$(YACC) $(COMMDIR)/parser.y
	mv y.tab.c $(COMMDIR)/y_tab.c

clean:
	rm -f $(GTKDIR)/*.$(OBJSUFF)
	rm -f $(COMMDIR)/y_tab.c
	rm -f $(COMMDIR)/lex_yy.c
	rm -f $(COMMDIR)/*.$(OBJSUFF)
	rm -f $(GENDIR)/*.$(OBJSUFF)
	rm -f $(PNGDIR)/*.$(OBJSUFF)
	rm -f $(ZLIBDIR)/*.$(OBJSUFF)
	rm -f $(JPEGDIR)/*.$(OBJSUFF)
	rm -f $(UNIXDIR)/*.$(OBJSUFF)
	rm -f $(HTMLDIR)/*.$(OBJSUFF)
	rm -f $(WXWIN)/lib/libwx_$(GUI).a

cleanall: clean

MFTYPE=gnu
makefile.$(MFTYPE) : $(WXWIN)\distrib\msw\tmake\filelist.txt $(WXWIN)\distrib\msw\tmake\$(GUI)_$(MFTYPE).t
	cd $(WXWIN)\distrib\msw\tmake
	tmake -t $(GUI)_$(MFTYPE) wxwin.pro -o makefile.$(MFTYPE)
	copy makefile.$(MFTYPE) $(WXWIN)\src\$(GUI)
