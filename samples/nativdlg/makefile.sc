#Digital Mars (was Symantec) C++ makefile 
WXDIR = $(WXWIN)
EXTRALIBS=
TARGET=nativdlg
OBJECTS = $(TARGET).obj
include $(WXDIR)\src\makeprog.sc
