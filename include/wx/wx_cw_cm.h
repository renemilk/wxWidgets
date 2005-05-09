/*
 Name:        wx/wx_cw_cm.h
 Purpose:     wxWidgets definitions for CodeWarrior builds
 Author:      Stefan Csomor
 Modified by:
 Created:
 RCS-ID:      $Id$
 Copyright:   (c) Stefan Csomor
 Licence:     wxWindows licence
*/

#define MSL_USE_PRECOMPILED_HEADERS 0
#if __WXDEBUG__
    // mac os assert levels
    #define DEBUG 1
    #define DEBUG_INTERNAL 1
#endif
#if !defined( __MWERKS__ )
    #error "this file is only for builds with Metrowerks CodeWarrior"
#endif

#define WX_COMP_INLINE_NO_CLASS // defined if the compiler does not want the classname repeated for inlines within a class definition

#if __MWERKS__ >= 0x2400 && __MWERKS__ <= 0x3200
#pragma old_argmatch on
#endif

#if (__MWERKS__ < 0x0900) || macintosh || __MACH__
    #define __MAC__
    #define __WXMAC__
#elif (__MWERKS__ >= 0x0900) && __INTEL__
    #define WIN32
    #define _WINDOWS
    #define __WXMSW__
    #define __WINDOWS__
    #define __WIN95__
    #define __WIN32__
    #define STRICT
    #define NOMINMAX
#elif __BEOS__
    #include <ansi_prefix.be.h>
    #include <Be.h>
#else
    #error unknown MW compiler
#endif

#ifdef __WXMSW__
    #include <ansi_prefix.win32.h>
    #include <ansi_parms.h>
    #ifdef __MWERKS__
    #if defined( __MSL__ ) && __MSL__ >= 0x5012 && __MSL__ < 0x7000
            #define fileno _fileno
            #define fdopen _fdopen
            #define tell   _tell
    #endif
    #endif
#elif defined( __WXMAC__)
    #define USE_PRECOMPILED_MAC_HEADERS  0  /*Set to 0 if you don't want to use precompiled MacHeaders*/
    #define ACCESSOR_CALLS_ARE_FUNCTIONS 1
    #define OPAQUE_TOOLBOX_STRUCTS 1
    #ifdef __MACH__
		typedef int mbstate_t;
		#define	_MBSTATE_T
    #endif
	#include <mslconfig>
    // for getting the correct expat includes
    #define MACOS_CLASSIC
    /*
    #include <MacTypes.h>
    #if UNIVERSAL_INTERFACES_VERSION < 0x0340
        #error "please update to Apple's lastest universal headers from http://developer.apple.com/sdk/"
    #endif
    */
#endif

#define USE_DEFINE

