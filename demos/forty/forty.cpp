/////////////////////////////////////////////////////////////////////////////
// Name:        forty.cpp
// Purpose:     Forty Thieves patience game
// Author:      Chris Breeze
// Modified by:
// Created:     21/07/97
// RCS-ID:      $Id$
// Copyright:   (c) 1993-1998 Chris Breeze
// Licence:   	wxWindows licence
//---------------------------------------------------------------------------
// Last modified: 22nd July 1998 - ported to wxWindows 2.0
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "canvas.h"
#include "forty.h"
#include "card.h"
#include "scoredg.h"

BEGIN_EVENT_TABLE(FortyFrame, wxFrame)
	EVT_MENU(NEW_GAME, FortyFrame::NewGame)
	EVT_MENU(EXIT, FortyFrame::Exit)
	EVT_MENU(ABOUT, FortyFrame::About)
	EVT_MENU(UNDO, FortyFrame::Undo)
	EVT_MENU(REDO, FortyFrame::Redo)
	EVT_MENU(SCORES, FortyFrame::Scores)
	EVT_MENU(RIGHT_BUTTON_UNDO, FortyFrame::ToggleRightButtonUndo)
	EVT_MENU(HELPING_HAND, FortyFrame::ToggleHelpingHand)
        EVT_MENU(LARGE_CARDS, FortyFrame::ToggleCardSize)
    EVT_CLOSE(FortyFrame::OnCloseWindow)
END_EVENT_TABLE()

// Create a new application object
IMPLEMENT_APP	(FortyApp)

wxColour* FortyApp::m_backgroundColour = 0;
wxColour* FortyApp::m_textColour = 0;
wxBrush*  FortyApp::m_backgroundBrush = 0;

FortyApp::FortyApp()
{
}

FortyApp::~FortyApp()
{
    delete m_backgroundColour;
    delete m_textColour;
    delete m_backgroundBrush;
    delete Card::m_symbolBmap;
    delete Card::m_pictureBmap;

}

bool FortyApp::OnInit()
{
        bool largecards = FALSE;
        wxSize size(668,510);

        if ((argc > 1) && (!wxStrcmp(argv[1],"-L")))
        {
            largecards = TRUE;
            size = wxSize(1000,750);
        }

	FortyFrame* frame = new FortyFrame(
			0,
			"Forty Thieves",
                        -1, -1, size.x, size.y,largecards
			);

 	// Show the frame
	frame->Show(TRUE);

        frame->GetCanvas()->ShowPlayerDialog();

	return TRUE;
}

const wxColour& FortyApp::BackgroundColour()
{
	if (!m_backgroundColour)
	{
		m_backgroundColour = new wxColour(0, 128, 0);
	}

	return *m_backgroundColour;
}

const wxBrush& FortyApp::BackgroundBrush()
{
	if (!m_backgroundBrush)
	{
		m_backgroundBrush = new wxBrush(BackgroundColour(), wxSOLID);
	}

	return *m_backgroundBrush;
}

const wxColour& FortyApp::TextColour()
{
	if (!m_textColour)
	{
		m_textColour = new wxColour("BLACK");
	}

	return *m_textColour;
}

