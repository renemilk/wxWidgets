# =========================================================================
#     This makefile was generated by
#     Bakefile 0.1.1 (http://bakefile.sourceforge.net)
#     Do not modify, all changes will be overwritten!
# =========================================================================

!include ../../build/msw/config.wat

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


### Variables: ###

MAKEARGS = CPPFLAGS="$(CPPFLAGS)" OFFICIAL_BUILD="$(OFFICIAL_BUILD)" &
	WXUNIV="$(WXUNIV)" DEBUG_FLAG="$(DEBUG_FLAG)" CFLAGS="$(CFLAGS)" &
	RUNTIME_LIBS="$(RUNTIME_LIBS)" CXX="$(CXX)" CFG="$(CFG)" &
	USE_GUI="$(USE_GUI)" MONOLITHIC="$(MONOLITHIC)" CXXFLAGS="$(CXXFLAGS)" &
	CC="$(CC)" USE_HTML="$(USE_HTML)" BUILD="$(BUILD)" LDFLAGS="$(LDFLAGS)" &
	DEBUG_INFO="$(DEBUG_INFO)" SHARED="$(SHARED)" VENDOR="$(VENDOR)" &
	USE_OPENGL="$(USE_OPENGL)" USE_ODBC="$(USE_ODBC)" UNICODE="$(UNICODE)"


### Targets: ###

all : .SYMBOLIC styles wxedit

clean : .SYMBOLIC 
	-if exist .\*.obj del .\*.obj
	-if exist .\*.res del .\*.res
	-if exist .\*.lbc del .\*.lbc
	-if exist .\*.ilk del .\*.ilk

styles : .SYMBOLIC 
	cd styles
	wmake $(__MAKEOPTS__) -f makefile.wat $(MAKEARGS) all
	cd $(WATCOM_CWD)

wxedit : .SYMBOLIC 
	cd wxedit
	wmake $(__MAKEOPTS__) -f makefile.wat $(MAKEARGS) all
	cd $(WATCOM_CWD)
