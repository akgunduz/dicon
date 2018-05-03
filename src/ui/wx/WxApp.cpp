/////////////////////////////////////////////////////////////////////////////
// Name:        WxApp.cpp
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

#include "WxApp.h"

////@begin XPM images

////@end XPM images


/*
 * Application instance implementation
 */

////@begin implement app
IMPLEMENT_APP_NO_MAIN( WxApp )
////@end implement app


/*
 * WxApp type definition
 */

IMPLEMENT_CLASS( WxApp, wxApp )


/*
 * WxApp event table definition
 */

BEGIN_EVENT_TABLE( WxApp, wxApp )

////@begin WxApp event table entries
////@end WxApp event table entries

END_EVENT_TABLE()


/*
 * Constructor for WxApp
 */

WxApp::WxApp()
{
    this->app = new Application(0, 0);
    Init();
}

WxApp::WxApp(Application* app)
{
    this->app = app;
    this->app->setStartState(true);
    Init();
}

/*
 * Member initialisation
 */

void WxApp::Init()
{
////@begin WxApp member initialisation
////@end WxApp member initialisation
}

/*
 * Initialisation for WxApp
 */

bool WxApp::OnInit()
{    
////@begin WxApp initialisation
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
    Wx* mainWindow = new Wx(app,  NULL );
    mainWindow->Show(true);
////@end WxApp initialisation

    return true;
}


/*
 * Cleanup for WxApp
 */

int WxApp::OnExit()
{    
////@begin WxApp cleanup
    return wxApp::OnExit();
////@end WxApp cleanup
}