// My frame constructor
FortyFrame::FortyFrame(wxFrame* frame, char* title, int x, int y, int w, int h,bool largecards):
	wxFrame(frame, -1, title, wxPoint(x, y), wxSize(w, h))
{
#ifdef __WXMAC__
	// we need this in order to allow the about menu relocation, since ABOUT is not the default id of the about menu 
	wxApp::s_macAboutMenuItemId = ABOUT ;
#endif
	// set the icon
#ifdef __WXMSW__
	SetIcon(wxIcon("CardsIcon"));
#else
#ifdef GTK_TBD
	SetIcon(wxIcon(Cards_bits, Cards_width, Cards_height));
#endif
#endif

	// Make a menu bar
	wxMenu* gameMenu = new wxMenu;
	gameMenu->Append(NEW_GAME, "&New", "Start a new game");
	gameMenu->Append(SCORES, "&Scores...", "Displays scores");
	gameMenu->Append(EXIT, "E&xit", "Exits Forty Thieves");

	wxMenu* editMenu = new wxMenu;
	editMenu->Append(UNDO, "&Undo", "Undo the last move");
	editMenu->Append(REDO, "&Redo", "Redo a move that has been undone");

	wxMenu*	optionsMenu = new wxMenu;
	optionsMenu->Append(RIGHT_BUTTON_UNDO,
			"&Right button undo",
			"Enables/disables right mouse button undo and redo",
			TRUE
			);
	optionsMenu->Append(HELPING_HAND,
			"&Helping hand",
			"Enables/disables hand cursor when a card can be moved",
			TRUE
			);
        optionsMenu->Append(LARGE_CARDS,
                        "&Large cards",
                        "Enables/disables large cards for high resolution displays",
                        TRUE
                        );
	optionsMenu->Check(HELPING_HAND, TRUE);
	optionsMenu->Check(RIGHT_BUTTON_UNDO, TRUE);
        optionsMenu->Check(LARGE_CARDS, largecards ? TRUE : FALSE);

	wxMenu* helpMenu = new wxMenu;
	helpMenu->Append(ABOUT, "&About", "Displays program version information");

	m_menuBar = new wxMenuBar;
	m_menuBar->Append(gameMenu,    "&Game");
	m_menuBar->Append(editMenu,    "&Edit");
	m_menuBar->Append(optionsMenu, "&Options");
	m_menuBar->Append(helpMenu,    "&Help");

	SetMenuBar(m_menuBar);

        if (largecards)
            Card::SetScale(1.3);

	m_canvas = new FortyCanvas(this, 0, 0, 400, 400);
	wxLayoutConstraints* constr = new wxLayoutConstraints;
	constr->left.SameAs(this, wxLeft);
	constr->top.SameAs(this, wxTop);
	constr->right.SameAs(this, wxRight);
	constr->height.SameAs(this, wxHeight);
	m_canvas->SetConstraints(constr);

	CreateStatusBar();
}

FortyFrame::~FortyFrame()
{
}

void FortyFrame::OnCloseWindow(wxCloseEvent& event)
{
    if (m_canvas->OnCloseCanvas() )
    {
        this->Destroy();
    }
    else
        event.Veto();
}

void
FortyFrame::NewGame(wxCommandEvent&)
{
	m_canvas->NewGame();
}

void
FortyFrame::Exit(wxCommandEvent&)
{
#ifdef __WXGTK__
	// wxGTK doesn't call OnClose() so we do it here
//	if (OnClose())
#endif
	Close(TRUE);
}

void
FortyFrame::About(wxCommandEvent&)
{
	wxMessageBox(
		"Forty Thieves\n\n"
		"A freeware program using the wxWindows\n"
		"portable C++ GUI toolkit.\n"
		"http://www.wxwindows.org\n"
		"http://www.freiburg.linux.de/~wxxt\n\n"
		"Author: Chris Breeze (c) 1992-1998\n"
		"email: chris.breeze@iname.com",
		"About Forty Thieves",
		wxOK, this
		);
}

void
FortyFrame::Undo(wxCommandEvent&)
{
	m_canvas->Undo();
}

void
FortyFrame::Redo(wxCommandEvent&)
{
	m_canvas->Redo();
}

void
FortyFrame::Scores(wxCommandEvent&)
{
	m_canvas->UpdateScores();
	ScoreDialog scores(this, m_canvas->GetScoreFile());
	scores.Display();
}

void
FortyFrame::ToggleRightButtonUndo(wxCommandEvent& event)
{
	bool checked = m_menuBar->IsChecked(event.GetId());
	m_canvas->EnableRightButtonUndo(checked);
}

void
FortyFrame::ToggleHelpingHand(wxCommandEvent& event)
{
	bool checked = m_menuBar->IsChecked(event.GetId());
	m_canvas->EnableHelpingHand(checked);
}

void
FortyFrame::ToggleCardSize(wxCommandEvent& event)
{
        bool checked = m_menuBar->IsChecked(event.GetId());
        Card::SetScale(checked ? 1.3 : 1);
        m_canvas->LayoutGame();
        m_canvas->Refresh();
}


