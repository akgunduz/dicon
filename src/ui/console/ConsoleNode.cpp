//
// Created by Haluk Akgunduz on 02/09/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include <Log.h>
#include <NodeState.h>
#include "ConsoleApp.h"

bool ConsoleApp::nodeStateHandler(int id) {

    auto *node = componentController->getNode(id);
    if (!node) {
        PRINT("Can not find the node with ID : %d !!!", id);
        return false;
    }

    auto nodeObj = (NodeObject&) node->getHost();

    PRINT("Node[%d] Status", id);

    PRINT("State", nodeObj.getState());

    PRINT("Processes");

    for (auto &process : node->getProcessList()) {

        std::string processCommand = process.get().getParsedProcess();
        Util::replaceStr(processCommand, ROOT_SIGN, "");

        PRINT("ID : %d, Assigned Collector : %d, Job ID : %s, Process : %s",
              process.getID(),
              process.getAssigned(),
              process.getJobName().c_str(),
              processCommand.c_str());
    }

    return true;
}
