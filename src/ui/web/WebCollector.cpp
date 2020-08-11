//
// Created by Haluk AKGUNDUZ on 27.06.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#include <paths.h>
#include <CollectorHost.h>
#include "JobItem.h"
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
        sendError(nullptr, conn, "Can not find the collector with ID : %d !!!", id);
        return false;
    }

    auto host = (CollectorHost&) collector->getHost();

    if (!fileName) {
        sendError(&collector->getHost(), conn, "Invalid upload process!!!");
        return false;
    }

    char buffer[TMP_BUFFER_SIZE];
    int len = 0;

    char tmpFile[PATH_MAX];
    sprintf(tmpFile, "%s%s", _PATH_TMP, fileName);
    FILE *uploadJobFile = fopen(tmpFile, "w");
    if (!uploadJobFile) {
        sendError(&collector->getHost(), conn, "Can not open tmp file : %s!!!", tmpFile);
        return false;
    }
    do {

        len = mg_read(conn, buffer, TMP_BUFFER_SIZE);
        if (len < 0) {
            mg_send_http_error(conn, 404, "%s", "No request body data");
            fclose(uploadJobFile);
            return false;
        }

        if (len > 0) {
            fwrite(buffer, 1, len, uploadJobFile);
        }

    } while (len > 0);

    fclose(uploadJobFile);

    JobItem* job = collector->loadJob(tmpFile);

    if (job->getStatus() != JOBSTATUS_OK) {

        sendError(&collector->getHost(), conn, "Job : %s is not a valid job, reason : %s !!!",
                  tmpFile, JobStatus::getDesc(job->getStatus()));
    } else {

        sendOK(&collector->getHost(), conn, "Job : %s is loaded...", tmpFile);
    }

    return true;
}

bool WebApp::collProcessHandler(struct mg_connection *conn, int id) {

    auto *collector = componentController->getCollector(id);
    if (collector != nullptr) {

        if (!collector->getJob()) {

            sendError(&collector->getHost(), conn, "No Job is loaded yet!!!");

            return false;
        }

        if (!collector->processJob()) {

            sendError(&collector->getHost(), conn, "Job : %s is not a valid job, reason : %s !!!",
                      collector->getJob()->getJobName(), JobStatus::getDesc(collector->getJob()->getStatus()));

            return false;
        }

        sendOK(&collector->getHost(), conn, "Job : %s 's execution is started...", collector->getJob()->getJobName());

        return true;
    }

    sendError(nullptr, conn, "No collector with ID : %d is found!!!", id);

    return false;
}

bool WebApp::collStateHandler(struct mg_connection *conn, int id) {

    auto *collector = componentController->getCollector(id);
    if (!collector) {
        sendError(nullptr, conn, "Can not find the collector with ID : %d !!!", id);
        return false;
    }

    auto host = (CollectorHost&) collector->getHost();

    auto *job = collector->getJob();
    if (job == nullptr) {
        sendError(&collector->getHost(), conn, "No Job is loaded yet!!!");
        return false;
    }

    auto* jsonObj = json_object_new_object();
    if (jsonObj == nullptr) {
        sendError(&collector->getHost(), conn, "Can not create json object!!!");
        return false;
    }

    json_object_object_add(jsonObj, "_jobName", json_object_new_string(job->getJobName()));
    json_object_object_add(jsonObj, "_state", json_object_new_int(host.getState()));
    json_object_object_add(jsonObj, "_duration", json_object_new_int(job->getDuration()));
    json_object_object_add(jsonObj, "_processCount",
            json_object_new_int(job->getProcessCount() - job->getProcessCount(PROCESS_STATE_ENDED)));
    json_object_object_add(jsonObj, "_jobStatus", json_object_new_int(job->getStatus()));

    auto* fileList = json_object_new_array();
    for (int j = 0; j < job->getFileCount(); j++) {

        auto *content = job->getFile(j);

        auto* fileItem = json_object_new_object();
        json_object_object_add(fileItem, "_name", json_object_new_string(content->getName()));
        json_object_object_add(fileItem, "_state", json_object_new_int(content->required() ? 1 : content->check() ? 0 : 2));
        json_object_object_add(fileItem, "_size", json_object_new_int64(content->getSize()));

        json_object_array_add(fileList, fileItem);
    }

    json_object_object_add(jsonObj, "_fileList", fileList);

    auto* processList = json_object_new_array();
    for (int j = 0; j < job->getProcessCount(); j++) {

        auto* processItem = json_object_new_object();
        json_object_object_add(processItem, "_id", json_object_new_int(job->getProcess(j)->getID()));
        json_object_object_add(processItem, "_process", json_object_new_string(job->getProcess(j)->getProcess()));
        json_object_object_add(processItem, "_state", json_object_new_int(job->getProcess(j)->getState()));
        json_object_object_add(processItem, "_node", json_object_new_int(job->getProcess(j)->getAssigned()));

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
