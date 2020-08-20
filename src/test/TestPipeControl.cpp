//
// Created by Haluk AKGUNDUZ on 23.07.2020.
//

#include <pstreams/pstream.h>
#include "TestApp.h"

void TestApp::testPipeControl(Distributor* distributor, Collector* collector, Node* node) {

    auto* job = collector->loadJob("../sample/Job1_x86_linux");
    if (!job) {
        PRINT("Job could not initialized");
        return;
    }

    PRINT("Execute Ready Processes");

    for (int j = 0; j < job->getProcessCount(); j++) {

        auto process = job->getProcess(j);
        if (process->getState() != PROCESS_STATE_READY) {
            continue;
        }

        std::string parsed = Util::parsePath(collector->getRootPath(), process->getParsedProcess());

        PRINT("\tID : %d, Process : %s, Parsed : %s",
              process->getID(),
              process->getProcess(),
              parsed.c_str());

        std::string childOut;

        redi::ipstream inProcess(parsed);
        if (!inProcess.good()) {
            continue;
        }

        while (std::getline(inProcess, childOut)) {
            Util::trim(childOut);
            if (!childOut.empty()) {
                PRINT("Process[%d] Output : %s", process->getID(), childOut.c_str());
            }
        }
    }
}
