# =========================================================================
#     This makefile was generated by
#     Bakefile 0.2.5 (http://www.bakefile.org)
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
__foldbardll___depname =
!ifeq SHARED 1
__foldbardll___depname = &
	$(LIBDIRNAME)\wx$(PORTNAME)$(WXUNIVNAME)$(WX_RELEASE_NODOT)$(WXUNICODEFLAG)$(WXDEBUGFLAG)$(WX_LIB_FLAVOUR)_foldbar_wat$(VENDORTAG).dll
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
__CAIRO_LIB_p =
!ifeq USE_CAIRO 1
__CAIRO_LIB_p = cairo.lib
!endif
____CAIRO_LIBDIR_FILENAMES_p =
!ifeq USE_CAIRO 1
____CAIRO_LIBDIR_FILENAMES_p = libpath $(CAIRO_ROOT)\lib
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
__foldbarlib___depname =
!ifeq SHARED 0
__foldbarlib___depname = &
	$(LIBDIRNAME)\wx$(PORTNAME)$(WXUNIVNAME)$(WX_RELEASE_NODOT)$(WXUNICODEFLAG)$(WXDEBUGFLAG)$(WX_LIB_FLAVOUR)_foldbar.lib
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
____CAIRO_INCLUDEDIR_FILENAMES =
!ifeq USE_CAIRO 1
____CAIRO_INCLUDEDIR_FILENAMES = -i=$(CAIRO_ROOT)\include\cairo
!endif

### Variables: ###

WX_RELEASE_NODOT = 28
OBJS = &
	wat_$(PORTNAME)$(WXUNIVNAME)$(WXUNICODEFLAG)$(WXDEBUGFLAG)$(WXDLLFLAG)$(CFG)
LIBDIRNAME = ..\..\src\foldbar\..\..\..\lib\wat_$(LIBTYPE_SUFFIX)$(CFG)
SETUPHDIR = &
	$(LIBDIRNAME)\$(PORTNAME)$(WXUNIVNAME)$(WXUNICODEFLAG)$(WXDEBUGFLAG)
FOLDBARDLL_CXXFLAGS = -bd $(__DEBUGINFO_1) $(__OPTIMIZEFLAG) $(__THREADSFLAG) &
	$(__RUNTIME_LIBS) -d__WXMSW__ $(__WXUNIV_DEFINE_p) $(__DEBUG_DEFINE_p) &
	$(__EXCEPTIONS_DEFINE_p) $(__RTTI_DEFINE_p) $(__THREAD_DEFINE_p) &
	$(__UNICODE_DEFINE_p) $(__GFXCTX_DEFINE_p) -i=$(SETUPHDIR) &
	-i=..\..\src\foldbar\..\..\..\include $(____CAIRO_INCLUDEDIR_FILENAMES) &
	-wx -wcd=549 -wcd=656 -wcd=657 -wcd=667 -i=..\..\src\foldbar\..\..\include &
	-dWXUSINGDLL -dWXMAKINGDLL_FOLDBAR /fh=$(OBJS)\wxprec_foldbardll.pch &
	$(__RTTIFLAG) $(__EXCEPTIONSFLAG) $(CPPFLAGS) $(CXXFLAGS)
FOLDBARDLL_OBJECTS =  &
	$(OBJS)\foldbardll_dummy.obj &
	$(OBJS)\foldbardll_captionbar.obj &
	$(OBJS)\foldbardll_foldpanelbar.obj &
	$(OBJS)\foldbardll_foldpanelitem.obj
FOLDBARLIB_CXXFLAGS = $(__DEBUGINFO_1) $(__OPTIMIZEFLAG) $(__THREADSFLAG) &
	$(__RUNTIME_LIBS) -d__WXMSW__ $(__WXUNIV_DEFINE_p) $(__DEBUG_DEFINE_p) &
	$(__EXCEPTIONS_DEFINE_p) $(__RTTI_DEFINE_p) $(__THREAD_DEFINE_p) &
	$(__UNICODE_DEFINE_p) $(__GFXCTX_DEFINE_p) -i=$(SETUPHDIR) &
	-i=..\..\src\foldbar\..\..\..\include $(____CAIRO_INCLUDEDIR_FILENAMES) &
	-wx -wcd=549 -wcd=656 -wcd=657 -wcd=667 -i=..\..\src\foldbar\..\..\include &
	/fh=$(OBJS)\wxprec_foldbarlib.pch $(__RTTIFLAG) $(__EXCEPTIONSFLAG) &
	$(CPPFLAGS) $(CXXFLAGS)
