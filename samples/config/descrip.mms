#*****************************************************************************
#                                                                            *
# Make file for VMS                                                          *
# Author : J.Jansen (joukj@hrem.stm.tudelft.nl)                              *
# Date : 10 November 1999                                                     *
#                                                                            *
#*****************************************************************************
.first
	define wx [--.include.wx]

.ifdef __WXMOTIF__
CXX_DEFINE = /define=(__WXMOTIF__=1)/name=(as_is,short)\
	   /assume=(nostdnew,noglobal_array_new)
.else
.ifdef __WXGTK__
CXX_DEFINE = /define=(__WXGTK__=1)/float=ieee/name=(as_is,short)/ieee=denorm\
	   /assume=(nostdnew,noglobal_array_new)
.else
CXX_DEFINE =
.endif
.endif

.suffixes : .cpp

.cpp.obj :
	cxx $(CXXFLAGS)$(CXX_DEFINE) $(MMS$TARGET_NAME).cpp

all :
.ifdef __WXMOTIF__
	$(MMS)$(MMSQUALIFIERS) conftest.exe
.else
.ifdef __WXGTK__
	$(MMS)$(MMSQUALIFIERS) conftest_gtk.exe
.endif
.endif

.ifdef __WXMOTIF__
conftest.exe : conftest.obj
	cxxlink conftest,[--.lib]vms/opt
.else
.ifdef __WXGTK__
conftest_gtk.exe : conftest.obj
	cxxlink/exec=conftest_gtk.exe conftest,[--.lib]vms_gtk/opt
.endif
.endif

conftest.obj : conftest.cpp
