//
// Created by Haluk AKGUNDUZ on 23.06.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#include <NetUtil.h>
#include "WebApp.h"
#include "WebOption.h"

#define MAIN_PAGE DOCUMENT_ROOT "./index.html"

int WebApp::mainHandler(struct mg_connection *conn)
{
#ifdef NDEBUG
    sendHtml(conn);
#else
    mg_send_mime_file(conn, MAIN_PAGE, nullptr);
#endif
    return 1;
}

int WebApp::eventHandler(struct mg_connection *conn)
{
    mg_printf(conn,
              "HTTP/1.1 200 OK\r\n"
              "Cache-Control: no-cache\r\n"
              "Content-Type: text/event-stream\r\n"
              "Connection: Keep-Alive\r\n\r\n"
    );

    int loopCount = 5;

    NOTIFYTYPE notifyState[COMP_MAX] = {NOTIFYTYPE_PASSIVE, NOTIFYTYPE_PASSIVE, NOTIFYTYPE_PASSIVE};

    do {

        notifyMutex.lock();

        if (notifyData[COMP_DISTRIBUTOR] == NOTIFYTYPE_ONCE) {
            notifyState[COMP_DISTRIBUTOR] = NOTIFYTYPE_ONCE;
            notifyData[COMP_DISTRIBUTOR] = NOTIFYTYPE_PASSIVE;
        }

        if (notifyData[COMP_COLLECTOR] == NOTIFYTYPE_ACTIVE) {
            notifyState[COMP_COLLECTOR] = NOTIFYTYPE_ACTIVE;
            notifyState[COMP_NODE] = NOTIFYTYPE_ACTIVE;
            loopCount = 5;

        } else if (notifyData[COMP_COLLECTOR] == NOTIFYTYPE_ONCE) {
            notifyState[COMP_COLLECTOR] = NOTIFYTYPE_ONCE;
            notifyData[COMP_COLLECTOR] = NOTIFYTYPE_PASSIVE;
        }

        notifyMutex.unlock();

        mg_printf(conn, "data: {\"dist\" : %d, \"coll\" : %d, \"node\" : %d}\r\n\r\n",
                  notifyState[COMP_DISTRIBUTOR], notifyState[COMP_COLLECTOR], notifyState[COMP_NODE]);

        std::this_thread::sleep_for(std::chrono::seconds(1));

    } while(loopCount--);

    return 1;
}

bool WebApp::sendServerEvent(struct mg_connection *conn, int source) {

    mg_printf(conn,
              "HTTP/1.1 200 OK\r\n"
              "Cache-Control: no-cache\r\n"
              "Content-Type: text/event-stream\r\n"
              "Connection: Keep-Alive\r\n\r\n"
    );

    mg_printf(conn,"data: %d\r\n\r\n", source);

    return true;
}

int WebApp::restHandler(struct mg_connection *conn) {

    const struct mg_request_info *ri = mg_get_request_info(conn);

    int restLen = strlen(REST_URI);
    if (0 != strncmp(ri->request_uri, REST_URI, restLen)) {
        LOGP_E("Wrong REST Call has came!!!");
        return 0;
    }

    const char *pos = ri->request_uri + restLen;

    int len = strlen(DIST_URI);
    if (0 == strncmp(pos, DIST_URI, len)) {
        return distHandler(conn, pos + len);
    }

    len = strlen(COLL_URI);
    if (0 == strncmp(pos, COLL_URI, len)) {
        return collHandler(conn, pos + len);
    }

    len = strlen(NODE_URI);
    if (0 == strncmp(pos, NODE_URI, len)) {
        return nodeHandler(conn, pos + len);
    }

    len = strlen(QUIT_URI);
    if (0 == strncmp(pos, QUIT_URI, len)) {
        return quitHandler(conn);
    }

    return 0;
}

int WebApp::quitHandler(struct mg_connection *conn) {

    runLoop = false;

    return true;
}

WebApp::WebApp(int *interfaceID, const LogInfo& _logInfo, std::vector<int>& componentCount)
    : App(APPTYPE_WEB, interfaceID, _logInfo, componentCount, true) {


    if (!initialized) {
        return;
    }

    WebOption options;
    options.setOption("enable_keep_alive", "yes");
    options.setOption("request_timeout_ms", 10000);
    options.setOption("additional_header", "Access-Control-Allow-Origin: *");

    memset(&callbacks, 0, sizeof(callbacks));

    int tryCount = 10;

    for (int j = tryCount; j > 0; j--) {

        options.setOption("listening_ports", webPort);

        context = mg_start(&callbacks, this, options.getOptions());
        if (context) {
            break;
        }

        webPort++;
    }

    if (!context) {
        LOGP_E("Can not start web server....");
        return;
    }

    mg_set_request_handler(context, REST_URI, [](struct mg_connection *conn, void *cbData) -> int {
        return ((WebApp*) cbData)->restHandler(conn);
    }, this);

    if (componentCount[COMP_DISTRIBUTOR]) {

        mg_set_request_handler(context, MAIN_URI, [](struct mg_connection *conn, void *cbData) -> int {
            return ((WebApp *) cbData)->mainHandler(conn);
        }, this);

        mg_set_request_handler(context, EVENT_URI, [](struct mg_connection *conn, void *cbData) -> int {
            return ((WebApp *) cbData)->eventHandler(conn);
        }, this);


        componentController->getDistributor()->getHost()->setAllUIAddress(webPort);

        LOGP_I("Link : http://localhost:%d", webPort);
    }

    if (componentController->getCollectorCount()) {

        for (auto &coll : componentController->getCollectors()) {

            coll->getHost()->setAllUIAddress(webPort);
        }
    }

    if (componentController->getNodeCount()) {

        for (auto &node : componentController->getNodes()) {

            node->getHost()->setAllUIAddress(webPort);
        }
    }
}

WebApp::~WebApp() {

    LOGP_T("Deallocating WebApp");

    if (!initialized) {
        return;
    }

    mg_stop(context);

}

int WebApp::run() {

    while (runLoop) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    LOGP_I("WebApp Exiting....");

    return EXIT_SUCCESS;
}

int WebApp::notifyHandler(COMPONENT target, NOTIFYTYPE state) {

    notifyMutex.lock();

    if (state != NOTIFYTYPE_TRANSPARENT) {

        notifyData[target] = state;
    }

    notifyMutex.unlock();

    return true;
}
