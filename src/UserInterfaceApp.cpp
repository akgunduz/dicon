//
// Created by Haluk AKGUNDUZ on 27/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include <wx/app.h>
#include "UserInterfaceApp.h"
#include "UserInterface.h"

wxIMPLEMENT_APP_NO_MAIN(UserInterfaceApp);

UserInterface *g_ui;

bool UserInterfaceApp::OnInit() {

	g_ui = new UserInterface( NULL);
	g_ui->Show(true);
	return true;
}
