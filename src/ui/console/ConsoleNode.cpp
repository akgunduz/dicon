//
// Created by Haluk Akgunduz on 02/09/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include <Log.h>
#include <NodeState.h>
#include "ConsoleApp.h"

void ConsoleApp::nodeInit() {

	uiUpdater[UI_UPDATE_NODE_STATE] = &ConsoleApp::nodeUpdateState;
	uiUpdater[UI_UPDATE_NODE_PROCESS_LIST] = &ConsoleApp::nodeUpdateProcessList;
	uiUpdater[UI_UPDATE_NODE_CLEAR] = &ConsoleApp::nodeUpdateClear;
}

void ConsoleApp::nodeUpdateState(ConsoleEvent &event) {

	auto *data = (UserInterfaceEvent *)event.GetClientData();

    LOG_S("Console UI ------> Node State : %s", NodeState::getName((NODE_STATES) data->getData(0)));
}

void ConsoleApp::nodeUpdateClear(ConsoleEvent &event) {

}

void ConsoleApp::nodeUpdateProcessList(ConsoleEvent &event) {

	auto *data = (UserInterfaceEvent *)event.GetClientData();

    LOG_S("Console UI ------> Node Process Added : Collector[%d], Job : %s and Process : %s",
          data->getData(0), data->getString(0).c_str(), data->getString(1).c_str()) ;
}
