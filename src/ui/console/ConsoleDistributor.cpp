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

    for (auto& collectorPair : componentController->getDistributor()->getCollectors()->get()) {

        auto *collector = collectorPair.second;

        PRINT("\t ID : %d, State : %s", collector->getID(), CollState::getName(((CollectorObject*)collector)->getState()));
    }

    PRINT("Node List : ");

    for (auto& nodePair : componentController->getDistributor()->getNodes()->get()) {

        auto *node = nodePair.second;

        PRINT("\t ID : %d, State : %s", node->getID(), NodeState::getName(((NodeHost*)node)->getState()));
    }

    return true;
}
