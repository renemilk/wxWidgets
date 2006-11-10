# =========================================================================
#     This makefile was generated by
#     Bakefile 0.2.1 (http://bakefile.sourceforge.net)
#     Do not modify, all changes will be overwritten!
# =========================================================================

!include ../../../build/msw/config.wat

# -------------------------------------------------------------------------
# Do not modify the rest of this file!
# -------------------------------------------------------------------------

# Speed up compilation a bit:
!ifdef __LOADDLL__
!  loaddll wcc      wccd
!  loaddll wccaxp   wccdaxp
!  loaddll wcc386   wccd386
!  loaddll wpp      wppdi86
!  loaddll wppaxp   wppdaxp
!  loaddll wpp386   wppd386
!  loaddll wlink    wlink
!  loaddll wlib     wlibd
!endif

# We need these variables in some bakefile-made rules:
WATCOM_CWD = $+ $(%cdrive):$(%cwd) $-

### Conditionally set variables: ###

PORTNAME =
!ifeq USE_GUI 0
PORTNAME = base
!endif
!ifeq USE_GUI 1
PORTNAME = msw
!endif
VENDORTAG =
!ifeq OFFICIAL_BUILD 0
VENDORTAG = _$(VENDOR)
!endif
!ifeq OFFICIAL_BUILD 1
VENDORTAG = 
!endif
WXDEBUGFLAG =
!ifeq BUILD debug
!ifeq DEBUG_FLAG default
WXDEBUGFLAG = d
!endif
!endif
!ifeq DEBUG_FLAG 1
WXDEBUGFLAG = d
!endif
WXUNICODEFLAG =
!ifeq UNICODE 1
WXUNICODEFLAG = u
!endif
WXUNIVNAME =
!ifeq WXUNIV 1
WXUNIVNAME = univ
!endif
WXDLLFLAG =
!ifeq SHARED 1
WXDLLFLAG = dll
!endif
LIBTYPE_SUFFIX =
!ifeq SHARED 0
LIBTYPE_SUFFIX = lib
!endif
!ifeq SHARED 1
LIBTYPE_SUFFIX = dll
!endif
EXTRALIBS_FOR_BASE =
!ifeq MONOLITHIC 0
EXTRALIBS_FOR_BASE = 
!endif
!ifeq MONOLITHIC 1
EXTRALIBS_FOR_BASE =  
!endif
__fldll___depname =
!ifeq SHARED 1
__fldll___depname = &
	$(LIBDIRNAME)\wx$(PORTNAME)$(WXUNIVNAME)$(WX_RELEASE_NODOT)$(WXUNICODEFLAG)$(WXDEBUGFLAG)$(WX_LIB_FLAVOUR)_fl_wat$(VENDORTAG).dll
!endif
__DEBUGINFO_2 =
!ifeq BUILD debug
!ifeq DEBUG_INFO default
__DEBUGINFO_2 = debug all
!endif
!endif
!ifeq BUILD release
!ifeq DEBUG_INFO default
__DEBUGINFO_2 = 
!endif
!endif
!ifeq DEBUG_INFO 0
__DEBUGINFO_2 = 
!endif
!ifeq DEBUG_INFO 1
__DEBUGINFO_2 = debug all
!endif
__WXLIB_MONO_p =
!ifeq MONOLITHIC 1
__WXLIB_MONO_p = &
	wx$(PORTNAME)$(WXUNIVNAME)$(WX_RELEASE_NODOT)$(WXUNICODEFLAG)$(WXDEBUGFLAG)$(WX_LIB_FLAVOUR).lib
!endif
__LIB_TIFF_p =
!ifeq USE_GUI 1
__LIB_TIFF_p = wxtiff$(WXDEBUGFLAG).lib
!endif
__LIB_JPEG_p =
!ifeq USE_GUI 1
__LIB_JPEG_p = wxjpeg$(WXDEBUGFLAG).lib
!endif
__LIB_PNG_p =
!ifeq USE_GUI 1
__LIB_PNG_p = wxpng$(WXDEBUGFLAG).lib
!endif
__GDIPLUS_LIB_p =
!ifeq USE_GDIPLUS 1
__GDIPLUS_LIB_p = gdiplus.lib
!endif
__WXLIB_CORE_p =
!ifeq MONOLITHIC 0
__WXLIB_CORE_p = &
	wx$(PORTNAME)$(WXUNIVNAME)$(WX_RELEASE_NODOT)$(WXUNICODEFLAG)$(WXDEBUGFLAG)$(WX_LIB_FLAVOUR)_core.lib
