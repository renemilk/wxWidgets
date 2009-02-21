///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/evtloopcmn.cpp
// Purpose:     common wxEventLoop-related stuff
// Author:      Vadim Zeitlin
// Modified by:
// Created:     2006-01-12
// RCS-ID:      $Id$
// Copyright:   (c) 2006 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/evtloop.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif //WX_PRECOMP

// ----------------------------------------------------------------------------
// wxEventLoopBase
// ----------------------------------------------------------------------------

wxEventLoopBase *wxEventLoopBase::ms_activeLoop = NULL;

wxEventLoopBase::wxEventLoopBase()
{
    m_isInsideYield = false;
    m_eventsToProcessInsideYield = wxEVT_CATEGORY_ALL;
}

void wxEventLoopBase::DelayPendingEventHandler(wxEvtHandler* toDelay)
{
    wxENTER_CRIT_SECT(m_handlersWithPendingEventsLocker);

    // move the handler from the list of handlers with processable pending events
    // to the list of handlers with pending events which needs to be processed later
    m_handlersWithPendingEvents.Remove(toDelay);

    if (m_handlersWithPendingDelayedEvents.Index(toDelay) == wxNOT_FOUND)
        m_handlersWithPendingDelayedEvents.Add(toDelay);

    wxLEAVE_CRIT_SECT(m_handlersWithPendingEventsLocker);
}

void wxEventLoopBase::RemovePendingEventHandler(wxEvtHandler* toRemove)
{
    wxENTER_CRIT_SECT(m_handlersWithPendingEventsLocker);

    if (m_handlersWithPendingEvents.Index(toRemove) != wxNOT_FOUND)
    {
        m_handlersWithPendingEvents.Remove(toRemove);

        // check that the handler was present only once in the list
        wxASSERT_MSG( m_handlersWithPendingEvents.Index(toRemove) == wxNOT_FOUND,
                        "Handler occurs twice in the m_handlersWithPendingEvents list!" );
    }
    //else: it wasn't in this list at all, it's ok

    if (m_handlersWithPendingDelayedEvents.Index(toRemove) != wxNOT_FOUND)
    {
        m_handlersWithPendingDelayedEvents.Remove(toRemove);

        // check that the handler was present only once in the list
        wxASSERT_MSG( m_handlersWithPendingDelayedEvents.Index(toRemove) == wxNOT_FOUND,
                        "Handler occurs twice in m_handlersWithPendingDelayedEvents list!" );
    }
    //else: it wasn't in this list at all, it's ok

    wxLEAVE_CRIT_SECT(m_handlersWithPendingEventsLocker);
}

void wxEventLoopBase::AppendPendingEventHandler(wxEvtHandler* toAppend)
{
    wxENTER_CRIT_SECT(m_handlersWithPendingEventsLocker);

    if ( m_handlersWithPendingEvents.Index(toAppend) == wxNOT_FOUND )
        m_handlersWithPendingEvents.Add(toAppend);

    wxLEAVE_CRIT_SECT(m_handlersWithPendingEventsLocker);
}

bool wxEventLoopBase::HasPendingEvents() const
{
    wxENTER_CRIT_SECT(const_cast<wxEventLoopBase*>(this)->m_handlersWithPendingEventsLocker);

    bool has = !m_handlersWithPendingEvents.IsEmpty();

    wxLEAVE_CRIT_SECT(const_cast<wxEventLoopBase*>(this)->m_handlersWithPendingEventsLocker);

    return has;
}

void wxEventLoopBase::SuspendProcessingOfPendingEvents()
{
    wxENTER_CRIT_SECT(m_handlersWithPendingEventsLocker);
}

void wxEventLoopBase::ResumeProcessingOfPendingEvents()
{
    wxLEAVE_CRIT_SECT(m_handlersWithPendingEventsLocker);
}

void wxEventLoopBase::ProcessPendingEvents()
{
    wxENTER_CRIT_SECT(m_handlersWithPendingEventsLocker);

    wxCHECK_RET( m_handlersWithPendingDelayedEvents.IsEmpty(),
                 "this helper list should be empty" );

    // iterate until the list becomes empty: the handlers remove themselves
    // from it when they don't have any more pending events
    while (!m_handlersWithPendingEvents.IsEmpty())
    {
        // In ProcessPendingEvents(), new handlers might be added
        // and we can safely leave the critical section here.
        wxLEAVE_CRIT_SECT(m_handlersWithPendingEventsLocker);

        // NOTE: we always call ProcessPendingEvents() on the first event handler
        //       with pending events because handlers auto-remove themselves
        //       from this list (see RemovePendingEventHandler) if they have no
        //       more pending events.
        m_handlersWithPendingEvents[0]->ProcessPendingEvents();

        wxENTER_CRIT_SECT(m_handlersWithPendingEventsLocker);
    }

    // now the wxHandlersWithPendingEvents is surely empty; however some event
    // handlers may have moved themselves into wxHandlersWithPendingDelayedEvents
    // because of a selective wxYield call in progress.
    // Now we need to move them back to wxHandlersWithPendingEvents so the next
    // call to this function has the chance of processing them:
    if (!m_handlersWithPendingDelayedEvents.IsEmpty())
    {
        WX_APPEND_ARRAY(m_handlersWithPendingEvents, m_handlersWithPendingDelayedEvents);
        m_handlersWithPendingDelayedEvents.Clear();
    }

    wxLEAVE_CRIT_SECT(m_handlersWithPendingEventsLocker);
}

