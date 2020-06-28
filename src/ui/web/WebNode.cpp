//
// Created by Haluk Akgunduz on 02/09/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include <Log.h>
#include <NodeState.h>
#include "WebApp.h"

void WebApp::nodeInit() {

    uiUpdater[UI_UPDATE_NODE_ID] = &WebApp::nodeUpdateID;
	uiUpdater[UI_UPDATE_NODE_STATE] = &WebApp::nodeUpdateState;
	uiUpdater[UI_UPDATE_NODE_PROCESS_LIST] = &WebApp::nodeUpdateProcessList;
	uiUpdater[UI_UPDATE_NODE_CLEAR] = &WebApp::nodeUpdateClear;
}

void WebApp::nodeUpdateID(WebEvent& event) {

}

void WebApp::nodeUpdateState(WebEvent &event) {

	auto *data = (UserInterfaceEvent *)event.GetClientData();

//    LOG_S("Console UI ------> Node State : %s", NodeState::getName((NODE_STATES) data->getData(0)));
}

void WebApp::nodeUpdateClear(WebEvent &event) {

}

void WebApp::nodeUpdateProcessList(WebEvent &event) {

	auto *data = (UserInterfaceEvent *)event.GetClientData();

//    LOG_S("Console UI ------> Node Process Added : Collector[%d], Job : %s and Process : %d",
//          data->getData(0), data->getString(0).c_str(), data->getData(1)) ;
}
