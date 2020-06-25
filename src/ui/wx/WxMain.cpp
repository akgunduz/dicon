/////////////////////////////////////////////////////////////////////////////
// Name:        WxMain.cpp
// Purpose:     
// Author:      Haluk Akgunduz
// Modified by: 
// Created:     Thu 03 May 2018 12:54:09 +03
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

#include "WxMain.h"

////@begin XPM images
////@end XPM images


/*
 * Application instance implementation
 */

////@begin implement app
IMPLEMENT_APP_NO_MAIN( WxMain )
////@end implement app


/*
 * WxMain type definition
 */

IMPLEMENT_CLASS(WxMain, wxApp )


/*
 * WxMain event table definition
 */

BEGIN_EVENT_TABLE( WxMain, wxApp )

////@begin WxApp event table entries
////@end WxApp event table entries

END_EVENT_TABLE()


/*
 * Constructor for WxMain
 */

WxMain::WxMain()
{
    Init();
}

WxMain::WxMain(void* controller)
{
    this->controller = controller;
    Init();
}

/*
 * Member initialisation
 */

void WxMain::Init()
{
////@begin WxApp member initialisation
////@end WxApp member initialisation
}

/*
 * Initialisation for WxMain
 */

bool WxMain::OnInit()
{    
////@begin WxMain initialisation
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
	Wx* mainWindow = new Wx( controller, NULL );
	mainWindow->Show(true);

////@end WxMain initialisation

    return true;
}


/*
 * Cleanup for WxApp
 */

int WxMain::OnExit()
{    
////@begin WxMain cleanup
	return wxApp::OnExit();
////@end WxMain cleanup
}


