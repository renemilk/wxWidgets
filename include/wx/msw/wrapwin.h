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

#ifndef STRICT
    #define STRICT 1
#endif

#include <windows.h>
#include "wx/msw/winundef.h"

// types DWORD_PTR, ULONG_PTR and so on might be not defined in old headers but
// unfortunately I don't know of any standard way to test for this (as they're
// typedefs and not #defines), so simply overwrite them in any case in Win32
// mode -- and if compiling for Win64 they'd better have new headers anyhow
//
// this is ugly but what else can we do? even testing for compiler version
// wouldn't help as you can perfectly well be using an older compiler (VC6)
// with newer SDK headers
#if !defined(__WIN64__) && !defined(__WXWINCE__)
    #define UINT_PTR unsigned int
    #define ULONG_PTR unsigned long
    #define DWORD_PTR unsigned long
#endif // !__WIN64__

#endif // _WX_WRAPWIN_H_

