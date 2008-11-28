///////////////////////////////////////////////////////////////////////////////
// Name:        motif/sockmot.cpp
// Purpose:     implementation of wxMotif-specific socket event handling
// Author:      Guilhem Lavaux, Vadim Zeitlin
// Created:     1999
// RCS-ID:      $Id$
// Copyright:   (c) 1999, 2007 wxWidgets dev team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_SOCKETS

#include <X11/Intrinsic.h>      // XtAppAdd/RemoveInput()
#include "wx/motif/private.h"   // wxGetAppContext()
#include "wx/private/socket.h"
#include "wx/apptrait.h"

extern "C" {

static void wxSocket_Motif_Input(XtPointer data, int *WXUNUSED(fid),
                                 XtInputId *WXUNUSED(id))
{
    wxFDIOHandler * const handler = static_cast<wxFDIOHandler *>(data);

    handler->OnReadWaiting();
}

static void wxSocket_Motif_Output(XtPointer data, int *WXUNUSED(fid),
                                  XtInputId *WXUNUSED(id))
{
    wxFDIOHandler * const handler = static_cast<wxFDIOHandler *>(data);

    handler->OnWriteWaiting();
}

}

class MotifSocketManager : public wxSocketInputBasedManager
{
public:
    virtual int AddInput(wxFDIOHandler *handler, int fd, SocketDir d)
    {
        return XtAppAddInput
               (
                    wxGetAppContext(),
                    fd,
                    (XtPointer)(d == FD_OUTPUT ? XtInputWriteMask
                                               : XtInputReadMask),
                    d == FD_OUTPUT ? wxSocket_Motif_Output
                                   : wxSocket_Motif_Input,
                    handler
               );
    }

    virtual void RemoveInput(int fd)
    {
        XtRemoveInput(fd);
    }
};

wxSocketManager *wxGUIAppTraits::GetSocketManager()
{
    static MotifSocketManager s_manager;
    return &s_manager;
}

#endif // wxUSE_SOCKETS
