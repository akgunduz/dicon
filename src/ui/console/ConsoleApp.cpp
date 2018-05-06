//
// Created by Haluk Akgunduz on 02/09/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include <Log.h>
#include "ConsoleApp.h"

ConsoleApp::ConsoleApp(void *controller)
    : componentController(controller) {

	Log::registerUIController(this, updateUICallback);

    distInit();
    collInit();
    nodeInit();
}

void ConsoleApp::updateUIEvent(int id, void *data) {

	ConsoleEvent event;
	event.SetId(id);
    event.SetClientData(data);

    updateUI(event);
}

void ConsoleApp::updateUI(ConsoleEvent& event) {

    int id = event.GetId();

    if (uiUpdater[id] != nullptr) {
        ((this)->*(uiUpdater[id]))(event);
    }
}

void ConsoleApp::updateUICallback(void *context, int id, void *data) {

    ((ConsoleApp*) context)->updateUIEvent(id, data);
}
