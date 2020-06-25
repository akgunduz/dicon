//
// Created by Haluk Akgunduz on 02/09/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_CONSOLEAPP_H
#define DICON_CONSOLEAPP_H

#include "Common.h"
#include "ConsoleEvent.h"
#include "UserInterfaceApp.h"

class ConsoleApp;

typedef void (ConsoleApp::*fConsoleUpdater)(ConsoleEvent &event);

class ConsoleApp : public UserInterfaceApp {

	fConsoleUpdater uiUpdater[MAX_UI_CB];

public:
    ConsoleApp(int argc, char** argv, int *interfaceID,
                        LOGLEVEL* logLevel, int* distCount, int* collInfo, int* nodeInfo);

    static void updateUICallback(void*, int, void*);
    void updateUIEvent(int, void*);
    void updateUI(ConsoleEvent& event);

    void distInit();
    void distAddtoCollectorList(ConsoleEvent &event);
    void distAddtoNodeList(ConsoleEvent &event);

    void collInit();
    void collUpdateID(ConsoleEvent &event);
    void collUpdateFileList(ConsoleEvent &event);
    void collUpdateFileListItem(ConsoleEvent &event);
    void collUpdateProcessList(ConsoleEvent &event);
    void collUpdateProcessListItem(ConsoleEvent &event);

	void nodeInit();
    void nodeUpdateID(ConsoleEvent& event);
	void nodeUpdateState(ConsoleEvent &event);
	void nodeUpdateProcessList(ConsoleEvent &event);
	void nodeUpdateClear(ConsoleEvent &event);

    int run() override;
};


#endif //DICON_CONSOLEAPP_H
