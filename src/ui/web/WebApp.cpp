//
// Created by Haluk AKGUNDUZ on 23.06.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#include <Log.h>
#include "WebApp.h"

volatile int exitNow = 0;

const char *options[] = {
#if !defined(NO_FILES)
        "document_root",
        DOCUMENT_ROOT,
#endif
        "listening_ports",
        PORT,
        "request_timeout_ms",
        "10000",
        "error_log_file",
        "error.log",
        "enable_auth_domain_check",
        "no",
        nullptr
};

#define MAIN_PAGE DOCUMENT_ROOT "./index.html"



//int postHandler(struct mg_connection *conn, ComponentController *controller) {
//
//    char buffer[1024];
//    int len = mg_read(conn, buffer, sizeof(buffer) - 1);
//    if ((len < 1) || (len >= sizeof(buffer))) {
//        mg_send_http_error(conn, 400, "%s", "No request body data");
//        return 400;
//    }
//    buffer[len] = 0;
//}

int mainHandlerWrapper(struct mg_connection *conn, void *cbData)
{
    return ((WebApp*) cbData)->mainHandler(conn);
}

int WebApp::mainHandler(struct mg_connection *conn)
{
    mg_send_mime_file(conn, MAIN_PAGE, nullptr);
    return 1;
}

int exitHandlerWrapper(struct mg_connection *conn, void *cbData)
{
    return ((WebApp*) cbData)->exitHandler(conn);
}

int WebApp::exitHandler(struct mg_connection *conn)
{
    exitNow = 1;
    return 1;
}

int restHandlerWrapper(struct mg_connection *conn, void *cbData) {

    return ((WebApp*) cbData)->restHandler(conn);
}