!endif
__WXLIB_BASE_p =
!ifeq MONOLITHIC 0
__WXLIB_BASE_p = &
	wxbase$(WX_RELEASE_NODOT)$(WXUNICODEFLAG)$(WXDEBUGFLAG)$(WX_LIB_FLAVOUR).lib
!endif
__fllib___depname =
!ifeq SHARED 0
__fllib___depname = &
	$(LIBDIRNAME)\wx$(PORTNAME)$(WXUNIVNAME)$(WX_RELEASE_NODOT)$(WXUNICODEFLAG)$(WXDEBUGFLAG)$(WX_LIB_FLAVOUR)_fl.lib
!endif
__DEBUGINFO_1 =
!ifeq BUILD debug
!ifeq DEBUG_INFO default
__DEBUGINFO_1 = -d2
!endif
!endif
!ifeq BUILD release
!ifeq DEBUG_INFO default
__DEBUGINFO_1 = -d0
!endif
!endif
!ifeq DEBUG_INFO 0
__DEBUGINFO_1 = -d0
!endif
!ifeq DEBUG_INFO 1
__DEBUGINFO_1 = -d2
!endif
__OPTIMIZEFLAG =
!ifeq BUILD debug
__OPTIMIZEFLAG = -od
!endif
!ifeq BUILD release
__OPTIMIZEFLAG = -ot -ox
!endif
__THREADSFLAG =
!ifeq USE_THREADS 0
__THREADSFLAG = 
!endif
!ifeq USE_THREADS 1
__THREADSFLAG = -bm
!endif
__RUNTIME_LIBS =
!ifeq RUNTIME_LIBS dynamic
__RUNTIME_LIBS = -br
!endif
!ifeq RUNTIME_LIBS static
__RUNTIME_LIBS = 
!endif
__RTTIFLAG =
!ifeq USE_RTTI 0
__RTTIFLAG = 
!endif
!ifeq USE_RTTI 1
__RTTIFLAG = -xr
!endif
__EXCEPTIONSFLAG =
!ifeq USE_EXCEPTIONS 0
__EXCEPTIONSFLAG = 
!endif
!ifeq USE_EXCEPTIONS 1
__EXCEPTIONSFLAG = -xs
!endif
__WXUNIV_DEFINE_p =
!ifeq WXUNIV 1
__WXUNIV_DEFINE_p = -d__WXUNIVERSAL__
!endif
__DEBUG_DEFINE_p =
!ifeq BUILD debug
!ifeq DEBUG_FLAG default
__DEBUG_DEFINE_p = -d__WXDEBUG__
!endif
!endif
!ifeq DEBUG_FLAG 1
__DEBUG_DEFINE_p = -d__WXDEBUG__
!endif
__EXCEPTIONS_DEFINE_p =
!ifeq USE_EXCEPTIONS 0
__EXCEPTIONS_DEFINE_p = -dwxNO_EXCEPTIONS
!endif
__RTTI_DEFINE_p =
!ifeq USE_RTTI 0
__RTTI_DEFINE_p = -dwxNO_RTTI
!endif
__THREAD_DEFINE_p =
!ifeq USE_THREADS 0
__THREAD_DEFINE_p = -dwxNO_THREADS
!endif
__UNICODE_DEFINE_p =
!ifeq UNICODE 1
__UNICODE_DEFINE_p = -d_UNICODE
!endif
__GFXCTX_DEFINE_p =
!ifeq USE_GDIPLUS 1
__GFXCTX_DEFINE_p = -dwxUSE_GRAPHICS_CONTEXT=1
!endif

### Variables: ###

