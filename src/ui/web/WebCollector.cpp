//
// Created by Haluk AKGUNDUZ on 27.06.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#include <ui/UserInterfaceEvent.h>
#include <Job.h>
#include "WebApp.h"

void WebApp::collInit() {

    uiUpdater[UI_UPDATE_COLL] = &WebApp::collUpdate;
}

bool WebApp::collHandler(struct mg_connection *conn, const char * uri) {

    const struct mg_request_info *ri = mg_get_request_info(conn);

    if (0 == strcmp(ri->request_method, "GET")) {

        char *nextStr;

        long collID = strtol(uri + 1, &nextStr, 10);

        if (0 == strcmp(nextStr, "/state")) {

            return collStateHandler(conn, collID);
        }

        if (0 == strcmp(nextStr, "/load")) {

            return collLoadJobHandler(conn, collID);
        }

        if (0 == strcmp(nextStr, "/process")) {

            return collProcessHandler(conn, collID);
        }
    }

    return 0;
}

bool WebApp::collLoadJobHandler(struct mg_connection *conn, long id) {

    auto *collector = componentController->getCollector(id);
    if (collector != nullptr) {
        collector->loadJob(nullptr);
    }

    mg_send_http_ok(conn, "application/json; charset=utf-8", 0);

    return true;
}

bool WebApp::collProcessHandler(struct mg_connection *conn, long id) {

    auto *collector = componentController->getCollector(id);
    if (collector != nullptr) {
        collector->processJob();
    }

    mg_send_http_ok(conn, "application/json; charset=utf-8", 0);

    return true;
}

bool WebApp::collStateHandler(struct mg_connection *conn, long id) {

    auto *collector = componentController->getCollector(id);

    auto *job = collector->getJob();
    if (job == nullptr) {
        return false;
    }

    auto* jsonObj = json_object_new_object();
    if (jsonObj == nullptr) {
        LOG_S("Can not create json object!!!");
        return false;
    }

    auto* fileList = json_object_new_array();
    for (int j = 0; j < job->getFileCount(); j++) {

        auto *content = job->getFile(j);

        auto* fileItem = json_object_new_object();
        json_object_object_add(fileItem, "file", json_object_new_string(content->getFileName()));
        json_object_object_add(fileItem, "validity", json_object_new_boolean(content->isValid()));

        json_object_array_add(fileList, fileItem);
    }

    json_object_object_add(jsonObj, "fileList", fileList);

    auto* processList = json_object_new_array();
    for (int j = 0; j < job->getProcessCount(); j++) {

        auto* processItem = json_object_new_object();
        json_object_object_add(processItem, "id", json_object_new_int(job->getProcess(j).getID()));
        json_object_object_add(processItem, "process", json_object_new_string(job->getProcess(j).get().getExec()));
        json_object_object_add(processItem, "validity", json_object_new_boolean(job->getProcess(j).get().isValid()));
        json_object_object_add(processItem, "state", json_object_new_int(job->getProcess(j).getState()));
        json_object_object_add(processItem, "node", json_object_new_int(job->getProcess(j).getAssigned()));

        json_object_array_add(processList, processItem);
    }

    json_object_object_add(jsonObj, "processList", processList);

    size_t json_str_len;

    const char *json_str = json_object_to_json_string_length(jsonObj, JSON_C_TO_STRING_SPACED, &json_str_len);

    mg_send_http_ok(conn, "application/json; charset=utf-8", json_str_len);

    mg_write(conn, json_str, json_str_len);

    json_object_put(jsonObj);

    return true;
}

void WebApp::collUpdate(WebEvent &event) {

    Timer::set("coll", 1000, WebApp::wsInform, this);
}
