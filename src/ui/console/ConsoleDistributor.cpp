//
// Created by Haluk Akgunduz on 02/09/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include <Log.h>
#include <NodeState.h>
#include <CollectorUnit.h>
#include "ConsoleApp.h"


bool ConsoleApp::distPollHandler() {

    componentController->getDistributor()->sendWakeupMessagesAll(false);

    return true;
}

bool ConsoleApp::distStateHandler() {

    LOGP_I("Collector List : ");

    for (auto& collectorPair : componentController->getDistributor()->getCollectors()->get()) {

        auto collector = std::static_pointer_cast<CollectorUnit>(collectorPair.second);

        LOGP_I("\t ID : %d, State : %s", collector->getID(), CollState::getName(collector->getState()));
    }

    LOGP_I("Node List : ");

    for (auto& nodePair : componentController->getDistributor()->getNodes()->get()) {

        auto node = std::static_pointer_cast<NodeUnit>(nodePair.second);

        LOGP_I("\t ID : %d, State : %s", node->getID(), NodeState::getName(node->getState()));
    }

    return true;
}