WX_RELEASE_NODOT = 28
OBJS = &
	wat_$(PORTNAME)$(WXUNIVNAME)$(WXUNICODEFLAG)$(WXDEBUGFLAG)$(WXDLLFLAG)$(CFG)
LIBDIRNAME = ..\..\src\fl\..\..\..\lib\wat_$(LIBTYPE_SUFFIX)$(CFG)
SETUPHDIR = &
	$(LIBDIRNAME)\$(PORTNAME)$(WXUNIVNAME)$(WXUNICODEFLAG)$(WXDEBUGFLAG)
FLDLL_CXXFLAGS = -bd $(__DEBUGINFO_1) $(__OPTIMIZEFLAG) $(__THREADSFLAG) &
	$(__RUNTIME_LIBS) -d__WXMSW__ $(__WXUNIV_DEFINE_p) $(__DEBUG_DEFINE_p) &
	$(__EXCEPTIONS_DEFINE_p) $(__RTTI_DEFINE_p) $(__THREAD_DEFINE_p) &
	$(__UNICODE_DEFINE_p) $(__GFXCTX_DEFINE_p) -i=$(SETUPHDIR) &
	-i=..\..\src\fl\..\..\..\include -wx -wcd=549 -wcd=656 -wcd=657 -wcd=667 &
	-i=..\..\src\fl\..\..\include -dWXUSINGDLL -dWXMAKINGDLL_FL &
	/fh=$(OBJS)\wxprec_fldll.pch $(__RTTIFLAG) $(__EXCEPTIONSFLAG) $(CPPFLAGS) &
	$(CXXFLAGS)
FLDLL_OBJECTS =  &
	$(OBJS)\fldll_dummy.obj &
	$(OBJS)\fldll_antiflickpl.obj &
	$(OBJS)\fldll_bardragpl.obj &
	$(OBJS)\fldll_barhintspl.obj &
	$(OBJS)\fldll_cbcustom.obj &
	$(OBJS)\fldll_controlbar.obj &
	$(OBJS)\fldll_dyntbar.obj &
	$(OBJS)\fldll_dyntbarhnd.obj &
	$(OBJS)\fldll_frmview.obj &
	$(OBJS)\fldll_garbagec.obj &
	$(OBJS)\fldll_gcupdatesmgr.obj &
	$(OBJS)\fldll_hintanimpl.obj &
	$(OBJS)\fldll_newbmpbtn.obj &
	$(OBJS)\fldll_panedrawpl.obj &
	$(OBJS)\fldll_rowdragpl.obj &
	$(OBJS)\fldll_rowlayoutpl.obj &
	$(OBJS)\fldll_toolwnd.obj &
	$(OBJS)\fldll_updatesmgr.obj
FLLIB_CXXFLAGS = $(__DEBUGINFO_1) $(__OPTIMIZEFLAG) $(__THREADSFLAG) &
	$(__RUNTIME_LIBS) -d__WXMSW__ $(__WXUNIV_DEFINE_p) $(__DEBUG_DEFINE_p) &
	$(__EXCEPTIONS_DEFINE_p) $(__RTTI_DEFINE_p) $(__THREAD_DEFINE_p) &
	$(__UNICODE_DEFINE_p) $(__GFXCTX_DEFINE_p) -i=$(SETUPHDIR) &
	-i=..\..\src\fl\..\..\..\include -wx -wcd=549 -wcd=656 -wcd=657 -wcd=667 &
	-i=..\..\src\fl\..\..\include /fh=$(OBJS)\wxprec_fllib.pch $(__RTTIFLAG) &
	$(__EXCEPTIONSFLAG) $(CPPFLAGS) $(CXXFLAGS)
