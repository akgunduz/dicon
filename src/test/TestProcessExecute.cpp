//
// Created by akgun on 18.09.2020.
//

#include <UvUtil.h>
#include "TestApp.h"

void TestApp::testProcessExecute(TypeDistributor& distributor, TypeCollector& collector, TypeNode& node) {

    auto job = std::make_shared<JobItem>(collector->getHost(), TEST_JOB_PATH, JobItem::jobID++);
    if (!job) {
        LOGP_E("Job could not initialized");
        return;
    }

    TypeID processID = 0;

    LOGP_I("Execute Process : %d", processID);

    auto process = job->getProcess(processID);

    std::string parsedCmd = Util::parsePath(collector->getHost()->getRootPath(), process->getParsedProcess());

    LOGP_I("\tID : %d, Process : %s, Parsed : %s",
           process->getID(),
           process->getProcess().c_str(),
           parsedCmd.c_str());

    UvUtil::executeProcess(parsedCmd, uv_default_loop(), (void *) collector.get(), [] (void *data) -> bool {

        auto collector = (Collector*) data;

        LOGS_I(collector->getHost(), "Executed Process");

        return true;
    });

    uv_loop_close(uv_default_loop());
}
