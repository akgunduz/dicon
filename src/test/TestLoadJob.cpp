//
// Created by Haluk AKGUNDUZ on 23.07.2020.
//

#include "TestApp.h"

void TestApp::testLoadJob(TypeDistributor& distributor, TypeCollector& collector, TypeNode& node) {

    auto job = collector->loadJob("../sample/Job1_x86_linux");
    if (!job) {
        LOGP_E("Job could not initialized");
        return;
    }

    LOGP_I("Job status : %d", job->getStatus());

    LOGP_I("Files of Job : %s", job->getName());

    for (int j = 0; j < job->getFileCount(); j++) {

        auto content = job->getFile(j);

        LOGP_I("\tFile : %s, State : %s", content->getName(), content->required() ? "required"
            : content->check() ? "exists" : "output");
    }

    LOGP_I("Processes");

    for (int j = 0; j < job->getProcessCount(); j++) {

        auto process = job->getProcess(j);
        LOGP_I("\tID : %d, Process : %s, State : %s, Assigned Node : %d",
              process->getID(),
              process->getProcess(),
              ProcessState::getName(process->getState()),
              process->getAssigned());

        for (auto file : process->getFileList()) {

            LOGP_I("\tFile => ID : %d, Name : %s, State : %s",
                  file.get()->getID(),
                  file.get()->getName(),
                  file.isOutput() ? "output" : "input");
        }
    }
}
