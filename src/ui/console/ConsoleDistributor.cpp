//
// Created by Haluk Akgunduz on 02/09/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include <ui/UserInterfaceEvent.h>
#include <Log.h>
#include <NodeState.h>
#include "ConsoleApp.h"

void ConsoleApp::distInit() {

    uiUpdater[UI_UPDATE_DIST_COLL_LIST] = &ConsoleApp::distAddtoCollectorList;
    uiUpdater[UI_UPDATE_DIST_NODE_LIST] = &ConsoleApp::distAddtoNodeList;
}

void ConsoleApp::distAddtoCollectorList(ConsoleEvent &event) {

    long i = 0;

    auto *data = (UserInterfaceEvent *)event.GetClientData();

    if (data->getData(1) > 0) {

        LOG_S("Console UI ------> Distributor Collector Updated : Collector[%d], Assigned Node[%d]",
              data->getData(0), data->getData(1));
    } else {

        LOG_S("Console UI ------> Distributor Collector Updated : Collector[%d], No Assigned Node",
              data->getData(0));
    }
}

void ConsoleApp::distAddtoNodeList(ConsoleEvent &event) {

    long i = 0;

    auto *data = (UserInterfaceEvent *)event.GetClientData();

    LOG_S("Console UI ------> Distributor Node Updated : Node[%d], State : %s",
          data->getData(0), NodeState::getName((NODE_STATES)data->getData(1)));
}
