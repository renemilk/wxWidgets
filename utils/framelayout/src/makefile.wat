# Objects makefile

WXDIR = ..\..\..

!include $(WXDIR)\src\makewat.env

EXTRACPPFLAGS=/DPROLOGIO

OBJECTSLIB = $(WXDIR)\utils\objects\lib\fl.lib
THISDIR = $(WXDIR)\utils\objects\src

NAME = fl
LNK = $(name).lnk

IFLAGS      = -i=$(WXINC) -i=$(WXBASEINC) -i=..\..\mfutils\src -i=..\..\prologio\src

OBJECTS = antiflickpl.obj bardragpl.obj barhintspl.obj cbcustom.obj\
 controlarea.obj controlbar.obj dyntbar.obj dyntbarhnd.obj\
 frmview.obj garbagec.obj gcupdatesmgr.obj hintanimpl.obj newbmpbtn.obj\
 panedrawpl.obj rowdragpl.obj rowlayoutpl.obj\
 toolwnd.obj updatesmgr.obj 

all: $(OBJECTSLIB)

$(OBJECTSLIB): $(OBJECTS)
	*wlib /b /c /n /P=256 $(OBJECTSLIB) $(OBJECTS)

clean:   .SYMBOLIC
    -erase *.obj *.bak *.err *.pch $(OBJECTSLIB) *.lbc



