/////////////////////////////////////////////////////////////////////////////
// Name:        menu.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "menu.h"
#endif

#include "wx/menu.h"
#include "wx/log.h"

//-----------------------------------------------------------------------------
// wxMenuBar
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMenuBar,wxWindow)

wxMenuBar::wxMenuBar()
{
  m_needParent = FALSE; // hmmm

  PreCreation( NULL, -1, wxDefaultPosition, wxDefaultSize, 0, "menu" );

  m_menus.DeleteContents( TRUE );

  m_widget = gtk_handle_box_new();

  m_menubar = gtk_menu_bar_new();

  gtk_container_add( GTK_CONTAINER(m_widget), m_menubar );

  gtk_widget_show( m_menubar );

  PostCreation();

  Show( TRUE );
};

void wxMenuBar::Append( wxMenu *menu, const wxString &title )
{
  m_menus.Append( menu );
  menu->m_title = title;    // ??????

  size_t pos;
  do {
    pos = menu->m_title.First( '&' );
    if (pos != wxString::npos) menu->m_title.Remove( pos, 1 );
  } while (pos != wxString::npos);

  GtkWidget *root_menu;
  root_menu = gtk_menu_item_new_with_label( WXSTRINGCAST(menu->m_title) );
  gtk_widget_show( root_menu );
  gtk_menu_item_set_submenu( GTK_MENU_ITEM(root_menu), menu->m_menu );

  gtk_menu_bar_append( GTK_MENU_BAR(m_menubar), root_menu );
};

static int FindMenuItemRecursive( const wxMenu *menu, const wxString &menuString, const wxString &itemString )
{
  if (menu->m_title == menuString)
  {
    int res = menu->FindItem( itemString );
    if (res != -1) return res;
  };
  wxNode *node = menu->m_items.First();
  while (node)
  {
    wxMenuItem *item = (wxMenuItem*)node->Data();
    if (item->IsSubMenu())
      return FindMenuItemRecursive(item->GetSubMenu(), menuString, itemString);
    node = node->Next();
  };
  return -1;
};

int wxMenuBar::FindMenuItem( const wxString &menuString, const wxString &itemString ) const
{
  wxNode *node = m_menus.First();
  while (node)
  {
    wxMenu *menu = (wxMenu*)node->Data();
    int res = FindMenuItemRecursive( menu, menuString, itemString);
    if (res != -1) return res;
    node = node->Next();
  };
  return -1;
};

// Find a wxMenuItem using its id. Recurses down into sub-menus
static wxMenuItem* FindMenuItemByIdRecursive(const wxMenu* menu, int id)
{
  wxMenuItem* result = menu->FindItem(id);

  wxNode *node = menu->m_items.First();
  while ( node && result == NULL ) {
    wxMenuItem *item = (wxMenuItem*)node->Data();
    if ( item->IsSubMenu() )
      result = FindMenuItemByIdRecursive( item->GetSubMenu(), id );
    node = node->Next();
  };

  return result;
};

wxMenuItem* wxMenuBar::FindMenuItemById( int id ) const
{
  wxMenuItem* result = 0;
  wxNode *node = m_menus.First();
  while (node && result == 0)
  {
    wxMenu *menu = (wxMenu*)node->Data();
    result = FindMenuItemByIdRecursive( menu, id );
    node = node->Next();
  }
  return result;
}

bool wxMenuBar::IsChecked( int id ) const
{
  wxMenuItem* item = FindMenuItemById( id );
  if (item) return item->IsChecked();
  return FALSE;
}

bool wxMenuBar::IsEnabled( int id ) const
{
  wxMenuItem* item = FindMenuItemById( id );
  if (item) return item->IsEnabled();
  return FALSE;
}

//-----------------------------------------------------------------------------
// wxMenu
//-----------------------------------------------------------------------------

void gtk_menu_clicked_callback( GtkWidget *widget, gpointer data )
{
  wxMenu *menu = (wxMenu*)data;
  int id = menu->FindMenuIdByMenuItem(widget);

  wxASSERT( id != -1 ); // should find it!

  if (!menu->IsEnabled(id))
    return;

  wxCommandEvent event( wxEVENT_TYPE_MENU_COMMAND, id );
  event.SetEventObject( menu );
  event.SetInt(id );
  wxWindow *win = menu->GetInvokingWindow();
  if (win) win->ProcessEvent( event );
};

IMPLEMENT_DYNAMIC_CLASS(wxMenuItem,wxObject)

wxMenuItem::wxMenuItem()
{
  m_id = ID_SEPARATOR;
  m_isCheckMenu = FALSE;
  m_isChecked = FALSE;
  m_isEnabled = TRUE;
  m_subMenu = NULL;
  m_menuItem = NULL;
};

void wxMenuItem::SetText(const wxString& str)
{
  for ( const char *pc = str; *pc != '\0'; pc++ ) {
    if ( *pc == '&' )
      pc++; // skip it

    m_text << *pc;
  }
}

