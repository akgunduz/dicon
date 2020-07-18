//
// Created by Haluk Akgunduz on 02/09/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "ConsoleApp.h"

bool ConsoleApp::collLoadJobHandler(int id) {

    auto *collector = componentController->getCollector(id);
    if (collector != nullptr) {
        collector->loadJob(nullptr);
    }

    return true;
}

bool ConsoleApp::collProcessHandler(int id) {

    auto *collector = componentController->getCollector(id);
    if (collector != nullptr) {
        collector->processJob();
    }

    return true;
}

bool ConsoleApp::collStateHandler(int id) {

    auto *collector = componentController->getCollector(id);
    if (!collector) {
        PRINT("Can not find the collector with ID : %d !!!", id);
        return false;
    }

    auto *job = collector->getJob();
    if (job == nullptr) {
        return false;
    }

    PRINT("Collector[%d] Status", id);

    PRINT("Files of Job : %s", job->getName());

    for (int j = 0; j < job->getFileCount(); j++) {

        auto *content = job->getFile(j);

        PRINT("\tFile : %s, valid? : %s", content->getFileName(), content->isValid() ? "yes" : "no");
    }

    PRINT("Processes");

    for (int j = 0; j < job->getProcessCount(); j++) {

        PRINT("\tID : %d, Process : %s, State : %s, Assigned Node : %d",
                job->getProcess(j).getID(),
              job->getProcess(j).get().getProcess(),
                ProcessState::getName(job->getProcess(j).getState()),
                job->getProcess(j).getAssigned());
    }

    return true;
}
