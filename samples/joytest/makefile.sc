#Digital Mars (was Symantec) C++ makefile 
WXDIR = $(WXWIN)
EXTRALIBS=

TARGET=joytest
OBJECTS = $(TARGET).obj
include $(WXDIR)\src\makeprog.sc