void wxMenuItem::Check( bool check )
{
  wxCHECK_RET( IsCheckable(), "can't check uncheckable item!" )

  m_isChecked = check;
  gtk_check_menu_item_set_state( (GtkCheckMenuItem*)m_menuItem, (gint)check );
}

bool wxMenuItem::IsChecked() const
{
  wxCHECK( IsCheckable(), FALSE ); // can't get state of uncheckable item!

  bool bIsChecked = ((GtkCheckMenuItem*)m_menuItem)->active != 0;

  wxASSERT( bIsChecked == m_isChecked ); // consistency check

  return bIsChecked;
}

IMPLEMENT_DYNAMIC_CLASS(wxMenu,wxEvtHandler)

wxMenu::wxMenu( const wxString &title )
{
  m_title = title;
  m_items.DeleteContents( TRUE );
  m_invokingWindow = NULL;
  m_menu = gtk_menu_new();  // Do not show!
};

void wxMenu::AppendSeparator()
{
  wxMenuItem *mitem = new wxMenuItem();
  mitem->SetId(ID_SEPARATOR);

  GtkWidget *menuItem = gtk_menu_item_new();
  gtk_menu_append( GTK_MENU(m_menu), menuItem );
  gtk_widget_show( menuItem );
  mitem->SetMenuItem(menuItem);
  m_items.Append( mitem );
};

void wxMenu::Append( int id, const wxString &item, const wxString &helpStr, bool checkable )
{
  wxMenuItem *mitem = new wxMenuItem();
  mitem->SetId(id);
  mitem->SetText(item);
  mitem->SetHelpString(helpStr);
  mitem->SetCheckable(checkable);
  const char *text = mitem->GetText();
  GtkWidget *menuItem = checkable ? gtk_check_menu_item_new_with_label(text)
                                  : gtk_menu_item_new_with_label(text);
  mitem->SetMenuItem(menuItem);

  gtk_signal_connect( GTK_OBJECT(menuItem), "activate",
                      GTK_SIGNAL_FUNC(gtk_menu_clicked_callback),
                      (gpointer*)this );

  gtk_menu_append( GTK_MENU(m_menu), menuItem );
  gtk_widget_show( menuItem );
  m_items.Append( mitem );
};

void wxMenu::Append( int id, const wxString &text, wxMenu *subMenu, const wxString &helpStr )
{
  wxMenuItem *mitem = new wxMenuItem();
  mitem->SetId(id);
  mitem->SetText(text);

  GtkWidget *menuItem = gtk_menu_item_new_with_label(mitem->GetText());
  mitem->SetHelpString(helpStr);
  mitem->SetMenuItem(menuItem);
  mitem->SetSubMenu(subMenu);

  gtk_menu_item_set_submenu( GTK_MENU_ITEM(menuItem), subMenu->m_menu );
  gtk_menu_append( GTK_MENU(m_menu), menuItem );
  gtk_widget_show( menuItem );
  m_items.Append( mitem );
};

int wxMenu::FindItem( const wxString itemString ) const
{
  wxString s( itemString );

  size_t pos;
  do {
    pos = s.First( '&' );
    if (pos != wxString::npos) s.Remove( pos, 1 );
  } while (pos != wxString::npos);

  wxNode *node = m_items.First();
  while (node)
  {
    wxMenuItem *item = (wxMenuItem*)node->Data();
    if (item->GetText() == s)
      return item->GetId();
    node = node->Next();
  };

  return -1;
};

void wxMenu::Enable( int id, bool enable )
{
  wxMenuItem *item = FindItem(id);
  if ( item )
    item->Enable(enable);
};

bool wxMenu::IsEnabled( int id ) const
{
  wxMenuItem *item = FindItem(id);
  if ( item )
    return item->IsEnabled();
  else
    return FALSE;
};

void wxMenu::Check( int id, bool enable )
{
  wxMenuItem *item = FindItem(id);
  if ( item )
    item->Check(enable);
};

bool wxMenu::IsChecked( int id ) const
{
  wxMenuItem *item = FindItem(id);
  if ( item )
    return item->IsChecked();
  else
    return FALSE;
};

void wxMenu::SetLabel( int id, const wxString &label )
{
  wxMenuItem *item = FindItem(id);
  if ( item )
    item->SetText(label);
};

int wxMenu::FindMenuIdByMenuItem( GtkWidget *menuItem ) const
{
  wxNode *node = m_items.First();
  while (node)
  {
    wxMenuItem *item = (wxMenuItem*)node->Data();
    if (item->GetMenuItem() == menuItem)
      return item->GetId();
    node = node->Next();
  };

  return -1;
};

wxMenuItem *wxMenu::FindItem(int id) const
{
  wxNode *node = m_items.First();
  while (node) {
    wxMenuItem *item = (wxMenuItem*)node->Data();
    if ( item->GetId() == id )
      return item;
    node = node->Next();
  };

  wxLogDebug("wxMenu::FindItem: item %d not found.", id);

  return NULL;
}

void wxMenu::SetInvokingWindow( wxWindow *win )
{
  m_invokingWindow = win;
};

wxWindow *wxMenu::GetInvokingWindow()
{
  return m_invokingWindow;
};


