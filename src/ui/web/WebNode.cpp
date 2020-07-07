//
// Created by Haluk Akgunduz on 02/09/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include <Log.h>
#include <NodeState.h>
#include "WebApp.h"

void WebApp::nodeInit() {

    uiUpdater[UI_UPDATE_NODE] = &WebApp::nodeUpdate;
}

bool WebApp::nodeHandler(struct mg_connection *conn, const char * uri) {

    const struct mg_request_info *ri = mg_get_request_info(conn);

    if (0 == strcmp(ri->request_method, "GET")) {

        char *nextStr;

        long nodeID = strtol(uri + 1, &nextStr, 10);

        if (0 == strcmp(nextStr, "/state")) {

            return nodeStateHandler(conn, nodeID);
        }
    }

    return 0;
}

bool WebApp::nodeStateHandler(struct mg_connection *conn, long id) {

    auto *node = componentController->getNode(id);

    if (!node) {
        LOG_S("Can not find the node with ID : %d !!!", id);
        return false;
    }

    auto nodeObj = (NodeObject&) node->getHost();

    auto* jsonObj = json_object_new_object();
    if (jsonObj == nullptr) {
        LOG_S("Can not create json object!!!");
        return false;
    }

    json_object_object_add(jsonObj, "state", json_object_new_int(nodeObj.getState()));

    auto* processList = json_object_new_array();
    for (auto &process : node->getProcessList()) {

        auto* processItem = json_object_new_object();
        json_object_object_add(processItem, "collectorID", json_object_new_int(process.getAssigned()));
        json_object_object_add(processItem, "jobID", json_object_new_string(process.getJobID().c_str()));
        json_object_object_add(processItem, "processID", json_object_new_int(process.getID()));

        json_object_array_add(processList, processItem);
    }

    json_object_object_add(jsonObj, "processList", processList);


    size_t json_str_len;

    const char *json_str = json_object_to_json_string_length(jsonObj, JSON_C_TO_STRING_SPACED, &json_str_len);

    mg_send_http_ok(conn, "application/json; charset=utf-8", json_str_len);

    mg_write(conn, json_str, json_str_len);

    json_object_put(jsonObj);

    return true;
}

void WebApp::nodeUpdate(WebEvent& event) {

    Timer::set("node", 1000, WebApp::wsInform, this);
}
