/* -------------------------------------------------------------------------
 * Project:     GSocket (Generic Socket)
 * Name:        src/msw/gsockmsw.cpp
 * Copyright:   (c) Guilhem Lavaux
 * Licence:     wxWindows Licence
 * Author:      Guillermo Rodriguez Garcia <guille@iies.es>
 * Purpose:     GSocket GUI-specific MSW code
 * CVSID:       $Id$
 * -------------------------------------------------------------------------
 */

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#warning Fix multithread problems with wxCHECK_RET

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

/*
 * DONE: for WinCE we need to replace WSAAsyncSelect
 * (Windows message-based notification of network events for a socket)
 * with another mechanism.
 * As WSAAsyncSelect is not present on WinCE, it now uses
 * WSACreateEvent, WSAEventSelect, WSAWaitForMultipleEvents and WSAEnumNetworkEvents.
 * When enabling eventhandling for a socket a new thread it created that keeps track of the events
 * and posts a messageto the hidden window to use the standard message loop.
 */

/* including rasasync.h (included from windows.h itself included from
 * wx/setup.h and/or winsock.h results in this warning for
 * RPCNOTIFICATION_ROUTINE
 */
#ifdef _MSC_VER
#   pragma warning(disable:4115) /* named type definition in parentheses */
#endif

/* This needs to be before the wx/defs/h inclusion
 * for some reason
 */

#ifdef __WXWINCE__
    /* windows.h results in tons of warnings at max warning level */
#   ifdef _MSC_VER
#       pragma warning(push, 1)
#   endif
#   include <windows.h>
#   ifdef _MSC_VER
#       pragma warning(pop)
#       pragma warning(disable:4514)
#   endif
#endif

#ifndef __GSOCKET_STANDALONE__
#   include "wx/platform.h"
#endif

#if wxUSE_SOCKETS || defined(__GSOCKET_STANDALONE__)

#ifndef __GSOCKET_STANDALONE__

#include "wx/gsocket.h"

extern "C" WXDLLIMPEXP_BASE HINSTANCE wxGetInstance(void);
#define INSTANCE wxGetInstance()

#else /* __GSOCKET_STANDALONE__ */

#include "gsocket.h"

/* If not using wxWidgets, a global var called hInst must
 * be available and it must contain the app's instance
 * handle.
 */
extern HINSTANCE hInst;
#define INSTANCE hInst

#endif /* !__GSOCKET_STANDALONE__/__GSOCKET_STANDALONE__ */

#ifndef __WXWINCE__
#include <assert.h>
#else
#define assert(x)
#include <winsock.h>
#include "wx/msw/wince/net.h"
#include "wx/hashmap.h"
WX_DECLARE_HASH_MAP(int,bool,wxIntegerHash,wxIntegerEqual,SocketHash);
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>

#include <winsock.h>

#ifdef _MSC_VER
#  pragma warning(default:4115) /* named type definition in parentheses */
#endif

#define CLASSNAME  TEXT("_GSocket_Internal_Window_Class")

/* implemented in utils.cpp */
extern "C" WXDLLIMPEXP_BASE HWND
wxCreateHiddenWindow(LPCTSTR *pclassname, LPCTSTR classname, WNDPROC wndproc);

/* Maximum number of different GSocket objects at a given time.
 * This value can be modified at will, but it CANNOT be greater
 * than (0x7FFF - WM_USER + 1)
 */
#define MAXSOCKETS 1024

#if (MAXSOCKETS > (0x7FFF - WM_USER + 1))
#error "MAXSOCKETS is too big!"
#endif

#define DATATYPE long
#define PLATFORM_DATA(x) (*(DATATYPE*)x)

#ifndef __WXWINCE__
typedef int (PASCAL *WSAAsyncSelectFunc)(SOCKET,HWND,u_int,long);
#else
/* Typedef the needed function prototypes and the WSANETWORKEVENTS structure
*/
typedef struct _WSANETWORKEVENTS {
       long lNetworkEvents;
       int iErrorCode[10];
} WSANETWORKEVENTS, FAR * LPWSANETWORKEVENTS;
typedef HANDLE (PASCAL *WSACreateEventFunc)(void);
typedef int (PASCAL *WSAEventSelectFunc)(SOCKET,HANDLE,long);
typedef int (PASCAL *WSAWaitForMultipleEventsFunc)(long,HANDLE,BOOL,long,BOOL);
typedef int (PASCAL *WSAEnumNetworkEventsFunc)(SOCKET,HANDLE,LPWSANETWORKEVENTS);
#endif //__WXWINCE__

