/////////////////////////////////////////////////////////////////////////////
// Name:        simple.cpp
// Author:      XX
// Created:     XX/XX/XX
// Copyright:   
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "simple.cpp"
#endif

// For compilers that support precompilation
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// Include private headers
#include "simple.h"

// Include icon header
#if defined(__WXGTK__) || defined(__WXMOTIF__)
    #include "mondrian.xpm"
#endif

// Include image
#include "smile.xpm"
    
// WDR: class implementations

//------------------------------------------------------------------------------
// MyFrame
//------------------------------------------------------------------------------

// WDR: event table for MyFrame

BEGIN_EVENT_TABLE(MyFrame,wxFrame)
    EVT_MENU(ID_QUIT, MyFrame::OnQuit)
    EVT_CLOSE(MyFrame::OnCloseWindow)
END_EVENT_TABLE()

MyFrame::MyFrame( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxFrame( parent, id, title, position, size, style )
{
    CreateMyMenuBar();
    
    CreateStatusBar(1);
    SetStatusText( "Welcome to wxCanvas sample!" );
    
    SetIcon(wxICON(mondrian));

    // Create wxCanvasAdmin and wxCanvas.
    m_admin = new wxCanvasAdmin;
    wxCanvas *canvas = new wxCanvas( m_admin, this, -1 );
    
    canvas->SetScroll( 0, 0, 400, 600 );
    canvas->SetMappingScroll( 0, 0, 400, 600, FALSE );
    
    // The wxCanvasAdmin need to know about all Admin wxCanvas objects.
    m_admin->Append( canvas );
    
    // One wxCanvas is the active one (current rendering and current
    // world coordinates).
    m_admin->SetActive( canvas );
    
    // One object group is the root in every canvas.
    wxCanvasObjectGroup *root = new wxCanvasObjectGroup(0,0);
    root->DeleteContents( TRUE );

    // Bunch of rects and images.
    wxBitmap bitmap( smile_xpm );
    wxImage image( bitmap );

    m_smile1 = new wxCanvasImage( image, 0,70,32,32 );
    root->Append( m_smile1 );

    int i;
    for (i = 10; i < 300; i+=10)
    {
        wxCanvasRect *r = new wxCanvasRect( i,50,3,140 );
        r->SetBrush( *wxRED_BRUSH );
        root->Append( r );
    }
    
    m_smile2 = new wxCanvasImage( image, 0,110,32,32 );
    root->Append( m_smile2 );

    for (i = 15; i < 300; i+=10)
    {
        wxCanvasRect *r = new wxCanvasRect( i,50,3,140 );
        r->SetBrush( *wxRED_BRUSH );
        root->Append( r );
    }
    
    // This will call all object and children recursivly so
    // all know what their wxCanvasAdmin is. Call at the end.
    root->SetAdmin( m_admin );
    
    // One object group is the root object.
    canvas->SetRoot( root );
}

void MyFrame::CreateMyMenuBar()
{
    wxMenu *file_menu = new wxMenu;
    file_menu->Append( ID_QUIT, "Quit...", "Quit program" );
    
    wxMenuBar *menu_bar = new wxMenuBar();
    menu_bar->Append( file_menu, "File" );
    
    SetMenuBar( menu_bar );
}

// WDR: handler implementations for MyFrame

void MyFrame::OnQuit( wxCommandEvent &event )
{
     Close( TRUE );
}

void MyFrame::OnCloseWindow( wxCloseEvent &event )
{
    // if ! saved changes -> return
    
    Destroy();
}

//------------------------------------------------------------------------------
// MyApp
//------------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

MyApp::MyApp()
{
}

bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame( NULL, -1, "SuperApp", wxPoint(20,20), wxSize(500,340) );
    frame->Show( TRUE );
    
    return TRUE;
}

int MyApp::OnExit()
{
    return 0;
}

