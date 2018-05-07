//
// Created by Haluk Akgunduz on 02/09/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#include "Common.h"
#include "ConsoleEvent.h"

#ifndef __ConsoleApp_H_
#define __ConsoleApp_H_

class ConsoleApp;

typedef void (ConsoleApp::*fConsoleUpdater)(ConsoleEvent &event);

#define MAX_UI_CB 100

class ConsoleApp {

    void* componentController;

	fConsoleUpdater uiUpdater[MAX_UI_CB];

public:
    ConsoleApp(void *);

	static void updateUICallback(void*, int, void*);

    void updateUI(ConsoleEvent& event);
    void updateUIEvent(int, void*);

    void distInit();
    void distAddtoCollectorList(ConsoleEvent &event);
    void distAddtoNodeList(ConsoleEvent &event);

    void collInit();
    void collUpdateFileList(ConsoleEvent &event);
    void collUpdateFileListItem(ConsoleEvent &event);
    void collUpdateProcessList(ConsoleEvent &event);
    void collUpdateProcessListItem(ConsoleEvent &event);

	void nodeInit();
	void nodeUpdateState(ConsoleEvent &event);
	void nodeUpdateProcessList(ConsoleEvent &event);
	void nodeUpdateClear(ConsoleEvent &event);
};


#endif //__ConsoleApp_H_
