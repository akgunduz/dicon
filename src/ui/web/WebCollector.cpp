//
// Created by Haluk AKGUNDUZ on 27.06.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

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

    auto &collector = componentFactory->getCollector(id);
    if (!collector) {
        sendError(conn, "Can not find the collector with ID : %d !!!", id);
        return false;
    }

    if (!fileName) {
        sendError(collector->getHost(), conn, "Invalid upload process!!!");
        return false;
    }

    char buffer[TMP_BUFFER_SIZE];
    int len = 0;

    std::filesystem::path tmpFile = Util::tmpPath / fileName;

    std::FILE* uploadJobFile = std::fopen(tmpFile.string().c_str(), "wb");

    if (!uploadJobFile) {
        sendError(collector->getHost(), conn, "Can not open tmp file : %s!!!", tmpFile.c_str());
        return false;
    }

    do {

        len = mg_read(conn, buffer, TMP_BUFFER_SIZE);

        if (len < 0) {
            mg_send_http_error(conn, 404, "%s", "No request body data");
            std::fclose(uploadJobFile);
            return false;
        }

        if (len > 0) {
            std::fwrite(buffer, 1, len, uploadJobFile);
        }

    } while (len > 0);

    std::fclose(uploadJobFile);

    auto job = collector->loadJob(tmpFile);

    if (job->getStatus() != JOBSTATUS_OK) {

        sendError(collector->getHost(), conn, "Job : %s is not a valid job, reason : %s !!!",
                  uploadJobFile, JobStatus::getDesc(job->getStatus()));
    } else {

        sendOK(collector->getHost(), conn, "Job : %s is loaded...", tmpFile.c_str());
    }

    return true;
}

bool WebApp::collProcessHandler(struct mg_connection *conn, int id) {

    auto &collector = componentFactory->getCollector(id);
    if (collector != nullptr) {

        if (!collector->getJob()) {

            sendError(collector->getHost(), conn, "No Job is loaded yet!!!");

            return false;
        }

        if (!collector->processJob()) {

            sendError(collector->getHost(), conn, "Job : %s is not a valid job, reason : %s !!!",
                      collector->getJob()->getJobName().c_str(), JobStatus::getDesc(collector->getJob()->getStatus()));

            return false;
        }

        sendOK(collector->getHost(), conn, "Job : %s 's execution is started...",
               collector->getJob()->getJobName().c_str());

        return true;
    }

    sendError(conn, "No collector with ID : %d is found!!!", id);

    return false;
}

bool WebApp::collStateHandler(struct mg_connection *conn, int id) {

    auto &collector = componentFactory->getCollector(id);
    if (!collector) {
        sendError(conn, "Can not find the collector with ID : %d !!!", id);
        return false;
    }

    auto collectorHost = std::static_pointer_cast<CollectorHost>(collector->getHost());

    auto job = collector->getJob();
    if (!job) {
        sendError(collector->getHost(), conn, "No Job is loaded yet!!!");
        return false;
    }

    nlohmann::json jobInfo;

    jobInfo["_jobName"] = job->getJobName();
    jobInfo["_state"] = collectorHost->getState();
    jobInfo["_duration"] = job->getDuration();
    jobInfo["_processCount"] = job->getProcessCount() - job->getProcessCount(PROCESS_STATE_ENDED);
    jobInfo["_jobStatus"] = job->getStatus();

    nlohmann::json fileList;

    for (int j = 0; j < job->getFileCount(); j++) {

        nlohmann::json fileItem;

        fileItem["_name"] = job->getFile(j)->getName();
        fileItem["_state"] = job->getFile(j)->required() ? 1 : job->getFile(j)->check() ? 0 : 2;
        fileItem["_size"] = job->getFile(j)->getSize();

        fileList += fileItem;

    }

    nlohmann::json processList;

    for (int j = 0; j < job->getProcessCount(); j++) {

        nlohmann::json processItem;

        processItem["_id"] = job->getProcess(j)->getID();
        processItem["_process"] = job->getProcess(j)->getProcess();
        processItem["_state"] = job->getProcess(j)->getState();
        processItem["_node"] = job->getProcess(j)->getAssigned();

        processList += processItem;

    }

    jobInfo["_fileList"] = fileList;
    jobInfo["_processList"] = processList;

    sendStr(conn, jobInfo.dump());

    return true;
}