FOLDBARLIB_OBJECTS =  &
	$(OBJS)\foldbarlib_dummy.obj &
	$(OBJS)\foldbarlib_captionbar.obj &
	$(OBJS)\foldbarlib_foldpanelbar.obj &
	$(OBJS)\foldbarlib_foldpanelitem.obj


all : $(OBJS)
$(OBJS) :
	-if not exist $(OBJS) mkdir $(OBJS)

### Targets: ###

all : .SYMBOLIC $(__foldbardll___depname) $(__foldbarlib___depname)

clean : .SYMBOLIC 
	-if exist $(OBJS)\*.obj del $(OBJS)\*.obj
	-if exist $(OBJS)\*.res del $(OBJS)\*.res
	-if exist $(OBJS)\*.lbc del $(OBJS)\*.lbc
	-if exist $(OBJS)\*.ilk del $(OBJS)\*.ilk
	-if exist $(OBJS)\*.pch del $(OBJS)\*.pch
	-if exist $(LIBDIRNAME)\wx$(PORTNAME)$(WXUNIVNAME)$(WX_RELEASE_NODOT)$(WXUNICODEFLAG)$(WXDEBUGFLAG)$(WX_LIB_FLAVOUR)_foldbar_wat$(VENDORTAG).dll del $(LIBDIRNAME)\wx$(PORTNAME)$(WXUNIVNAME)$(WX_RELEASE_NODOT)$(WXUNICODEFLAG)$(WXDEBUGFLAG)$(WX_LIB_FLAVOUR)_foldbar_wat$(VENDORTAG).dll
	-if exist $(LIBDIRNAME)\wx$(PORTNAME)$(WXUNIVNAME)$(WX_RELEASE_NODOT)$(WXUNICODEFLAG)$(WXDEBUGFLAG)$(WX_LIB_FLAVOUR)_foldbar.lib del $(LIBDIRNAME)\wx$(PORTNAME)$(WXUNIVNAME)$(WX_RELEASE_NODOT)$(WXUNICODEFLAG)$(WXDEBUGFLAG)$(WX_LIB_FLAVOUR)_foldbar.lib
	-if exist $(LIBDIRNAME)\wx$(PORTNAME)$(WXUNIVNAME)$(WX_RELEASE_NODOT)$(WXUNICODEFLAG)$(WXDEBUGFLAG)$(WX_LIB_FLAVOUR)_foldbar.lib del $(LIBDIRNAME)\wx$(PORTNAME)$(WXUNIVNAME)$(WX_RELEASE_NODOT)$(WXUNICODEFLAG)$(WXDEBUGFLAG)$(WX_LIB_FLAVOUR)_foldbar.lib

!ifeq SHARED 1
$(LIBDIRNAME)\wx$(PORTNAME)$(WXUNIVNAME)$(WX_RELEASE_NODOT)$(WXUNICODEFLAG)$(WXDEBUGFLAG)$(WX_LIB_FLAVOUR)_foldbar_wat$(VENDORTAG).dll :  $(FOLDBARDLL_OBJECTS) $(OBJS)\foldbardll_version.res
	@%create $(OBJS)\foldbardll.lbc
	@%append $(OBJS)\foldbardll.lbc option quiet
	@%append $(OBJS)\foldbardll.lbc name $^@
	@%append $(OBJS)\foldbardll.lbc option caseexact
	@%append $(OBJS)\foldbardll.lbc  $(__DEBUGINFO_2)  libpath $(LIBDIRNAME) $(____CAIRO_LIBDIR_FILENAMES_p) $(LDFLAGS)
	@for %i in ($(FOLDBARDLL_OBJECTS)) do @%append $(OBJS)\foldbardll.lbc file %i
	@for %i in ( $(__WXLIB_MONO_p) $(__LIB_TIFF_p) $(__LIB_JPEG_p) $(__LIB_PNG_p)  wxzlib$(WXDEBUGFLAG).lib  wxregex$(WXUNICODEFLAG)$(WXDEBUGFLAG).lib wxexpat$(WXDEBUGFLAG).lib $(EXTRALIBS_FOR_BASE)  $(__GDIPLUS_LIB_p) $(__CAIRO_LIB_p) kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib $(__WXLIB_CORE_p)  $(__WXLIB_BASE_p) ) do @%append $(OBJS)\foldbardll.lbc library %i
	@%append $(OBJS)\foldbardll.lbc option resource=$(OBJS)\foldbardll_version.res
	@%append $(OBJS)\foldbardll.lbc system nt_dll
	wlink @$(OBJS)\foldbardll.lbc
	wlib -q -n -b $(LIBDIRNAME)\wx$(PORTNAME)$(WXUNIVNAME)$(WX_RELEASE_NODOT)$(WXUNICODEFLAG)$(WXDEBUGFLAG)$(WX_LIB_FLAVOUR)_foldbar.lib +$^@