FLLIB_OBJECTS =  &
	$(OBJS)\fllib_dummy.obj &
	$(OBJS)\fllib_antiflickpl.obj &
	$(OBJS)\fllib_bardragpl.obj &
	$(OBJS)\fllib_barhintspl.obj &
	$(OBJS)\fllib_cbcustom.obj &
	$(OBJS)\fllib_controlbar.obj &
	$(OBJS)\fllib_dyntbar.obj &
	$(OBJS)\fllib_dyntbarhnd.obj &
	$(OBJS)\fllib_frmview.obj &
	$(OBJS)\fllib_garbagec.obj &
	$(OBJS)\fllib_gcupdatesmgr.obj &
	$(OBJS)\fllib_hintanimpl.obj &
	$(OBJS)\fllib_newbmpbtn.obj &
	$(OBJS)\fllib_panedrawpl.obj &
	$(OBJS)\fllib_rowdragpl.obj &
	$(OBJS)\fllib_rowlayoutpl.obj &
	$(OBJS)\fllib_toolwnd.obj &
	$(OBJS)\fllib_updatesmgr.obj


all : $(OBJS)
$(OBJS) :
	-if not exist $(OBJS) mkdir $(OBJS)

### Targets: ###

all : .SYMBOLIC $(__fldll___depname) $(__fllib___depname)

clean : .SYMBOLIC 
	-if exist $(OBJS)\*.obj del $(OBJS)\*.obj
	-if exist $(OBJS)\*.res del $(OBJS)\*.res
	-if exist $(OBJS)\*.lbc del $(OBJS)\*.lbc
	-if exist $(OBJS)\*.ilk del $(OBJS)\*.ilk
	-if exist $(OBJS)\*.pch del $(OBJS)\*.pch
	-if exist $(LIBDIRNAME)\wx$(PORTNAME)$(WXUNIVNAME)$(WX_RELEASE_NODOT)$(WXUNICODEFLAG)$(WXDEBUGFLAG)$(WX_LIB_FLAVOUR)_fl_wat$(VENDORTAG).dll del $(LIBDIRNAME)\wx$(PORTNAME)$(WXUNIVNAME)$(WX_RELEASE_NODOT)$(WXUNICODEFLAG)$(WXDEBUGFLAG)$(WX_LIB_FLAVOUR)_fl_wat$(VENDORTAG).dll
	-if exist $(LIBDIRNAME)\wx$(PORTNAME)$(WXUNIVNAME)$(WX_RELEASE_NODOT)$(WXUNICODEFLAG)$(WXDEBUGFLAG)$(WX_LIB_FLAVOUR)_fl.lib del $(LIBDIRNAME)\wx$(PORTNAME)$(WXUNIVNAME)$(WX_RELEASE_NODOT)$(WXUNICODEFLAG)$(WXDEBUGFLAG)$(WX_LIB_FLAVOUR)_fl.lib
	-if exist $(LIBDIRNAME)\wx$(PORTNAME)$(WXUNIVNAME)$(WX_RELEASE_NODOT)$(WXUNICODEFLAG)$(WXDEBUGFLAG)$(WX_LIB_FLAVOUR)_fl.lib del $(LIBDIRNAME)\wx$(PORTNAME)$(WXUNIVNAME)$(WX_RELEASE_NODOT)$(WXUNICODEFLAG)$(WXDEBUGFLAG)$(WX_LIB_FLAVOUR)_fl.lib

!ifeq SHARED 1
$(LIBDIRNAME)\wx$(PORTNAME)$(WXUNIVNAME)$(WX_RELEASE_NODOT)$(WXUNICODEFLAG)$(WXDEBUGFLAG)$(WX_LIB_FLAVOUR)_fl_wat$(VENDORTAG).dll :  $(FLDLL_OBJECTS) $(OBJS)\fldll_version.res
	@%create $(OBJS)\fldll.lbc
	@%append $(OBJS)\fldll.lbc option quiet
	@%append $(OBJS)\fldll.lbc name $^@
	@%append $(OBJS)\fldll.lbc option caseexact
	@%append $(OBJS)\fldll.lbc $(LDFLAGS) $(__DEBUGINFO_2)  libpath $(LIBDIRNAME)
	@for %i in ($(FLDLL_OBJECTS)) do @%append $(OBJS)\fldll.lbc file %i
	@for %i in ( $(__WXLIB_MONO_p) $(__LIB_TIFF_p) $(__LIB_JPEG_p) $(__LIB_PNG_p)  wxzlib$(WXDEBUGFLAG).lib  wxregex$(WXUNICODEFLAG)$(WXDEBUGFLAG).lib wxexpat$(WXDEBUGFLAG).lib $(EXTRALIBS_FOR_BASE)  $(__GDIPLUS_LIB_p) kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib $(__WXLIB_CORE_p)  $(__WXLIB_BASE_p) ) do @%append $(OBJS)\fldll.lbc library %i
	@%append $(OBJS)\fldll.lbc option resource=$(OBJS)\fldll_version.res
	@%append $(OBJS)\fldll.lbc system nt_dll
	wlink @$(OBJS)\fldll.lbc
	wlib -q -n -b $(LIBDIRNAME)\wx$(PORTNAME)$(WXUNIVNAME)$(WX_RELEASE_NODOT)$(WXUNICODEFLAG)$(WXDEBUGFLAG)$(WX_LIB_FLAVOUR)_fl.lib +$^@
