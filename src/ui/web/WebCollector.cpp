//
// Created by Haluk AKGUNDUZ on 27.06.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#include <Job.h>
#include <paths.h>
#include "WebApp.h"

bool WebApp::collHandler(struct mg_connection *conn, const char * uri) {

    const struct mg_request_info *ri = mg_get_request_info(conn);

    char *nextStr;

    int collID = (int) strtol(uri + 1, &nextStr, 10);

    if (0 == strcmp(ri->request_method, "GET")) {

        if (0 == strcmp(nextStr, "/state")) {

            return collStateHandler(conn, collID);
        }

        if (0 == strcmp(nextStr, "/process")) {

            return collProcessHandler(conn, collID);
        }

        return false;

    } else if (0 == strcmp(ri->request_method, "POST")) {

        if (0 == strcmp(nextStr, "/load")) {

            const char* fileName = mg_get_header(conn, "FileName");

            return collLoadJobHandler(conn, collID, fileName);
        }

    }

    return false;
}

bool WebApp::collLoadJobHandler(struct mg_connection *conn, int id, const char* fileName) {

    auto *collector = componentController->getCollector(id);
    if (!collector) {
        PRINT("Can not find the collector with ID : %d !!!", id);
        mg_send_http_ok(conn, "application/json; charset=utf-8", 0);
        return false;
    }

    if (!fileName) {
        LOGS_I(collector->getHost(), "Invalid upload process!!!");
        mg_send_http_ok(conn, "application/json; charset=utf-8", 0);
        return false;
    }

    char buffer[TMP_BUFFER_SIZE];
    int len = 0;

    char tmpFile[PATH_MAX];
    sprintf(tmpFile, "%s%s", _PATH_TMP, fileName);

    FILE *uploadJobFile = fopen(tmpFile, "w");
    do {

        len = mg_read(conn, buffer, TMP_BUFFER_SIZE);
        if (len < 0) {
            mg_send_http_error(conn, 400, "%s", "No request body data");
            fclose(uploadJobFile);
            mg_send_http_ok(conn, "application/json; charset=utf-8", 0);
            return false;
        }

        if (len > 0) {
            fwrite(buffer, 1, len, uploadJobFile);
        }

    } while (len > 0);

    fclose(uploadJobFile);

    collector->loadJob(tmpFile);

    mg_send_http_ok(conn, "application/json; charset=utf-8", 0);

    return true;
}

bool WebApp::collProcessHandler(struct mg_connection *conn, int id) {

    auto *collector = componentController->getCollector(id);
    if (collector != nullptr) {
        collector->processJob();
    }

    mg_send_http_ok(conn, "application/json; charset=utf-8", 0);

    return true;
}

bool WebApp::collStateHandler(struct mg_connection *conn, int id) {

    auto *collector = componentController->getCollector(id);
    if (!collector) {
        PRINT("Can not find the collector with ID : %d !!!", id);
        mg_send_http_ok(conn, "application/json; charset=utf-8", 0);
        return false;
    }

    auto *job = collector->getJob();
    if (job == nullptr) {
        LOGS_I(collector->getHost(), "No Job is loaded yet!!!");
        mg_send_http_ok(conn, "application/json; charset=utf-8", 0);
        return false;
    }

    auto* jsonObj = json_object_new_object();
    if (jsonObj == nullptr) {
        LOGS_I(collector->getHost(), "Can not create json object!!!");
        mg_send_http_ok(conn, "application/json; charset=utf-8", 0);
        return false;
    }

    json_object_object_add(jsonObj, "_jobName", json_object_new_string(job->getName()));

    auto* fileList = json_object_new_array();
    for (int j = 0; j < job->getFileCount(); j++) {

        auto *content = job->getFile(j);

        auto* fileItem = json_object_new_object();
        json_object_object_add(fileItem, "_name", json_object_new_string(content->getFileName()));
        json_object_object_add(fileItem, "_validity", json_object_new_boolean(content->isValid()));
        json_object_object_add(fileItem, "_size", json_object_new_int64(content->getSize()));

        json_object_array_add(fileList, fileItem);
    }

    json_object_object_add(jsonObj, "_fileList", fileList);

    auto* processList = json_object_new_array();
    for (int j = 0; j < job->getProcessCount(); j++) {

        auto* processItem = json_object_new_object();
        json_object_object_add(processItem, "_id", json_object_new_int(job->getProcess(j).getID()));
        json_object_object_add(processItem, "_process", json_object_new_string(job->getProcess(j).get().getExec()));
        json_object_object_add(processItem, "_validity", json_object_new_boolean(job->getProcess(j).get().isValid()));
        json_object_object_add(processItem, "_state", json_object_new_int(job->getProcess(j).getState()));
        json_object_object_add(processItem, "_node", json_object_new_int(job->getProcess(j).getAssigned()));

        json_object_array_add(processList, processItem);
    }

    json_object_object_add(jsonObj, "_processList", processList);

    size_t json_str_len;

    const char *json_str = json_object_to_json_string_length(jsonObj, JSON_C_TO_STRING_SPACED, &json_str_len);

    mg_send_http_ok(conn, "application/json; charset=utf-8", json_str_len);

    mg_write(conn, json_str, json_str_len);

    json_object_put(jsonObj);

    return true;
}