LRESULT CALLBACK _GSocket_Internal_WinProc(HWND, UINT, WPARAM, LPARAM);

/* Global variables */

static HWND hWin;
static CRITICAL_SECTION critical;
static GSocket* socketList[MAXSOCKETS];
static int firstAvailable;

#ifndef __WXWINCE__
static WSAAsyncSelectFunc gs_WSAAsyncSelect = NULL;
#else
static SocketHash socketHash;
static unsigned int currSocket;
HANDLE hThread[MAXSOCKETS];
static WSACreateEventFunc gs_WSACreateEvent = NULL;
static WSAEventSelectFunc gs_WSAEventSelect = NULL;
static WSAWaitForMultipleEventsFunc gs_WSAWaitForMultipleEvents = NULL;
static WSAEnumNetworkEventsFunc gs_WSAEnumNetworkEvents = NULL;
/* This structure will be used to pass data on to the thread that handles socket events.
*/
typedef struct thread_data{
    HWND hEvtWin;
    unsigned long msgnumber;
    unsigned long fd;
    unsigned long lEvent;
}thread_data;
#endif

static HMODULE gs_wsock32dll = 0;

long TranslateEventCondition(GSocket* socket, GSocketEvent event) {
  switch (event) {
    case GSOCK_INPUT: return FD_READ;
    case GSOCK_OUTPUT: return FD_WRITE;
    case GSOCK_CONNECTION: return (socket->m_server ? FD_ACCEPT : FD_CONNECT);
    case GSOCK_LOST: return FD_CLOSE;
    default: assert(0); return 0;
  }  
}

#ifdef __WXWINCE__
/* This thread handles socket events on WinCE using WSAEventSelect() as WSAAsyncSelect is not supported.
*  When an event occures for the socket, it is checked what kind of event happend and the correct message gets posted
*  so that the hidden window can handle it as it would in other MSW builds.
*/
DWORD WINAPI SocketThread(LPVOID data)
{
    WSANETWORKEVENTS NetworkEvents;
    thread_data* d = (thread_data *)data;

    HANDLE NetworkEvent = gs_WSACreateEvent();
    gs_WSAEventSelect(d->fd, NetworkEvent, d->lEvent);

    while(socketHash[d->fd] == true)
    {
        if ((gs_WSAWaitForMultipleEvents(1, &NetworkEvent, FALSE,INFINITE, FALSE)) == WAIT_FAILED)
        {
            printf("WSAWaitForMultipleEvents failed with error %d\n", WSAGetLastError());
            return 0;
        }
        if (gs_WSAEnumNetworkEvents(d->fd ,NetworkEvent, &NetworkEvents) == SOCKET_ERROR)
        {
            printf("WSAEnumNetworkEvents failed with error %d\n", WSAGetLastError());
            return 0;
        }

        long flags = NetworkEvents.lNetworkEvents;
        if (flags & FD_READ)
            ::PostMessage(d->hEvtWin, d->msgnumber,d->fd, FD_READ);
        if (flags & FD_WRITE)
            ::PostMessage(d->hEvtWin, d->msgnumber,d->fd, FD_WRITE);
        if (flags & FD_OOB)
            ::PostMessage(d->hEvtWin, d->msgnumber,d->fd, FD_OOB);
        if (flags & FD_ACCEPT)
            ::PostMessage(d->hEvtWin, d->msgnumber,d->fd, FD_ACCEPT);
        if (flags & FD_CONNECT)
            ::PostMessage(d->hEvtWin, d->msgnumber,d->fd, FD_CONNECT);
        if (flags & FD_CLOSE)
            ::PostMessage(d->hEvtWin, d->msgnumber,d->fd, FD_CLOSE);

    }
    gs_WSAEventSelect(d->fd, NetworkEvent, 0);
    ExitThread(0);
    return 0;
}
#endif


bool GSocketGUIFunctionsTableConcrete::CanUseEventLoop()
{
    return true;
}

/* Global initializers */

