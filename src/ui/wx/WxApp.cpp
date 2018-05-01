/////////////////////////////////////////////////////////////////////////////
// Name:        UserInterfaceApp.cpp
// Purpose:     
// Author:      Haluk Akgunduz
// Modified by: 
// Created:     Sat 28 Apr 19:13:18 2018
// RCS-ID:      
// Copyright:   Licensed with AGPL v3.0
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include "WxApp.h"

////@begin XPM images
////@end XPM images


/*
 * Application instance implementation
 */

////@begin implement app
IMPLEMENT_APP( UserInterfaceApp )
////@end implement app


/*
 * UserInterfaceApp type definition
 */

IMPLEMENT_CLASS( UserInterfaceApp, wxApp )


/*
 * UserInterfaceApp event table definition
 */

BEGIN_EVENT_TABLE( UserInterfaceApp, wxApp )

////@begin UserInterfaceApp event table entries
////@end UserInterfaceApp event table entries

END_EVENT_TABLE()


/*
 * Constructor for UserInterfaceApp
 */

UserInterfaceApp::UserInterfaceApp()
{
    Init();
}


/*
 * Member initialisation
 */

void UserInterfaceApp::Init()
{
////@begin UserInterfaceApp member initialisation
////@end UserInterfaceApp member initialisation
}

/*
 * Initialisation for UserInterfaceApp
 */

bool UserInterfaceApp::OnInit()
{    
////@begin UserInterfaceApp initialisation
	// Remove the comment markers above and below this block
	// to make permanent changes to the code.

#if wxUSE_XPM
	wxImage::AddHandler(new wxXPMHandler);
#endif
#if wxUSE_LIBPNG
	wxImage::AddHandler(new wxPNGHandler);
#endif
#if wxUSE_LIBJPEG
	wxImage::AddHandler(new wxJPEGHandler);
#endif
#if wxUSE_GIF
	wxImage::AddHandler(new wxGIFHandler);
#endif
	UserInterface* mainWindow = new UserInterface( NULL );
	mainWindow->Show(true);
////@end UserInterfaceApp initialisation

    return true;
}


/*
 * Cleanup for UserInterfaceApp
 */

int UserInterfaceApp::OnExit()
{    
////@begin UserInterfaceApp cleanup
	return wxApp::OnExit();
////@end UserInterfaceApp cleanup
}

