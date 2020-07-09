//
// Created by Haluk AKGUNDUZ on 24.06.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#include "Application.h"

App::App(enum APPTYPE type, int argc, char** argv, int *interfaceID,
                                   LOGLEVEL* logLevel, bool enableDistributor, int* collInfo, int* nodeInfo) :
        type(type), argc(argc), argv(argv) {

    Log::init(logLevel[0], logLevel[1]);

    Component::registerNotify(this, notifyCB);

    deviceList = DeviceList::getInstance();

    PRINT("Using network interfaces : %s and %s",
          deviceList->get(interfaceID[0])->getName(), deviceList->get(interfaceID[1])->getName());

    componentController = ComponentController::newInstance(interfaceID[0], interfaceID[1]);

    if (enableDistributor) {
        componentController->startDistributor();
    }

    if (collInfo[0]) {
        componentController->startCollector(collInfo[0], collInfo[1]);
    }

    if (nodeInfo[0]) {
        componentController->startNode(nodeInfo[0], nodeInfo[1]);
    }

    PRINT("Running in %s Mode with %d Distributor, %d Collector and %d Node",
          type == APPTYPE_WEB ? "Web" : type == APPTYPE_CONSOLE ? "Console" : "WxWidgets",
          enableDistributor ? 1 : 0, collInfo[0], nodeInfo[0]);
}

enum APPTYPE App::getAppType() {

    return type;
}

bool App::notifyCB(void *context, int target, int id) {

    return ((App*) context)->notifyHandler(target, id);
}