bool GSocketGUIFunctionsTableConcrete::OnInit()
{
  static LPCTSTR pclassname = NULL;
  int i;

  /* Create internal window for event notifications */
  hWin = wxCreateHiddenWindow(&pclassname, CLASSNAME, _GSocket_Internal_WinProc);
  if (!hWin)
      return false;

  /* Initialize socket list */
  InitializeCriticalSection(&critical);

  for (i = 0; i < MAXSOCKETS; i++)
  {
    socketList[i] = NULL;
  }
  firstAvailable = 0;

  /* Load WSAAsyncSelect from wsock32.dll (we don't link against it
     statically to avoid dependency on wsock32.dll for apps that don't use
     sockets): */
#ifndef __WXWINCE__
  gs_wsock32dll = LoadLibrary(wxT("wsock32.dll"));
  if (!gs_wsock32dll)
      return false;
  gs_WSAAsyncSelect =(WSAAsyncSelectFunc)GetProcAddress(gs_wsock32dll,
                                                        "WSAAsyncSelect");
  if (!gs_WSAAsyncSelect)
      return false;
#else
/*  On WinCE we load ws2.dll which will provide the needed functions.
*/
  gs_wsock32dll = LoadLibrary(wxT("ws2.dll"));
  if (!gs_wsock32dll)
      return false;
  gs_WSAEventSelect =(WSAEventSelectFunc)GetProcAddress(gs_wsock32dll,
                                                        wxT("WSAEventSelect"));
  if (!gs_WSAEventSelect)
      return false;

  gs_WSACreateEvent =(WSACreateEventFunc)GetProcAddress(gs_wsock32dll,
                                                        wxT("WSACreateEvent"));
  if (!gs_WSACreateEvent)
      return false;

  gs_WSAWaitForMultipleEvents =(WSAWaitForMultipleEventsFunc)GetProcAddress(gs_wsock32dll,
                                                                            wxT("WSAWaitForMultipleEvents"));
  if (!gs_WSAWaitForMultipleEvents)
      return false;

  gs_WSAEnumNetworkEvents =(WSAEnumNetworkEventsFunc)GetProcAddress(gs_wsock32dll,
                                                                    wxT("WSAEnumNetworkEvents"));
  if (!gs_WSAEnumNetworkEvents)
      return false;

  currSocket = 0;
#endif

  return true;
}

void GSocketGUIFunctionsTableConcrete::OnExit()
{
#ifdef __WXWINCE__
/* Delete the threads here */
    for(unsigned int i=0; i < currSocket; i++)
        CloseHandle(hThread[i]);
#endif
  /* Destroy internal window */
  DestroyWindow(hWin);
  UnregisterClass(CLASSNAME, INSTANCE);

  /* Unlock wsock32.dll */
  if (gs_wsock32dll)
  {
      FreeLibrary(gs_wsock32dll);
      gs_wsock32dll = 0;
  }

  /* Delete critical section */
  DeleteCriticalSection(&critical);
}

/* Per-socket GUI initialization / cleanup */

bool GSocketGUIFunctionsTableConcrete::Init_Socket(GSocket *socket)
{
  int i;

  wxCHECK_MSG( !socket->m_platform_specific_data, false, wxT("Critical: Double inited socket.") );
  
  /* Allocate a new message number for this socket */
  EnterCriticalSection(&critical);

  i = firstAvailable;
  while (socketList[i] != NULL)
  {
    i = (i + 1) % MAXSOCKETS;

    if (i == firstAvailable)    /* abort! */
    {
      LeaveCriticalSection(&critical);
      return false;
    }
  }
  socketList[i] = socket;
  firstAvailable = (i + 1) % MAXSOCKETS;
  
  socket->m_platform_specific_data = malloc(sizeof(DATATYPE));

  PLATFORM_DATA(socket->m_platform_specific_data) = (i + WM_USER);

  LeaveCriticalSection(&critical);

  return true;
}

void GSocketGUIFunctionsTableConcrete::Destroy_Socket(GSocket *socket)
{
  /* Remove the socket from the list */
  wxCHECK_RET( socket->m_platform_specific_data, wxT("Critical: Destroying non-inited socket or double destroy.") );  
  
  EnterCriticalSection(&critical);
  if ( socket->IsOk() )
      socketList[(PLATFORM_DATA(socket->m_platform_specific_data) - WM_USER)] = NULL;
  LeaveCriticalSection(&critical);
}

/* Windows proc for asynchronous event handling */

