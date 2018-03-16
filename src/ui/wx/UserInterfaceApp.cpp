//
// Created by Haluk AKGUNDUZ on 27/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include <wx/app.h>
#include "UserInterfaceApp.h"
#include "UserInterface.h"

//wxIMPLEMENT_APP_NO_MAIN(UserInterfaceApp);
wxIMPLEMENT_APP(UserInterfaceApp);

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

UserInterface *g_ui;

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
 * Cleanup for UserInterfaceApp
 */

int UserInterfaceApp::OnExit()
{
////@begin UserInterfaceApp cleanup
    return wxApp::OnExit();
////@end UserInterfaceApp cleanup
}

bool UserInterfaceApp::OnInit() {

	g_ui = new UserInterface( NULL);
	g_ui->Show(true);
	return true;
}
