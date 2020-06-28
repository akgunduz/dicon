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
    //    return collHandler(conn, pos + len);
    }

    len = strlen(NODE_URI);
    if (0 == strncmp(pos, NODE_URI, len)) {
     //   return nodeHandler(conn, pos + len);
    }

    return 0;
}

int wsHandlerWrapper(struct mg_connection *conn, void *cbData) {

    return ((WebApp*) cbData)->wsHandler(conn);
}

int WebApp::wsHandler(struct mg_connection *conn) {

    return 1;
}

int wsConnectHandlerWrapper(const struct mg_connection *conn, void *cbData) {

    return ((WebApp*) cbData)->wsConnectHandler(conn);
}

int WebApp::wsConnectHandler(const struct mg_connection *conn)
{
    struct mg_context *ctx = mg_get_context(conn);
    int reject = 1;
    int i;

    mg_lock_context(ctx);
    for (i = 0; i < MAX_WS_CLIENTS; i++) {
        if (ws_clients[i].conn == nullptr) {
            ws_clients[i].conn = (struct mg_connection *)conn;
            ws_clients[i].state = 1;
            mg_set_user_connection_data(ws_clients[i].conn,(void *)(ws_clients + i));
            reject = 0;
            break;
        }
    }
    mg_unlock_context(ctx);

    fprintf(stdout,
            "Websocket client %s\r\n\r\n",
            (reject ? "rejected" : "accepted"));
    return reject;
}

void wsReadyHandlerWrapper(struct mg_connection *conn, void *cbData)
{
    ((WebApp*) cbData)->wsReadyHandler(conn);
}

void WebApp::wsReadyHandler(struct mg_connection *conn)
{
    const char *text = "Hello from the websocket ready handler";
    struct ws_client *client = (struct ws_client *) mg_get_user_connection_data(conn);

    mg_websocket_write(conn, MG_WEBSOCKET_OPCODE_TEXT, text, strlen(text));
    fprintf(stdout, "Greeting message sent to websocket client\r\n\r\n");
    assert(client->conn == conn);
    assert(client->state == 1);

    client->state = 2;
}

int wsDataHandlerWrapper(struct mg_connection *conn, int bits, char *data, size_t len, void *cbData)
{
    return ((WebApp*) cbData)->wsDataHandler(conn, bits, data, len);
}

int WebApp::wsDataHandler(struct mg_connection *conn, int bits, char *data, size_t len)
{
    struct ws_client *client = (struct ws_client *) mg_get_user_connection_data(conn);
    assert(client->conn == conn);
    assert(client->state >= 1);

    fprintf(stdout, "Websocket got %lu bytes of ", (unsigned long)len);
    switch (((unsigned char)bits) & 0x0F) {
        case MG_WEBSOCKET_OPCODE_CONTINUATION:
            fprintf(stdout, "continuation");
            break;
        case MG_WEBSOCKET_OPCODE_TEXT:
            fprintf(stdout, "text");
            break;
        case MG_WEBSOCKET_OPCODE_BINARY:
            fprintf(stdout, "binary");
            break;
        case MG_WEBSOCKET_OPCODE_CONNECTION_CLOSE:
            fprintf(stdout, "close");
            break;
        case MG_WEBSOCKET_OPCODE_PING:
            fprintf(stdout, "ping");
            break;
        case MG_WEBSOCKET_OPCODE_PONG:
            fprintf(stdout, "pong");
            break;
        default:
            fprintf(stdout, "unknown(%1xh)", ((unsigned char)bits) & 0x0F);
            break;
    }
    fprintf(stdout, " data:\r\n");
    fwrite(data, len, 1, stdout);
    fprintf(stdout, "\r\n\r\n");

    return 1;
}

void wsCloseHandlerWrapper(const struct mg_connection *conn, void *cbData)
{
    ((WebApp*) cbData)->wsCloseHandler(conn);
}

void WebApp::wsCloseHandler(const struct mg_connection *conn)
{
    struct mg_context *ctx = mg_get_context(conn);
    struct ws_client *client = (struct ws_client *) mg_get_user_connection_data(conn);
    assert(client->conn == conn);
    assert(client->state >= 1);

    mg_lock_context(ctx);
    while (client->state == 3) {
        /* "inform" state, wait a while */
        mg_unlock_context(ctx);
        usleep(1000);
        mg_lock_context(ctx);
    }
    client->state = 0;
    client->conn = NULL;
    mg_unlock_context(ctx);

    fprintf(stdout,
            "Client dropped from the set of webserver connections\r\n\r\n");
}



void WebApp::wsInform()
{
    static unsigned long cnt = 0;
    char text[32];
    size_t textlen;
    int i;

    sprintf(text, "%lu", ++cnt);
    textlen = strlen(text);

    for (i = 0; i < MAX_WS_CLIENTS; i++) {
        int inform = 0;

        mg_lock_context(context);
        if (ws_clients[i].state == 2) {
            /* move to "inform" state */
            ws_clients[i].state = 3;
            inform = 1;
        }
        mg_unlock_context(context);

        if (inform) {
            mg_websocket_write(ws_clients[i].conn,
                               MG_WEBSOCKET_OPCODE_TEXT,
                               text,
                               textlen);
            mg_lock_context(context);
            ws_clients[i].state = 2;
            mg_unlock_context(context);
        }
    }
}

WebApp::WebApp(int argc, char** argv, int *interfaceID,
                       LOGLEVEL* logLevel, int* distCount, int* collInfo, int* nodeInfo)
    : UserInterfaceApp(APPTYPE_WEB, argc, argv, interfaceID, logLevel,
                       distCount, collInfo, nodeInfo) {

    Log::registerUIController(this, updateUICallback);

    /* Start CivetWeb web server */
    memset(&callbacks, 0, sizeof(callbacks));

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
        wsInform();
    }

    /* Stop the server */
    mg_stop(context);
    LOG_S("Exiting....");
    return 1;
}
