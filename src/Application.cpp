//
// Created by Haluk AKGUNDUZ on 24.06.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#include "Application.h"

App::App(enum APPTYPE type, int *interfaces, LOGLEVEL logLevel, std::vector<int> componentCount, bool autoWake) :
        type(type) {

    Log::init(logLevel);

    deviceList = DeviceList::getInstance();

    LOGP_I("Using network interfaces : %s and %s",
          deviceList->get(interfaces[0])->getName().c_str(),
          deviceList->get(interfaces[1])->getName().c_str());

    componentController = ComponentFactory::newInstance(interfaces);

    if (!componentCount[COMP_DISTRIBUTOR] &&
        !componentCount[COMP_COLLECTOR] &&
        !componentCount[COMP_NODE]) {
            return;
    }

    if (componentCount[COMP_DISTRIBUTOR]) {
        if (!componentController->startDistributor(autoWake)) {
            return;
        }
    }

    if (componentCount[COMP_COLLECTOR]) {
        if (!componentController->startCollector(componentCount[COMP_COLLECTOR])) {
            return;
        }
    }

    if (componentCount[COMP_NODE]) {
        if (!componentController->startNode(componentCount[COMP_NODE])) {
            return;
        }
    }

    Component::registerNotify(this, [] (void *context, COMPONENT target, NOTIFYTYPE state) -> bool {

        LOGP_T("Notifying Application UI, target : %s, state : %s",
               ComponentType::getName(target), NotifyType::getName(state));
        return ((App*) context)->notifyHandler(target, state);
    });

    initialized = true;

    LOGP_I("Running in %s Mode with %d Distributor, %d Collector and %d Node",
          type == APPTYPE_WEB ? "Web" : "Console",
          componentCount[COMP_DISTRIBUTOR], componentCount[COMP_COLLECTOR], componentCount[COMP_NODE]);
}

App::~App() {

    LOGP_T("Deallocating App");

    delete componentController;

    delete deviceList;

    if (!initialized) {
        return;
    }

    Component::deRegisterNotify();

}

bool App::isInitialized() {

    return initialized;
}
