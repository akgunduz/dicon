//
// Created by Haluk AKGUNDUZ on 27.06.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#include <ui/UserInterfaceEvent.h>
#include <Log.h>
#include <NodeState.h>
#include "WebApp.h"

#define REST_DIST_URI REST_URI DIST_URI
#define REST_DIST_POLL REST_DIST_URI "/poll"

void WebApp::distInit() {

    uiUpdater[UI_UPDATE_DIST_COLL_LIST] = &WebApp::distAddtoCollectorList;
    uiUpdater[UI_UPDATE_DIST_NODE_LIST] = &WebApp::distAddtoNodeList;
}

int WebApp::distHandler(struct mg_connection *conn, const char * uri) {

    const struct mg_request_info *ri = mg_get_request_info(conn);

    if (0 == strcmp(ri->request_method, "GET")) {

        if (0 == strcmp(uri, "/poll")) {

            componentController->getDistributor()->sendWakeupMessagesAll(false);

            mg_send_http_ok(conn, "application/json; charset=utf-8", 0);

            return 1;
        }
    }

    return 0;
}

void WebApp::distAddtoCollectorList(WebEvent &event) {

    long i = 0;

    auto *data = (UserInterfaceEvent *)event.GetClientData();

    if (data->getData(1) > 0) {

        LOG_S("Console UI ------> Distributor Collector Updated : Collector[%d], Assigned Node[%d]",
              data->getData(0), data->getData(1));
    } else {

        LOG_S("Console UI ------> Distributor Collector Updated : Collector[%d], No Assigned Node",
              data->getData(0));
    }

}

void WebApp::distAddtoNodeList(WebEvent &event) {

    long i = 0;

    auto *data = (UserInterfaceEvent *)event.GetClientData();

    LOG_S("Console UI ------> Distributor Node Updated : Node[%d], State : %s",
          data->getData(0), NodeState::getName((NODE_STATES)data->getData(1)));

}