int WebApp::restHandler(struct mg_connection *conn) {

    const struct mg_request_info *ri = mg_get_request_info(conn);

    int restLen = strlen(REST_URI);
    if (0 != strncmp(ri->request_uri, REST_URI, restLen)) {
        LOG_S("Wrong REST Call has came!!!");
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

int wsHandlerWrapper(struct mg_connection *conn, void *cbData) {

    return ((WebApp*) cbData)->wsHandler(conn);
}

int WebApp::wsHandler(struct mg_connection *conn) {

    //Note to myself: if /ws is needed use this handler, seems not needed now;
    LOG_S("WS Handler is executed....");
    return 1;
}

int wsConnectHandlerWrapper(const struct mg_connection *conn, void *cbData) {

    return ((WebApp*) cbData)->wsConnectHandler(conn);
}

int WebApp::wsConnectHandler(const struct mg_connection *conn)
{
    struct mg_context *ctx = mg_get_context(conn);
    int accepted = 0;

    mg_lock_context(ctx);
    for (int i = 0; i < MAX_UI_CB; i++) {
        if (wsClients[i].conn == nullptr) {
            wsClients[i].conn = (struct mg_connection *)conn;
            wsClients[i].state = WSSTATE_CONNECTED;
            mg_set_user_connection_data(wsClients[i].conn,(void *)(wsClients + i));
            accepted = 1;
            break;
        }
    }
    mg_unlock_context(ctx);

    const struct mg_request_info *ri = mg_get_request_info(conn);
    LOG_S("WS Client is %s from %s:%d", accepted ? "accepted" : "rejected", ri->remote_addr, ri->remote_port);

    return !accepted;
}

void wsReadyHandlerWrapper(struct mg_connection *conn, void *cbData)
{
    ((WebApp*) cbData)->wsReadyHandler(conn);
}

void WebApp::wsReadyHandler(struct mg_connection *conn)
{
    auto *client = (struct ws_client *) mg_get_user_connection_data(conn);
    client->state = WSSTATE_READY;
}

int wsDataHandlerWrapper(struct mg_connection *conn, int bits, char *data, size_t len, void *cbData)
{
    return ((WebApp*) cbData)->wsDataHandler(conn, bits, data, len);
}

int WebApp::wsDataHandler(struct mg_connection *conn, int bits, char *data, size_t len) {

    auto *client = (struct ws_client *) mg_get_user_connection_data(conn);

    LOG_S("WS Client send data : %s ", Util::hex2str((uint8_t*)data, len).c_str());

    return 1;
}

void wsCloseHandlerWrapper(const struct mg_connection *conn, void *cbData)
{
    ((WebApp*) cbData)->wsCloseHandler(conn);
}

void WebApp::wsCloseHandler(const struct mg_connection *conn)
{
    struct mg_context *ctx = mg_get_context(conn);
    auto *client = (struct ws_client *) mg_get_user_connection_data(conn);

    mg_lock_context(ctx);
    while (client->state == WSSTATE_PROCESS) {
        mg_unlock_context(ctx);
        usleep(1000);
        mg_lock_context(ctx);
    }
    client->state = WSSTATE_NOTCONNECTED;
    client->conn = nullptr;
    mg_unlock_context(ctx);

    const struct mg_request_info *ri = mg_get_request_info(conn);
    LOG_S("WS Client at %s:%d is disconnected", ri->remote_addr, ri->remote_port);
}

bool WebApp::wsInform(void *data, const char* data2)
{
    WebApp* webApp = (WebApp*) data;

    for (auto & wsClient : webApp->wsClients) {

        bool process = false;

        mg_lock_context(webApp->context);
        if (wsClient.state == WSSTATE_READY) {
            wsClient.state = WSSTATE_PROCESS;
            process = true;
        }
        mg_unlock_context(webApp->context);

        if (process) {
            mg_websocket_write(wsClient.conn, MG_WEBSOCKET_OPCODE_TEXT, data2, strlen(data2));
            mg_lock_context(webApp->context);
            wsClient.state = WSSTATE_READY;
            mg_unlock_context(webApp->context);
        }
    }

    return true;
}

WebApp::WebApp(int argc, char** argv, int *interfaceID,
                       LOGLEVEL* logLevel, int* distCount, int* collInfo, int* nodeInfo)
    : UserInterfaceApp(APPTYPE_WEB, argc, argv, interfaceID, logLevel,
                       distCount, collInfo, nodeInfo) {

    Log::registerUIController(this, updateUICallback);

    /* Start CivetWeb web server */
    memset(&callbacks, 0, sizeof(callbacks));
    memset(&wsClients, 0, sizeof(wsClients));

    context = mg_start(&callbacks, 0, options);
    if (!context) {
        LOG_S("Can not start server....");
        return;
    }

    mg_set_request_handler(context, MAIN_URI, mainHandlerWrapper, this);
    mg_set_request_handler(context, REST_URI, restHandlerWrapper, this);
    mg_set_request_handler(context, EXIT_URI, exitHandlerWrapper, this);
    mg_set_request_handler(context, WS_URI, wsHandlerWrapper, this);

    mg_set_websocket_handler(context,
                             WS_URI,
                             wsConnectHandlerWrapper,
                             wsReadyHandlerWrapper,
                             wsDataHandlerWrapper,
                             wsCloseHandlerWrapper,
                             this);

    LOG_S("Link : %s", HOSTING);

    distInit();
    collInit();
    nodeInit();
}

void WebApp::updateUICallback(void *context, int id, void *data) {

    ((WebApp*) context)->updateUIEvent(id, data);
}

void WebApp::updateUIEvent(int id, void *data) {

    WebEvent event;
    event.SetId(id);
    event.SetClientData(data);

    //On real UI architectures this updateUI should be asynchronous like in WX
    updateUI(event);
}

void WebApp::updateUI(WebEvent& event) {

    int id = event.GetId();

    if (uiUpdater[id] != nullptr) {
        ((this)->*(uiUpdater[id]))(event);
    }
}

int WebApp::run() {

    while (!exitNow) {
        sleep(1);
    }

    /* Stop the server */
    mg_stop(context);
    LOG_S("Exiting....");
    return 1;
}

