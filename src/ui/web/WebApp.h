//
// Created by Haluk AKGUNDUZ on 23.06.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_WEBAPP_H
#define DICON_WEBAPP_H

#include "Common.h"
#include "civetweb.h"
#include "WebEvent.h"
#include "UserInterfaceApp.h"

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

class WebApp;

typedef void (WebApp::*fWebUpdater)(WebEvent &event);

class WebApp : public UserInterfaceApp {

    struct mg_callbacks callbacks;
    struct mg_context *context;
    struct ws_client wsClients[MAX_UI_CB];

    fWebUpdater uiUpdater[MAX_UI_CB];

public:
    WebApp(int argc, char** argv, int *interfaceID,
                        LOGLEVEL* logLevel, int* distCount, int* collInfo, int* nodeInfo);

    static void updateUICallback(void*, int, void*);
	void updateUIEvent(int, void*);
    void updateUI(WebEvent& event);

    int restHandler(struct mg_connection *conn);
    int mainHandler(struct mg_connection *conn);
    int exitHandler(struct mg_connection *conn);
    int wsHandler(struct mg_connection *conn);
    int wsConnectHandler(const struct mg_connection *conn);
    void wsReadyHandler(struct mg_connection *conn);
    int wsDataHandler(struct mg_connection *conn, int bits, char *data, size_t len);
    void wsCloseHandler(const struct mg_connection *conn);
    static bool wsInform(void*, const char*);

    void distInit();
    bool distHandler(struct mg_connection *conn, const char * uri);
    bool distPollHandler(struct mg_connection *conn);
    bool distDevicesHandler(struct mg_connection *conn);
    void distUpdateCollectorListItem(WebEvent &event);
    void distUpdateNodeListItem(WebEvent &event);

    void collInit();
    bool collHandler(struct mg_connection *conn, const char * uri);
    bool collLoadJobHandler(struct mg_connection *conn, long id);
    bool collProcessHandler(struct mg_connection *conn, long id);
    bool collListHandler(struct mg_connection *conn, long id);
    void collUpdateID(WebEvent &event);
    void collUpdateFileList(WebEvent &event);
    void collUpdateFileListItem(WebEvent &event);
    void collUpdateProcessList(WebEvent &event);
    void collUpdateProcessListItem(WebEvent &event);

    void nodeInit();
    void nodeUpdateID(WebEvent& event);
    void nodeUpdateState(WebEvent &event);
    void nodeUpdateProcessList(WebEvent &event);
    void nodeUpdateClear(WebEvent &event);

    int run() override;
};

#endif //DICON_WEBAPP_H