LRESULT CALLBACK _GSocket_Internal_WinProc(HWND hWnd,
                                           UINT uMsg,
                                           WPARAM wParam,
                                           LPARAM lParam)
{
  GSocket *socket;

  if (uMsg >= WM_USER && uMsg <= (WM_USER + MAXSOCKETS - 1))
  {
    EnterCriticalSection(&critical);
    socket = socketList[(uMsg - WM_USER)];

    /* Check that the socket still exists (it has not been
     * destroyed) and for safety, check that the m_fd field
     * is what we expect it to be.
     */
    if ((socket != NULL) && (socket->m_fd == wParam))
    {
      switch WSAGETSELECTEVENT(lParam)
      {
        case FD_READ:    socket->Detected_Read(); break;
        case FD_WRITE:   socket->Detected_Write(); break;
        case FD_ACCEPT:  socket->Detected_Connect(); break;
        case FD_CONNECT:
        {
          if (WSAGETSELECTERROR(lParam) != 0)
            socket->Detected_Lost();
          else
            socket->Detected_Connect();
          break;
        }
        case FD_CLOSE:   socket->Detected_Lost(); break;
      }
    }

    /* OK, we can now leave the critical section because we have
     * already obtained the callback address (we make no further
     * accesses to socket->whatever). However, the app should
     * be prepared to handle events from a socket that has just
     * been closed!
     */
    LeaveCriticalSection(&critical);

    return (LRESULT) 0;
  }
  else
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// Helper function for {En|Dis}able*
void SetNewCallback(GSocket* socket)
{
  wxCHECK_RET( socket->m_platform_specific_data, wxT("Critical: Setting callback for non-init or destroyed socket") );
  gs_WSAAsyncSelect(socket->m_fd, hWin, PLATFORM_DATA(socket->m_platform_specific_data), socket->m_eventflags); 
}

/* _GSocket_Enable_Events:
 *  Enable all event notifications; we need to be notified of all
 *  events for internal processing, but we will only notify users
 *  when an appropiate callback function has been installed.
 */
void GSocketGUIFunctionsTableConcrete::Enable_Events(GSocket *socket)
{
  assert (socket != NULL);

  if (socket->m_fd != INVALID_SOCKET)
  {
    /* We could probably just subscribe to all events regardless
     * of the socket type, but MS recommends to do it this way.
     */
    if (socket->m_server)
    {
      socket->m_eventflags = (TranslateEventCondition(socket,GSOCK_CONNECTION));
    }
    else
    {
      socket->m_eventflags = (TranslateEventCondition(socket,GSOCK_CONNECTION))
                                            | (TranslateEventCondition(socket,GSOCK_INPUT))
                                            | (TranslateEventCondition(socket,GSOCK_OUTPUT))
                                            | (TranslateEventCondition(socket,GSOCK_LOST));
    }
    
#ifndef __WXWINCE__
    SetNewCallback(socket);   
#else
/*
*  WinCE creates a thread for socket event handling.
*  All needed parameters get passed through the thread_data structure.
*/

    thread_data* d = new thread_data;
    d->lEvent = socket->m_eventflags;
    d->hEvtWin = hWin;
    d->msgnumber = PLATFORM_DATA(socket->m_platform_specific_data);
    d->fd = socket->m_fd;
    socketHash[socket->m_fd] = true;
    hThread[currSocket++] = CreateThread(NULL, 0, &SocketThread,(LPVOID)d, 0, NULL);
#endif
  }
}

/* _GSocket_Disable_Events:
 *  Disable event notifications (when shutdowning the socket)
 */
void GSocketGUIFunctionsTableConcrete::Disable_Events(GSocket *socket)
{
  assert (socket != NULL);

  if (socket->m_fd != INVALID_SOCKET)
  {
    socket->m_eventflags = 0;
#ifndef __WXWINCE__
    SetNewCallback(socket);
#else
    //Destroy the thread
    socketHash[socket->m_fd] = false;
#endif
  }
}

/*
 * Event-Specific disabling.
 */

void GSocketGUIFunctionsTableConcrete::Enable_Event(GSocket *socket, GSocketEvent event)
{
#ifndef __WXWINCE__
  socket->m_eventflags |= TranslateEventCondition(socket,event);
  SetNewCallback(socket);
#else
  #error WinCE not supported yet
#endif  
}

void GSocketGUIFunctionsTableConcrete::Disable_Event(GSocket *socket, GSocketEvent event)
{
#ifndef __WXWINCE__
  socket->m_eventflags &= ~(TranslateEventCondition(socket,event));
  SetNewCallback(socket);
#else
  #error WinCE not supported yet
#endif  
}

#else /* !wxUSE_SOCKETS */

/*
 * Translation unit shouldn't be empty, so include this typedef to make the
 * compiler (VC++ 6.0, for example) happy
 */
typedef void (*wxDummy)();

#endif  /* wxUSE_SOCKETS || defined(__GSOCKET_STANDALONE__) */
