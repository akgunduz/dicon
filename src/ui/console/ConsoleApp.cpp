//
// Created by Haluk Akgunduz on 02/09/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "ConsoleApp.h"

ConsoleApp::ConsoleApp(int argc, char** argv, int *interfaceID,
                       LOGLEVEL* logLevel, int* distCount, int* collInfo, int* nodeInfo)
    : UserInterfaceApp(APPTYPE_CONSOLE, argc, argv, interfaceID, logLevel,
                       distCount, collInfo, nodeInfo) {

    Log::registerUIController(this, updateUICallback);

    distInit();
    collInit();
    nodeInit();
}

void ConsoleApp::updateUICallback(void *context, int id, void *data) {

    ((ConsoleApp*) context)->updateUIEvent(id, data);
}

void ConsoleApp::updateUIEvent(int id, void *data) {

	ConsoleEvent event;
	event.SetId(id);
    event.SetClientData(data);

    //On real UI architectures this updateUI should be asynchronous like in WX
    updateUI(event);
}

void ConsoleApp::updateUI(ConsoleEvent& event) {

    int id = event.GetId();

    if (uiUpdater[id] != nullptr) {
        ((this)->*(uiUpdater[id]))(event);
    }
}

int ConsoleApp::run() {

    int in;
    do {

        in = getchar();
        switch(in) {
            case 'p':
                componentController->getDistributor()->sendWakeupMessagesAll(false);
                break;
            case 'l':
                componentController->getCollector(1)->loadJob(NULL);
                break;
            case 'x':
                componentController->getCollector(1)->processJob();
                break;
            case 'q':

                return 0;
            default:
                break;
        }

    } while(true);

}