!endif

!ifeq SHARED 0
$(LIBDIRNAME)\wx$(PORTNAME)$(WXUNIVNAME)$(WX_RELEASE_NODOT)$(WXUNICODEFLAG)$(WXDEBUGFLAG)$(WX_LIB_FLAVOUR)_fl.lib :  $(FLLIB_OBJECTS)
	@%create $(OBJS)\fllib.lbc
	@for %i in ($(FLLIB_OBJECTS)) do @%append $(OBJS)\fllib.lbc +%i
	wlib -q -p4096 -n -b $^@ @$(OBJS)\fllib.lbc
!endif

$(OBJS)\fldll_dummy.obj :  .AUTODEPEND ../../src/fl\..\..\..\src\common\dummy.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FLDLL_CXXFLAGS) $<

$(OBJS)\fldll_version.res :  .AUTODEPEND ../../src/fl\..\..\..\src\msw\version.rc
	wrc -q -ad -bt=nt -r -fo=$^@   -d__WXMSW__ $(__WXUNIV_DEFINE_p) $(__DEBUG_DEFINE_p) $(__EXCEPTIONS_DEFINE_p) $(__RTTI_DEFINE_p) $(__THREAD_DEFINE_p) $(__UNICODE_DEFINE_p)  $(__GFXCTX_DEFINE_p) -i=$(SETUPHDIR) -i=..\..\src\fl\..\..\..\include -dWXDLLNAME=wx$(PORTNAME)$(WXUNIVNAME)$(WX_RELEASE_NODOT)$(WXUNICODEFLAG)$(WXDEBUGFLAG)$(WX_LIB_FLAVOUR)_fl_wat$(VENDORTAG) $<

$(OBJS)\fldll_antiflickpl.obj :  .AUTODEPEND ../../src/fl\antiflickpl.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FLDLL_CXXFLAGS) $<

$(OBJS)\fldll_bardragpl.obj :  .AUTODEPEND ../../src/fl\bardragpl.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FLDLL_CXXFLAGS) $<

$(OBJS)\fldll_barhintspl.obj :  .AUTODEPEND ../../src/fl\barhintspl.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FLDLL_CXXFLAGS) $<

$(OBJS)\fldll_cbcustom.obj :  .AUTODEPEND ../../src/fl\cbcustom.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FLDLL_CXXFLAGS) $<

$(OBJS)\fldll_controlbar.obj :  .AUTODEPEND ../../src/fl\controlbar.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FLDLL_CXXFLAGS) $<

$(OBJS)\fldll_dyntbar.obj :  .AUTODEPEND ../../src/fl\dyntbar.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FLDLL_CXXFLAGS) $<

$(OBJS)\fldll_dyntbarhnd.obj :  .AUTODEPEND ../../src/fl\dyntbarhnd.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FLDLL_CXXFLAGS) $<

$(OBJS)\fldll_frmview.obj :  .AUTODEPEND ../../src/fl\frmview.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FLDLL_CXXFLAGS) $<

$(OBJS)\fldll_garbagec.obj :  .AUTODEPEND ../../src/fl\garbagec.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FLDLL_CXXFLAGS) $<

$(OBJS)\fldll_gcupdatesmgr.obj :  .AUTODEPEND ../../src/fl\gcupdatesmgr.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FLDLL_CXXFLAGS) $<

