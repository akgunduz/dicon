//
// Created by Haluk Akgunduz on 02/09/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include <cstring>
#include <Util.h>
#include <NodeHost.h>
#include "WebApp.h"
#include "ComponentFactory.h"

bool WebApp::nodeHandler(struct mg_connection *conn, const char * uri) {

    const struct mg_request_info *ri = mg_get_request_info(conn);

    char *nextStr;

    int nodeID = (int) strtol(uri + 1, &nextStr, 10);

    if (0 == strcmp(ri->request_method, "GET")) {

        if (0 == strcmp(nextStr, "/state")) {

            return nodeStateHandler(conn, nodeID);
        }
    }

    return false;
}

bool WebApp::nodeStateHandler(struct mg_connection *conn, int id) {

    auto &node = componentFactory->getNode(id);
    if (!node) {
        sendError(conn, "Can not find the node with ID : %d !!!", id);
        return false;
    }

    auto nodeHost = std::static_pointer_cast<NodeHost>(node->getHost());

    nlohmann::json nodeInfo;

    nodeInfo["_state"] = nodeHost->getState();

    nlohmann::json processList;

    for (auto &process : node->getProcessList()) {

        nlohmann::json processItem;

        std::string processCommand = process->getParsedProcess();
        Util::replaceStr(processCommand, ROOT_SIGN, "");

        processItem["_processID"] = process->getID();
        processItem["_collectorID"] = process->getAssigned();
        processItem["_jobID"] = process->getAssignedJob();
        processItem["_process"] = processCommand;
        processItem["_duration"] = process->getDuration();

        processList += processItem;

    }

    nodeInfo["_processList"] = processList;

    sendStr(conn, nodeInfo.dump());

    return true;
}
