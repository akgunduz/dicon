//
// Created by Haluk AKGUNDUZ on 23.06.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_WEBAPP_H
#define DICON_WEBAPP_H

#include "civetweb/civetweb.h"
#include "Application.h"

#define DOCUMENT_ROOT "../data/html/"
#define PORT "8081"
#define MAIN_URI "/"
#define EXIT_URI "/exit"
#define HOSTING "http://localhost:" PORT "/"
#define REST_URI "/rest"
#define WS_URI "/ws"

#define DIST_URI "/dist"
#define COLL_URI "/coll"
#define NODE_URI "/node"

enum WSSTATE {
    WSSTATE_NOTCONNECTED,
    WSSTATE_CONNECTED,
    WSSTATE_READY,
    WSSTATE_PROCESS,
    WSSTATE_MAX
};

struct ws_client {
    struct mg_connection *conn;
    enum WSSTATE state;
};

class WebApp : public App {

    struct mg_callbacks callbacks;
    struct mg_context *context;
    struct ws_client wsClients[MAX_UI_CB];

public:
    WebApp(int argc, char** argv, int *interfaceID,
                        LOGLEVEL* logLevel, bool enableDistributor, int* collInfo, int* nodeInfo);

    int restHandler(struct mg_connection *conn);
    int mainHandler(struct mg_connection *conn);
    int exitHandler(struct mg_connection *conn);
    int wsHandler(struct mg_connection *conn);
    int wsConnectHandler(const struct mg_connection *conn);
    void wsReadyHandler(struct mg_connection *conn);
    int wsDataHandler(struct mg_connection *conn, int bits, char *data, size_t len);
    void wsCloseHandler(const struct mg_connection *conn);

    bool distHandler(struct mg_connection *conn, const char * uri);
    bool distPollHandler(struct mg_connection *conn);
    bool distStateHandler(struct mg_connection *conn);

    bool collHandler(struct mg_connection *conn, const char * uri);
    bool collStateHandler(struct mg_connection *conn, int id);
    bool collLoadJobHandler(struct mg_connection *conn, int id);
    bool collProcessHandler(struct mg_connection *conn, int id);

    bool nodeHandler(struct mg_connection *conn, const char * uri);
    bool nodeStateHandler(struct mg_connection *conn, int id);

    int run() override;
    int notifyHandler(int, int) override;
};

#endif //DICON_WEBAPP_H
