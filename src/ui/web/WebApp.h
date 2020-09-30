//
// Created by Haluk AKGUNDUZ on 23.06.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_WEBAPP_H
#define DICON_WEBAPP_H

#include "civetweb/civetweb.h"
#include "Application.h"

#define PORT "8081"
#define MAIN_URI "/"
#define REST_URI "/rest"
#define EVENT_URI "/event"
#define QUIT_URI "/quit"

#define DIST_URI "/dist"
#define COLL_URI "/coll"
#define NODE_URI "/node"

class WebApp : public App {

    struct mg_callbacks callbacks{};
    struct mg_context *context;
    NOTIFYTYPE notifyData[COMP_MAX]{};
    std::mutex notifyMutex;
    int webPort{DEFAULT_WEB_PORT};
    volatile bool runLoop{true};

public:

    WebApp(int *, const LogInfo&, std::vector<int>&);
    ~WebApp();

    int restHandler(struct mg_connection *conn);
    int mainHandler(struct mg_connection *conn);
    int eventHandler(struct mg_connection *conn);
    int quitHandler(struct mg_connection *conn);

    bool distHandler(struct mg_connection *conn, const char * uri);
    bool distStateHandler(struct mg_connection *conn);
    bool distPollHandler(struct mg_connection *conn);
    bool distAddCollHandler(struct mg_connection *conn);
    bool distAddNodeHandler(struct mg_connection *conn);

    bool collHandler(struct mg_connection *conn, const char * uri);
    bool collStateHandler(struct mg_connection *conn, int id);
    bool collLoadJobHandler(struct mg_connection *conn, int id, const char*);
    bool collProcessHandler(struct mg_connection *conn, int id);

    bool nodeHandler(struct mg_connection *conn, const char * uri);
    bool nodeStateHandler(struct mg_connection *conn, int id);

    int run() override;
    int notifyHandler(COMPONENT, NOTIFYTYPE) override;
    bool sendServerEvent(struct mg_connection *conn, int id);

    bool sendStr(struct mg_connection *conn, const std::string& str) {

        mg_send_http_ok(conn, "application/json; charset=utf-8", str.size());

        mg_write(conn, str.c_str(), str.size());

        return true;
    }

    bool sendFormatted(struct mg_connection *conn,
                       const std::string& type, const char* response) {

        char webOut[PATH_MAX];

        sprintf(webOut, "%s : %s \n",
                type.c_str(),
                response);

        sendStr(conn, webOut);

        return true;
    }

    bool sendFormatted(const TypeHostUnit& host, struct mg_connection *conn,
                 const std::string& type, const char* response) {

        char webOut[PATH_MAX];

        sprintf(webOut, "%s : %s(%d) => %s \n",
                type.c_str(),
                ComponentType::getName(host->getType()),
                host->getID(),
                response);

        sendStr(conn, webOut);

        return true;
    }

    void sendResponse(const TypeHostUnit& host, struct mg_connection *conn,
            const std::string& type, const char* format) {

        char response[PATH_MAX - NAME_MAX];
        std::sprintf(response, "%s", format);

        sendFormatted(host, conn, type, response);
    }

    template<typename... Args>
    void sendResponse(const TypeHostUnit& host, struct mg_connection *conn,
            const std::string& type, const char* format, Args&&... args) {

        char response[PATH_MAX - NAME_MAX];
        std::sprintf(response, format, args...);

        sendFormatted(host, conn, type, response);
    }

    void sendResponse(struct mg_connection *conn,
                      const std::string& type, const char* format) {

        char response[PATH_MAX - NAME_MAX];
        std::sprintf(response, "%s", format);

        sendFormatted(conn, type, response);
    }

    template<typename... Args>
    void sendResponse(struct mg_connection *conn, const std::string& type,
                      const char* format, Args&&... args) {

        char response[PATH_MAX - NAME_MAX];
        std::sprintf(response, format, args...);

        sendFormatted(conn, type, response);
    }

    void sendOK(const TypeHostUnit& host, struct mg_connection *conn, const char* format) {

        sendResponse(host, conn, "OK", format);
    }

    template<typename... Args>
    void sendOK(const TypeHostUnit& host, struct mg_connection *conn,
                      const char* format, Args&&... args) {

        sendResponse(host, conn, "OK", format, std::forward<Args>(args)...);
    }

    void sendOK(struct mg_connection *conn, const char* format) {

        sendResponse(conn, "OK", format);
    }

    template<typename... Args>
    void sendOK(struct mg_connection *conn,
                const char* format, Args&&... args) {

        sendResponse(conn, "OK", format, std::forward<Args>(args)...);
    }

    void sendError(const TypeHostUnit& host, struct mg_connection *conn, const char* format) {

        sendResponse(host, conn, "ERROR", format);
    }

    template<typename... Args>
    void sendError(const TypeHostUnit& host, struct mg_connection *conn,
                const char* format, Args&&... args) {

        sendResponse(host, conn, "ERROR", format, std::forward<Args>(args)...);
    }

    void sendError(struct mg_connection *conn, const char* format) {

        sendResponse(conn, "ERROR", format);
    }

    template<typename... Args>
    void sendError(struct mg_connection *conn,
                   const char* format, Args&&... args) {

        sendResponse(conn, "ERROR", format, std::forward<Args>(args)...);
    }
};

void sendHtml(struct mg_connection *conn);

#endif //DICON_WEBAPP_H
