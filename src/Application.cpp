//
// Created by Haluk AKGUNDUZ on 24.06.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#include "Application.h"
#include "ComponentFactory.h"
#include "DeviceList.h"
#include "Util.h"
#include "Log.h"

App::App(enum APPTYPE type, AppParams& params) :
        type(type) {

    Log::init(params.logInfo);

    LOGP_I("Using Temp path : %s ", Util::tmpPath.c_str());

    deviceList = DeviceList::getInstance();

    LOGP_I("Using network interfaces : %s and %s",
          deviceList->get(params.interfaceID[0])->getName().c_str(),
          deviceList->get(params.interfaceID[1])->getName().c_str());

    componentFactory = ComponentFactory::newInstance(params.interfaceID);

    if (!params.componentCount[COMP_DISTRIBUTOR] &&
        !params.componentCount[COMP_COLLECTOR] &&
        !params.componentCount[COMP_NODE]) {
            return;
    }

    if (params.componentCount[COMP_DISTRIBUTOR]) {
        if (!componentFactory->startDistributor(params.autoWake)) {
            return;
        }
    }

    if (params.componentCount[COMP_COLLECTOR]) {
        if (!componentFactory->startCollector(params.componentCount[COMP_COLLECTOR])) {
            return;
        }
    }

    if (params.componentCount[COMP_NODE]) {
        if (!componentFactory->startNode(params.componentCount[COMP_NODE])) {
            return;
        }
    }

    Component::registerNotify(this, [] (void *context, COMPONENT target, NOTIFYTYPE state) -> bool {

        if (!context) {

            return false;
        }

        LOGP_T("Notifying Application UI, target : %s, state : %s",
               ComponentType::getName(target), NotifyType::getName(state));
        return ((App*) context)->notifyHandler(target, state);
    });

    initialized = true;

    LOGP_I("Running in %s Mode with %d Distributor, %d Collector and %d Node",
          type == APPTYPE_WEB ? "Web" : "Console",
           params.componentCount[COMP_DISTRIBUTOR],
           params.componentCount[COMP_COLLECTOR],
           params.componentCount[COMP_NODE]);
}

App::~App() {

    LOGP_T("Deallocating App");

    Component::deRegisterNotify();

    delete componentFactory;

    delete deviceList;

    if (!initialized) {
        return;
    }

}

bool App::isInitialized() {

    return initialized;
}
