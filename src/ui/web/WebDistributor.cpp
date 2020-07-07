//
// Created by Haluk AKGUNDUZ on 27.06.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#include <ui/UserInterfaceEvent.h>
#include <Log.h>
#include <NodeState.h>
#include <CollectorObject.h>
#include "WebApp.h"

void WebApp::distInit() {

    uiUpdater[UI_UPDATE_DIST] = &WebApp::distUpdate;
}

bool WebApp::distHandler(struct mg_connection *conn, const char * uri) {

    const struct mg_request_info *ri = mg_get_request_info(conn);

    if (0 == strcmp(ri->request_method, "GET")) {

        if (0 == strcmp(uri, "/state")) {

            return distStateHandler(conn);
        }

        if (0 == strcmp(uri, "/poll")) {

            return distPollHandler(conn);
        }
    }

    return false;
}

bool WebApp::distPollHandler(struct mg_connection *conn) {

    componentController->getDistributor()->sendWakeupMessagesAll(false);

    mg_send_http_ok(conn, "application/json; charset=utf-8", 0);

    return true;
}

bool WebApp::distStateHandler(struct mg_connection *conn) {

    auto* jsonObj = json_object_new_object();
    if (jsonObj == nullptr) {
        LOG_S("Can not create json object!!!");
        return false;
    }

    auto* collList = json_object_new_array();

    for (int i = 0 ; i < componentController->getDistributor()->getCollectors()->size(); i++) {

        auto *collector = componentController->getDistributor()->getCollectors()->getByIndex(i);
        auto* collItem = json_object_new_object();
        json_object_object_add(collItem, "collectorID", json_object_new_int(collector->getID()));
        json_object_object_add(collItem, "assignedNode", json_object_new_int(((CollectorObject*)collector)->getAssigned().getID()));

        json_object_array_add(collList, collItem);
    }

    json_object_object_add(jsonObj, "collList", collList);

    auto* nodeList = json_object_new_array();

    for (int i = 0 ; i < componentController->getDistributor()->getNodes()->size(); i++) {

        auto *node = componentController->getDistributor()->getNodes()->getByIndex(i);
        auto* nodeItem = json_object_new_object();
        json_object_object_add(nodeItem, "nodeID", json_object_new_int(node->getID()));
        json_object_object_add(nodeItem, "state", json_object_new_int(((NodeObject*)node)->getState()));

        json_object_array_add(nodeList, nodeItem);
    }

    json_object_object_add(jsonObj, "nodeList", nodeList);

    size_t json_str_len;

    const char *json_str = json_object_to_json_string_length(jsonObj, JSON_C_TO_STRING_SPACED, &json_str_len);

    mg_send_http_ok(conn, "application/json; charset=utf-8", json_str_len);

    mg_write(conn, json_str, json_str_len);

    json_object_put(jsonObj);

    return true;
}

void WebApp::distUpdate(WebEvent &event) {

    Timer::set("dist", 1000, WebApp::wsInform, this);
}


