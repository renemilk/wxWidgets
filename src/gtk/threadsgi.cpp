/////////////////////////////////////////////////////////////////////////////
// Name:        threadsgi.cpp
// Purpose:     wxThread (SGI) Implementation
// Author:      Original from Wolfram Gloger/Guilhem Lavaux
// Modified by:
// Created:     04/22/98
// RCS-ID:      $Id$
// Copyright:   (c) Wolfram Gloger (1996, 1997); Guilhem Lavaux (1998)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#ifdef __GNUG__
#pragma implementation "thread.h"
#endif

#include <stdio.h>
#include <unistd.h>

#include <signal.h>
#include <sys/wait.h>
#include <sys/prctl.h>

enum thread_state {
  STATE_IDLE = 0,
  STATE_RUNNING,
  STATE_CANCELED,
  STATE_EXITED
};

/////////////////////////////////////////////////////////////////////////////
// Static variables
/////////////////////////////////////////////////////////////////////////////

#include "wx/thread.h"

static int p_mainid;
wxMutex wxMainMutex;

#include "threadgui.inc"

/////////////////////////////////////////////////////////////////////////////
// Unix implementations (SGI threads)
/////////////////////////////////////////////////////////////////////////////

class wxMutexInternal {
public:
  abilock_t p_mutex;
};

wxMutex::wxMutex()
{
  m_locked = 0;
  p_internal = new wxMutexInternal;
  init_lock(&(p_internal->p_mutex));
}

wxMutex::~wxMutex()
{
  if (m_locked > 0)
    wxDebugMsg("wxMutex warning: freeing a locked mutex (%d locks)\n",
               m_locked);
  delete p_internal;
}

wxMutexError wxMutex::Lock()
{
  spin_lock(&(p_internal->p_mutex));
  m_locked++;
  return MUTEX_NO_ERROR;
}

wxMutexError wxMutex::TryLock()
{
  if (acquire_lock(&(p_internal->p_mutex)) != 0)
    return MUTEX_BUSY;
  m_locked++;
  return MUTEX_NO_ERROR;
}

wxMutexError wxMutex::Unlock()
{
  if (m_locked == 0)
    return MUTEX_UNLOCKED; 
  release_lock(&(p_internal->p_mutex));
  m_locked--;
  return MUTEX_NO_ERROR;
}

// GL: Don't know how it works on SGI. Wolfram ?

wxCondition::wxCondition() {}
wxCondition::~wxCondition() {}
int wxCondition::Wait(wxMutex& WXUNUSED(mutex)) { return 0;}
int wxCondition::Wait(wxMutex& WXUNUSED(mutex), unsigned long WXUNUSED(sec),
                      unsigned long WXUNUSED(nsec)) { return 0; }
int wxCondition::Signal() { return 0; }
int wxCondition::Broadcast() { return 0; }

class
wxThreadPrivate {
public:
  wxThreadPrivate() { thread_id = 0; state = STATE_IDLE; }
  ~wxThreadPrivate() {}
  static void SprocStart(void *ptr);
  static void SignalHandler(int sig);
public:
  int state, thread_id;
  void* exit_status;
};

void wxThreadPrivate::SprocStart(void *ptr)
{
  void* status;

  wxThread *thr = (wxThread *)ptr;

  thr->p_internal->thread_id = getpid();
  thr->p_internal->exit_status = 0;
  status = thr->Entry();
  thr->Exit(status);
}

void wxThread::Exit(void* status)
{
  wxThread* ptr = this;
  THREAD_SEND_EXIT_MSG(ptr);
  p_internal->state = STATE_EXITED;
  p_internal->exit_status = status;
  _exit(0);
}

wxThreadError wxThread::Create()
{
  if (p_internal->state != STATE_IDLE)
    return THREAD_RUNNING;
  p_internal->state = STATE_RUNNING;
  if (sproc(p_internal->SprocStart, PR_SALL, this) < 0) {
    p_internal->state = STATE_IDLE;
    return THREAD_NO_RESOURCE;
  }
  return THREAD_NO_ERROR;
}

void wxThread::Destroy()
{
  if (p_internal->state == STATE_RUNNING)
    p_internal->state = STATE_CANCELED;
}

void *wxThread::Join()
{
  if (p_internal->state != STATE_IDLE) {
    bool do_unlock = wxThread::IsMain();
    int stat;

    if (do_unlock)
      wxMainMutex.Unlock();
    waitpid(p_internal->thread_id, &stat, 0);
    if (do_unlock)
      wxMainMutex.Lock();
    if (!WIFEXITED(stat) && !WIFSIGNALED(stat))
      return 0;
    p_internal->state = STATE_IDLE;
    return p_internal->exit_status;
  }
  return 0;
}

unsigned long wxThread::GetID() const
{
  return (unsigned long)p_internal->thread_id;
}

void wxThread::TestDestroy()
{
  if (p_internal->state == STATE_CANCELED) {
    p_internal->exit_status = 0;
    _exit(0);
  }
}

void wxThread::SetPriority(int prio)
{
}

int wxThread::GetPriority() const
{
    return 0;
}

bool wxThreadIsMain()
{
  return (int)getpid() == main_id;
}

wxThread::wxThread()
{
  p_internal = new wxThreadPrivate();
}

wxThread::~wxThread()
{
  Cancel();
  Join();
  delete p_internal;
}

// The default callback just joins the thread and throws away the result.
void wxThread::OnExit()
{
  Join();
}

// Global initialization
class wxThreadModule : public wxModule {
  DECLARE_DYNAMIC_CLASS(wxThreadModule)
public:
  virtual bool OnInit() {
    wxThreadGuiInit();
    p_mainid = (int)getpid();
    wxMainMutex.Lock();
  }

  virtual void OnExit() {
    wxMainMutex.Unlock();
    wxThreadGuiExit();
  }
};

IMPLEMENT_DYNAMIC_CLASS(wxThreadModule, wxModule)
