//
// Created by Haluk AKGUNDUZ on 29/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Node.h"
#include "Util.h"
#include "CollectorUnit.h"
#include "pstreams/pstream.h"

#define PROCESS_SLEEP_TIME 1000

Node::Node(int interface) {

    host = std::make_shared<NodeHost>();

    LOGS_T(getHost(), "Node[%d] is initializing", host->getID());

    addProcessHandler(COMP_DISTRIBUTOR, MSGTYPE_WAKEUP, static_cast<TypeProcessComponentMsg>(&Node::processDistributorWakeupMsg));
    addProcessHandler(COMP_DISTRIBUTOR, MSGTYPE_ID, static_cast<TypeProcessComponentMsg>(&Node::processDistributorIDMsg));
    addProcessHandler(COMP_DISTRIBUTOR, MSGTYPE_PROCESS, static_cast<TypeProcessComponentMsg>(&Node::processDistributorProcessMsg));

    addProcessHandler(COMP_COLLECTOR, MSGTYPE_PROCESS, static_cast<TypeProcessComponentMsg>(&Node::processCollectorProcessMsg));
    addProcessHandler(COMP_COLLECTOR, MSGTYPE_BINARY, static_cast<TypeProcessComponentMsg>(&Node::processCollectorBinaryMsg));
    addProcessHandler(COMP_COLLECTOR, MSGTYPE_READY, static_cast<TypeProcessComponentMsg>(&Node::processCollectorReadyMsg));

    initInterfaces(COMP_NODE, interface, interface);

    processItem = std::make_shared<ProcessItem>(host);

    LOGS_T(getHost(), "Node is initialized");
}

Node::~Node() {

    LOGP_T("Deallocating Node");

};

bool Node::processDistributorWakeupMsg(ComponentUnit& owner, TypeMessage msg) {

    LOGC_T(getHost(), owner, MSGDIR_RECEIVE, "Setting Distributor");

    setDistributor(owner);

    return send2DistributorAliveMsg(owner);
}

bool Node::processDistributorIDMsg(ComponentUnit& owner, TypeMessage msg) {

    if (!setID(msg->getHeader().getVariant(0))) {

        LOGC_E(getHost(), owner, MSGDIR_RECEIVE, "New ID : 0 is tried to assigned, it should not!!!");

        return false;
    }

    LOGC_I(getHost(), owner, MSGDIR_RECEIVE, "New ID : %d is assigned by Distributor", getHost()->getID());

    return send2DistributorIDMsg(owner);
}

bool Node::processDistributorProcessMsg(ComponentUnit& owner, TypeMessage msg) {

    auto nodeHost = std::static_pointer_cast<NodeHost>(host);

    LOGC_I(getHost(), owner, MSGDIR_RECEIVE, "Collector[%d]:Process[%d] is approved by distributor",
           nodeHost->getAssigned()->getID(),
           processItem->getID());

    TypeProcessFileList requiredList;

    LOGC_T(getHost(), owner, MSGDIR_RECEIVE, "Collector[%d]:Process[%d] Figuring out required File List",
           nodeHost->getAssigned()->getID(),
           processItem->getID());

    for (auto processFile : processItem->getFileList()) {
        if (!processFile.isOutput() &&
                !std::filesystem::exists(nodeHost->getRootPath() /
                    std::to_string(processFile.get()->getAssignedJob()) /
                    processFile.get()->getName())) {
            requiredList.emplace_back(processFile);
        }
    }

    if (!requiredList.empty()) {

        LOGC_T(getHost(), owner, MSGDIR_RECEIVE, "Collector[%d]:Process[%d] Required %d Files is requested",
               nodeHost->getAssigned()->getID(),
               processItem->getID(), requiredList.size());

        CollectorUnit collObj(*nodeHost->getAssigned());

        return send2CollectorInfoMsg(collObj, processItem->getID(),requiredList);

    } else {

        LOGC_T(getHost(), owner, MSGDIR_RECEIVE, "Collector[%d]:Process[%d] Processing....",
               nodeHost->getAssigned()->getID(),
               processItem->getID());

        return processJob(owner, std::move(msg));
    }
}

bool Node::processCollectorProcessMsg(ComponentUnit& owner, TypeMessage msg) {

    auto nodeHost = std::static_pointer_cast<NodeHost>(host);

    LOGC_I(getHost(), owner, MSGDIR_RECEIVE, "Collector[%d]:Process[%d] request is received ",
           owner.getID(), processItem->getID());

    componentWatch.start();

    nodeHost->setState(NODESTATE_BUSY);
    nodeHost->setAssigned(owner.getType(), owner.getArch(), owner.getID(), owner.getAddress());

    processItem = msg->getData().getProcess(0);
    processItem->setAssigned(owner.getID());
    processItem->setState(PROCESS_STATE_STARTED);
    processItem->addFileList(msg->getData().getFileList());

    LOGC_T(getHost(), owner, MSGDIR_RECEIVE, "Collector[%d]:Process[%d] Prepared....",
           nodeHost->getAssigned()->getID(),
           processItem->getID());

    return send2DistributorBusyMsg(distributor, owner.getID());
}

bool Node::processCollectorBinaryMsg(ComponentUnit& owner, TypeMessage msg) {

    auto nodeHost = std::static_pointer_cast<NodeHost>(host);

    LOGC_I(getHost(), owner, MSGDIR_RECEIVE, "Collector[%d]:Process[%d] binaries are received",
           owner.getID(), processItem->getID());

    LOGC_T(getHost(), owner, MSGDIR_RECEIVE, "Collector[%d]:Process[%d] Processing....",
           nodeHost->getAssigned()->getID(),
           processItem->getID());

    return processJob(owner, std::move(msg));
}