$(OBJS)\fldll_hintanimpl.obj :  .AUTODEPEND ../../src/fl\hintanimpl.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FLDLL_CXXFLAGS) $<

$(OBJS)\fldll_newbmpbtn.obj :  .AUTODEPEND ../../src/fl\newbmpbtn.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FLDLL_CXXFLAGS) $<

$(OBJS)\fldll_panedrawpl.obj :  .AUTODEPEND ../../src/fl\panedrawpl.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FLDLL_CXXFLAGS) $<

$(OBJS)\fldll_rowdragpl.obj :  .AUTODEPEND ../../src/fl\rowdragpl.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FLDLL_CXXFLAGS) $<

$(OBJS)\fldll_rowlayoutpl.obj :  .AUTODEPEND ../../src/fl\rowlayoutpl.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FLDLL_CXXFLAGS) $<

$(OBJS)\fldll_toolwnd.obj :  .AUTODEPEND ../../src/fl\toolwnd.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FLDLL_CXXFLAGS) $<

$(OBJS)\fldll_updatesmgr.obj :  .AUTODEPEND ../../src/fl\updatesmgr.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FLDLL_CXXFLAGS) $<

$(OBJS)\fllib_dummy.obj :  .AUTODEPEND ../../src/fl\..\..\..\src\common\dummy.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FLLIB_CXXFLAGS) $<

$(OBJS)\fllib_antiflickpl.obj :  .AUTODEPEND ../../src/fl\antiflickpl.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FLLIB_CXXFLAGS) $<

$(OBJS)\fllib_bardragpl.obj :  .AUTODEPEND ../../src/fl\bardragpl.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FLLIB_CXXFLAGS) $<

$(OBJS)\fllib_barhintspl.obj :  .AUTODEPEND ../../src/fl\barhintspl.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FLLIB_CXXFLAGS) $<

$(OBJS)\fllib_cbcustom.obj :  .AUTODEPEND ../../src/fl\cbcustom.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FLLIB_CXXFLAGS) $<

$(OBJS)\fllib_controlbar.obj :  .AUTODEPEND ../../src/fl\controlbar.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FLLIB_CXXFLAGS) $<

$(OBJS)\fllib_dyntbar.obj :  .AUTODEPEND ../../src/fl\dyntbar.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FLLIB_CXXFLAGS) $<

$(OBJS)\fllib_dyntbarhnd.obj :  .AUTODEPEND ../../src/fl\dyntbarhnd.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FLLIB_CXXFLAGS) $<

$(OBJS)\fllib_frmview.obj :  .AUTODEPEND ../../src/fl\frmview.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FLLIB_CXXFLAGS) $<

$(OBJS)\fllib_garbagec.obj :  .AUTODEPEND ../../src/fl\garbagec.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FLLIB_CXXFLAGS) $<

$(OBJS)\fllib_gcupdatesmgr.obj :  .AUTODEPEND ../../src/fl\gcupdatesmgr.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FLLIB_CXXFLAGS) $<

$(OBJS)\fllib_hintanimpl.obj :  .AUTODEPEND ../../src/fl\hintanimpl.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FLLIB_CXXFLAGS) $<

$(OBJS)\fllib_newbmpbtn.obj :  .AUTODEPEND ../../src/fl\newbmpbtn.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FLLIB_CXXFLAGS) $<

$(OBJS)\fllib_panedrawpl.obj :  .AUTODEPEND ../../src/fl\panedrawpl.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FLLIB_CXXFLAGS) $<

$(OBJS)\fllib_rowdragpl.obj :  .AUTODEPEND ../../src/fl\rowdragpl.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FLLIB_CXXFLAGS) $<

$(OBJS)\fllib_rowlayoutpl.obj :  .AUTODEPEND ../../src/fl\rowlayoutpl.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FLLIB_CXXFLAGS) $<

$(OBJS)\fllib_toolwnd.obj :  .AUTODEPEND ../../src/fl\toolwnd.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FLLIB_CXXFLAGS) $<

$(OBJS)\fllib_updatesmgr.obj :  .AUTODEPEND ../../src/fl\updatesmgr.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FLLIB_CXXFLAGS) $<

