///////////////////////////////////////////////////////////////////////////////
// Name:        menuitem.cpp
// Purpose:     wxMenuItem implementation
// Author:      Vadim Zeitlin
// Modified by: 
// Created:     11.11.97
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// headers & declarations
// ============================================================================

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/menu.h"
#endif

#include "wx/ownerdrw.h"
#include "wx/menuitem.h"

#include <windows.h>

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// dynamic classes implementation
// ----------------------------------------------------------------------------

#if     !defined(USE_SHARED_LIBRARY) || !USE_SHARED_LIBRARY
#if wxUSE_OWNER_DRAWN
  IMPLEMENT_DYNAMIC_CLASS2(wxMenuItem, wxObject, wxOwnerDrawn)
#else   //!USE_OWNER_DRAWN
  IMPLEMENT_DYNAMIC_CLASS(wxMenuItem, wxObject)
#endif  //USE_OWNER_DRAWN

#endif  //USE_SHARED_LIBRARY

// ----------------------------------------------------------------------------
// wxMenuItem
// ----------------------------------------------------------------------------

// ctor & dtor
// -----------

wxMenuItem::wxMenuItem(wxMenu *pParentMenu, int id,
                       const wxString& strName, const wxString& strHelp,
                       bool bCheckable,
                       wxMenu *pSubMenu) :
#if wxUSE_OWNER_DRAWN
                        wxOwnerDrawn(strName, bCheckable),
#else   //no owner drawn support
                        m_bCheckable(bCheckable),
                        m_strName(strName),
#endif  //owner drawn
                        m_strHelp(strHelp)
{
  wxASSERT( pParentMenu != NULL );

#if  wxUSE_OWNER_DRAWN
    // set default menu colors
    #define SYS_COLOR(c) (wxSystemSettings::GetSystemColour(wxSYS_COLOUR_##c))
    
    SetTextColour(SYS_COLOR(MENUTEXT));
    SetBackgroundColour(SYS_COLOR(MENU));

    // we don't want normal items be owner-drawn
    ResetOwnerDrawn();

    #undef  SYS_COLOR
#endif

  m_pParentMenu = pParentMenu;
  m_pSubMenu    = pSubMenu;
  m_idItem      = id;
  m_bEnabled    = TRUE;
}

wxMenuItem::~wxMenuItem() 
{
}

// misc
// ----

// delete the sub menu
void wxMenuItem::DeleteSubMenu()
{
  wxASSERT( m_pSubMenu != NULL );

  delete m_pSubMenu;
  m_pSubMenu = NULL;
}

// change item state
// -----------------

void wxMenuItem::Enable(bool bDoEnable)
{
  if ( m_bEnabled != bDoEnable ) {
    if ( m_pSubMenu == NULL ) {     // normal menu item
      EnableMenuItem((HMENU)m_pParentMenu->GetHMenu(), m_idItem, 
                     MF_BYCOMMAND | (bDoEnable ? MF_ENABLED: MF_GRAYED));
    }
    else                            // submenu
    {
      wxMenu *father = m_pSubMenu->m_topLevelMenu ;
      wxNode *node = father->m_menuItems.First() ;
      int i = 0 ;
      while (node) {
        wxMenuItem *matched = (wxMenuItem*)node->Data();
        if ( matched == this)
          break;
        i++;
        node = node->Next();
      }
      EnableMenuItem((HMENU)father->m_savehMenu, i, 
                     MF_BYPOSITION | (bDoEnable ? MF_ENABLED: MF_GRAYED));
    }

    m_bEnabled = bDoEnable;
  }
}

void wxMenuItem::Check(bool bDoCheck)
{
  wxCHECK_RET( IsCheckable(), "only checkable items may be checked" );

  if ( m_bChecked != bDoCheck ) {
    CheckMenuItem((HMENU)m_pParentMenu->GetHMenu(), m_idItem, 
                  MF_BYCOMMAND | (bDoCheck ? MF_CHECKED : MF_UNCHECKED));

    m_bChecked = bDoCheck;
  }
}