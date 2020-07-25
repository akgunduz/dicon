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
#define HOSTING "http://localhost:" PORT "/"
#define REST_URI "/rest"
#define EVENT_URI "/event"

#define DIST_URI "/dist"
#define COLL_URI "/coll"
#define NODE_URI "/node"

class WebApp : public App {

    struct mg_callbacks callbacks{};
    struct mg_context *context;
    bool notifyFlag[COMP_MAX]{};
    std::mutex notifyMutex;

public:

    WebApp(int *, LOGLEVEL*, int*);

    int restHandler(struct mg_connection *conn);
    int mainHandler(struct mg_connection *conn);
    int eventHandler(struct mg_connection *conn);

    bool distHandler(struct mg_connection *conn, const char * uri);
    bool distPollHandler(struct mg_connection *conn);
    bool distStateHandler(struct mg_connection *conn);

    bool collHandler(struct mg_connection *conn, const char * uri);
    bool collStateHandler(struct mg_connection *conn, int id);
    bool collLoadJobHandler(struct mg_connection *conn, int id, const char*);
    bool collProcessHandler(struct mg_connection *conn, int id);

    bool nodeHandler(struct mg_connection *conn, const char * uri);
    bool nodeStateHandler(struct mg_connection *conn, int id);

    int run() override;
    int notifyHandler(COMPONENT, long) override;
    bool sendServerEvent(struct mg_connection *conn, int id);
};

void sendHtml(struct mg_connection *conn);

#endif //DICON_WEBAPP_H
