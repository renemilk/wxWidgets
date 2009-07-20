///////////////////////////////////////////////////////////////////////////////
// Name:        os2/sockpm.cpp
// Purpose:     implementation of OS-2-specific handler event handling
// Author:      Guilhem Lavaux, Vadim Zeitlin
// Created:     1999
// RCS-ID:      $Id$
// Copyright:   (c) 1999-2008 wxWidgets dev team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_SOCKETS

#include <stdlib.h>
#include "wx/private/socket.h"
#include "wx/app.h"
#include "wx/apptrait.h"

#define wxSockReadMask  0x01
#define wxSockWriteMask 0x02

static void wxSocket_PM_Input(void *data)
{
    wxSocketImplUnix *handler = static_cast<wxSocketImplUnix *>(data);

    handler->OnReadWaiting();
}

static void wxSocket_PM_Output(void *data)
{
    wxSocketImplUnix *handler = static_cast<wxSocketImplUnix *>(data);

    handler->OnWriteWaiting();
}

class PMSocketManager : public wxSocketInputBasedManager
{
public:
    virtual int AddInput(wxSocketImplUnix *handler, int fd, SocketDir d)
    {
      if (d == FD_OUTPUT)
          return wxTheApp->AddSocketHandler(fd, wxSockWriteMask,
                                            wxSocket_PM_Output, handler);
      else
          return wxTheApp->AddSocketHandler(fd, wxSockReadMask,
                                            wxSocket_PM_Input, handler);
    }

    virtual void RemoveInput(int fd)
    {
        wxTheApp->RemoveSocketHandler(fd);
    }
};

wxSocketManager *wxGUIAppTraits::GetSocketManager()
{
    static PMSocketManager s_manager;
    return &s_manager;
}


#else /* !wxUSE_SOCKETS */

/* some compilers don't like having empty source files */
static int wxDummyGsockVar = 0;

#endif /* wxUSE_SOCKETS/!wxUSE_SOCKETS */
