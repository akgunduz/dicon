//
// Created by Haluk AKGUNDUZ on 24.06.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#include <Log.h>
#include "UserInterfaceApp.h"

UserInterfaceApp::UserInterfaceApp(enum APPTYPE type, int argc, char** argv, int *interfaceID,
                                   LOGLEVEL* logLevel, const int* distCount, int* collInfo, int* nodeInfo) :
                                   type(type), argc(argc), argv(argv) {

    Log::init(logLevel[0], logLevel[1]);

    deviceList = DeviceList::getInstance();

    componentController = ComponentController::newInstance(interfaceID[0], interfaceID[1]);

    if (*distCount) {
        componentController->startDistributor();
    }

    if (collInfo[0]) {
        componentController->startCollector(collInfo[0], collInfo[1]);
    }

    if (nodeInfo[0]) {
        componentController->startNode(nodeInfo[0], nodeInfo[1]);
    }

    LOG_S("Running in %s Mode with %d Distributor, %d Collector and %d Node, using interfaces : %s and %s",
          type == APPTYPE_WEB ? "Web" : type == APPTYPE_CONSOLE ? "Console" : "WxWidgets",
          *distCount, collInfo[0], nodeInfo[0],
          deviceList->get(interfaceID[0])->getName(), deviceList->get(interfaceID[1])->getName());
}

enum APPTYPE UserInterfaceApp::getAppType() {

    return type;
}