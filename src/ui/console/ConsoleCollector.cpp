//
// Created by Haluk Akgunduz on 02/09/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "ConsoleApp.h"

bool ConsoleApp::collLoadJobHandler(long id) {

    auto &collector = componentController->getCollector(id);
    if (collector != nullptr) {
        std::filesystem::path zipFile = "../sample/Job1_x86_linux.zip";
        collector->loadJob(zipFile);
    }

    return true;
}

bool ConsoleApp::collProcessHandler(long id) {

    auto &collector = componentController->getCollector(id);
    if (collector != nullptr) {
        collector->processJob();
    }

    return true;
}

bool ConsoleApp::collStateHandler(long id) {

    auto &collector = componentController->getCollector(id);
    if (!collector) {
        LOGP_E("Can not find the collector with ID : %d !!!", id);
        return false;
    }

    auto job = collector->getJob();
    if (!job) {
        return false;
    }

    LOGP_I("Collector[%d] Status", id);

    LOGP_I("Files of Job : %s", job->getName());

    for (int j = 0; j < job->getFileCount(); j++) {

        auto content = job->getFile(j);

        LOGP_I("\tFile : %s, valid? : %s", content->getName().c_str(), content->check() ? "yes" : "no");
    }

    LOGP_I("Processes");

    for (int j = 0; j < job->getProcessCount(); j++) {

        LOGP_I("\tID : %d, Process : %s, State : %s, Assigned Node : %d",
                job->getProcess(j)->getID(),
                job->getProcess(j)->getProcess(),
                ProcessState::getName(job->getProcess(j)->getState()),
                job->getProcess(j)->getAssigned());
    }

    return true;
}