void wxEventLoopBase::WakeUpIdle()
{
    WakeUp();
}

bool wxEventLoopBase::ProcessIdle()
{
    // process pending wx events before sending idle events
    ProcessPendingEvents();

    wxIdleEvent event;

    event.SetEventObject(wxTheApp);
    wxTheApp->ProcessEvent(event);
    return event.MoreRequested();
}

bool wxEventLoopBase::Yield(bool onlyIfNeeded)
{
    if ( m_isInsideYield )
    {
        if ( !onlyIfNeeded )
        {
            wxFAIL_MSG( wxT("wxYield called recursively" ) );
        }

        return false;
    }

    return YieldFor(wxEVT_CATEGORY_ALL);
}

// wxEventLoopManual is unused in the other ports
#if defined(__WXMSW__) || defined(__WXMAC__) || defined(__WXDFB__) || (defined(__UNIX__) && wxUSE_BASE)

// ============================================================================
// wxEventLoopManual implementation
// ============================================================================

wxEventLoopManual::wxEventLoopManual()
{
    m_exitcode = 0;
    m_shouldExit = false;
}

int wxEventLoopManual::Run()
{
    // event loops are not recursive, you need to create another loop!
    wxCHECK_MSG( !IsRunning(), -1, _T("can't reenter a message loop") );

    // ProcessIdle() and Dispatch() below may throw so the code here should
    // be exception-safe, hence we must use local objects for all actions we
    // should undo
    wxEventLoopActivator activate(this);

    // we must ensure that OnExit() is called even if an exception is thrown
    // from inside Dispatch() but we must call it from Exit() in normal
    // situations because it is supposed to be called synchronously,
    // wxModalEventLoop depends on this (so we can't just use ON_BLOCK_EXIT or
    // something similar here)
#if wxUSE_EXCEPTIONS
    for ( ;; )
    {
        try
        {
#endif // wxUSE_EXCEPTIONS

            // this is the event loop itself
            for ( ;; )
            {
                // give them the possibility to do whatever they want
                OnNextIteration();

                // generate and process idle events for as long as we don't
                // have anything else to do
                while ( !Pending() && (wxTheApp && wxTheApp->ProcessIdle()) )
                    ;

                // if the "should exit" flag is set, the loop should terminate
                // but not before processing any remaining messages so while
                // Pending() returns true, do process them
                if ( m_shouldExit )
                {
                    while ( Pending() )
                        Dispatch();

                    break;
                }

                // a message came or no more idle processing to do, sit in
                // Dispatch() waiting for the next message
                if ( !Dispatch() )
                {
                    // we got WM_QUIT
                    break;
                }
            }

#if wxUSE_EXCEPTIONS
            // exit the outer loop as well
            break;
        }
        catch ( ... )
        {
            try
            {
                if ( !wxTheApp || !wxTheApp->OnExceptionInMainLoop() )
                {
                    OnExit();
                    break;
                }
                //else: continue running the event loop
            }
            catch ( ... )
            {
                // OnException() throwed, possibly rethrowing the same
                // exception again: very good, but we still need OnExit() to
                // be called
                OnExit();
                throw;
            }
        }
    }
#endif // wxUSE_EXCEPTIONS

    return m_exitcode;
}

void wxEventLoopManual::Exit(int rc)
{
    wxCHECK_RET( IsRunning(), _T("can't call Exit() if not running") );

    m_exitcode = rc;
    m_shouldExit = true;

    OnExit();

    // all we have to do to exit from the loop is to (maybe) wake it up so that
    // it can notice that Exit() had been called
    //
    // in particular, do *not* use here calls such as PostQuitMessage() (under
    // MSW) which terminate the current event loop here because we're not sure
    // that it is going to be processed by the correct event loop: it would be
    // possible that another one is started and terminated by mistake if we do
    // this
    WakeUp();
}

#endif // __WXMSW__ || __WXMAC__ || __WXDFB__

