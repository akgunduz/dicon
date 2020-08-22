//
// Created by Haluk AKGUNDUZ on 23.07.2020.
//

#include <pstreams/pstream.h>
#include "TestApp.h"

void TestApp::testPipeControl(TypeDistributor& distributor, TypeCollector& collector, TypeNode& node) {

    auto job = collector->loadJob("../sample/Job1_x86_linux");
    if (!job) {
        LOGP_E("Job could not initialized");
        return;
    }

    LOGP_I("Execute Ready Processes");

    for (int j = 0; j < job->getProcessCount(); j++) {

        auto process = job->getProcess(j);
        if (process->getState() != PROCESS_STATE_READY) {
            continue;
        }

        std::string parsed = Util::parsePath(collector->getHost()->getRootPath(), process->getParsedProcess());

        LOGP_I("\tID : %d, Process : %s, Parsed : %s",
              process->getID(),
              process->getProcess().c_str(),
              parsed.c_str());

        std::string childOut;

        redi::ipstream inProcess(parsed);
        if (!inProcess.good()) {
            continue;
        }

        while (std::getline(inProcess, childOut)) {
            Util::trim(childOut);
            if (!childOut.empty()) {
                LOGP_I("Process[%d] Output : %s", process->getID(), childOut.c_str());
            }
        }
    }
}
