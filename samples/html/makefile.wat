# =========================================================================
#     This makefile was generated by
#     Bakefile 0.1.3 (http://bakefile.sourceforge.net)
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

MAKEARGS = CC="$(CC)" CXX="$(CXX)" CFLAGS="$(CFLAGS)" CXXFLAGS="$(CXXFLAGS)" &
	CPPFLAGS="$(CPPFLAGS)" LDFLAGS="$(LDFLAGS)" SHARED="$(SHARED)" &
	WXUNIV="$(WXUNIV)" UNICODE="$(UNICODE)" BUILD="$(BUILD)" &
	DEBUG_INFO="$(DEBUG_INFO)" DEBUG_FLAG="$(DEBUG_FLAG)" &
	MONOLITHIC="$(MONOLITHIC)" USE_GUI="$(USE_GUI)" USE_HTML="$(USE_HTML)" &
	USE_OPENGL="$(USE_OPENGL)" USE_ODBC="$(USE_ODBC)" &
	USE_EXCEPTIONS="$(USE_EXCEPTIONS)" OFFICIAL_BUILD="$(OFFICIAL_BUILD)" &
	VENDOR="$(VENDOR)" CFG="$(CFG)" RUNTIME_LIBS="$(RUNTIME_LIBS)"


### Targets: ###

all : .SYMBOLIC about help helpview printing test virtual widget zip

clean : .SYMBOLIC 
	-if exist .\*.obj del .\*.obj
	-if exist .\*.res del .\*.res
	-if exist .\*.lbc del .\*.lbc
	-if exist .\*.ilk del .\*.ilk

about : .SYMBOLIC 
	cd about
	wmake $(__MAKEOPTS__) -f makefile.wat $(MAKEARGS) all
	cd $(WATCOM_CWD)

help : .SYMBOLIC 
	cd help
	wmake $(__MAKEOPTS__) -f makefile.wat $(MAKEARGS) all
	cd $(WATCOM_CWD)

helpview : .SYMBOLIC 
	cd helpview
	wmake $(__MAKEOPTS__) -f makefile.wat $(MAKEARGS) all
	cd $(WATCOM_CWD)

printing : .SYMBOLIC 
	cd printing
	wmake $(__MAKEOPTS__) -f makefile.wat $(MAKEARGS) all
	cd $(WATCOM_CWD)

test : .SYMBOLIC 
	cd test
	wmake $(__MAKEOPTS__) -f makefile.wat $(MAKEARGS) all
	cd $(WATCOM_CWD)

virtual : .SYMBOLIC 
	cd virtual
	wmake $(__MAKEOPTS__) -f makefile.wat $(MAKEARGS) all
	cd $(WATCOM_CWD)

widget : .SYMBOLIC 
	cd widget
	wmake $(__MAKEOPTS__) -f makefile.wat $(MAKEARGS) all
	cd $(WATCOM_CWD)

zip : .SYMBOLIC 
	cd zip
	wmake $(__MAKEOPTS__) -f makefile.wat $(MAKEARGS) all
	cd $(WATCOM_CWD)