bool Node::processCollectorReadyMsg(ComponentUnit& owner, TypeMessage msg) {

    auto nodeHost = std::static_pointer_cast<NodeHost>(host);

    nodeHost->setState(NODESTATE_IDLE);

    processItem->setState(PROCESS_STATE_ENDED);

    processItem->setDuration(componentWatch.stop());

    processList.emplace_back(processItem);

    notifyUI(NOTIFYTYPE_TRANSPARENT);

    return send2DistributorReadyMsg(distributor);
}


bool Node::processJob(const ComponentUnit& owner, TypeMessage msg) {

    auto nodeHost = std::static_pointer_cast<NodeHost>(host);

    LOGS_I(getHost(), "Collector[%d]:Process[%d] starts execution",
           nodeHost->getAssigned()->getID(),
           processItem->getID());

    int result = processCommand(nodeHost->getAssigned()->getID(),
                                processItem->getID(),
                                processItem->getParsedProcess());

    if (!result) {
        return false;
    }

    TypeProcessFileList outputList;

    for (auto processFile : processItem->getFileList()) {
        if (processFile.isOutput()) {
            if (processFile.get()->check()) {
                processFile.setOutputState(false);
                outputList.emplace_back(processFile);
            }
        }
    }

    CollectorUnit obj(*nodeHost->getAssigned());

    return send2CollectorBinaryMsg(obj, processItem->getID(),outputList);
}

bool Node::send2DistributorReadyMsg(ComponentUnit& target) {

	auto msg = std::make_unique<Message>(getHost(), target, MSGTYPE_READY);

	return send(target, std::move(msg));
}

bool Node::send2DistributorAliveMsg(ComponentUnit& target) {

    auto msg = std::make_unique<Message>(getHost(), target, MSGTYPE_ALIVE);

    return send(target, std::move(msg));
}

bool Node::send2DistributorIDMsg(ComponentUnit& target) {

    auto msg = std::make_unique<Message>(getHost(), target, MSGTYPE_ID);

    return send(target, std::move(msg));
}

bool Node::send2DistributorBusyMsg(ComponentUnit& target, long collID) {

    auto msg = std::make_unique<Message>(getHost(), target, MSGTYPE_BUSY);

    msg->getHeader().setVariant(0, collID);

    return send(target, std::move(msg));
}

bool Node::send2CollectorInfoMsg(ComponentUnit& target, long processID, TypeProcessFileList &fileList) {

	auto msg = std::make_unique<Message>(getHost(), target, MSGTYPE_INFO);

    msg->getData().setStreamFlag(STREAM_FILEINFO);
    msg->getData().addFileList(processID, fileList);

    LOGC_I(getHost(), target, MSGDIR_SEND, "Collector[%d]:Process[%d]'s missing files are requested, total %d files",
           target.getID(),
           processID,
           fileList.size());

	return send(target, std::move(msg));
}

bool Node::send2CollectorBinaryMsg(ComponentUnit& target, long processID, TypeProcessFileList &fileList) {

    auto msg = std::make_unique<Message>(getHost(), target, MSGTYPE_BINARY);

    msg->getData().setStreamFlag(STREAM_FILEBINARY);
    msg->getData().addFileList(processID, fileList);

    LOGC_I(getHost(), target, MSGDIR_SEND, "Collector[%d]:Process[%d] is successfully executed, sending back output data",
           target.getID(),
           processID);

    return send(target, std::move(msg));
}

void Node::setDistributor(const ComponentUnit& _distributor) {

    this->distributor = _distributor;
}

void Node::parseCommand(char *cmd, char **argv) {

    while (*cmd != '\0') {

        while (*cmd == ' ' || *cmd == '\t' || *cmd == '\n') {
            *cmd++ = '\0';
        }

        *argv++ = cmd;

        while (*cmd != '\0' && *cmd != ' ' && *cmd != '\t' && *cmd != '\n') {
            cmd++;
        }
    }
    *argv = nullptr;
}

bool Node::processCommand(long collID, long processID, const std::string& cmd) {

    std::string childOut;

    std::string parsedCmd = Util::parsePath(getHost()->getRootPath(), cmd);

    LOGS_I(getHost(), "Collector[%d]:Process[%d] Command : %s",
           collID, processID, parsedCmd.c_str());

    int tryCount = 1;
    while(tryCount++ < 10) {

        redi::ipstream inProcess(parsedCmd);

        if (!inProcess.good()) {
            LOGS_E(getHost(), "Collector[%d]:Process[%d] Can not execute command",
                   collID, processID, parsedCmd.c_str());
            return false;
        }

        while (std::getline(inProcess, childOut)) {
            Util::trim(childOut);
            if (!childOut.empty()) {
                LOGS_I(getHost(), "Collector[%d]:Process[%d] Output : %s", collID, processID, childOut.c_str());
            }
        }

        int res = inProcess.close();
        if (res == 0) {
            return true;
        }

        LOGS_E(getHost(), "Collector[%d]:Process[%d] has execution problem retrying", collID, processID);

        std::this_thread::sleep_for(std::chrono::milliseconds(PROCESS_SLEEP_TIME * tryCount));
    }

    LOGS_E(getHost(), "Collector[%d]:Process[%d] is ended with error", collID, processID);

    return false;
}

TypeProcessList& Node::getProcessList() {

    return processList;
}
