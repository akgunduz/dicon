//
// Created by Haluk AKGUNDUZ on 23.07.2020.
//

#include "TestApp.h"

void TestApp::testProcessList(TypeDistributor& distributor, TypeCollector& collector, TypeNode& node) {

    auto job = std::make_shared<JobItem>(collector->getHost(), TEST_JOB_PATH, JobItem::jobID++);
    if (!job) {
        LOGP_E("Job could not initialized");
        return;
    }

    LOGP_I("List Processes");

    for (int j = 0; j < job->getProcessCount(); j++) {

        auto process = job->getProcess(j);

        std::string parsed = Util::parsePath(collector->getHost()->getRootPath(), process->getParsedProcess());

        LOGP_I("\tID : %d, State : %s, Process : %s, Parsed : %s",
              process->getID(),
              ProcessState::getName(process->getState()),
              process->getProcess().c_str(),
              parsed.c_str());
    }
}
