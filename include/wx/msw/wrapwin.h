/////////////////////////////////////////////////////////////////////////////
// Name:        msw/wrapwin.h
// Purpose:     Wrapper around <windows.h>, to be included instead of it
// Author:      Vaclav Slavik
// Created:     2003/07/22
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WRAPWIN_H_
#define _WX_WRAPWIN_H_

#include "wx/platform.h"

// strict type checking to detect conversion from HFOO to HBAR at compile-time
#ifndef STRICT
    #define STRICT 1
#endif

// this macro tells windows.h to not define min() and max() as macros: we need
// this as otherwise they conflict with standard C++ functions
#ifndef NOMINMAX
    #define NOMINMAX
#endif // NOMINMAX


// before including windows.h, define version macros at (currently) maximal
// values because we do all our checks at run-time anyhow
#ifndef WINVER
    // the only exception to the above is MSVC 6 which has a time bomb in its
    // headers: they warn against using them with WINVER >= 0x0500 as they
    // contain only part of the declarations and they're not always correct, so
    // don't define WINVER for it at all as this allows everything to work as
    // expected both with standard VC6 headers (which define WINVER as 0x0400
    // by default) and headers from a newer SDK (which may define it as 0x0500)
    #if !defined(__VISUALC__) || (__VISUALC__ >= 1300)
        #define WINVER 0x0600
    #endif
#endif

#ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0600
#endif


#include <windows.h>

#ifdef __WXWINCE__
    // this doesn't make any sense knowing that windows.h includes all these
    // headers anyhow, but the fact remains that when building using eVC 4 the
    // functions and constants from these headers are not defined unless we
    // explicitly include them ourselves -- how is it possible is beyond me...
    #include <winbase.h>
    #include <wingdi.h>
    #include <winuser.h>

    // this one OTOH contains many useful CE-only functions
    #include <shellapi.h>
#endif // __WXWINCE__


// #undef the macros defined in winsows.h which conflict with code elsewhere
#include "wx/msw/winundef.h"


// Types DWORD_PTR, ULONG_PTR and so on are used for 64-bit compatability 
// in the WINAPI SDK (they are an integral type that is the size of a
// pointer) on MSVC 7 and later. However, they are not available in older 
// Platform SDKs, and since they are typedefs and not #defines we simply 
// overwrite them if there is a chance that they're not defined
#if !defined(_MSC_VER) || (_MSC_VER < 1300)
    #define UINT_PTR unsigned int
    #define LONG_PTR long
    #define ULONG_PTR unsigned long
    #define DWORD_PTR unsigned long
#endif // !defined(_MSC_VER) || _MSC_VER < 1300

#endif // _WX_WRAPWIN_H_

