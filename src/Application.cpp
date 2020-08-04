//
// Created by Haluk AKGUNDUZ on 24.06.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#include "Application.h"

App::App(enum APPTYPE type, int *interfaces, LOGLEVEL* logLevel, std::vector<int> componentCount, bool autoWake) :
        type(type) {

    Log::init(logLevel[0], logLevel[1]);

    Component::registerNotify(this, [] (void *context, COMPONENT target, NOTIFYSTATE state) -> bool {

        return ((App*) context)->notifyHandler(target, state);
    });

    deviceList = DeviceList::getInstance();

    PRINT("Using network interfaces : %s and %s",
          deviceList->get(interfaces[0])->getName(), deviceList->get(interfaces[1])->getName());

    componentController = ComponentController::newInstance(interfaces);

    if (componentCount[COMP_DISTRIBUTOR]) {
        componentController->startDistributor(autoWake);
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
