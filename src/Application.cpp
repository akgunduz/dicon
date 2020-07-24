//
// Created by Haluk AKGUNDUZ on 24.06.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#include "Application.h"

App::App(enum APPTYPE type, int argc, char** argv, int *interfaceID,
                                   LOGLEVEL* logLevel, int* componentCount) :
        type(type), argc(argc), argv(argv) {

    Log::init(logLevel[0], logLevel[1]);

    Component::registerNotify(this, notifyCB);

    deviceList = DeviceList::getInstance();

    PRINT("Using network interfaces : %s and %s",
          deviceList->get(interfaceID[0])->getName(), deviceList->get(interfaceID[1])->getName());

    componentController = ComponentController::newInstance(interfaceID[0], interfaceID[1]);

    if (componentCount[COMP_DISTRIBUTOR]) {
        componentController->startDistributor();
    }

    if (componentCount[COMP_COLLECTOR]) {
        componentController->startCollector(componentCount[COMP_COLLECTOR]);
    }

    if (componentCount[COMP_NODE]) {
        componentController->startNode(componentCount[COMP_NODE]);
    }

    PRINT("Running in %s Mode with %d Distributor, %d Collector and %d Node",
          type == APPTYPE_WEB ? "Web" : "Console",
          componentCount[COMP_DISTRIBUTOR], componentCount[COMP_COLLECTOR], componentCount[COMP_NODE]);
}

enum APPTYPE App::getAppType() {

    return type;
}

bool App::notifyCB(void *context, long target, long id) {

    return ((App*) context)->notifyHandler(target, id);
}