!endif

!ifeq SHARED 0
$(LIBDIRNAME)\wx$(PORTNAME)$(WXUNIVNAME)$(WX_RELEASE_NODOT)$(WXUNICODEFLAG)$(WXDEBUGFLAG)$(WX_LIB_FLAVOUR)_foldbar.lib :  $(FOLDBARLIB_OBJECTS)
	@%create $(OBJS)\foldbarlib.lbc
	@for %i in ($(FOLDBARLIB_OBJECTS)) do @%append $(OBJS)\foldbarlib.lbc +%i
	wlib -q -p4096 -n -b $^@ @$(OBJS)\foldbarlib.lbc
!endif

$(OBJS)\foldbardll_dummy.obj :  .AUTODEPEND ..\..\src\foldbar\..\..\..\src\common\dummy.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FOLDBARDLL_CXXFLAGS) $<

$(OBJS)\foldbardll_version.res :  .AUTODEPEND ..\..\src\foldbar\..\..\..\src\msw\version.rc
	wrc -q -ad -bt=nt -r -fo=$^@    -d__WXMSW__ $(__WXUNIV_DEFINE_p) $(__DEBUG_DEFINE_p) $(__EXCEPTIONS_DEFINE_p) $(__RTTI_DEFINE_p) $(__THREAD_DEFINE_p) $(__UNICODE_DEFINE_p)  $(__GFXCTX_DEFINE_p) -i=$(SETUPHDIR) -i=..\..\src\foldbar\..\..\..\include $(____CAIRO_INCLUDEDIR_FILENAMES) -dWXDLLNAME=wx$(PORTNAME)$(WXUNIVNAME)$(WX_RELEASE_NODOT)$(WXUNICODEFLAG)$(WXDEBUGFLAG)$(WX_LIB_FLAVOUR)_foldbar_wat$(VENDORTAG) -i=..\..\src\foldbar\..\..\include -dWXUSINGDLL -dWXMAKINGDLL_FOLDBAR $<

$(OBJS)\foldbardll_captionbar.obj :  .AUTODEPEND ..\..\src\foldbar\captionbar.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FOLDBARDLL_CXXFLAGS) $<

$(OBJS)\foldbardll_foldpanelbar.obj :  .AUTODEPEND ..\..\src\foldbar\foldpanelbar.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FOLDBARDLL_CXXFLAGS) $<

$(OBJS)\foldbardll_foldpanelitem.obj :  .AUTODEPEND ..\..\src\foldbar\foldpanelitem.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FOLDBARDLL_CXXFLAGS) $<

$(OBJS)\foldbarlib_dummy.obj :  .AUTODEPEND ..\..\src\foldbar\..\..\..\src\common\dummy.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FOLDBARLIB_CXXFLAGS) $<

$(OBJS)\foldbarlib_captionbar.obj :  .AUTODEPEND ..\..\src\foldbar\captionbar.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FOLDBARLIB_CXXFLAGS) $<

$(OBJS)\foldbarlib_foldpanelbar.obj :  .AUTODEPEND ..\..\src\foldbar\foldpanelbar.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FOLDBARLIB_CXXFLAGS) $<

$(OBJS)\foldbarlib_foldpanelitem.obj :  .AUTODEPEND ..\..\src\foldbar\foldpanelitem.cpp
	$(CXX) -bt=nt -zq -fo=$^@ $(FOLDBARLIB_CXXFLAGS) $<

