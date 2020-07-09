//
// Created by Haluk Akgunduz on 02/09/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include <Log.h>
#include <NodeState.h>
#include "ConsoleApp.h"


bool ConsoleApp::distPollHandler() {

    componentController->getDistributor()->sendWakeupMessagesAll(false);

    return true;
}

bool ConsoleApp::distStateHandler() {

    PRINT("Collector List : ");

    for (int i = 0 ; i < componentController->getDistributor()->getCollectors()->size(); i++) {

        auto *collector = componentController->getDistributor()->getCollectors()->getByIndex(i);

        PRINT("\t ID : %d, State : %s", collector->getID(), CollState::getName(((CollectorObject*)collector)->getState()));
    }

    PRINT("Node List : ");

    for (int i = 0 ; i < componentController->getDistributor()->getNodes()->size(); i++) {

        auto *node = componentController->getDistributor()->getNodes()->getByIndex(i);

        PRINT("\t ID : %d, State : %s", node->getID(), NodeState::getName(((NodeObject*)node)->getState()));
    }

    return true;
}
