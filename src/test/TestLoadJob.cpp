//
// Created by Haluk AKGUNDUZ on 23.07.2020.
//

#include "TestApp.h"

void TestApp::testLoadJob(Distributor* distributor, Collector* collector, Node* node) {

    auto* job = collector->loadJob("../sample/Job2_macos");
    if (!job) {
        PRINT("Job file is not exist");
        return;
    }

    PRINT("Files of Job : %s", job->getName());

    for (int j = 0; j < job->getFileCount(); j++) {

        auto *content = job->getFile(j);

        PRINT("\tFile : %s, valid? : %s", content->getName(), content->check() ? "yes" : "no");
    }

    PRINT("Processes");

    for (int j = 0; j < job->getProcessCount(); j++) {

        auto* process = job->getProcess(j);
        PRINT("\tID : %d, Process : %s, State : %s, Assigned Node : %d",
              process->getID(),
              process->getProcess(),
              ProcessState::getName(process->getState()),
              process->getAssigned());
        for (auto file : process->getFileList()) {

            PRINT("\tFile => ID : %d, Name : %s, State : %s",
                  file.get()->getID(),
                  file.get()->getName(),
                  file.isOutput() ? "output" : "input");
        }
    }
}