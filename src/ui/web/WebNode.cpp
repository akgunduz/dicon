//
// Created by Haluk Akgunduz on 02/09/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include <Log.h>
#include <NodeState.h>
#include "WebApp.h"

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

    auto &node = componentController->getNode(id);
    if (!node) {
        sendError(nullptr, conn, "Can not find the node with ID : %d !!!", id);
        return false;
    }

    auto host = (NodeHost&) node->getHost();

    auto* jsonObj = json_object_new_object();
    if (jsonObj == nullptr) {
        sendError(&node->getHost(), conn, "Can not create json object!!!");
        return false;
    }

    json_object_object_add(jsonObj, "_state", json_object_new_int(host.getState()));

    auto* processList = json_object_new_array();
    for (auto &process : node->getProcessList()) {

        auto* processItem = json_object_new_object();
        json_object_object_add(processItem, "_processID", json_object_new_int(process.getID()));
        json_object_object_add(processItem, "_collectorID", json_object_new_int(process.getAssigned()));
        json_object_object_add(processItem, "_jobID", json_object_new_int(process.getAssignedJob()));
        std::string processCommand = process.getParsedProcess();
        Util::replaceStr(processCommand, ROOT_SIGN, "");
        json_object_object_add(processItem, "_process", json_object_new_string(processCommand.c_str()));
        json_object_object_add(processItem, "_duration", json_object_new_int64(process.getDuration()));

        json_object_array_add(processList, processItem);
    }

    json_object_object_add(jsonObj, "_processList", processList);

    const char *json_str = json_object_to_json_string(jsonObj);

    sendStr(conn, json_str);

    json_object_put(jsonObj);

    return true;
}
