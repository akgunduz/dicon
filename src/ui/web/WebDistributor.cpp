//
// Created by Haluk AKGUNDUZ on 27.06.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#include <CollectorUnit.h>
#include <NodeUnit.h>
#include <NetUtil.h>
#include "WebApp.h"
#include "WebOption.h"

bool WebApp::distHandler(struct mg_connection *conn, const char * uri) {

    const struct mg_request_info *ri = mg_get_request_info(conn);

    if (0 == strcmp(ri->request_method, "GET")) {

        if (0 == strcmp(uri, "/state")) {

            return distStateHandler(conn);
        }

        if (0 == strcmp(uri, "/poll")) {

            return distPollHandler(conn);
        }

        if (0 == strcmp(uri, "/addcoll")) {

            return distAddCollHandler(conn);
        }

        if (0 == strcmp(uri, "/addnode")) {

            return distAddNodeHandler(conn);
        }
    }

    return false;
}

bool WebApp::distPollHandler(struct mg_connection *conn) {

    auto &distributor = componentController->getDistributor();

    distributor->sendWakeupMessagesAll(false);

    sendOK(distributor->getHost(), conn, "Polling is Executed");

    return true;
}

bool WebApp::distAddCollHandler(struct mg_connection *conn) {

    auto &distributor = componentController->getDistributor();

    auto &collector = componentController->startCollector();

    collector->getHost()->setAllUIAddress(webPort);

    sendOK(distributor->getHost(), conn, "New Collector is Added");

    return true;
}

bool WebApp::distAddNodeHandler(struct mg_connection *conn) {

    auto &distributor = componentController->getDistributor();

    auto &node = componentController->startNode();

    node->getHost()->setAllUIAddress(webPort);

    sendOK(distributor->getHost(), conn, "New Node is Added");

    return true;
}

bool WebApp::distStateHandler(struct mg_connection *conn) {

    auto &distributor = componentController->getDistributor();
    if (!distributor) {
        sendError(nullptr, conn, "Can not find the distributor !!!");
        return false;
    }

    auto* jsonObj = json_object_new_object();
    if (jsonObj == nullptr) {
        sendError(distributor->getHost(), conn, "Can not create json object!!!");
        return false;
    }

    auto* collList = json_object_new_array();

    for (auto& collectorPair : componentController->getDistributor()->getCollectors()->get()) {

        auto collector = std::static_pointer_cast<CollectorUnit>(collectorPair.second);

        auto *collItem = json_object_new_object();
        json_object_object_add(collItem, "_id", json_object_new_int(collector->getID()));
        json_object_object_add(collItem, "_arch", json_object_new_int(collector->getArch()));
        json_object_object_add(collItem, "_state", json_object_new_int(collector->getState()));
        json_object_object_add(collItem, "_address",
                               json_object_new_string(NetUtil::getIPPortString(collector->getAddress().get()).c_str()));
        json_object_object_add(collItem, "_webAddress",
                               json_object_new_string(NetUtil::getIPPortString(collector->getAddress().getUI()).c_str()));

        json_object_array_add(collList, collItem);
    }

    json_object_object_add(jsonObj, "_collList", collList);

    auto* nodeList = json_object_new_array();

    for (auto& nodePair : componentController->getDistributor()->getNodes()->get()) {

        auto node = std::static_pointer_cast<NodeUnit>(nodePair.second);

        auto *nodeItem = json_object_new_object();
        json_object_object_add(nodeItem, "_id", json_object_new_int(node->getID()));
        json_object_object_add(nodeItem, "_arch", json_object_new_int(node->getArch()));
        json_object_object_add(nodeItem, "_state", json_object_new_int(node->getState()));
        json_object_object_add(nodeItem, "_address",
                               json_object_new_string(NetUtil::getIPPortString(node->getAddress().get()).c_str()));
        json_object_object_add(nodeItem, "_webAddress",
                               json_object_new_string(NetUtil::getIPPortString(node->getAddress().getUI()).c_str()));

        json_object_array_add(nodeList, nodeItem);
    }

    json_object_object_add(jsonObj, "_nodeList", nodeList);

    const char *json_str = json_object_to_json_string(jsonObj);

    sendStr(conn, json_str);

    json_object_put(jsonObj);

    return true;
}
