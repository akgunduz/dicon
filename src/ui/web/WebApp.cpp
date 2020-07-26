//
// Created by Haluk AKGUNDUZ on 23.06.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#include "WebApp.h"

volatile int exitNow = 0;

const char *options[] = {
#if !defined(NO_FILES)
        "document_root",
        DOCUMENT_ROOT,
#endif
        "listening_ports",
        PORT,
        "enable_keep_alive",
        "yes",
        "request_timeout_ms",
        "10000",
        "error_log_file",
        "error.log",
        "enable_auth_domain_check",
        "no",
        nullptr
};

#define MAIN_PAGE DOCUMENT_ROOT "./index.html"

int mainHandlerWrapper(struct mg_connection *conn, void *cbData)
{
    return ((WebApp*) cbData)->mainHandler(conn);
}

int WebApp::mainHandler(struct mg_connection *conn)
{
    sendHtml(conn);
    //mg_send_mime_file(conn, MAIN_PAGE, nullptr);
    return 1;
}

int eventHandlerWrapper(struct mg_connection *conn, void *cbData)
{
    return ((WebApp*) cbData)->eventHandler(conn);
}

int WebApp::eventHandler(struct mg_connection *conn)
{
    uint32_t mask = 0;

    notifyMutex.lock();

    if (notifyFlag[COMP_DISTRIBUTOR]) {
        notifyFlag[COMP_DISTRIBUTOR] = false;
        mask |= (uint32_t)1 << COMP_DISTRIBUTOR;
    }

    if (notifyFlag[COMP_COLLECTOR]) {
        notifyFlag[COMP_COLLECTOR] = false;
        mask |= (uint32_t)1 << COMP_COLLECTOR;
    }

    if (notifyFlag[COMP_NODE]) {
        notifyFlag[COMP_NODE] = false;
        mask |= (uint32_t)1 << COMP_NODE;
    }

    notifyMutex.unlock();

    if (mask) {
        sendServerEvent(conn, mask);
    }

    return 1;
}

bool WebApp::sendServerEvent(struct mg_connection *conn, int id) {

    mg_printf(conn,
              "HTTP/1.1 200 OK\r\n"
              "Cache-Control: no-cache\r\n"
              "Content-Type: text/event-stream\r\n"
              "Connection: Keep-Alive\r\n\r\n"
    );

    mg_printf(conn,"data: %d\r\n\r\n", id);

    return true;
}

int restHandlerWrapper(struct mg_connection *conn, void *cbData) {

    return ((WebApp*) cbData)->restHandler(conn);
}

int WebApp::restHandler(struct mg_connection *conn) {

    const struct mg_request_info *ri = mg_get_request_info(conn);

    int restLen = strlen(REST_URI);
    if (0 != strncmp(ri->request_uri, REST_URI, restLen)) {
        PRINT("Wrong REST Call has came!!!");
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

    return 0;
}

WebApp::WebApp(int *interfaceID, LOGLEVEL* logLevel, std::vector<int>& componentCount)
    : App(APPTYPE_WEB, interfaceID, logLevel, componentCount) {

    /* Start CivetWeb web server */
    memset(&callbacks, 0, sizeof(callbacks));

    context = mg_start(&callbacks, 0, options);
    if (!context) {
        PRINT("Can not start server....");
        return;
    }

    mg_set_request_handler(context, MAIN_URI, mainHandlerWrapper, this);
    mg_set_request_handler(context, REST_URI, restHandlerWrapper, this);
    mg_set_request_handler(context, EVENT_URI, eventHandlerWrapper, this);

    PRINT("Link : %s", HOSTING);
}

int WebApp::run() {

    while (!exitNow) {
        sleep(1);
    }

    /* Stop the server */
    mg_stop(context);
    PRINT("Exiting....");
    return 1;
}

int WebApp::notifyHandler(COMPONENT target, long id) {

    notifyMutex.lock();

    notifyFlag[target] = true;

    notifyMutex.unlock();

    return true;
}

